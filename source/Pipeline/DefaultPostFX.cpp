#include "DefaultPostFX.hpp"
#include <r3d/Core/Core.hpp>
#include <r3d/Shader/Program.hpp>
#include <r3d/Core/Texture.hpp>

static const char *vertex_shader=
	"#version 330\n"
	"void main(){}";

static const char *geometry_shader=
	"#version 330\n"
	"layout(points) in;\n"
	"layout(triangle_strip, max_vertices=6) out;\n"
	"void main(){"
	"gl_Position=vec4(-1, -1, 0, 1);\n"
	"EmitVertex();\n"
	"gl_Position=vec4(1, -1, 0, 1);\n"
	"EmitVertex();\n"
	"gl_Position=vec4(1, 1, 0, 1);\n"
	"EmitVertex();\n"
	"EndPrimitive();\n"
	"gl_Position=vec4(-1, -1, 0, 1);\n"
	"EmitVertex();\n"
	"gl_Position=vec4(1, 1, 0, 1);\n"
	"EmitVertex();\n"
	"gl_Position=vec4(-1, 1, 0, 1);\n"
	"EmitVertex();\n"
	"EndPrimitive();\n}"
	"\n";

namespace r3d
{
	void EffectDeleter(PostEffect *effect)
	{
		delete effect;
	}

	ShaderPostEffect::ShaderPostEffect(PostFX *pfx, const std::string &name, 
		Texture2D *input, const char *fragment_shader):
		PostEffect(pfx, name, input)
	{
		m_program=MakeShaderProgram(pfx->getEngine(), vertex_shader, geometry_shader, fragment_shader);
		m_cw=m_pfx->getContextWindow();
		
		ColorTexture2D *text = m_cw->getTextureManager()->registerColorTexture2D(name, m_cw->getWidth()/2, m_cw->getHeight()/2, PF_BGR);

		// Prepare render target
		m_renderTarget=pfx->getEngine()->newRenderTarget2D();
		ColorTexture2D *texts[]={text};
		m_renderTarget->attachColorTextures(1, texts);
		m_textResult=text;

		m_vao=m_cw->getVertexArrayManager()->registerVertexArray("ATTRIBUTELESS");

	}

	void ShaderPostEffect::run()
	{
		m_renderTarget->bind();
		m_engine->getRenderer()->setViewport(0, 0, m_cw->getWidth()/2, m_cw->getHeight()/2);

		m_text->bind(0);
		m_program->setUniform("viewport", {m_cw->getWidth()/2, m_cw->getHeight()/2});
		m_program->setUniform("text", 0);
		m_engine->getRenderer()->drawArrays(m_program.get(), m_vao, PT_POINTS, 1);

		m_renderTarget->unbind();
		m_engine->getRenderer()->setViewport(0, 0, m_cw->getWidth(), m_cw->getHeight());
	}
}