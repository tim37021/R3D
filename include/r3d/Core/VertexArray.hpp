#ifndef __CORE_VERTEX_ARRAY_HPP_
#define __CORE_VERTEX_ARRAY_HPP_

#include <memory>
#include <r3d/Core/Buffer.hpp>
#include <r3d/Core/AttribPointer.hpp>
#include <map>

namespace r3d
{
	class VertexArray
	{
	public:
		VertexArray(): m_vbo(nullptr), m_ibo(nullptr){}
		virtual ~VertexArray(){}
		virtual void bindVertexBuffer(Buffer *vbo)
		{ m_vbo=vbo; }
		virtual void bindIndexBuffer(Buffer *ibo)
		{ m_ibo=ibo; }

		Buffer *getVertexBuffer()
		{ return m_vbo;}
		Buffer *getIndexBuffer()
		{ return m_ibo;}

		virtual void bind()=0;
		virtual void unbind()=0;

		virtual void enableAttribArray(uint32_t index, const AttribPointer &val)
		{ m_attrib[index]=val; }

		virtual void disableAttribArray(uint32_t index)
		{ m_attrib.erase(index); }
	protected:
		Buffer *m_vbo;
		Buffer *m_ibo;
		std::map<uint32_t, AttribPointer> m_attrib;
	};

	typedef VertexArray *VertexArrayPtr;
}

#endif