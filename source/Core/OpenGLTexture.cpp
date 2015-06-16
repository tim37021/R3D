#include <GL/glew.h>
#include "OpenGLTexture.hpp"
#include <r3d/Utils/Image.hpp>
#include <cstring>

#define PUSHSTATE() GLint restoreId; glGetIntegerv( GL_TEXTURE_BINDING_2D, &restoreId );
#define POPSTATE() glBindTexture( GL_TEXTURE_2D, restoreId );

static GLenum PixelFormatOpenGLMap[]={GL_R, GL_R, GL_R, GL_RGB, GL_BGR, GL_RGBA, GL_BGRA, GL_RGB, GL_BGR, GL_RGBA, GL_BGRA};
static GLenum WrappingOpenGLMap[]={GL_CLAMP_TO_EDGE, GL_CLAMP_TO_BORDER, GL_REPEAT, GL_MIRRORED_REPEAT};
static GLenum FilterOpenGLMap[]={GL_NEAREST, GL_LINEAR, GL_NEAREST_MIPMAP_NEAREST,
			GL_LINEAR_MIPMAP_NEAREST, GL_NEAREST_MIPMAP_LINEAR, GL_LINEAR_MIPMAP_LINEAR};
static GLenum AccessLevelOpenGLMap[]={GL_READ_ONLY, GL_WRITE_ONLY, GL_READ_WRITE};

namespace r3d
{
	OpenGLColorTexture2D::OpenGLColorTexture2D(uint32_t width, uint32_t height, PixelFormat pf)
		: ColorTexture2D(width, height, pf), OpenGLObject(glGenTextures, glDeleteTextures)
	{
		m_internalFormat=getGLInternelFormat();
		resetGLTexture();
	}

	OpenGLColorTexture2D::OpenGLColorTexture2D(const Image *image)
		: ColorTexture2D(image->GetWidth(), image->GetHeight(), PF_RGBA), OpenGLObject(glGenTextures, glDeleteTextures)
	{
		memcpy(m_data.get(), image->GetPixels(), image->GetWidth()*image->GetHeight()*4);
		m_internalFormat=image->HasAlpha()? GL_RGBA8: GL_RGB8;
		resetGLTexture();
	}

	void OpenGLColorTexture2D::bind(uint32_t channel)
	{
		glActiveTexture(GL_TEXTURE0+channel);
		glBindTexture(GL_TEXTURE_2D, getID());
	}

	void OpenGLColorTexture2D::bindImage(uint32_t channel, int level, AccessLevel al)
	{
		glBindImageTexture(channel, getID(), level, GL_FALSE, 0, AccessLevelOpenGLMap[al], m_internalFormat);
	}

	void OpenGLColorTexture2D::unbind()
	{
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	void *OpenGLColorTexture2D::lock()
	{
		return m_data.get();
	}

	void OpenGLColorTexture2D::unlock()
	{
		PUSHSTATE();

		glBindTexture(GL_TEXTURE_2D, getID());
		GLenum gltype;
		PixelFormat pf=getPixelFormat();
		if(pf==PF_RGBF||pf==PF_BGRF||pf==PF_RGBAF||pf==PF_BGRAF)
			gltype=GL_FLOAT;
		else
			gltype=GL_UNSIGNED_BYTE;
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, getWidth(), getHeight(),
			PixelFormatOpenGLMap[getPixelFormat()], gltype, m_data.get());

		glGenerateMipmap(GL_TEXTURE_2D);

		POPSTATE();
	}

	void OpenGLColorTexture2D::generateMipmap()
	{
		PUSHSTATE();

		glBindTexture(GL_TEXTURE_2D, getID());
		glGenerateMipmap(GL_TEXTURE_2D);

		POPSTATE();
	}

	GLenum OpenGLColorTexture2D::getGLInternelFormat() const
	{
		switch(getPixelFormat())
		{
			case PF_R:
				return GL_R8UI;
			case PF_R16:
				return GL_R16UI;
			case PF_R32:
				return GL_R32UI;
			case PF_RGB:
			case PF_BGR:
				return GL_RGB8;
			case PF_RGBA:
			case PF_BGRA:
				return GL_RGBA8;
			case PF_RGBF:
			case PF_BGRF:
				return GL_RGB32F;
			case PF_RGBAF:
			case PF_BGRAF:
				return GL_RGBA32F;
			default:
				return GL_RGB8;
		}
	}

