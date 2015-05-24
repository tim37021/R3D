#include "GLFW3MouseInput.hpp"
#include "../Window/GLFW3ContextWindow.hpp"
#include <GLFW/glfw3.h>





namespace r3d
{
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

	static void MouseBtnCallback(GLFWwindow* window, int button, int action, int mods)
	{
		auto win=FindWindow(window);
		EventListener *el=win->getEventListener();
		if(el)
			el->OnMouseButtonStateChange(button, action, mods);
	}

	static void MouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
	{
		auto win=FindWindow(window);
		EventListener *el=win->getEventListener();
		if(el)
			el->OnMouseScrollStateChange(xoffset, yoffset);
	}

	GLFW3MouseInput::GLFW3MouseInput(ContextWindow *window):
		MouseInput(window)
	{
		GLFW3ContextWindow *glfw3window=static_cast<GLFW3ContextWindow *>(window);
		cw_list.push_back(glfw3window);
		
		glfwSetMouseButtonCallback(glfw3window->getGLFW3Handle(), MouseBtnCallback);
		glfwSetScrollCallback(glfw3window->getGLFW3Handle(), MouseScrollCallback);
	}

	void GLFW3MouseInput::setPos(double x, double y)
	{
		glfwSetCursorPos(static_cast<GLFW3ContextWindow *>(getWindow())->getGLFW3Handle(), x, y);
	}

	void GLFW3MouseInput::getPos(double *x, double *y)
	{
		glfwGetCursorPos(static_cast<GLFW3ContextWindow *>(getWindow())->getGLFW3Handle(), x, y);
	}

	bool GLFW3MouseInput::getButton(MouseButton mb)
	{
		// GLFW3 Button mapping is same
		return glfwGetMouseButton(static_cast<GLFW3ContextWindow *>(getWindow())->getGLFW3Handle(), mb);
	}
}