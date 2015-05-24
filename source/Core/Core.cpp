#include <iostream>

#include <r3d/Core/Core.hpp>
#include <GLFW/glfw3.h>
#include <r3d/Shader/OpenGLProgram.hpp>
#include <r3d/Shader/OpenGLShader.hpp>
#include <r3d/Utils/Image.hpp>

#include "OpenGLTexture.hpp"
#include "../Window/GLFW3ContextWindow.hpp"
#include "../Core/OpenGLRenderTarget.hpp"
#include "../Core/OpenGLBuffer.hpp"
#include "../Core/OpenGLVertexArray.hpp"
#include "../Renderer/OpenGLRenderer.hpp"

namespace r3d
{
	Engine::Engine(RenderAPI ra):
		m_ra(ra)
	{
		glfwInit();

		switch(ra)
		{
			case RA_OPENGL_3_3:
			case RA_OPENGL_2_1:
				m_renderer=new OpenGLRenderer(this); break;
			case RA_SOFTWARE:
				log("Software renderer is not yet implemented.");
		}
	}

	Engine::~Engine()
	{
		delete m_renderer;
		for(auto &element: m_MgrMap)
		{
			// delete resource
			element.second.freeAll();
			// delete window
			delete element.first;
		}
		glfwTerminate();
	}

	double Engine::getTime() const
	{
		return glfwGetTime();
	}

	void Engine::log(const std::string &msg) const
	{
		std::cerr<<msg<<std::endl;
	}

	ContextWindow *Engine::newContextWindow(uint32_t width, uint32_t height, const std::string &title)
	{
		int major=2, minor=1;
		if(m_ra==RA_OPENGL_3_3)
		{
			major=3;
			minor=3;
		}
		ContextWindow::SetHint(CO_GL_VERSION_MAJOR, major);
		ContextWindow::SetHint(CO_GL_VERSION_MINOR, minor);
		
		ContextWindow *newed = new GLFW3ContextWindow(width, height, title);

		//Construct context buffer managers
		Managers mgrs;
		mgrs.bMgr=new BufferManager(this, newed);
		mgrs.vaoMgr=new VertexArrayManager(this, newed);
		mgrs.tMgr = new TextureManager(this, newed);
		mgrs.sMgr = new SceneManager(this);
		m_MgrMap[newed] = mgrs;
		newed->m_managers=&m_MgrMap[newed];

		return newed;
	}

	ColorTexture2D *Engine::newColorTexture2D(uint32_t width, uint32_t height, PixelFormat pf) const
	{
		switch(m_ra)
		{
			case RA_OPENGL_3_3:
			case RA_OPENGL_2_1:
				return new OpenGLColorTexture2D(width, height, pf);
				break;
			case RA_SOFTWARE:
				log("TODO: 2D Texture for software renderer");
		}
		return {nullptr};
	}

	ColorTexture2D *Engine::newColorTexture2D(const std::string &filename) const
	{
		try{
			Image image(filename);
			switch(m_ra)
			{
				case RA_OPENGL_3_3:
				case RA_OPENGL_2_1:
					return new OpenGLColorTexture2D(&image);
					break;
				case RA_SOFTWARE:
					log("TODO: 2D Texture for software renderer");
			}
		}catch(...)
		{
			log(std::string("Core: ")+filename+" Format is not supported!");
			return nullptr;
		}

		return {nullptr};
	}


	ColorTexture2D *Engine::newColorTexture2D(uint16_t width, uint16_t height, uint8_t *pixels) const
	{
		try{
			Image image(width, height, pixels);
			switch(m_ra)
			{
				case RA_OPENGL_3_3:
				case RA_OPENGL_2_1:
					return new OpenGLColorTexture2D(&image);
					break;
				case RA_SOFTWARE:
					log("TODO: 2D Texture for software renderer");
			}
		}catch(...)
		{
			log(std::string("Core: Cannot construct texture from byte data"));
			return nullptr;
		}

		return {nullptr};
	}

	DepthTexture2D *Engine::newDepthTexture2D(uint32_t width, uint32_t height, DepthFormat df) const
	{
		switch(m_ra)
		{
			case RA_OPENGL_3_3:
			case RA_OPENGL_2_1:
				return new OpenGLDepthTexture2D(width, height, df);
				break;
			case RA_SOFTWARE:
				log("TODO: 2D Texture for software renderer");
		}
		return {nullptr};
	}