	void OpenGLColorTexture2D::resize(uint32_t width, uint32_t height)
	{
		Texture2D::resize(width, height);
		resetGLTexture();
	}
	
	void OpenGLColorTexture2D::resetGLTexture() const
	{
		PUSHSTATE();
		glBindTexture(GL_TEXTURE_2D, getID());
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		GLenum gltype;
		PixelFormat pf=getPixelFormat();
		if(pf==PF_RGBF||pf==PF_BGRF||pf==PF_RGBAF||pf==PF_BGRAF)
			gltype=GL_FLOAT;
		else
			gltype=GL_UNSIGNED_BYTE;

		glTexImage2D(GL_TEXTURE_2D, 0, m_internalFormat, getWidth(), getHeight(), 0, PixelFormatOpenGLMap[pf], gltype, m_data.get());
		
		glGenerateMipmap( GL_TEXTURE_2D );
		POPSTATE();
	}

	void OpenGLColorTexture2D::setWrapping(Wrapping s, Wrapping t)
	{
		PUSHSTATE();

		glBindTexture( GL_TEXTURE_2D, getID());
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, WrappingOpenGLMap[s]);
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, WrappingOpenGLMap[t]);

		POPSTATE();
	}

	void OpenGLColorTexture2D::setFilter(Filter min, Filter mag)
	{
		PUSHSTATE();

		glBindTexture( GL_TEXTURE_2D, getID() );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, FilterOpenGLMap[min]);
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, FilterOpenGLMap[mag]);

		POPSTATE();
	}

	OpenGLDepthTexture2D::OpenGLDepthTexture2D(uint32_t width, uint32_t height, DepthFormat df):
		DepthTexture2D(width, height, df), OpenGLObject(glGenTextures, glDeleteTextures)
	{
		resetGLTexture();
	}

	void OpenGLDepthTexture2D::bind(uint32_t channel)
	{
		glActiveTexture(GL_TEXTURE0+channel);
		glBindTexture(GL_TEXTURE_2D, getID());
	}

	void OpenGLDepthTexture2D::bindImage(uint32_t channel, int level, AccessLevel al)
	{
		if(getDepthFormat()==DF_32)
			glBindImageTexture(channel, getID(), level, GL_FALSE, 0, AccessLevelOpenGLMap[al], GL_R32F);
	}

	void OpenGLDepthTexture2D::unbind()
	{
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	void *OpenGLDepthTexture2D::lock()
	{
		return m_data.get();
	}

	void OpenGLDepthTexture2D::unlock()
	{
		PUSHSTATE();

		glBindTexture(GL_TEXTURE_2D, getID());
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, getWidth(), getHeight(),
			GL_DEPTH_COMPONENT, GL_FLOAT, m_data.get());
		POPSTATE();
	}

	void OpenGLDepthTexture2D::resetGLTexture() const
	{
		PUSHSTATE();

		glBindTexture(GL_TEXTURE_2D, getID());
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

		if(getDepthFormat()==DF_24)
			glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, getWidth(), getHeight(), 0, GL_DEPTH_COMPONENT, GL_FLOAT, m_data.get());
		else if(getDepthFormat()==DF_32)
			glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, getWidth(), getHeight(), 0, GL_DEPTH_COMPONENT, GL_FLOAT, m_data.get());

		
		POPSTATE();
	}

	void OpenGLDepthTexture2D::resize(uint32_t width, uint32_t height)
	{
		Texture2D::resize(width, height);
		resetGLTexture();
	}

	void OpenGLDepthTexture2D::setWrapping(Wrapping s, Wrapping t)
	{
		PUSHSTATE();

		glBindTexture( GL_TEXTURE_2D, getID());
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, WrappingOpenGLMap[s]);
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, WrappingOpenGLMap[t]);

		POPSTATE();
	}

	void OpenGLDepthTexture2D::setFilter(Filter min, Filter mag)
	{
		PUSHSTATE();

		glBindTexture( GL_TEXTURE_2D, getID() );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, FilterOpenGLMap[min]);
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, FilterOpenGLMap[mag]);

		POPSTATE();
	}

}
		

		
		