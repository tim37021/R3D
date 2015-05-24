#ifndef __PLATFORM_GLFW3KEYBOARDINPUT_HPP_
#define __PLATFORM_GLFW3KEYBOARDINPUT_HPP_

#include <r3d/Input/KeyboardInput.hpp>

namespace r3d
{
	class GLFW3KeyboardInput: public KeyboardInput
	{
	public:
		GLFW3KeyboardInput(ContextWindow *window);

		virtual bool getKeyState(KeyCode key);
		virtual void updateInput(){}
	private:
	};
}

#endif