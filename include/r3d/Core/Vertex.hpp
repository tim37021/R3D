#ifndef __R3D_CORE_VERTEX_HPP_
#define __R3D_CORE_VERTEX_HPP_

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

	struct VertexTangent
	{
		glm::vec3 tangent;
		glm::vec3 bitangent;

		VertexTangent()=default;
		VertexTangent(const glm::vec3 &tan, const glm::vec3 &bitan):
			tangent(tan), bitangent(bitan){}
	};
}

#endif