#include <GL/glew.h>
#include "OpenGLRenderTarget.hpp"
#include "OpenGLTexture.hpp"

namespace r3d
{
	OpenGLRenderTarget2D::OpenGLRenderTarget2D()
		:OpenGLObject(glGenFramebuffers, glDeleteFramebuffers)
	{
	}

	void OpenGLRenderTarget2D::attachColorTextures(uint32_t n, ColorTexture2D **texts)
	{
		bind();
		m_colorTextures.resize(n);
		std::vector<GLenum> DrawBuffers;
		for(uint32_t i=0; i<n; i++)
		{
			m_colorTextures[i]=texts[i];
			DrawBuffers.push_back(GL_COLOR_ATTACHMENT0+i);
			glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0+i, GL_TEXTURE_2D, 
				((OpenGLColorTexture2D *)texts[i])->getID(), 0);
		}

		glDrawBuffers(DrawBuffers.size(), &DrawBuffers[0]);

		checkStatus();

		unbind();
	}

	void OpenGLRenderTarget2D::attachDepthTexture(DepthTexture2D *text)
	{
		bind();
		glFramebufferTexture2D(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, 
			((OpenGLDepthTexture2D *)text)->getID(), 0);
		m_stencilDepthTexture=text;
		//glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, rboId);
		checkStatus();
		unbind();
	}

	void OpenGLRenderTarget2D::bind()
	{
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, getID());
	}

	void OpenGLRenderTarget2D::unbind()
	{
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	}

	void OpenGLRenderTarget2D::checkStatus()
	{
		GLenum Status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

		if (Status != GL_FRAMEBUFFER_COMPLETE)
			throw FramebufferError("Cannot setup OpenGL Framebuffer");
	}
}