#ifndef __CORE_VERTEX_HPP_
#define __CORE_VERTEX_HPP_

#include <glm/glm.hpp>

namespace r3d
{
	struct Vertex
	{
		glm::vec3 pos;
		glm::vec2 texCoord;
		glm::vec3 norm;

		Vertex()=default;
		Vertex(const glm::vec3 &p, const glm::vec2 &t, const glm::vec3 &n):
			pos(p), texCoord(t), norm(n){}
	};
}

#endif