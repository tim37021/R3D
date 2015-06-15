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
	class Camera;
	class SSAO;

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

		GBuffer *m_gBuffer;

		SSAO *m_ssao;

		// point light program
		ProgramPtr m_programPL, m_programA;

		// attributeless vao
		VertexArray *m_vao;

		void litPointLight(PointLight *light);
		void litAmbientLight();
		void beginLightPass();
		void endLightPass();

		void renderMaterial(Camera *cam);

		void prepareProgramInput();
		void prepareViewRelativeUniform(ProgramPtr program, Camera *cam);
	};

}

#endif