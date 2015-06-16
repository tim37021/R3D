#ifndef __R3D_SCENE_LIGHT_HPP_
#define __R3D_SCENE_LIGHT_HPP_

#include <glm/glm.hpp>

namespace r3d
{
	class DepthTexture2D;
	class ContextWindow;
	enum LightType
	{
		LT_POINT_LIGHT,
		LT_SPOT_LIGHT,
		LT_DIRECTIONAL_LIGHT
	};

	struct Light
	{
		glm::vec3 color;
		LightType type;
		
		Light(LightType type_): type(type_){}
		Light()=delete;
		virtual ~Light()=default;
	};

	struct PointLight: public Light
	{
		glm::vec3 pos;
		PointLight(): Light(LT_POINT_LIGHT) {}
		virtual ~PointLight()=default;
	};

	struct SpotLight: public Light 
	{
		glm::vec3 pos;
		glm::vec3 dir;
		float angle;
		DepthTexture2D *dMap;
		SpotLight(ContextWindow *cw, int width = 1024, int height = 1024);

	};
}

#endif