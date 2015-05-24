#include "OpenGLVertexArray.hpp"
#include "OpenGLBuffer.hpp"

static GLenum R3DTypeOpenGLMap[]={GL_BYTE, GL_UNSIGNED_BYTE, GL_INT, GL_UNSIGNED_INT, GL_FLOAT};

namespace r3d
{
	OpenGLVertexArray::OpenGLVertexArray()
		: OpenGLObject(glGenVertexArrays, glDeleteVertexArrays)
	{

	}

	void OpenGLVertexArray::bindVertexBuffer(Buffer *vbo)
	{
		VertexArray::bindVertexBuffer(vbo);

		OpenGLVertexBuffer *ogl_vbo=static_cast<OpenGLVertexBuffer *>(vbo);

		glBindVertexArray(getID());
		
		glBindBuffer(GL_ARRAY_BUFFER, ogl_vbo->getID());

		for(auto &att: m_attrib)
		{
			uint32_t arrayIndex=att.first;
			auto &attrib=att.second;
			glEnableVertexAttribArray(arrayIndex);
			glVertexAttribPointer(arrayIndex, attrib.size, R3DTypeOpenGLMap[attrib.type],
				GL_FALSE, attrib.stride, (const GLvoid *)attrib.offset);
		}

		glBindVertexArray(0);
	}

	void OpenGLVertexArray::bindIndexBuffer(Buffer *ibo)
	{
		VertexArray::bindIndexBuffer(ibo);

		OpenGLIndexBuffer *ogl_ibo=static_cast<OpenGLIndexBuffer *>(ibo);

		glBindVertexArray(getID());
		
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ogl_ibo->getID());
		
		glBindVertexArray(0);
	}

	void OpenGLVertexArray::enableAttribArray(uint32_t index, const AttribPointer &val)
	{
		VertexArray::enableAttribArray(index, val);

		if(m_vbo)
		{
			OpenGLVertexBuffer *ogl_vbo=static_cast<OpenGLVertexBuffer *>(m_vbo);
			glBindVertexArray(getID());

			glBindBuffer(GL_ARRAY_BUFFER, ogl_vbo->getID());

			glEnableVertexAttribArray(index);
			glVertexAttribPointer(index, val.size, R3DTypeOpenGLMap[val.type], GL_FALSE,
				val.stride, (const GLvoid *)val.offset);

			glBindVertexArray(0);
		}
	}

	void OpenGLVertexArray::disableAttribArray(uint32_t index)
	{
		VertexArray::disableAttribArray(index);
		if(m_vbo)
		{
			OpenGLVertexBuffer *ogl_vbo=static_cast<OpenGLVertexBuffer *>(m_vbo);
			glBindVertexArray(getID());
			glBindBuffer(GL_ARRAY_BUFFER, ogl_vbo->getID());
			glDisableVertexAttribArray(index);
			glBindVertexArray(0);
		}
	}

	void OpenGLVertexArray::bind()
	{
		glBindVertexArray(getID());
	}

	void OpenGLVertexArray::unbind()
	{
		glBindVertexArray(0);
	}
}