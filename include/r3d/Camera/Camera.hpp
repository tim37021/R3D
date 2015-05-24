#ifndef __CAMERA_CAMERA_HPP_
#define __CAMERA_CAMERA_HPP_

#include <glm/glm.hpp>

namespace r3d
{
	class ContextWindow;

	class Camera
	{
	public:
		Camera(ContextWindow *window, 
			const glm::vec3 &pos, 
			const glm::vec3 &dir, 
			const glm::vec3 &up);

		virtual void update(double elapsedTime){}

		const glm::vec3 &getPos() const { return m_pos; }

		const glm::mat4 &getVPMatrix() const;
	protected:
		Camera(ContextWindow *window);

		ContextWindow *m_window;

		glm::vec3 m_pos;
		glm::vec3 m_dir;
		glm::vec3 m_up;

		float m_fov;

		mutable bool m_dirty;
		mutable glm::mat4 m_cache;
	};

	class FPSCamera: public Camera
	{
	public:
		FPSCamera(ContextWindow *window, float fov, const glm::vec3 &pos);
		
		virtual void update(double elapsedTime);

		void setSpeed(float speed) { m_speed=speed; }
		void setMouseSpeed(float speed) { m_mouseSpeed=speed; }
	private:
		

		float m_horizontalAngle;
		float m_verticalAngle;

		float m_speed;
		float m_mouseSpeed;

		double m_lastUpdate;
	};
}

#endif