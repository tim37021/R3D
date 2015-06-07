#ifndef __R3D_SHADER_OPENGL_PROGRAM_HPP_
#define __R3D_SHADER_OPENGL_PROGRAM_HPP_

#include <GL/glew.h>
#include <r3d/Shader/Program.hpp>
#include <glm/glm.hpp>

namespace r3d
{
	class Shader;
	class OpenGLProgram: public Program
	{
	public:
		OpenGLProgram();
		virtual ~OpenGLProgram();
		virtual void attachShader(ShaderPtr);
		virtual void link();

		virtual void use();

		virtual void setUniform(const std::string &name, GLint value);
		virtual void setUniform(const std::string &name, GLfloat value);
		virtual void setUniform(const std::string &name, const glm::vec2 &v);
		virtual void setUniform(const std::string &name, const glm::vec3 &v);
		virtual void setUniform(const std::string &name, const glm::mat3 &mat);
		virtual void setUniform(const std::string &name, const glm::mat4 &mat);

		virtual void dispatchCompute(uint32_t x, uint32_t y, uint32_t z);

		GLuint getID() const { return m_id; }
	private:
		GLuint m_id;
		std::string getOpenGLInfoLog();
	};
}

#endif