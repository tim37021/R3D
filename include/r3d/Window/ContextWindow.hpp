#ifndef __WINDOW_CONTEXT_WINDOW_HPP_
#define __WINDOW_CONTEXT_WINDOW_HPP_

#include <cstdint>
#include <r3d/Core/Rectangle.hpp>
#include <map>
#include <string>
#include <r3d/Input/MouseInput.hpp>
#include <r3d/Input/KeyboardInput.hpp>
#include <r3d/Window/EventListener.hpp>
#include <r3d/Manager/Managers.hpp>

namespace r3d
{
	class Engine;
	class TextureManager;
	class SceneManager;
	class BufferManager;
	class VertexArrayManager;

	enum ContextOptions
	{
		CO_RESIZABLE,		//TRUE
		CO_VISIBLE,			//TRUE

		CO_RED_BITS,		//8
		CO_GREEN_BITS,		//8
		CO_BLUE_BITS,		//8
		CO_DEPTH_BITS,		//24
		CO_STENCIL_BITS,	//8

		// Default OpenGL 3.3
		CO_GL_VERSION_MAJOR,
		CO_GL_VERSION_MINOR
	};

	class ContextWindow
	{
		friend class Engine;
	public:
		ContextWindow(uint32_t width, uint32_t height, const std::string &title);
		virtual ~ContextWindow();
		Rectangle<uint32_t> getDefaultViewport()
		{ return Rectangle<uint32_t>(0, 0, m_width, m_height); }

		uint32_t getWidth() const
		{ return m_width; }
		uint32_t getHeight() const
		{ return m_height; }
		const char *getTitle() const
		{ return m_title.c_str(); }

		virtual void setSize(uint32_t width, uint32_t height)
		{ m_width=width; m_height=height; }
		virtual void setTitle(const std::string &title)
		{ m_title=title; }
		virtual void setPos(int32_t, int32_t)=0;
		virtual void getPos(int32_t *, int32_t *)=0;

		virtual void makeCurrent()=0;
		virtual void pollInput()=0;
		virtual void swapBuffers()=0;

		// this method will clear state after calling it
		virtual bool isCloseButtonClicked()=0;

		virtual void updateInput()
		{
			m_mouse->updateInput();
			m_keybd->updateInput();
		}
		MouseInput *getMouse(){ return m_mouse; }
		KeyboardInput *getKeyboard() { return m_keybd; }

		static void SetHint(ContextOptions, bool);
		static void SetHint(ContextOptions, int32_t);

		EventListener* getEventListener()
		{ return m_el; }

		void setEventListener(EventListener *el)
		{ m_el=el; }

		TextureManager *getTextureManager()
		{ return m_managers->tMgr; }


		SceneManager *getSceneManager()
		{ return m_managers->sMgr; }
		BufferManager *getBufferManager()
		{ return m_managers->bMgr; }
		VertexArrayManager *getVertexArrayManager()
		{ return m_managers->vaoMgr; }
	protected:
		static std::map<ContextOptions, std::string> WindowCreationHint;
		MouseInput *m_mouse;
		KeyboardInput *m_keybd;
	private:
		uint32_t m_width, m_height;
		std::string m_title;

		EventListener *m_el;
		Managers *m_managers;


	};
}

#endif