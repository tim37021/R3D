#ifndef __R3D_CORE_OPENGL_TEXTURE_HPP
#define __R3D_CORE_OPENGL_TEXTURE_HPP

#include <r3d/Core/Texture.hpp>
#include "OpenGLObject.hpp"

namespace r3d
{
	class Image;
	class OpenGLColorTexture2D: public ColorTexture2D, public OpenGLObject
	{
	public:
		OpenGLColorTexture2D(uint32_t, uint32_t, PixelFormat pf);
		OpenGLColorTexture2D(const Image *);

		// ITexture
		virtual void bind(uint32_t channel);
		virtual void bindImage(uint32_t channel, int level, AccessLevel);
		virtual void unbind();
		virtual void *lock();
		virtual void unlock();

		//ColorTexture
		virtual void setWrapping(Wrapping s, Wrapping t);
		virtual void setFilter(Filter min, Filter mag);
		virtual void generateMipmap();

		//Set Border
		virtual void setBorder(const glm::vec4 border) ;

		// must update gpu
		virtual void resize(uint32_t width, uint32_t height);
	private:
		GLenum m_internalFormat;
		GLenum getGLInternelFormat() const;
		void resetGLTexture() const;
	};

	class OpenGLDepthTexture2D: public DepthTexture2D, public OpenGLObject
	{
	public:
		OpenGLDepthTexture2D(uint32_t, uint32_t, DepthFormat df);

		// ITexture
		virtual void bind(uint32_t channel);
		virtual void bindImage(uint32_t channel, int level, AccessLevel);
		virtual void unbind();
		virtual void *lock();
		virtual void unlock();

		//ColorTexture
		virtual void setWrapping(Wrapping s, Wrapping t);
		virtual void setFilter(Filter min, Filter mag);
		virtual void generateMipmap();

		//Set Border
		virtual void setBorder(const glm::vec4 border) ;
		
		// must update gpu
		virtual void resize(uint32_t width, uint32_t height);
	private:
		void resetGLTexture() const;
	};
}

#endif