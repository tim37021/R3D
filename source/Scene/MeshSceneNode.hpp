#ifndef __R3D_SCENE_MESH_SCENENODE_HPP_
#define __R3D_SCENE_MESH_SCENENODE_HPP_

#include <r3d/Scene/SceneNode.hpp>
#include <cstdint>
#include <glm/glm.hpp>
#include <vector>
#include <r3d/Core/Vertex.hpp>
#include <r3d/Core/AABB.hpp>

namespace tinyobj
{
	struct shape_t;
}

namespace r3d
{
	class SceneManager;
	class VertexArray;

	typedef tinyobj::shape_t Shape;

	class MeshSceneNode: public SceneNode
	{
		friend class SceneManager;
	public:
		//!> SceneManager
		MeshSceneNode(SceneNodePtr parent, 
			ContextWindow *cw, 
			Shape &shape,
			const char *name=nullptr, 
			const Transformation &relative=Transformation(),
			bool useTangent=false);

		//!> Render whole scene
		virtual void render(Renderer *, Program *program, Camera *, 
			const glm::mat4 &current=glm::mat4(1.0f), const glm::mat4 &currentRotation=glm::mat4(1.0f));
	private:
		glm::mat4 m_last;
		VertexArray *m_vao;
		uint32_t m_indicesCount;
		std::vector<Vertex> m_vertices;
		std::vector<VertexTangent> m_vertexTangents;
		AABB m_aabb;
		AABB m_aabbOriginal;
		
		void findAABB();
		void transformAABB(const glm::mat4 &trans);
	};


}

#endif