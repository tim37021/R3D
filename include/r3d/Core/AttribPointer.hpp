#ifndef __CORE_ATTRIB_POINTER_HPP_
#define __CORE_ATTRIB_POINTER_HPP_

#include <r3d/Core/Type.hpp>

namespace r3d
{
	struct AttribPointer
	{
		R3DType type;
		//!> must be 1 2 3 4
		int32_t size;
		uint32_t stride;
		uint32_t offset;

		AttribPointer(R3DType t_, int32_t size_, uint32_t stride_, uint32_t offset_)
			: type(t_), size(size_), stride(stride_), offset(offset_)
		{}

		AttribPointer()=default;
		AttribPointer(const AttribPointer &)=default;
	};
}

#endif