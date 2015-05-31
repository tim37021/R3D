#ifndef __R3D_FRUSTUM_FRUSTUM_HPP_
#define __R3D_FRUSTUM_FRUSTUM_HPP_
#include <glm/glm.hpp>
#include <r3d/Core/AABB.hpp>
#include <vector>

namespace r3d
{	

	class Camera;

	class Frustum{
		public:
			Frustum();
			void setFrustum(glm::vec3 m_pos, glm::vec3 m_dir, glm::vec3 m_up, float fov, float ratio, float far, float near);
			glm::vec4 calcPlane(glm::vec3 a, glm::vec3 b, glm::vec3 c);
			bool AABBinFrustum(AABB aabb);
			std::vector <glm::vec4> p;
	};	
}

#endif