#include <r3d/Pipeline/GBuffer.hpp>
#include <r3d/Core/Core.hpp>
#include <r3d/Core/Texture.hpp>
#include <r3d/Core/RenderTarget.hpp>

namespace r3d
{
	GBuffer::GBuffer(Engine *engine, uint32_t width, uint32_t height):
		m_engine(engine), m_width(width), m_height(height)
	{
		// Create framebuffer
		m_fbo = engine->newRenderTarget2D();

		auto tMgr = engine->getTextureManager();

		m_posMap = tMgr->registerColorTexture2D("PositionMap", width, height, PF_BGRF);
		m_diffuseMap = tMgr->registerColorTexture2D("DiffuseMap", width, height, PF_BGRF);
		m_normMap = tMgr->registerColorTexture2D("NormalMap", width, height, PF_BGRF);

		m_posMap->setFilter(F_LINEAR, F_LINEAR);
		m_diffuseMap->setFilter(F_LINEAR, F_LINEAR);
		m_normMap->setFilter(F_LINEAR, F_LINEAR);


		m_depthMap = tMgr->registerDepthTexture2D("DepthMap", width, height, DF_32);
		m_depthMap->setFilter(F_LINEAR, F_LINEAR);		

		r3d::ColorTexture2D *texts[]={m_posMap, m_diffuseMap, m_normMap};
		m_fbo->attachColorTextures(3, texts);
		m_fbo->attachDepthTexture(m_depthMap);
	}

	void GBuffer::beginScene()
	{
		m_fbo->bind();
		m_engine->getRenderer()->clear();
		m_engine->getRenderer()->enableDepthTest(true);
		m_engine->getRenderer()->enableBackfaceCulling(true);
		m_engine->getRenderer()->enableBlending(false);
	}

	void GBuffer::endScene()
	{
		m_fbo->unbind();
	}
}