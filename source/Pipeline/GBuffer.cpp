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
		m_diffuseMap = tMgr->registerColorTexture2D("DiffuseMap", width, height, PF_BGR);
		m_normMap = tMgr->registerColorTexture2D("NormalMap", width, height, PF_BGRF);
		m_specMap = tMgr->registerColorTexture2D("SpecularMap", width, height, PF_BGR);
		m_objMap = tMgr->registerColorTexture2D("ObjectMap", width, height, PF_OBJECT_R);

		m_posMap->setWrapping(W_CLAMP_TO_BORDER, W_CLAMP_TO_BORDER);
		m_diffuseMap->setWrapping(W_CLAMP_TO_BORDER, W_CLAMP_TO_BORDER);
		m_normMap->setWrapping(W_CLAMP_TO_BORDER, W_CLAMP_TO_BORDER);
		m_specMap->setWrapping(W_CLAMP_TO_BORDER, W_CLAMP_TO_BORDER);
		m_objMap->setWrapping(W_CLAMP_TO_BORDER, W_CLAMP_TO_BORDER);


		m_depthMap = tMgr->registerDepthTexture2D("DepthMap", width, height, DF_32);	
		m_depthMap->setWrapping(W_CLAMP_TO_BORDER, W_CLAMP_TO_BORDER);

		ColorTexture2D *texts[]={m_posMap, m_diffuseMap, m_normMap, m_specMap, m_objMap};
		m_fbo->attachColorTextures(5, texts);
		m_fbo->attachDepthTexture(m_depthMap);
	}

	void GBuffer::beginScene()
	{
		m_fbo->bind();
		m_engine->getRenderer()->clear();
		m_engine->getRenderer()->enableDepthTest(true);
		m_engine->getRenderer()->enableFaceCulling(F_BACK, true);
		m_engine->getRenderer()->enableBlending(false);
	}

	void GBuffer::endScene()
	{
		m_fbo->unbind();
	}
}