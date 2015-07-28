#ifndef __R3D_CORE_TRANSFORMATION_HPP_
#define __R3D_CORE_TRANSFORMATION_HPP_

#include <glm/glm.hpp>

namespace r3d
{
	class Transformation
	{
	public:
		Transformation(const glm::vec3 &t=glm::vec3(0.0f), 
			const glm::vec3 &r=glm::vec3(0.0f, 0.0f, 0.0f), 
			const glm::vec3 &s=glm::vec3(1.0f));

		void setTranslation(const glm::vec3 &t)
		{ m_translation=t; dirty=true; }
		void setRotation(const glm::vec3 &r)
		{ m_rotation=r; dirty=true; }
		void setScale(const glm::vec3 &s)
		{ m_scale=s; dirty=true; }

		glm::vec3 getTranslation() const
		{ return m_translation; }
		glm::vec3 getRotation() const
		{ return m_rotation; }
		glm::vec3 getScale() const
		{ return m_scale; }

		const glm::mat4 &getMatrix() const;
		const glm::mat4 &getRotationMatrix() const;
		const glm::mat4 &getTranslationMatrix() const;
	private:
		mutable bool dirty;
		mutable glm::mat4 m_cacheMatrix;
		mutable glm::mat4 m_cacheRotationMatrix;
		mutable glm::mat4 m_cacheTranslationMatrix;

		glm::vec3 m_translation;
		//Eular rotation
		glm::vec3 m_rotation;
		glm::vec3 m_scale;
	};
}

#endif