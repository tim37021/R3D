#include "DefaultPostFX.hpp"
#include <r3d/Core/Core.hpp>

namespace r3d
{
	void EffectDeleter(PostEffect *effect)
	{
		delete effect;
	}

	ShaderPostEffect::ShaderPostEffect(PostFX *pfx, const std::string &name, 
		const char *vsource, const char *fsource):
		PostEffect(pfx, name)
	{
		m_program=pfx->getEngine()->newProgram();
		auto vs=pfx->getEngine()->newShader(ST_VERTEX_SHADER);
		auto fs=pfx->getEngine()->newShader(ST_FRAGMENT_SHADER);
		vs->source(vsource);
		fs->source(fsource);
		vs->compile();
		fs->compile();
		m_program->attachShader(vs);
		m_program->attachShader(fs);
		m_program->link();

		text=m_cw->getTextureManager()->registerColorTexture2D("LightedMap");
	}

	void ShaderPostEffect::run()
	{
		m_program->use();
		text->bind(0);
		m_program->setUniform("viewport", {m_cw->getWidth(), m_cw->getHeight()});
		m_program->setUniform("text", 0);
		m_engine->getRenderer()->drawElements(m_program.get(), m_pfx->getVAO(), r3d::PT_TRIANGLES, 6);
	}
}