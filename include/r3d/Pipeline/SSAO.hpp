#ifndef __R3D_PIPELINE_SSAO_HPP_
#define __R3D_PIPELINE_SSAO_HPP_

#include <memory>
#include <vector>
#include <glm/glm.hpp>

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
		SSAO(Engine *engine, ContextWindow *cw, Texture2D *posMap, Texture2D *depthMap, Texture2D *normMap);
		void update(Camera *cam);

		void setSampleRadius(int32_t r);

		ColorTexture2D *getAmbientMap() const
		{ return m_ambientMap; }

		ColorTexture2D *getBlurredAmbientMap() const
		{ return m_ambientMapBlurred; }
	private:
		Engine *m_engine;
		ContextWindow *m_cw;
		RenderTarget2DPtr m_fbo, m_fboBlur;
		Texture2D *m_posMap, *m_depthMap, *m_normMap;
		ColorTexture2D *m_ambientMap, *m_ambientMapBlurred, *m_noiseMap;

		ProgramPtr m_programSSAO, m_programBlur;
		VertexArray *m_vao;

		std::vector<glm::vec3> m_kernel;

		// generate sample points within unit hemisphere (+z)
		void generateKernel(int num_kernels);
		void uploadKernel();
	};
}

#endif