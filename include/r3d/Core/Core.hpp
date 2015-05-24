#ifndef __CORE_CORE_HPP_
#define __CORE_CORE_HPP_

#include <GL/glew.h>
#include <memory>
#include <stdexcept>
#include <list>
#include <map>
#include <string>

#include <r3d/Core/Texture.hpp>
#include <r3d/Core/RenderTarget.hpp>
#include <r3d/Core/VertexArray.hpp>
#include <r3d/Core/Buffer.hpp>
#include <r3d/Window/ContextWindow.hpp>
#include <r3d/Shader/Shader.hpp>
#include <r3d/Shader/Program.hpp>
#include <r3d/Manager/Managers.hpp>
#include <r3d/Renderer/Renderer.hpp>
namespace r3d
{
	class NotSupportedError: public std::runtime_error
	{
	public:
		explicit NotSupportedError(const std::string &what_): std::runtime_error(what_){}
	};

	class NotImplemented: public std::runtime_error
	{
	public:
		explicit NotImplemented(const std::string &what_): std::runtime_error(what_){}
	};

	enum RenderAPI
	{
		RA_OPENGL_2_1,
		RA_OPENGL_3_3,
		RA_SOFTWARE
	};

	class Renderer;

	class Engine
	{
		friend class BufferManager;
		friend class VertexArrayManager;
		friend class ContextWindow;
		friend class TextureManager;
	public:
		Engine(RenderAPI ra);
		~Engine();

		double getTime() const;

		ContextWindow *newContextWindow(uint32_t width, uint32_t height, const std::string &title);

		std::shared_ptr<RenderTarget2D> newRenderTarget2D() const;

		std::shared_ptr<Shader> newShader(ShaderType st) const;
		std::shared_ptr<Program> newProgram() const;

		BufferManager *getBufferManager(ContextWindow *window);
		VertexArrayManager *getVertexArrayManager(ContextWindow *window);
		SceneManager *getSceneManager(ContextWindow *window);
		TextureManager *getTextureManager(ContextWindow *window);
		BufferManager *getBufferManager();
		VertexArrayManager *getVertexArrayManager();
		SceneManager *getSceneManager();
		TextureManager *getTextureManager();

		Renderer *getRenderer() const { return m_renderer; }

		ContextWindow *getCurrentContext() const;

		void log(const std::string &msg) const;
	private:
		RenderAPI m_ra;
		std::map<ContextWindow *, Managers> m_MgrMap;
		Renderer *m_renderer;

		// Managed object creation function only available to friends
		Buffer *newIndexBuffer(const std::vector<uint32_t> &, BufferUsage bu) const;
		Buffer *newVertexBuffer(const void *data, uint32_t n, BufferUsage bu) const;
		VertexArray *newVertexArray() const;

		ColorTexture2D *newColorTexture2D(uint32_t width, uint32_t height, PixelFormat pf) const;
		ColorTexture2D *newColorTexture2D(const std::string &) const;
		ColorTexture2D *newColorTexture2D(uint16_t width, uint16_t height, uint8_t *pixels) const;
		DepthTexture2D *newDepthTexture2D(uint32_t width, uint32_t height, DepthFormat df) const;

		// This function will only be call by ContextWindow;
		void OnContextClosed();
		std::pair<ContextWindow *, Managers> findCurrentContext() const;
	};

	typedef std::shared_ptr<Program> ProgramPtr;
}

#endif