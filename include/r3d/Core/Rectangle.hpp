#ifndef __R3D_CORE_RECTANGLE_HPP_
#define __R3D_CORE_RECTANGLE_HPP_

#include <glm/glm.hpp>

namespace r3d
{
	template <class T>
	struct Rectangle
	{
		glm::tvec2<T> bottomLeft;
		glm::tvec2<T> topRight;
		Rectangle(T x, T y, T width, T height):
			bottomLeft(x, y), topRight(x+width, y+height)
		{  }
	};
}

#endif