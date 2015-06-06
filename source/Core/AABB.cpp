#include <glm/glm.hpp>
#include <r3d/Core/AABB.hpp>

namespace r3d{
	AABB::AABB(const glm::vec3 &max, const glm::vec3 &min)
		: m_max(max), m_min(min)
	{

	}

	glm::vec3 AABB::getVertexP(glm::vec3 n)
	{
		glm::vec3 ret;

		ret.x = (n.x > 0) ? m_max.x : m_min.x;
		ret.y = (n.y > 0) ? m_max.y : m_min.y;
		ret.z = (n.z > 0) ? m_max.z : m_min.z;

		return ret;
	}

	glm::vec3 AABB::getVertexN(glm::vec3 n)
	{
		glm::vec3 ret;

		ret.x = (n.x > 0) ? m_min.x : m_max.x;
		ret.y = (n.y > 0) ? m_min.y : m_max.y;
		ret.z = (n.z > 0) ? m_min.z : m_max.z;

		return ret;
	}

}