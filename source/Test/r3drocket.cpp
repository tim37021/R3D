#include "r3drocket.h"
#include <r3d/Core/Core.hpp>
#include <cstdio>
#include <glm/glm.hpp>
#include <r3d/Utils/Image.hpp>
#include <vector>
#include <cstring>

static const char *vertex_shader=
	"#version 330\n"
	"layout(location=0) in vec2 pos;\n"
	"layout(location=1) in vec4 colour;\n"
	"layout(location=2) in vec2 texcoord;\n"
	"out vec2 vTexCoord;\n"
	"out vec4 vColor;\n"
	"uniform vec2 viewport;\n"
	"uniform vec2 translation;\n"
	"void main(){\n"
	"vTexCoord=texcoord;\n"
	"vColor=colour/255.0;\n"
	"gl_Position=vec4((pos.x+translation.x)*2.0/viewport.x-1, (viewport.y-(pos.y+translation.y))*2.0/viewport.y-1, 0.0, 1.0);"
	"}\n";

// -1 0  1 x
static const char *fragment_shader=
	"#version 330\n"
	"out vec4 color;\n"
	"in vec4 vColor;\n"
	"uniform sampler2D text;\n"
	"in vec2 vTexCoord;\n"
	"void main(){\n"
	"color=vColor*texture(text, vTexCoord);\n"
	"}";

static const char *fragment_shader_no_texture=
	"#version 330\n"
	"out vec4 color;\n"
	"in vec4 vColor;\n"
	"in vec2 vTexCoord;\n"
	"void main(){\n"
	"color=vColor;\n"
	"}\n";

struct tuple4
{
	r3d::Buffer *vbo, *ibo;
	r3d::Texture2D *texture;
	uint32_t num_indices;
	tuple4(r3d::Buffer *vbo_, r3d::Buffer *ibo_, r3d::Texture2D *texture_, uint32_t num_indices_): 
		vbo(vbo_), ibo(ibo_), texture(texture_), num_indices(num_indices_){}
};

static r3d::ProgramPtr MakeShaderProgram(const r3d::Engine *engine, const char *vsource,
	const char *fsource)
{
	auto program=engine->newProgram();
	auto vs=engine->newShader(r3d::ST_VERTEX_SHADER);
	auto fs=engine->newShader(r3d::ST_FRAGMENT_SHADER);
	vs->source(vsource);
	fs->source(fsource);
	vs->compile();
	fs->compile();
	program->attachShader(vs);
	program->attachShader(fs);
	program->link();
 
	return program;
}


namespace R3DRocket
{
	SystemInterface::SystemInterface(r3d::Engine *engine):
		m_engine(engine)
	{

	}

	float SystemInterface::GetElapsedTime()
	{
		return m_engine->getTime();
	}

	// Log the specified message.
	bool SystemInterface::LogMessage(Rocket::Core::Log::Type type, const Rocket::Core::String& message)
	{
		m_engine->log(std::string("libRocket: ")+message.CString());
		return true;
	}

	////////////////////////////////////////////////////////////

	RenderInterface::RenderInterface(r3d::Engine *engine):
		m_engine(engine), m_tmp_id(0)
	{

		m_vMgr=engine->getVertexArrayManager();
		m_bMgr=engine->getBufferManager();
		vbo=m_bMgr->registerVertexBuffer("RI_TMP", nullptr, 200*(2+2)*sizeof(float), r3d::BU_DYNAMIC_DRAW);
		std::vector<uint32_t> dummy; dummy.resize(200);
		ibo=m_bMgr->registerIndexBuffer("RI_TMP", dummy, r3d::BU_DYNAMIC_DRAW);

		vao=m_vMgr->registerVertexArray("RI_TMP");
		vao->bindVertexBuffer(vbo);
		vao->bindIndexBuffer(ibo);

		r3d::AttribPointer vertAtt(r3d::RT_FLOAT, 2, sizeof(Rocket::Core::Vertex), 0);
		r3d::AttribPointer colourAtt(r3d::RT_UNSIGNED_BYTE, 4, sizeof(Rocket::Core::Vertex), 8);
		r3d::AttribPointer texAtt(r3d::RT_FLOAT, 2, sizeof(Rocket::Core::Vertex), 12);
		vao->enableAttribArray(0, vertAtt);
		vao->enableAttribArray(1, colourAtt);
		vao->enableAttribArray(2, texAtt);

		// for compiled geometry
		vaoc=m_vMgr->registerVertexArray("RI_OPTIMIZED_TMP");
		vaoc->enableAttribArray(0, vertAtt);
		vaoc->enableAttribArray(1, colourAtt);
		vaoc->enableAttribArray(2, texAtt);

		m_program=MakeShaderProgram(m_engine, vertex_shader, fragment_shader);
		m_programNoTexture=MakeShaderProgram(m_engine, vertex_shader, fragment_shader_no_texture);

		auto cw=engine->getCurrentContext();
		m_program->setUniform("viewport", {cw->getWidth(), cw->getHeight()});
		m_programNoTexture->setUniform("viewport", {cw->getWidth(), cw->getHeight()});
	}

