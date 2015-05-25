#ifndef __R3D_MANAGER_TEXTURE_MANAGER_HPP_
#define __R3D_MANAGER_TEXTURE_MANAGER_HPP_

#include <r3d/Core/Texture.hpp>
#include <map>
#include <string>
#include <cstdint>

namespace r3d
{
	class Engine;
	class ContextWindow;
	class TextureManager
	{
	public:
		TextureManager(Engine *, ContextWindow *);
		~TextureManager();

		ColorTexture2D *registerColorTexture2D(const std::string &name, uint32_t, uint32_t, PixelFormat pf);
		ColorTexture2D *registerColorTexture2D(const std::string &name, uint16_t, uint16_t, uint8_t *pixels);
		ColorTexture2D *registerColorTexture2D(const std::string &filename);
		ColorTexture2D *registerColorTexture2D(const std::string &name,
			const std::string &filename);

		DepthTexture2D *registerDepthTexture2D(const std::string &name, uint32_t, uint32_t, DepthFormat df);

		void clear();
	private:
		Engine *m_engine;
		ContextWindow *m_cw;
		std::map<std::string, Texture2D *> m_texts;
	};
}

#endif