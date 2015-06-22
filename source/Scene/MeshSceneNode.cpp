#include <GL/glew.h>
#include "MeshSceneNode.hpp"
#include <r3d/Renderer/Renderer.hpp>
#include <r3d/Core/VertexArray.hpp>
#include <r3d/Camera/Camera.hpp>
#include <r3d/Window/ContextWindow.hpp>
#include <r3d/Utils/ObjLoader.hpp>
#include "../Core/Frustum.hpp"

#include <glm/gtc/matrix_inverse.hpp>

#include <iostream>
#include <cstdint>

#define INF 9999999.9f

template <class T>
inline static void swap(T &a, T &b)
{
	T tmp=a;
	a=b;
	b=tmp;
}

namespace r3d
{

	MeshSceneNode::MeshSceneNode(SceneNodePtr parent, ContextWindow *cw, 
		const Shape &shape,	const char *name_, const Transformation &relative):
		SceneNode(parent, cw, (name_? name_: shape.name.c_str()), relative)
	{
		std::string name=(name_? name_: shape.name.c_str());
		// construct vertex struct
		for(uint32_t v=0; v<shape.mesh.positions.size()/3; v++)
		{
			m_vertices.push_back({glm::vec3(shape.mesh.positions[v*3], shape.mesh.positions[v*3+1], shape.mesh.positions[v*3+2]),
							shape.mesh.texcoords.size()>0?glm::vec2(shape.mesh.texcoords.at(v*2), 1.0f-shape.mesh.texcoords.at(v*2+1)): glm::vec2(),
							glm::vec3(shape.mesh.normals.at(v*3), shape.mesh.normals.at(v*3+1), shape.mesh.normals.at(v*3+2))});
		}

		// We must move object to the middle
		findAABB();
		const glm::vec3 &middle=(m_aabb.m_max+m_aabb.m_min)/2.0f;
		for(auto &v: m_vertices)
			v.pos-=middle;
		// We change the vertex data, must find aabb again
		findAABB();
		m_relative.setTranslation(middle);

		auto vaoMgr=cw->getVertexArrayManager();
		auto bMgr=cw->getBufferManager();
		
		auto vbo=bMgr->registerVertexBuffer(name, m_vertices.data(), sizeof(Vertex)*m_vertices.size(), BU_STATIC_DRAW);
		auto ibo=bMgr->registerIndexBuffer(name, shape.mesh.indices, BU_STATIC_DRAW);
	
		m_vao=vaoMgr->registerVertexArray(name);
		m_vao->bindVertexBuffer(vbo);
		m_vao->bindIndexBuffer(ibo);

		AttribPointer vertAtt(r3d::RT_FLOAT, 3, sizeof(Vertex), 0);
		AttribPointer texCoordAtt(r3d::RT_FLOAT, 2, sizeof(Vertex), 3*sizeof(float));
		AttribPointer normAtt(r3d::RT_FLOAT, 3, sizeof(Vertex), 5*sizeof(float));

		m_vao->enableAttribArray(0, vertAtt);
		m_vao->enableAttribArray(1, texCoordAtt);
		m_vao->enableAttribArray(2, normAtt);

		m_indicesCount=shape.mesh.indices.size();
	}

	void MeshSceneNode::render(Renderer *renderer, Program *program, Camera *cam, 
		const glm::mat4 &current, const glm::mat4 &currentRotation)
	{
		auto material=getMaterial();
		const glm::mat4 tmpMatrix=current*m_relative.getMatrix();
		const glm::mat4 tmpRotation=currentRotation*m_relative.getRotationMatrix();
		if(m_last != tmpMatrix)
			transformAABB(tmpMatrix);
		Frustum frustum = cam->getFrustum();
		if((frustum.AABBinFrustum(m_aabb)) && material)
		{
			program->setUniform("mvp", cam->getVPMatrix()*tmpMatrix);
			program->setUniform("model", tmpMatrix);
			program->setUniform("nmat", tmpRotation);
			renderer->drawElements(program, m_vao, PT_TRIANGLES, m_indicesCount);
		}
	}

	void MeshSceneNode::findAABB()
	{
		const glm::mat4 &trans=glm::mat4(1.0f);
		float xmin=INF, ymin=INF, zmin=INF, xmax=-INF, ymax=-INF, zmax=-INF;
		for(uint32_t v=0; v<m_vertices.size(); v++)
		{	//todo: pos * tmpmatrix
			glm::vec4 transV = trans * glm::vec4(m_vertices[v].pos, 1);
			xmin = (transV.x < xmin) ? transV.x : xmin;
			ymin = (transV.y < ymin) ? transV.y : ymin;
			zmin = (transV.z < zmin) ? transV.z : zmin;
			xmax = (transV.x > xmax) ? transV.x : xmax;
			ymax = (transV.y > ymax) ? transV.y : ymax;
			zmax = (transV.z > zmax) ? transV.z : zmax;
		}
		m_aabb.set(glm::vec3(xmax, ymax, zmax), glm::vec3(xmin, ymin, zmin));
		m_aabbOriginal=m_aabb;
		m_last = trans;
	}

	void MeshSceneNode::transformAABB(const glm::mat4 &trans)
	{
		const glm::vec3 &old_max=m_aabbOriginal.m_max;
		const glm::vec3 &old_min=m_aabbOriginal.m_min;

		std::vector<glm::vec3> box; box.resize(8);
		box[0]={old_max.x, old_max.y, old_max.z};
		box[1]={old_min.x, old_max.y, old_max.z};
		box[2]={old_max.x, old_min.y, old_max.z};
		box[3]={old_max.x, old_max.y, old_min.z};
		box[4]={old_min.x, old_min.y, old_max.z};
		box[5]={old_max.x, old_min.y, old_min.z};
		box[6]={old_min.x, old_max.y, old_min.z};
		box[7]={old_min.x, old_min.y, old_min.z};

		float xmin=INF, ymin=INF, zmin=INF, xmax=-INF, ymax=-INF, zmax=-INF;

		for(uint32_t v=0; v<box.size(); v++)
		{	//todo: pos * tmpmatrix
			glm::vec4 transV = trans * glm::vec4(box[v], 1);
			xmin = (transV.x < xmin) ? transV.x : xmin;
			ymin = (transV.y < ymin) ? transV.y : ymin;
			zmin = (transV.z < zmin) ? transV.z : zmin;
			xmax = (transV.x > xmax) ? transV.x : xmax;
			ymax = (transV.y > ymax) ? transV.y : ymax;
			zmax = (transV.z > zmax) ? transV.z : zmax;
		}

		m_aabb.set({xmax, ymax, zmax}, {xmin, ymin, zmin});
		m_last = trans;
	}
}