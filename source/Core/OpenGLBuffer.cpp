#include "OpenGLBuffer.hpp"
#include <cstring>

static GLenum BufferUsageOpenGLMap[]={GL_STATIC_DRAW, GL_DYNAMIC_DRAW};
static GLenum BufferAccessOpenGLMap[]={GL_READ_ONLY, GL_WRITE_ONLY, GL_READ_WRITE};

namespace r3d
{
	OpenGLVertexBuffer::OpenGLVertexBuffer(const void *data, const uint32_t size,
		BufferUsage bu): 
		Buffer(bu), OpenGLObject(glGenBuffers, glDeleteBuffers), m_size(size),
		lockedPtr(nullptr)
	{
		glBindBuffer(GL_ARRAY_BUFFER, getID());
		glBufferData(GL_ARRAY_BUFFER, m_size, data, BufferUsageOpenGLMap[getBufferUsage()]);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	OpenGLVertexBuffer::~OpenGLVertexBuffer()
	{
		unlock();
	}

	void *OpenGLVertexBuffer::lock(BufferAccess ba)
	{
		if(lockedPtr)
			return lockedPtr;
		glBindBuffer(GL_ARRAY_BUFFER, getID());
		lockedPtr=glMapBuffer(GL_ARRAY_BUFFER, BufferAccessOpenGLMap[ba]);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		return lockedPtr;
	}

	void OpenGLVertexBuffer::unlock()
	{
		if(lockedPtr)
		{
			glBindBuffer(GL_ARRAY_BUFFER, getID());
			glUnmapBuffer(GL_ARRAY_BUFFER);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			lockedPtr=nullptr;
		}
	}

	OpenGLIndexBuffer::OpenGLIndexBuffer(const std::vector<uint32_t> &indices,
		BufferUsage bu): 
		Buffer(bu), OpenGLObject(glGenBuffers, glDeleteBuffers), 
		m_size(indices.size()*sizeof(uint32_t)), lockedPtr(nullptr)
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, getID());
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_size, indices.data(), BufferUsageOpenGLMap[getBufferUsage()]);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

	OpenGLIndexBuffer::~OpenGLIndexBuffer()
	{
		
	}

	void *OpenGLIndexBuffer::lock(BufferAccess ba)
	{
		if(lockedPtr)
			return lockedPtr;
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, getID());
		lockedPtr=glMapBuffer(GL_ELEMENT_ARRAY_BUFFER, BufferAccessOpenGLMap[ba]);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		return lockedPtr;
	}

	void OpenGLIndexBuffer::unlock()
	{
		if(lockedPtr)
		{
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, getID());
			glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
			lockedPtr=nullptr;
		}
	}
}