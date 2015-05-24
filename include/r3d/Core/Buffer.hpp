#ifndef __CORE_BUFFER_HPP_
#define __CORE_BUFFER_HPP_

#include <cstdint>

namespace r3d
{
	enum BufferUsage
	{
		BU_STATIC_DRAW,
		BU_DYNAMIC_DRAW
	};

	enum BufferAccess
	{
		BA_READ_ONLY,
		BA_WRITE_ONLY,
		BA_READ_WRITE
	};

	class Buffer
	{
	public:
		Buffer(BufferUsage bu): m_bu(bu){}
		virtual ~Buffer()=default;

		virtual uint32_t getSize() const=0;

		virtual bool isLocked()=0;

		//! Allow user to modify buffer outside;
		virtual void *lock(BufferAccess ba=BA_READ_WRITE)=0;
		virtual void unlock()=0;

		BufferUsage getBufferUsage() const { return m_bu; }
	private:
		BufferUsage m_bu;
	};
}

#endif