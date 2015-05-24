#ifndef __INPUT_MOUSE_INPUT_HPP_
#define __INPUT_MOUSE_INPUT_HPP_

#include <cstdint>
#include <vector>

namespace r3d
{
	enum MouseButton
	{
		MB_LEFT,
		MB_RIGHT,
		MB_MIDDLE
	};

	class ContextWindow;

	class MouseInput
	{
	public:
		MouseInput(ContextWindow *window)
		: m_window(window) {}
		virtual ~MouseInput()=default;

		virtual void setPos(double, double)=0;
		virtual void getPos(double *xpos, double *ypos)=0;
		virtual bool getButton(MouseButton)=0;

		virtual void updateInput()=0;

		ContextWindow *getWindow()
		{ return m_window; }
	private:
		ContextWindow *m_window;
	};
}

#endif