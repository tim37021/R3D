#include <r3d/Scene/Light.hpp>
#include <r3d/Window/ContextWindow.hpp>
#include <string>

namespace r3d
{
	SpotLight::SpotLight(ContextWindow *cw, int width, int height): Light(LT_SPOT_LIGHT)
	{
		dMap=cw->getTextureManager()->registerDepthTexture2D(
			"ShadowMap["+std::to_string(width)+"x"+std::to_string(height)+"]", width, height, DF_24);
	}
}