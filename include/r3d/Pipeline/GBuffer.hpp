#ifndef __R3D_PIPELINE_GBUFFER_HPP_
#define __R3D_PIPELINE_GBUFFER_HPP_

#include <memory>

namespace r3d
{
	class RenderTarget2D;
	class ColorTexture2D;
	class DepthTexture2D;
	class Engine;

	typedef std::shared_ptr<RenderTarget2D> RenderTarget2DPtr;

	class GBuffer
	{
	public:
		GBuffer(Engine *engine, uint32_t width, uint32_t height);

		void beginScene();
		void endScene();

		ColorTexture2D *getPositionMap() { return m_posMap; }
		ColorTexture2D *getDiffuseMap() { return m_diffuseMap; }
		ColorTexture2D *getNormalMap() { return m_normMap; }
		ColorTexture2D *getSpecularMap() { return m_specMap; }
	private:
		Engine *m_engine;

		RenderTarget2DPtr m_fbo;

		ColorTexture2D *m_posMap, *m_diffuseMap, *m_normMap, *m_specMap;

		DepthTexture2D *m_depthMap;

		uint32_t m_width, m_height;
	};


}

#endif