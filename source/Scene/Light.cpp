#include <r3d/Scene/Light.hpp>
#include <r3d/Window/ContextWindow.hpp>
#include <string>

namespace r3d
{
	SpotLight::SpotLight(ContextWindow *cw, int width, int height): Light(LT_SPOT_LIGHT)
	{
		dMap=cw->getTextureManager()->registerDepthTexture2D(
			"ShadowMap["+std::to_string(width)+"x"+std::to_string(height)+"]", width, height, DF_24);
		up = glm::vec3(1.0f, 0.0f, 0.0f);// Just set a up vector 
		
		dMap->setBorder(glm::vec4(1.0f));
		dMap->setWrapping(W_CLAMP_TO_BORDER, W_CLAMP_TO_BORDER);
	}

	LightSceneNode::LightSceneNode(SceneNodePtr parent, ContextWindow *cw, Light *light,
		const char *name, const Transformation &relative):
		SceneNode(parent, cw, name? name:"", relative), m_light(light)
	{
		switch(m_light->type)
		{
			case LT_POINT_LIGHT:
				m_nodeType="PLight"; break;
			case LT_SPOT_LIGHT:
				m_nodeType="SLight"; break;
			case LT_DIRECTIONAL_LIGHT:
				m_nodeType="DLight"; break;
		}

		if(std::string(getName())=="")
		{
			setName(("NewLight"+std::to_string(getID())).c_str());
		}
	}
} 