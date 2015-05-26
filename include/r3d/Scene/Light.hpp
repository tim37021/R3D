#ifndef __R3D_SCENE_LIGHT_HPP_
#define __R3D_SCENE_LIGHT_HPP_

#include <glm/glm.hpp>

namespace r3d
{
	struct Light
	{
		glm::vec3 pos;
		glm::vec3 color;
		bool castShadow;
		
		Light(const glm::vec3 &pos_, const glm::vec3 &color_, bool castShadow_=false): 
			pos(pos_), color(color_), castShadow(castShadow_){}
		Light(): pos(), color(1.0f), castShadow(false){}
	};

	typedef Light PointLight;
}

#endif