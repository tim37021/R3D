#include <GL/glew.h>
#include "OpenGLRenderer.hpp"
#include <r3d/Core/Core.hpp>
#include <r3d/Core/VertexArray.hpp>

static GLenum PrimitiveTypeOpenGLMap[]={GL_POINTS, GL_LINE_LOOP, GL_LINES, GL_TRIANGLES};
static GLenum BlendParameterOpenGLMap[]=
{GL_ZERO, GL_ONE, GL_SRC_COLOR, GL_ONE_MINUS_SRC_COLOR, GL_DST_COLOR, GL_ONE_MINUS_DST_COLOR,
	GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_DST_ALPHA,	GL_ONE_MINUS_DST_ALPHA};
static GLenum BlendFunctionOpenGLMap[]={GL_FUNC_ADD, GL_FUNC_SUBTRACT, GL_FUNC_REVERSE_SUBTRACT};
namespace r3d
{
	OpenGLRenderer::OpenGLRenderer(Engine *engine)
		: m_engine(engine)
	{

	}
	void OpenGLRenderer::clear()
	{
		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	}

	void OpenGLRenderer::enableDepthTest(bool value)
	{
		if(value)
			glEnable(GL_DEPTH_TEST);
		else
			glDisable(GL_DEPTH_TEST);
	}

	void OpenGLRenderer::enableBackfaceCulling(bool value)
	{
		if(value)
			glEnable(GL_CULL_FACE);
		else
			glDisable(GL_CULL_FACE);
		glCullFace(GL_BACK);
	}
	void OpenGLRenderer::enableScissorTest(bool enable)
	{
		if(enable)
			glEnable(GL_SCISSOR_TEST);
		else
			glDisable(GL_SCISSOR_TEST);

	}
	void OpenGLRenderer::setScissorRegion(int32_t x, int32_t y, uint32_t width, uint32_t height)
	{
		ContextWindow *window = m_engine->getCurrentContext();
		glScissor(x, window->getHeight() - (y + height), width, height);
	}

	void OpenGLRenderer::enableBlending(bool value, BlendParameter sf, BlendParameter df, BlendFunction bf)
	{
		if(value)
		{
			glEnable(GL_BLEND);
			glBlendFunc(BlendParameterOpenGLMap[sf], BlendParameterOpenGLMap[df]);
			glBlendEquation(BlendFunctionOpenGLMap[bf]);
		}else
			glDisable(GL_BLEND);
	}

	void OpenGLRenderer::drawElements(Program *program, VertexArray *vao, PrimitiveType pt, uint32_t count)
	{
		vao->bind();
		program->use();
		glDrawElements(PrimitiveTypeOpenGLMap[pt], count, GL_UNSIGNED_INT, nullptr);
		vao->unbind();
	}

	void OpenGLRenderer::drawArrays(Program *program, VertexArray *vao, PrimitiveType pt, uint32_t count)
	{
		vao->bind();
		program->use();
		glDrawArrays(PrimitiveTypeOpenGLMap[pt], 0, count);
		vao->unbind();
	}

	void OpenGLRenderer::setViewport(int32_t x, int32_t y, uint32_t width, uint32_t height)
	{
		glViewport(x, y, width, height);
	}
}