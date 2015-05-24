#include <r3d/Shader/OpenGLShader.hpp>
#include <vector>

namespace r3d
{
	OpenGLShader::OpenGLShader(ShaderType t)
		: Shader(t)
	{
		switch(t)
		{
			case ST_VERTEX_SHADER:
				m_id=glCreateShader(GL_VERTEX_SHADER); break;
			case ST_FRAGMENT_SHADER:
				m_id=glCreateShader(GL_FRAGMENT_SHADER); break;
		}
	}

	OpenGLShader::~OpenGLShader()
	{
		glDeleteShader(m_id);
	}

	void OpenGLShader::source(const std::string &code)
	{
		const char *c=code.c_str();
		glShaderSource(m_id, 1, &c, nullptr);
	}

	void OpenGLShader::compile()
	{
		glCompileShader(m_id);

		GLint res;
		glGetShaderiv( m_id, GL_COMPILE_STATUS, &res );

		if ( res == GL_FALSE )
			throw CompileError(getOpenGLInfoLog());
	}

	std::string OpenGLShader::getOpenGLInfoLog()
	{
		int maxLength;
		glGetShaderiv(m_id, GL_INFO_LOG_LENGTH, &maxLength);

		std::vector<char> errorLog(maxLength, '\0');
		glGetShaderInfoLog(m_id, maxLength, &maxLength, &errorLog[0]);

		return std::string(errorLog.cbegin(), errorLog.cend());
	}
}