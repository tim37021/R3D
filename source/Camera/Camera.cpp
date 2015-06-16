#include <r3d/Camera/Camera.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "../Core/Frustum.hpp"
#include <r3d/Window/ContextWindow.hpp>
#include <cmath>
#include <cstdio>
#include <iostream>

namespace r3d
{
	Camera::Camera(ContextWindow *window,
		const glm::vec3 &pos, const glm::vec3 &dir, const glm::vec3 &up):
		m_window(window), m_pos(pos), m_dir(dir), m_up(up), m_dirty(true)
	{
		m_near = 0.1f;
		m_far = 1000.0f;
		m_aspect = (float)window->getWidth() / window->getHeight();
	}

	Camera::Camera(ContextWindow *window): m_window(window)
	{
		m_near = 0.1f;
		m_far = 1000.0f;
		m_aspect = (float)window->getWidth() / window->getHeight();
	}

	const glm::mat4 &Camera::getVMatrix() const
	{
		if(m_dirty)
		{
			m_proj=glm::perspective(m_fov, m_aspect, m_near, m_far);
			m_viewcache=glm::lookAt(m_pos, m_pos+m_dir, m_up);
			m_cache=m_proj*m_viewcache;
			m_dirty=false;
		}
		return m_viewcache;
	}

	const glm::mat4 &Camera::getPMatrix() const
	{
		if(m_dirty)
		{
			m_proj=glm::perspective(m_fov, m_aspect, m_near, m_far);
			m_viewcache=glm::lookAt(m_pos, m_pos+m_dir, m_up);
			m_cache=m_proj*m_viewcache;
			m_dirty=false;
		}
		return m_proj;
	}

	const glm::mat4 &Camera::getVPMatrix() const
	{
		if(m_dirty)
		{
			m_proj=glm::perspective(m_fov, m_aspect, m_near, m_far);
			m_viewcache=glm::lookAt(m_pos, m_pos+m_dir, m_up);
			m_cache=m_proj*m_viewcache;
			m_dirty=false;
		}
		return m_cache;
	}

	const Frustum Camera::getFrustum(){
		Frustum frustum;
		frustum.setFrustum(m_pos, m_dir, m_up, m_fov, m_aspect, m_near, m_far);
		// std::cout<<frustum.p[0].x<<" "<<frustum.p[0].y<<" "<<frustum.p[0].z<<" "<<frustum.p[0].w<<std::endl;
		return frustum;
	}

	FPSCamera::FPSCamera(ContextWindow *window, float fov, const glm::vec3 &pos)
		: Camera(window), m_horizontalAngle(3.14f), m_verticalAngle(0.0f),
		m_speed(3.0f), m_mouseSpeed(0.2f), m_lastUpdate(0.0f)
	{
		m_pos=pos;
		m_fov=fov;
	}

	void FPSCamera::update(double elapsedTime)
	{
		double xpos, ypos;
		m_window->getMouse()->getPos(&xpos, &ypos);

		float deltaTime=elapsedTime-m_lastUpdate;
		m_lastUpdate=elapsedTime;

		// mouse
		m_horizontalAngle += m_mouseSpeed * deltaTime * float(m_window->getWidth()/2.0 - xpos );
		m_verticalAngle   += m_mouseSpeed * deltaTime * float(m_window->getHeight()/2.0 - ypos );
		
		m_dir=glm::vec3(cos(m_verticalAngle)*sin(m_horizontalAngle),
    		sin(m_verticalAngle),
   			cos(m_verticalAngle)*cos(m_horizontalAngle));
		glm::vec3 right = glm::vec3(
		    sin(m_horizontalAngle - 3.14f/2.0f),
		    0,
		    cos(m_horizontalAngle - 3.14f/2.0f)
		);
		m_up = glm::cross(right, m_dir);
		////////////////////////////////////////////////////

		// Move forward
		auto keybd=m_window->getKeyboard();
		if (keybd->getKeyState(KC_UP))
		    m_pos += m_dir * deltaTime * m_speed;
		
		// Move backward
		if (keybd->getKeyState(KC_DOWN))
		    m_pos -= m_dir * deltaTime * m_speed;
		
		// Strafe right
		if (keybd->getKeyState(KC_RIGHT))
		    m_pos += right * deltaTime * m_speed;
		
		// Strafe left
		if (keybd->getKeyState(KC_LEFT))
		    m_pos -= right * deltaTime * m_speed;
		


		m_dirty=true;

		m_window->getMouse()->setPos(m_window->getWidth()/2.0, m_window->getHeight()/2.0);
	}
}