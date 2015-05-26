#ifndef __R3D_PIPELINE_DEFAULT_POSTFX_HPP_
#define __R3D_PIPELINE_DEFAULT_POSTFX_HPP_

#include <r3d/Pipeline/PostFX.hpp>
#include <memory>

namespace r3d
{
	class VertexArray;
	class Program;
	class Texture2D;

	typedef std::shared_ptr<Program> ProgramPtr;

	class ShaderPostEffect: public PostEffect
	{
	public:
		ShaderPostEffect(PostFX *, const std::string &name, const char *vs, const char *fs);
		virtual void run();
	protected:
		ProgramPtr m_program;
		Texture2D *text;
	};

	void EffectDeleter(PostEffect *);
}

#endif