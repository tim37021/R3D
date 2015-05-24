#include "GLFW3KeyboardInput.hpp"
#include <r3d/Window/ContextWindow.hpp>
#include <GLFW/glfw3.h>
#include "../Window/GLFW3ContextWindow.hpp"

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

	static void CharCallback(GLFWwindow *window, unsigned int chr)
	{
		auto win=FindWindow(window);
		EventListener *el=win->getEventListener();
		if(el)
			el->OnCharIncoming(chr);
	}

	static void KeyCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
	{
		auto win=FindWindow(window);
		EventListener *el=win->getEventListener();
		if(el)
			el->OnKeyStateChange(key, scancode, action, mods);
	}

	GLFW3KeyboardInput::GLFW3KeyboardInput(ContextWindow *window):
		KeyboardInput(window)
	{
		cw_list.push_back(static_cast<GLFW3ContextWindow *>(window));
		GLFWwindow *win=static_cast<GLFW3ContextWindow *>(window)->getGLFW3Handle();
		glfwSetCharCallback(win, CharCallback);
		glfwSetKeyCallback(win, KeyCallback);
	}

	bool GLFW3KeyboardInput::getKeyState(KeyCode key)
	{
		return glfwGetKey(static_cast<GLFW3ContextWindow *>(getWindow())->getGLFW3Handle(), key)==GLFW_PRESS;
	}
}