	std::shared_ptr<RenderTarget2D> Engine::newRenderTarget2D() const
	{
		switch(m_ra)
		{
			case RA_OPENGL_3_3:
			case RA_OPENGL_2_1:
				return std::shared_ptr<RenderTarget2D>(new OpenGLRenderTarget2D());
				break;
			case RA_SOFTWARE:
				log("TODO: render target for software renderer");
		}
		return {nullptr};
	}

	VertexArray *Engine::newVertexArray() const
	{
		switch(m_ra)
		{
			case RA_OPENGL_3_3:
			case RA_OPENGL_2_1:
				return new OpenGLVertexArray();
				break;
			case RA_SOFTWARE:
				log("TODO: vertex array for software renderer");
		}
		return {nullptr};
	}

	Buffer *Engine::newIndexBuffer(const std::vector<uint32_t> &indices, BufferUsage bu) const
	{
		switch(m_ra)
		{
			case RA_OPENGL_3_3:
			case RA_OPENGL_2_1:
				return new OpenGLIndexBuffer(indices, bu);
				break;
			case RA_SOFTWARE:
				log("TODO: index array for software renderer");
		}
		return {nullptr};
	}
	Buffer *Engine::newVertexBuffer(const void *data, uint32_t n, BufferUsage bu) const
	{
		switch(m_ra)
		{
			case RA_OPENGL_3_3:
			case RA_OPENGL_2_1:
				return new OpenGLVertexBuffer(data, n, bu);
				break;
			case RA_SOFTWARE:
				log("TODO: vertex buffer for software renderer");
		}
		return {nullptr};
	}

	std::shared_ptr<Shader> Engine::newShader(ShaderType st) const
	{
		switch(m_ra)
		{
			case RA_OPENGL_3_3:
			case RA_OPENGL_2_1:
				return std::shared_ptr<Shader>(new OpenGLShader(st));
				break;
			case RA_SOFTWARE:
				log("Software renderer cannot use shader yet");
		}
		return {nullptr};
	}

	std::shared_ptr<Program> Engine::newProgram() const
	{
		switch(m_ra)
		{
			case RA_OPENGL_3_3:
			case RA_OPENGL_2_1:
				return std::shared_ptr<Program>(new OpenGLProgram());
				break;
			case RA_SOFTWARE:
				log("Software renderer cannot use program yet");
		}
		return {nullptr};
	}

	SceneManager *Engine::getSceneManager(ContextWindow *window)
	{
		auto it=m_MgrMap.find(window);
		if(it!=m_MgrMap.cend())
		{
			return it->second.sMgr;
		}
			return nullptr;
	}

	BufferManager *Engine::getBufferManager(ContextWindow *window)
	{
		auto it=m_MgrMap.find(window);
		if(it!=m_MgrMap.cend())
		{
			return it->second.bMgr;
		}
			return nullptr;
	}

	TextureManager *Engine::getTextureManager(ContextWindow *window)
	{
		auto it=m_MgrMap.find(window);
		if(it!=m_MgrMap.cend())
		{
			return it->second.tMgr;
		}
			return nullptr;
	}

	TextureManager *Engine::getTextureManager()
	{
		return findCurrentContext().second.tMgr;
	}

	VertexArrayManager *Engine::getVertexArrayManager(ContextWindow *window)
	{
		auto it=m_MgrMap.find(window);
		if(it!=m_MgrMap.cend())
		{
			return it->second.vaoMgr;
		}
			return nullptr;
	}

	BufferManager *Engine::getBufferManager()
	{
		auto element=findCurrentContext();
		return element.second.bMgr;
	}

	VertexArrayManager *Engine::getVertexArrayManager()
	{
		auto element=findCurrentContext();
		return element.second.vaoMgr;
	}

	SceneManager *Engine::getSceneManager()
	{
		auto element=findCurrentContext();
		return element.second.sMgr;
	}

	ContextWindow *Engine::getCurrentContext() const
	{
		auto element=findCurrentContext();
		return element.first;
	}

	std::pair<ContextWindow *, Managers> Engine::findCurrentContext() const
	{
		GLFWwindow *window=glfwGetCurrentContext();
		for(auto &element: m_MgrMap)
		{
			if(((GLFW3ContextWindow *)element.first)->getGLFW3Handle()==window)
				return element;
		}
		return std::make_pair<ContextWindow *, Managers>(nullptr, Managers());
	}
}