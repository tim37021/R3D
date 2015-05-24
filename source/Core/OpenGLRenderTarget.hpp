#ifndef __CORE_OPENGL_RENDERTARGET_H_
#define __CORE_OPENGL_RENDERTARGET_H_

#include <r3d/Core/RenderTarget.hpp>
#include <stdexcept>
#include <cstdint>
#include "OpenGLObject.hpp"

namespace r3d
{

	class FramebufferError: public std::runtime_error
	{
	public:
		explicit FramebufferError(const std::string &s): std::runtime_error(s){}
	};

	class OpenGLRenderTarget2D: public RenderTarget2D, public OpenGLObject
	{
	public:
		explicit OpenGLRenderTarget2D();
		virtual void attachColorTextures(uint32_t n, ColorTexture2D **);
		virtual void attachDepthTexture(DepthTexture2D *);

		virtual void bind();
		virtual void unbind();
	private:
		void checkStatus();
	};
}

#endif