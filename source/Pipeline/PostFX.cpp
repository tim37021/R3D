#include <r3d/Pipeline/PostFX.hpp>
#include <r3d/Core/Core.hpp>
#include <r3d/Core/VertexArray.hpp>
#include <r3d/Window/ContextWindow.hpp>
#include "DefaultPostFX.hpp"
#include <map>
#include <string>
#include <fstream>
#include <streambuf>

static std::string readfile(const char *filename)
{
std::ifstream t(filename);
return std::string((std::istreambuf_iterator<char>(t)),
                 std::istreambuf_iterator<char>());
}

namespace r3d
{
	static std::map<std::string, PostEffectInstancer> RegisteredEffects;

	PostEffect::PostEffect(PostFX *pfx, const std::string &effectName, Texture2D *input):
		m_pfx(pfx), m_engine(pfx->getEngine()), m_effectName(effectName),
		m_text(input), m_cw(pfx->getContextWindow())
	{

	}

	PostFX::PostFX(Engine *engine, ContextWindow *cw):
		m_engine(engine), m_cw(cw)
	{
	}

	PostFX::~PostFX()
	{
		clear();
	}

	void PostFX::runAll()
	{
		for(auto effect: effects)
			effect->run();
	}

	PostEffect *PostFX::pushEffect(const std::string &effectName, Texture2D *input)
	{
		PostEffect *effect=nullptr;
		auto it=RegisteredEffects.begin();
		if((it=RegisteredEffects.find(effectName))!=RegisteredEffects.end())
			effect=it->second.newPostEffect(this, input);
		else
			effect=new ShaderPostEffect(this, effectName, input, readfile((std::string("./shaders/")+effectName+".fs").c_str()).c_str());
		effects.push_back(effect);
		return effect;
	}

	void PostFX::clear()
	{
		for(auto effect: effects)
		{
			auto it=RegisteredEffects.begin();
			if((it=RegisteredEffects.find(effect->getName()))!=RegisteredEffects.end())
				it->second.freePostEffect(effect);
			else
				delete effect;
		}
	}

	void PostFX::Initialise()
	{
	}

	void PostFXFactory::RegisterEffect(const std::string &name, PostEffectInstancer instancer)
	{
		RegisteredEffects[name]=instancer;
	}
}