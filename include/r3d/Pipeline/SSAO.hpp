#ifndef __R3D_PIPELINE_SSAO_HPP_
#define __R3D_PIPELINE_SSAO_HPP_

#include <memory>

namespace r3d
{
	class Engine;
	class ContextWindow;
	class Texture2D;
	class ColorTexture2D;
	class RenderTarget2D;
	class Program;
	class VertexArray;
	class Camera;

	typedef std::shared_ptr<RenderTarget2D> RenderTarget2DPtr;
	typedef std::shared_ptr<Program> ProgramPtr;

	class SSAO
	{
	public:
		SSAO(Engine *engine, ContextWindow *cw, Texture2D *depthMap, Texture2D *normMap);
		void update(Camera *cam);
	private:
		Engine *m_engine;
		ContextWindow *m_cw;
		RenderTarget2DPtr m_fbo;
		Texture2D *m_depthMap, *m_normMap;
		ColorTexture2D *m_ambientMap, *m_noiseMap;

		ProgramPtr m_programSSAO;
		VertexArray *m_vao;
	};
}

#endif