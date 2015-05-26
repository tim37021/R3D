#include <r3d/Pipeline/PostFX.hpp>
#include <r3d/Core/Core.hpp>
#include <r3d/Core/VertexArray.hpp>
#include <r3d/Window/ContextWindow.hpp>
#include "DefaultPostFX.hpp"
#include <map>
#include <string>
#include <fstream>
#include <streambuf>

static const char *commom_vertex_shader=
	"#version 330\n"
	"layout(location=0) in vec3 pos;\n"
	"void main(){\n"
	"gl_Position=vec4(pos, 1.0);"
	"}\n";

static std::string readfile(const char *filename)
{
std::ifstream t(filename);
return std::string((std::istreambuf_iterator<char>(t)),
                 std::istreambuf_iterator<char>());
}

namespace r3d
{
	static std::map<std::string, PostEffectInstancer> RegisteredEffects;

	PostEffect::PostEffect(PostFX *pfx, const std::string &effectName):
		m_pfx(pfx), m_effectName(effectName), m_engine(pfx->getEngine()),
		m_cw(pfx->getContextWindow())
	{

	}

	PostFX::PostFX(Engine *engine, ContextWindow *cw):
		m_engine(engine), m_cw(cw)
	{
		std::vector<glm::vec3> vertices=
		{glm::vec3(-1, -1, 0),glm::vec3(1, -1, 0),glm::vec3(1, 1, 0), glm::vec3(-1, 1, 0)};
	 
	 	auto bMgr=engine->getBufferManager();
	 	auto aMgr=engine->getVertexArrayManager();

		auto vbo=bMgr->registerVertexBuffer("POSTFX_QUAD", &vertices[0], vertices.size()*3*sizeof(float), r3d::BU_STATIC_DRAW);
		auto ibo=bMgr->registerIndexBuffer("POSTFX_QUAD", {0, 1, 2, 0, 2, 3}, r3d::BU_STATIC_DRAW);

		m_vao=aMgr->registerVertexArray("POSTFX_QUAD");
		m_vao->bindVertexBuffer(vbo);
		m_vao->bindIndexBuffer(ibo);

		AttribPointer vertAtt(RT_FLOAT, 3, 0, 0);
		m_vao->enableAttribArray(0, vertAtt);

		m_fbosource=engine->newRenderTarget2D();
		auto colorText=cw->getTextureManager()->registerColorTexture2D("LightedMap", cw->getWidth(), cw->getHeight(), PF_BGRF);
		colorText->setFilter(F_LINEAR, F_LINEAR);
		ColorTexture2D *texts[]={colorText};
		m_fbosource->attachColorTextures(1, texts);
	}

	PostFX::~PostFX()
	{
		clear();
	}

	void PostFX::beginSource()
	{
		m_fbosource->bind();
	}

	void PostFX::endSource()
	{
		m_fbosource->unbind();
	}

	void PostFX::runAll()
	{
		for(auto effect: effects)
			effect->run();
	}

	void PostFX::pushEffect(const std::string &effectName)
	{
		auto it=RegisteredEffects.begin();
		if((it=RegisteredEffects.find(effectName))!=RegisteredEffects.end())
			effects.push_back(it->second.newPostEffect(this));
		else
			effects.push_back(new ShaderPostEffect(this, effectName, commom_vertex_shader, readfile((std::string("./shaders/")+effectName+".fs").c_str()).c_str()));
	}

	void PostFX::clear()
	{
		for(auto effect: effects)
		{
			auto it=RegisteredEffects.begin();
			if((it=RegisteredEffects.find(effect->getName()))!=RegisteredEffects.end())
				it->second.freePostEffect(effect);
			else
				delete effect;
		}
	}

	void PostFX::Initialise()
	{
	}

	void PostFXFactory::RegisterEffect(const std::string &name, PostEffectInstancer instancer)
	{
		RegisteredEffects[name]=instancer;
	}
}