	void RenderInterface::RenderGeometry(Rocket::Core::Vertex* vertices,
		int num_vertices, int* indices, int num_indices, 
		Rocket::Core::TextureHandle texture, 
		const Rocket::Core::Vector2f& translation)
	{
		Rocket::Core::Vertex *gpu_vertices=(Rocket::Core::Vertex *)vbo->lock(r3d::BA_WRITE_ONLY);
		memcpy(gpu_vertices, vertices, num_vertices*sizeof(Rocket::Core::Vertex));
		vbo->unlock();

		uint32_t *gpu_indices=(uint32_t *)ibo->lock(r3d::BA_WRITE_ONLY);
		memcpy(gpu_indices, indices, num_indices*sizeof(uint32_t));
		ibo->unlock();
		if(texture)
		{
			m_program->use();
			((r3d::Texture2D *)texture)->bind(0);
			m_program->setUniform("text", 0);
			m_program->setUniform("translation", {translation.x, translation.y});
			m_engine->getRenderer()->drawElements(m_program.get(), vao, r3d::PT_TRIANGLES, num_indices);
		}else
		{
			m_programNoTexture->use();
			m_programNoTexture->setUniform("translation", {translation.x, translation.y});
			m_engine->getRenderer()->drawElements(m_programNoTexture.get(), vao, r3d::PT_TRIANGLES, num_indices);
		}
	}

	Rocket::Core::CompiledGeometryHandle RenderInterface::CompileGeometry(Rocket::Core::Vertex* vertices,
 		int num_vertices, int* indices, int num_indices,
 		Rocket::Core::TextureHandle texture)
	{
		auto vbo=m_bMgr->registerVertexBuffer(std::string("LIBROCKET_"+std::to_string(m_tmp_id)), vertices, 
			sizeof(Rocket::Core::Vertex)*num_vertices, r3d::BU_STATIC_DRAW);
		std::vector<uint32_t> indices_;
		indices_.resize(num_indices);
		memcpy(&indices_[0], indices, sizeof(uint32_t)*num_indices);
		auto ibo=m_bMgr->registerIndexBuffer(std::string("LIBROCKET_"+std::to_string(m_tmp_id++)), indices_,
			r3d::BU_STATIC_DRAW);

		return (Rocket::Core::CompiledGeometryHandle)new tuple4(vbo, ibo, 
			reinterpret_cast<r3d::Texture2D *>(texture), num_indices);
	}

	void RenderInterface::RenderCompiledGeometry(Rocket::Core::CompiledGeometryHandle geometry, 
			const Rocket::Core::Vector2f& translation)
	{
		tuple4 *tuple=reinterpret_cast<tuple4 *>(geometry);
		vaoc->bindVertexBuffer(tuple->vbo);
		vaoc->bindIndexBuffer(tuple->ibo);
		if(tuple->texture)
		{
			m_program->use();
			tuple->texture->bind(0);
			m_program->setUniform("text", 0);
			m_program->setUniform("translation", {translation.x, translation.y});
			m_engine->getRenderer()->drawElements(m_program.get(), vaoc, r3d::PT_TRIANGLES, tuple->num_indices);
		}else
		{
			m_programNoTexture->use();
			m_programNoTexture->setUniform("translation", {translation.x, translation.y});
			m_engine->getRenderer()->drawElements(m_programNoTexture.get(), vaoc, r3d::PT_TRIANGLES, tuple->num_indices);
		}
	}

	void RenderInterface::ReleaseCompiledGeometry(Rocket::Core::CompiledGeometryHandle geometry)
	{
		tuple4 *tuple=reinterpret_cast<tuple4 *>(geometry);
		
		if(!m_bMgr->releaseBuffer(tuple->vbo)||!m_bMgr->releaseBuffer(tuple->ibo))
			m_engine->log("R3DRocket: Cannot release buffer for libRocket.");
		delete tuple;
	}

	bool RenderInterface::LoadTexture(Rocket::Core::TextureHandle& texture_handle,
		                         Rocket::Core::Vector2i& texture_dimensions,
		                         const Rocket::Core::String& source)
	{
		auto tMgr=m_engine->getTextureManager();
		auto text=tMgr->registerColorTexture2D(source.CString());
		if(text){
			texture_dimensions=Rocket::Core::Vector2i(text->getWidth(), text->getHeight());
			texture_handle=reinterpret_cast<Rocket::Core::TextureHandle>(text);
			return true;
		}else
			return false;
	}

	bool RenderInterface::GenerateTexture(Rocket::Core::TextureHandle& texture_handle,
		const Rocket::Core::byte* source,
		const Rocket::Core::Vector2i& source_dimensions)
	{
		auto tMgr=m_engine->getTextureManager();
		auto text=tMgr->registerColorTexture2D(std::string("LIBROCKET_FONT_")+std::to_string(m_tmp_id++), source_dimensions.x, 
			source_dimensions.y, const_cast<Rocket::Core::byte*>(source));
		if(text){
			texture_handle=reinterpret_cast<Rocket::Core::TextureHandle>(text);
			return true;
		}else
			return false;
	}

		// Called by Rocket when it wants to enable or disable scissoring to clip content.
	void RenderInterface::EnableScissorRegion(bool enable)
	{
		m_engine->getRenderer()->enableScissorTest(enable);
	}

		// Called by Rocket when it wants to change the scissor region.
	void RenderInterface::SetScissorRegion(int x, int y, int width, int height)
	{
		m_engine->getRenderer()->setScissorRegion(x, y, width, height);
	}

	void RenderInterface::SetViewport(uint32_t width, uint32_t height)
	{
		m_program->setUniform("viewport", {width, height});
	}
}