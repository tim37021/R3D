#ifndef __R3D_PIPELINE_DEFAULT_POSTFX_HPP_
#define __R3D_PIPELINE_DEFAULT_POSTFX_HPP_

#include <r3d/Pipeline/PostFX.hpp>
#include <memory>

namespace r3d
{
	class VertexArray;
	class Program;
	class ColorTexture2D;
	class RenderTarget2D;

	typedef std::shared_ptr<Program> ProgramPtr;
	typedef std::shared_ptr<RenderTarget2D> RenderTarget2DPtr;

	class ShaderPostEffect: public PostEffect
	{
	public:
		ShaderPostEffect(PostFX *, const std::string &name, Texture2D *input, const char *fs);
		virtual void run();
		virtual Texture2D *getResult() { return m_textResult; }
	protected:
		ProgramPtr m_program;
		Texture2D *m_textResult;
		RenderTarget2DPtr m_renderTarget;
		VertexArray *m_vao;
	};

	void EffectDeleter(PostEffect *);
}

#endif