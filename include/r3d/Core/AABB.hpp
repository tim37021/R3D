#ifndef __R3D_AABB_AABB_H_
#define __R3D_AABB_AABB_H_

#include <glm/glm.hpp>

namespace r3d
{
	class AABB{
		public:
			AABB(const glm::vec3 &max=glm::vec3(0.0f), const glm::vec3 &min=glm::vec3(0.0f));
			void set(const glm::vec3 &max, const glm::vec3 &min)
			{
				m_max = max;
				m_min = min;
			}

			glm::vec3 getVertexP(glm::vec3 n);
			glm::vec3 getVertexN(glm::vec3 n);

			glm::vec3 m_max;
			glm::vec3 m_min;
	};
}

#endif