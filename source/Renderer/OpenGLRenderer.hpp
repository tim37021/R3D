#ifndef __R3D_RENDERER_OPENGLRENDERER_HPP_
#define __R3D_RENDERER_OPENGLRENDERER_HPP_	

#include <r3d/Renderer/Renderer.hpp>

namespace r3d
{
	class Engine;
	class OpenGLRenderer: public Renderer
	{
	public:
		OpenGLRenderer(Engine *);
		virtual void clear();
		virtual void setViewport(int32_t, int32_t, uint32_t width, uint32_t height);
		virtual void setFillMode(FillMode);
		virtual void enableDepthTest(bool value);
		virtual void enableFaceCulling(Face, bool value);
		virtual void enableScissorTest(bool enable);
		virtual void setScissorRegion(int32_t, int32_t, uint32_t, uint32_t);
		virtual void enableBlending(bool value, BlendParameter sf, BlendParameter df, BlendFunction bf);
		virtual void drawElements(Program *program, VertexArray *vao, PrimitiveType, uint32_t count);
		virtual void drawArrays(Program *program, VertexArray *vao, PrimitiveType, uint32_t count);
	private:
		Engine *m_engine;
	};
}

#endif