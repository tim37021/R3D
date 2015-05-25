#ifndef __R3D_PLATFORM_GLFW3_CONTEXT_WINDOW_HPP_
#define __R3D_PLATFORM_GLFW3_CONTEXT_WINDOW_HPP_

#include <r3d/Window/ContextWindow.hpp>
#include <GLFW/glfw3.h>

namespace r3d
{
	class GLFW3ContextWindow: public ContextWindow
	{
	public:
		GLFW3ContextWindow(uint32_t width, uint32_t height, const std::string &title);
		virtual ~GLFW3ContextWindow();

		// Disable copy operator
		GLFW3ContextWindow &operator=(const GLFW3ContextWindow &)=delete;

		virtual void makeCurrent();
		virtual void setPos(int32_t, int32_t);
		virtual void getPos(int32_t *, int32_t *);
		virtual void pollInput();
		virtual void swapBuffers();

		virtual void setSize(uint32_t width, uint32_t height);
		virtual void setTitle(const std::string &title);

		virtual bool isCloseButtonClicked();

		GLFWwindow *getGLFW3Handle() const { return m_window; }
	private:
		static const std::map<ContextOptions, int32_t> WindowHintMapping;
		void prepareGLFW3Hint();
		GLFWwindow *m_window;
	};
}

#endif