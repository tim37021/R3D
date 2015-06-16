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
		
		dMap->setWrapping(W_CLAMP_TO_BORDER, W_CLAMP_TO_BORDER);
	}
} 