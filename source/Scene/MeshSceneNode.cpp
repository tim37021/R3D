#include <GL/glew.h>
#include "MeshSceneNode.hpp"
#include <r3d/Renderer/Renderer.hpp>
#include <r3d/Core/VertexArray.hpp>
#include <r3d/Camera/Camera.hpp>
#include <r3d/Window/ContextWindow.hpp>

namespace r3d
{
	MeshSceneNode::MeshSceneNode(SceneNodePtr parent, ContextWindow *cw, 
		const std::vector<Vertex> &vertices, const std::vector<uint32_t> &indices,
		const char *name, 
		const Transformation &relative):
		SceneNode(parent, cw, name, relative)
	{
		auto vaoMgr=cw->getVertexArrayManager();
		auto bMgr=cw->getBufferManager();
		
		auto vbo=bMgr->registerVertexBuffer(name, vertices.data(), sizeof(Vertex)*vertices.size(), BU_STATIC_DRAW);
		auto ibo=bMgr->registerIndexBuffer(name, indices, BU_STATIC_DRAW);
	
		m_vao=vaoMgr->registerVertexArray(name);
		m_vao->bindVertexBuffer(vbo);
		m_vao->bindIndexBuffer(ibo);

		AttribPointer vertAtt(r3d::RT_FLOAT, 3, sizeof(Vertex), 0);
		AttribPointer texCoordAtt(r3d::RT_FLOAT, 2, sizeof(Vertex), 3*sizeof(float));
		AttribPointer normAtt(r3d::RT_FLOAT, 3, sizeof(Vertex), 5*sizeof(float));

		m_vao->enableAttribArray(0, vertAtt);
		m_vao->enableAttribArray(1, texCoordAtt);
		m_vao->enableAttribArray(2, normAtt);

		m_indicesCount=indices.size();
	}

	void MeshSceneNode::render(Renderer *renderer, Camera *cam, const glm::mat4 &current)
	{
		auto material=getMaterial();
		const glm::mat4 tmpMatrix=current*m_relative.getMatrix();
		if(material)
		{
			material->prepareShader();
			material->getProgram()->setUniform("mvp", cam->getVPMatrix()*tmpMatrix);
			material->getProgram()->setUniform("model", tmpMatrix);
			renderer->drawElements(material->getProgram().get(), m_vao, r3d::PT_TRIANGLES, m_indicesCount);
		}

		for(SceneNodePtr &child: m_children)
				child->render(renderer, cam, tmpMatrix);
	}
}