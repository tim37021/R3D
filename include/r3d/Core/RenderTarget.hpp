#ifndef __R3D_CORE_RENDERTARGET_HPP_
#define __R3D_CORE_RENDERTARGET_HPP_

#include <vector>
#include <cstdint>

namespace r3d
{
	class ColorTexture2D;
	class DepthTexture2D;

	class RenderTarget2D
	{
	public:
		virtual ~RenderTarget2D(){}
		virtual void attachColorTextures(uint32_t n, ColorTexture2D **)=0;
		virtual void attachDepthTexture(DepthTexture2D *)=0;

		virtual void bind()=0;
		virtual void unbind()=0;

	protected:
		std::vector<ColorTexture2D *> m_colorTextures;
		DepthTexture2D *m_stencilDepthTexture;
	};
}

#endif