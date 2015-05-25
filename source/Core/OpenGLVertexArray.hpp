#ifndef __R3D_CORE_OPENGL_VERTEXARRAY_HPP_
#define __R3D_CORE_OPENGL_VERTEXARRAY_HPP_

#include "OpenGLObject.hpp"
#include <r3d/Core/VertexArray.hpp>

namespace r3d
{
	class OpenGLVertexArray: public VertexArray, public OpenGLObject
	{
	public:
		OpenGLVertexArray();

		virtual void bindVertexBuffer(Buffer *vbo);
		virtual void bindIndexBuffer(Buffer *ibo);

		virtual void bind();
		virtual void unbind();

		virtual void enableAttribArray(uint32_t index, const AttribPointer &val);
		virtual void disableAttribArray(uint32_t index);
	private:
	};
}

#endif