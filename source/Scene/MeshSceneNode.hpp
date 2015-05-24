#ifndef __SCENE_MESH_SCENENODE_HPP_
#define __SCENE_MESH_SCENENODE_HPP_

#include <r3d/Scene/SceneNode.hpp>
#include <cstdint>
#include <glm/glm.hpp>
#include <vector>
#include <r3d/Core/Vertex.hpp>

namespace r3d
{
	class SceneManager;
	class VertexArray;
	class MeshSceneNode: public SceneNode
	{
		friend class SceneManager;
	public:
		//!> SceneManager
		MeshSceneNode(SceneNodePtr parent, 
			ContextWindow *cw, 
			const std::vector<Vertex> &vertices, const std::vector<uint32_t> &indices,
			const char *name="Untitled", const Transformation &relative=Transformation());

		//!> Render whole scene
		virtual void render(Renderer *, Camera *, 
			const glm::mat4 &current=glm::mat4(1.0f));
	private:
		VertexArray *m_vao;
		uint32_t m_indicesCount;
	};
}

#endif