#include <r3d/Shader/OpenGLProgram.hpp>
#include <r3d/Shader/OpenGLShader.hpp>
#include <string>
#include <vector>
#include <glm/gtc/type_ptr.hpp>

namespace r3d
{
	OpenGLProgram::OpenGLProgram()
	{
		m_id=glCreateProgram();
	}
	
	OpenGLProgram::~OpenGLProgram()
	{
		glDeleteProgram(m_id);
	}

	void OpenGLProgram::attachShader(ShaderPtr shader)
	{
		const OpenGLShader *ogl_shader=static_cast<const OpenGLShader *>(shader.get());
		glAttachShader(m_id, ogl_shader->getID());
	}

	void OpenGLProgram::link()
	{
		glLinkProgram(m_id);

		GLint res;
		glGetProgramiv( m_id, GL_LINK_STATUS, &res );

		if ( res == GL_FALSE )
			throw LinkError(getOpenGLInfoLog());
	}

	void OpenGLProgram::use()
	{
		glUseProgram(m_id);
	}

	std::string OpenGLProgram::getOpenGLInfoLog()
	{
		int maxLength;
		glGetProgramiv(m_id, GL_INFO_LOG_LENGTH, &maxLength);

		std::vector<char> errorLog(maxLength, '\0');
		glGetProgramInfoLog(m_id, maxLength, &maxLength, &errorLog[0]);

		return std::string(errorLog.cbegin(), errorLog.cend());
	}
	#define CHECKNAME(n) GLint id; if((id=glGetUniformLocation(m_id, n.c_str()))==-1) return;
	void OpenGLProgram::setUniform(const std::string &name, GLint value)
	{
		CHECKNAME(name);
		glUniform1i(id, value);
	}
	void OpenGLProgram::setUniform(const std::string &name, GLfloat value)
	{
		glUseProgram(m_id);
		CHECKNAME(name);
		glUniform1f(id, value);
	}
	void OpenGLProgram::setUniform(const std::string &name, const glm::vec2 &v)
	{
		glUseProgram(m_id);
		CHECKNAME(name);
		glUniform2fv(id, 1, glm::value_ptr(v));
	}
	void OpenGLProgram::setUniform(const std::string &name, const glm::vec3 &v)
	{
		glUseProgram(m_id);
		CHECKNAME(name);
		glUniform3fv(id, 1, glm::value_ptr(v));
	}

	void OpenGLProgram::setUniform(const std::string &name, const glm::mat3 &mat)
	{
		glUseProgram(m_id);
		CHECKNAME(name);
		glUniformMatrix3fv(id, 1, GL_FALSE, glm::value_ptr(mat));
	}

	void OpenGLProgram::setUniform(const std::string &name, const glm::mat4 &mat)
	{
		glUseProgram(m_id);
		CHECKNAME(name);
		glUniformMatrix4fv(id, 1, GL_FALSE, glm::value_ptr(mat));
	}

	void OpenGLProgram::dispatchCompute(uint32_t x, uint32_t y, uint32_t z)
	{
		glUseProgram(m_id);
		glDispatchCompute(x, y, z);
	}
}