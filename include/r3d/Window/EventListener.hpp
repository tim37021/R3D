#ifndef __R3D_WINDOW_EVENTLISTENER_HPP_
#define __R3D_WINDOW_EVENTLISTENER_HPP_

#include <cstdint>

namespace r3d
{
	class EventListener
	{
	public:
		virtual void OnMouseButtonStateChange(int button, int action, int mods){}
		virtual void OnMouseScrollStateChange(double xoffset, double yoffset){}
		virtual void OnCharIncoming(uint32_t chr){}
		virtual void OnKeyStateChange(int32_t key, int32_t scancode, 
			int32_t action, int32_t mods){}
		virtual void OnFilesDropIn(uint32_t count, const char *filenames[]){}
	};
}

#endif