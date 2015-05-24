#ifndef __PRIMITIVE_MESH_HPP_
#define __PRIMITIVE_MESH_HPP_

#include <r3d/Core/Vertex.hpp>
#include <vector>
#include <memory>
#include <string>
#include <utility>

namespace r3d
{

	struct Mesh3D
	{
		std::vector<Vertex> vertices;
		std::vector<uint32_t> indices;
	};

	typedef std::shared_ptr<Mesh3D> Mesh3DPtr;

	std::vector<std::pair<std::string, Mesh3DPtr> > LoadObjMesh(const char *filename);
}

#endif