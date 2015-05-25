#ifndef __R3D_SHADER_OPENGL_SHADER_HPP_
#define __R3D_SHADER_OPENGL_SHADER_HPP_

#include <GL/glew.h>
#include <r3d/Shader/Shader.hpp>

namespace r3d
{
	class OpenGLShader: public Shader
	{
	public:
		OpenGLShader(ShaderType type);
		virtual ~OpenGLShader();

		virtual void source(const std::string &code);

		// Software renderer may leave the implementation blank
		virtual void compile();

		GLuint getID() const { return m_id; }
	private:
		GLuint m_id;

		std::string getOpenGLInfoLog();
	};

}
#endif