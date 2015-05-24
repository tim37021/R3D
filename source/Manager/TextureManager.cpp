#include <r3d/Manager/TextureManager.hpp>
#include <r3d/Core/Core.hpp>
#include <r3d/Window/ContextWindow.hpp>

namespace r3d
{
	TextureManager::TextureManager(Engine *engine, ContextWindow *cw):
		m_engine(engine), m_cw(cw)
	{

	}

	TextureManager::~TextureManager()
	{
		clear();	
	}

	ColorTexture2D *TextureManager::registerColorTexture2D(const std::string &name, 
		uint32_t width, uint32_t height, PixelFormat pf)
	{
		auto it=m_texts.find(name+"_T2D");
		if(it==m_texts.end())
		{
			ContextWindow *current=m_engine->getCurrentContext();
			m_cw->makeCurrent();
			ColorTexture2D *new_one=m_engine->newColorTexture2D(width, height, pf);
			current->makeCurrent();
			m_texts[name+"_T2D"]=new_one;
			return new_one;
		}else
			return reinterpret_cast<ColorTexture2D *>(it->second);
	}

	ColorTexture2D *TextureManager::registerColorTexture2D(const std::string &name, 
		uint16_t width, uint16_t height, uint8_t *pixels)
	{
		auto it=m_texts.find(name+"_T2D");
		if(it==m_texts.end())
		{
			ContextWindow *current=m_engine->getCurrentContext();
			m_cw->makeCurrent();
			ColorTexture2D *new_one=m_engine->newColorTexture2D(width, height, pixels);
			current->makeCurrent();
			m_texts[name+"_T2D"]=new_one;
			return new_one;
		}else
			return reinterpret_cast<ColorTexture2D *>(it->second);
	}

	ColorTexture2D *TextureManager::registerColorTexture2D(const std::string &filename)
	{
		auto it=m_texts.find(filename+"_T2D");
		if(it==m_texts.end())
		{
			m_engine->log(std::string("r3d: Loading texture ")+filename);
			ContextWindow *current=m_engine->getCurrentContext();
			m_cw->makeCurrent();
			ColorTexture2D *new_one=m_engine->newColorTexture2D(filename);
			current->makeCurrent();
			m_texts[filename+"_T2D"]=new_one;
			return new_one;
		}else
			return reinterpret_cast<ColorTexture2D *>(it->second);
	}

	ColorTexture2D *TextureManager::registerColorTexture2D(const std::string &name,
		const std::string &filename)
	{
		auto it=m_texts.find(name+"_T2D");
		if(it==m_texts.end())
		{
			ContextWindow *current=m_engine->getCurrentContext();
			m_cw->makeCurrent();
			ColorTexture2D *new_one=m_engine->newColorTexture2D(filename);
			current->makeCurrent();
			m_texts[name+"_T2D"]=new_one;
			return new_one;
		}else
			return reinterpret_cast<ColorTexture2D *>(it->second);
	}
	
	DepthTexture2D *TextureManager::registerDepthTexture2D(const std::string &name, 
		uint32_t width, uint32_t height, DepthFormat df)
	{
		auto it=m_texts.find(name+"_T2D");
		if(it==m_texts.end())
		{
			ContextWindow *current=m_engine->getCurrentContext();
			m_cw->makeCurrent();
			DepthTexture2D *new_one=m_engine->newDepthTexture2D(width, height, df);
			current->makeCurrent();
			m_texts[name+"_T2D"]=new_one;
			return new_one;
		}else
			return reinterpret_cast<DepthTexture2D *>(it->second);
	}

	void TextureManager::clear()
	{
		ContextWindow *current=m_engine->getCurrentContext();
		m_cw->makeCurrent();
		for(auto &element: m_texts)
			delete element.second;
		current->makeCurrent();
	}
}