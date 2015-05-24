#include <GL/glew.h>
#include <vector>
#include "GLFW3ContextWindow.hpp"
#include "../Input/GLFW3MouseInput.hpp"
#include "../Input/GLFW3KeyboardInput.hpp"

namespace r3d
{
	const std::map<ContextOptions, int32_t> GLFW3ContextWindow::WindowHintMapping=
	{
		{CO_RESIZABLE, GLFW_RESIZABLE},
		{CO_VISIBLE, GLFW_VISIBLE},

		{CO_RED_BITS, GLFW_RED_BITS},
		{CO_GREEN_BITS, GLFW_GREEN_BITS},
		{CO_BLUE_BITS, GLFW_BLUE_BITS},
		{CO_DEPTH_BITS, GLFW_DEPTH_BITS},
		{CO_STENCIL_BITS, GLFW_STENCIL_BITS},

		{CO_GL_VERSION_MAJOR, GLFW_CONTEXT_VERSION_MAJOR},
		{CO_GL_VERSION_MINOR, GLFW_CONTEXT_VERSION_MINOR}
	};

	static std::vector<GLFW3ContextWindow *> cw_list;

	static GLFW3ContextWindow *FindWindow(GLFWwindow *window)
	{
		for(auto win: cw_list)
		{
			if(win->getGLFW3Handle()==window)
				return win;
		}
		return nullptr;
	}

	static void DropCallback(GLFWwindow *window, int count, const char **filenames)
	{
		auto win=FindWindow(window);
		EventListener *el=win->getEventListener();
		if(el)
			el->OnFilesDropIn(count, filenames);
	}

	GLFW3ContextWindow::GLFW3ContextWindow(uint32_t width, uint32_t height,
	const std::string &title):
		ContextWindow(width, height, title)
	{
		// Preparing window creation hint
		prepareGLFW3Hint();
		m_window=glfwCreateWindow(getWidth(), getHeight(), getTitle(), nullptr, nullptr);
		glfwMakeContextCurrent(m_window);
		glfwSwapInterval(0);
		glewExperimental=GL_TRUE;
		glewInit();
		m_mouse=new GLFW3MouseInput(this);
		m_keybd=new GLFW3KeyboardInput(this);

		cw_list.push_back(this);
		glfwSetDropCallback(m_window, DropCallback);
	}

	GLFW3ContextWindow::~GLFW3ContextWindow()
	{
		glfwDestroyWindow(m_window);
	}

	void GLFW3ContextWindow::setPos(int32_t x, int32_t y)
	{
		glfwSetWindowPos(m_window, x, y);
	}

	void GLFW3ContextWindow::getPos(int32_t *x, int32_t *y)
	{
		glfwGetWindowPos(m_window, x, y);
	}

	void GLFW3ContextWindow::makeCurrent()
	{
		glfwMakeContextCurrent(m_window);
	}

	void GLFW3ContextWindow::pollInput()
	{
		glfwPollEvents();
	}

	void GLFW3ContextWindow::swapBuffers()
	{
		glfwSwapBuffers(m_window);
	}

	void GLFW3ContextWindow::setSize(uint32_t width, uint32_t height)
	{
		ContextWindow::setSize(width, height);
		glfwSetWindowSize(m_window, width, height);
	}
	void GLFW3ContextWindow::setTitle(const std::string &title)
	{
		ContextWindow::setTitle(title);
		glfwSetWindowTitle(m_window, title.c_str());
	}

	bool GLFW3ContextWindow::isCloseButtonClicked()
	{
		bool value=glfwWindowShouldClose(m_window);
		glfwSetWindowShouldClose(m_window, GL_FALSE);
		return value;
	}

	void GLFW3ContextWindow::prepareGLFW3Hint()
	{
		for(auto &element: WindowCreationHint)
		{
			int value;
			if(element.second=="TRUE") value=1;
			else if(element.second=="FALSE") value=0;
			else value=std::stoi(element.second);
			glfwWindowHint(WindowHintMapping.find(element.first)->second, value);
		}
	}
}