#ifndef __R3D_PIPELINE_DEFERRED_HPP_
#define __R3D_PIPELINE_DEFERRED_HPP_

#include <memory>

namespace r3d
{
	class Engine;
	class ContextWindow;
	class Renderer;
	class GBuffer;
	class Program;
	class VertexArray;
	class PointLight;
	class SpotLight;
	class Camera;
	class SSAO;
	class RenderTarget2D;

	typedef std::shared_ptr<RenderTarget2D> RenderTarget2DPtr;
	typedef std::shared_ptr<Program> ProgramPtr;

	class Deferred
	{
	public:
		Deferred(Engine *, ContextWindow *);
		~Deferred();
		void run();
	private:
		Engine *m_engine;
		ContextWindow *m_cw;
		Renderer *m_renderer;
		Camera *m_lightCamera;
		RenderTarget2DPtr m_renderTarget;

		GBuffer *m_gBuffer;

		SSAO *m_ssao;

		// point light program
		ProgramPtr m_programPL, m_programSL, m_programA, m_programDepth;

		// attributeless vao
		VertexArray *m_vao;

		void litPointLight(PointLight *light);
		void litSpotLight(SpotLight *);
		void litAmbientLight();
		void beginLightPass();
		void endLightPass();

		void renderMaterial(Camera *cam);
		void renderDepth(Camera *cam);

		void prepareProgramInput();
		void prepareViewRelativeUniform(ProgramPtr program, Camera *cam);
	};

}

#endif