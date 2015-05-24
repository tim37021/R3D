#ifndef __CORE_OPENGL_VERTEXBUFFER_HPP_
#define __CORE_OPENGL_VERTEXBUFFER_HPP_

#include <r3d/Core/Buffer.hpp>
#include <vector>
#include "OpenGLObject.hpp"

namespace r3d
{
	class OpenGLVertexBuffer: public Buffer, public OpenGLObject
	{
	public:
		OpenGLVertexBuffer(const void *data, const uint32_t size, BufferUsage bu);
		virtual ~OpenGLVertexBuffer();

		virtual bool isLocked() { return lockedPtr!=nullptr; }
		virtual uint32_t getSize() const{ return m_size; }

		virtual void *lock(BufferAccess ba=BA_READ_WRITE);
		virtual void unlock();
	private:
		uint32_t m_size;
		void *lockedPtr;
	};

	class OpenGLIndexBuffer: public Buffer, public OpenGLObject
	{
	public:
		OpenGLIndexBuffer(const std::vector<uint32_t> &indices, BufferUsage bu);
		virtual ~OpenGLIndexBuffer();

		virtual uint32_t getSize() const{ return m_size; }

		virtual bool isLocked() { return lockedPtr!=nullptr; }
		virtual void *lock(BufferAccess ba=BA_READ_WRITE);
		virtual void unlock();
	private:
		uint32_t m_size;
		void *lockedPtr;
	};
}

#endif