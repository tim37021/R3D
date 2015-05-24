#ifndef __SHADER_PROGRAM_HPP_
#define __SHADER_PROGRAM_HPP_

#include <stdexcept>
#include <memory>
#include <string>
#include <cstdint>
#include <glm/glm.hpp>

namespace r3d
{
	class Shader;

	typedef std::shared_ptr<Shader> ShaderPtr;

	class LinkError: public std::runtime_error
	{
	public:
		explicit LinkError(const std::string &what_): std::runtime_error(what_){}
	};

	class Program
	{
	public:
		virtual ~Program(){}
		virtual void attachShader(ShaderPtr)=0;
		virtual void link()=0;

		virtual void use()=0;

		virtual void setUniform(const std::string &name, int32_t value)=0;
		virtual void setUniform(const std::string &name, float value)=0;
		virtual void setUniform(const std::string &name, const glm::vec2 &v)=0;
		virtual void setUniform(const std::string &name, const glm::vec3 &v)=0;
		virtual void setUniform(const std::string &name, const glm::mat4 &mat)=0;
	};

	typedef std::shared_ptr<Program> ProgramPtr; 
	typedef std::weak_ptr<Program> ProgramWeakPtr; 
}

#endif