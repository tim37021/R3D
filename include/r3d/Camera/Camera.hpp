#ifndef __R3D_CAMERA_CAMERA_HPP_
#define __R3D_CAMERA_CAMERA_HPP_

#include <glm/glm.hpp>

namespace r3d
{
	class ContextWindow;
	class Frustum;

	class Camera
	{
	public:
		Camera(ContextWindow *window, 
			const glm::vec3 &pos, 
			const glm::vec3 &dir, 
			const glm::vec3 &up);

		virtual void update(double elapsedTime){}

		
		
		const glm::vec3 &getPos() const { return m_pos; }
		const glm::vec3 &getDir() const { return m_dir; }
		const glm::vec3 &getUp() const { return m_up; }
		float getFov() const {return m_fov;}
		void setPos(const glm::vec3 &pos) { m_pos = pos; m_dirty = true; }
		void setDir(const glm::vec3 &dir) { m_dir = dir; m_dirty = true; }
		void setUp(const glm::vec3 &up) { m_up = up; m_dirty = true; }
		void setNear(float near) { m_near = near; m_dirty = true; }
		void setFar(float far) { m_far = far; m_dirty = true; }
		void setAspect(float a) { m_aspect=a; m_dirty = true; }
		void setFov(float fov) {m_fov = fov; m_dirty=true;}

		const Frustum getFrustum();

		const glm::mat4 &getVMatrix() const;
		const glm::mat4 &getPMatrix() const;
		const glm::mat4 &getVPMatrix() const;
	protected:
		Camera(ContextWindow *window);

		ContextWindow *m_window;

		glm::vec3 m_pos;
		glm::vec3 m_dir;
		glm::vec3 m_up;
		float m_aspect;

		float m_fov;

		float m_near;
		float m_far;

		mutable bool m_dirty;
		mutable glm::mat4 m_cache;
		mutable glm::mat4 m_proj;
		mutable glm::mat4 m_viewcache;
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