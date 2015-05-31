#include <GL/glew.h>
#include "MeshSceneNode.hpp"
#include <r3d/Renderer/Renderer.hpp>
#include <r3d/Core/VertexArray.hpp>
#include <r3d/Camera/Camera.hpp>
#include <r3d/Window/ContextWindow.hpp>
#include <iostream>

#include "../Core/Frustum.hpp"

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

		m_vertices = vertices;

	}

	void MeshSceneNode::render(Renderer *renderer, Camera *cam, const glm::mat4 &current)
	{
		auto material=getMaterial();
		const glm::mat4 tmpMatrix=current*m_relative.getMatrix();
		if(m_last != tmpMatrix)
		{
			float xmin=0, ymin=0, zmin=0, xmax=0, ymax=0, zmax=0;
			for(int v=0; v<m_vertices.size(); v++)
			{	//todo: pos * tmpmatrix
				glm::vec4 transV = tmpMatrix * glm::vec4(m_vertices[v].pos, 1);
				xmin = (transV.x < xmin) ? transV.x : xmin;
				ymin = (transV.y < ymin) ? transV.y : ymin;
				zmin = (transV.z < zmin) ? transV.z : zmin;
				xmax = (transV.x > xmax) ? transV.x : xmax;
				ymax = (transV.y > ymax) ? transV.y : ymax;
				zmax = (transV.z > zmax) ? transV.z : zmax;
			}
			m_aabb.set(glm::vec3(xmax, ymax, zmax), glm::vec3(xmin, ymin, zmin));
		}
		m_last = tmpMatrix;
		Frustum frustum = cam->getFrustum();
		if((frustum.AABBinFrustum(m_aabb)) && material)
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