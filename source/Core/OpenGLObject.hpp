#ifndef __R3D_CORE_OPENGL_OBJECT_HPP_
#define __R3D_CORE_OPENGL_OBJECT_HPP_

#include <GL/glew.h>
#include <vector>
#include <cstdint>

namespace r3d
{
	/*
		OpenGL object creation/destruction function prototypes
	*/
#ifdef _WIN32
	typedef void ( __stdcall * createFunc ) ( GLsizei, GLuint* );
	typedef void ( __stdcall * deleteFunc ) ( GLsizei, const GLuint* );
	typedef void ( __stdcall * deleteFunc2 ) ( GLuint );
#elif defined( _LINUX )
	typedef void ( * createFunc ) ( GLsizei, GLuint* );
	typedef void ( * deleteFunc ) ( GLsizei, const GLuint* );
	typedef void ( * deleteFunc2 ) ( GLuint );
#endif

	class OpenGLObject
	{
	public:
		OpenGLObject(createFunc cfunc, deleteFunc dfunc, uint32_t count=1):
			m_dfunc(dfunc)
		{
			if(count<1) count=1;
			m_ids.resize(count);
			cfunc(count, m_ids.data());
		}
		virtual ~OpenGLObject()
		{ m_dfunc(m_ids.size(), getIDs()); }

		GLuint getID() const
		{ return m_ids[0]; }

		GLuint getID(uint32_t index) const
		{ return m_ids[index]; }

		const GLuint *getIDs() const
		{ return &m_ids[0]; }
	private:
		std::vector<GLuint> m_ids;

		deleteFunc m_dfunc;
	};
}

#endif