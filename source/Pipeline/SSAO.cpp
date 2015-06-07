#include <r3d/Pipeline/SSAO.hpp>
#include <r3d/Core/Core.hpp>
#include <r3d/Window/ContextWindow.hpp>
#include <r3d/Core/Texture.hpp>
#include <r3d/Core/RenderTarget.hpp>
#include <r3d/Renderer/Renderer.hpp>
#include <r3d/Camera/Camera.hpp>

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

static const char *fragment_shader=
	"#version 330\n"
	"uniform sampler2D depthMap;"
	"uniform sampler2D normMap;"
	"uniform vec2 viewport;"
	"uniform mat4 view;"
	"out vec4 color;"
	"void main(){"
	"vec2 vTexCoord=gl_FragCoord.xy/viewport;"
	"vec4 normView=view*texture(normMap, vTexCoord);"
	"color=normView;"
	"}\n";

namespace r3d
{
	SSAO::SSAO(Engine *engine, ContextWindow *cw, Texture2D *depthMap, Texture2D *normMap):
		m_engine(engine), m_cw(cw), m_depthMap(depthMap), m_normMap(normMap)
	{
		m_programSSAO=MakeShaderProgram(engine, vertex_shader, geometry_shader, fragment_shader);

		auto tMgr = cw->getTextureManager();
		// Load needed noise texture
		m_noiseMap = tMgr->registerColorTexture2D("noise.png");

		m_ambientMap = tMgr->registerColorTexture2D("SSAOMap", cw->getWidth(), cw->getHeight(), PF_BGR);
		m_ambientMap->setFilter(F_LINEAR, F_LINEAR);
		m_ambientMap->setWrapping(W_CLAMP_TO_BORDER, W_CLAMP_TO_BORDER);

		m_fbo = engine->newRenderTarget2D();
		ColorTexture2D *texts[]={m_ambientMap};
		m_fbo->attachColorTextures(1, texts);

		m_vao = cw->getVertexArrayManager()->registerVertexArray("ATTRIBUTELESS");
	}

	void SSAO::update(Camera *cam)
	{
		glm::mat4 pureLookAt=cam->getVMatrix();
		pureLookAt[3][0]=0;
		pureLookAt[3][1]=0;
		pureLookAt[3][2]=0;
		m_fbo->bind();
		m_programSSAO->use();
		m_programSSAO->setUniform("viewport", {m_cw->getWidth(), m_cw->getHeight()});
		m_programSSAO->setUniform("view", pureLookAt);
		m_depthMap->bind(0);
		m_normMap->bind(1);
		m_programSSAO->setUniform("depthMap", 0);
		m_programSSAO->setUniform("normMap", 1);
		m_engine->getRenderer()->clear();
		m_engine->getRenderer()->drawArrays(m_programSSAO.get(), m_vao, PT_POINTS, 1);
		m_fbo->unbind();
	}
}