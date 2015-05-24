#ifndef __PRIMITIVE_PRIMITIVE_TYPE_HPP_
#define __PRIMITIVE_PRIMITIVE_TYPE_HPP_

namespace r3d
{
	//! Modified from Irrlicht engine
	enum PrimitiveType
	{
		//! All vertices are non-connected points.
		PT_POINTS=0,

		//! All vertices form a single connected line.
		PT_LINE_STRIP,

		//! Just as LINE_STRIP, but the last and the first vertex is also connected.
		PT_LINE_LOOP,

		//! Every two vertices are connected creating n/2 lines.
		PT_LINES,

		//! After the first two vertices each vertex defines a new triangle.
		//! Always the two last and the new one form a new triangle.
		PT_TRIANGLE_STRIP,

		//! After the first two vertices each vertex defines a new triangle.
		//! All around the common first vertex.
		PT_TRIANGLE_FAN,

		//! Explicitly set all vertices for each triangle.
		PT_TRIANGLES,

		//! After the first two vertices each further tw vetices create a quad with the preceding two.
		PT_QUAD_STRIP,

		//! Every four vertices create a quad.
		PT_QUADS,

		//! Just as LINE_LOOP, but filled.
		PT_POLYGON,

		//! The single vertices are expanded to quad billboards on the GPU.
		PT_POINT_SPRITES
	};
}

#endif