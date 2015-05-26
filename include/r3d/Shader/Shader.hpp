#ifndef __R3D_SHADER_SHADER_HPP_
#define __R3D_SHADER_SHADER_HPP_

#include <string>
#include <stdexcept>
#include <vector>

namespace r3d
{
	enum ShaderType
	{
		ST_VERTEX_SHADER,
		ST_FRAGMENT_SHADER
	};

	class CompileError: public std::runtime_error
	{
	public:
		explicit CompileError(const std::string &str) throw(): runtime_error(str){}
	};

	class Shader
	{
	public:
		Shader(ShaderType t): m_type(t){}
		virtual ~Shader(){}
		virtual void source(const std::string &code)=0;

		virtual void compile()=0;

		ShaderType getType() const { return m_type; }
	private:
		ShaderType m_type;
	};
}

#endif