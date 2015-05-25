#ifndef __R3D_INPUT_KEYBOARD_INPUT_HPP_
#define __R3D_INPUT_KEYBOARD_INPUT_HPP_

namespace r3d
{
	class ContextWindow;
	enum KeyCode
	{
		KC_SPACE=32,
		KC_COMMA=44,
		KC_MINUS=45,
		KC_PERIOD=46,
		KC_SLASH=47,
		KC_0=48,
		KC_1,
		KC_2,
		KC_3,
		KC_4,
		KC_5,
		KC_6,
		KC_7,
		KC_8,
		KC_9,
		KC_A=65,
		KC_B,
		KC_C,
		KC_D,
		KC_E,
		KC_F,
		KC_G,
		KC_H,
		KC_I,
		KC_J,
		KC_K,
		KC_L,
		KC_M,
		KC_N,
		KC_O,
		KC_P,
		KC_Q,
		KC_R,
		KC_S,
		KC_T,
		KC_U,
		KC_V,
		KC_W,
		KC_X,
		KC_Y,
		KC_Z,

		KC_ESC=256,
		KC_ENTER=257,
		KC_TAB=258,
		KC_BACKSPACE=259,
		KC_INSERT=260,
		KC_DELETE=261,

		KC_RIGHT=262,
		KC_LEFT,
		KC_DOWN,
		KC_UP,

		KC_F1=290,
		KC_F2,
		KC_F3,
		KC_F4,
		KC_F5,
		KC_F6,
		KC_F7,
		KC_F8,
		KC_F9,
		KC_F10,
		KC_F11,
		KC_F12
	};

	class KeyboardInput
	{
	public:
		KeyboardInput(ContextWindow *window): m_window(window) {}
		virtual ~KeyboardInput()=default;

		virtual bool getKeyState(KeyCode key)=0;
		virtual void updateInput()=0;

		ContextWindow *getWindow() const { return m_window; }
	private:
		ContextWindow *m_window;
	};
}

#endif