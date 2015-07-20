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
		Shape &shape, bool useTangent, const char *name_, const Transformation &relative):
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

		if(useTangent)
		{
			// construct tangent and bitangent vector if useTangent is true
			m_vertexTangents.resize(m_vertices.size());
			for(uint32_t i=0; i<shape.mesh.indices.size(); i+=3)
			{
				const glm::vec3 &o = m_vertices[shape.mesh.indices[i]].pos;
				const glm::vec3 &a = m_vertices[shape.mesh.indices[i+1]].pos;
				const glm::vec3 &b = m_vertices[shape.mesh.indices[i+2]].pos;

				const glm::vec2 &uvo = m_vertices[shape.mesh.indices[i]].texCoord;
				const glm::vec2 &uva = m_vertices[shape.mesh.indices[i+1]].texCoord;
				const glm::vec2 &uvb = m_vertices[shape.mesh.indices[i+2]].texCoord;

				// Edges of the triangle : position delta
				const glm::vec3 &o_a = a-o;
				const glm::vec3 &o_b = b-o;

				// UV delta
				const glm::vec2 &uv_o_a = uva-uvo;
				const glm::vec2 &uv_o_b = uvb-uvo;

				float r = 1.0f / (uv_o_a.x*uv_o_b.y - uv_o_a.y*uv_o_b.x);
				const glm::vec3 &tangent = (o_a*uv_o_b.y - o_b*uv_o_a.y)*r;
				const glm::vec3 &bitangent = (o_b*uv_o_a.x - o_a*uv_o_b.x)*r;

				m_vertexTangents[shape.mesh.indices[i]].tangent+=tangent;
				m_vertexTangents[shape.mesh.indices[i]].bitangent+=bitangent;
				m_vertexTangents[shape.mesh.indices[i+1]].tangent+=tangent;
				m_vertexTangents[shape.mesh.indices[i+1]].bitangent+=bitangent;
				m_vertexTangents[shape.mesh.indices[i+2]].tangent+=tangent;
				m_vertexTangents[shape.mesh.indices[i+2]].bitangent+=bitangent;
			}
		}

		for(uint32_t i=0; i<m_vertexTangents.size(); i++)
		{
			m_vertexTangents[i].tangent=glm::normalize(m_vertexTangents[i].tangent);
			m_vertexTangents[i].bitangent=glm::normalize(m_vertexTangents[i].bitangent);
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
		
		uint32_t stride = sizeof(Vertex)+(useTangent? sizeof(VertexTangent): 0);

		auto vbo=bMgr->registerVertexBuffer(name, nullptr, stride*m_vertices.size(), BU_STATIC_DRAW);
		auto ibo=bMgr->registerIndexBuffer(name, shape.mesh.indices, BU_STATIC_DRAW);
	
		// upload our data manually
		void *mappedBuffer=vbo->lock(BA_WRITE_ONLY);
		for(uint32_t i=0; i<m_vertices.size(); i++)
		{
			*(Vertex *)mappedBuffer=m_vertices[i];
			if(useTangent)
			{
				void *mappedBufferTangent=(int8_t *)mappedBuffer+sizeof(Vertex);
				*(VertexTangent *)mappedBufferTangent=m_vertexTangents[i];
			}
			mappedBuffer=(uint8_t *)mappedBuffer+stride;
		}
		vbo->unlock();

		m_vao=vaoMgr->registerVertexArray(name);
		m_vao->bindVertexBuffer(vbo);
		m_vao->bindIndexBuffer(ibo);

		AttribPointer vertAtt(RT_FLOAT, 3, stride, 0);
		AttribPointer texCoordAtt(RT_FLOAT, 2, stride, 3*sizeof(float));
		AttribPointer normAtt(RT_FLOAT, 3, stride, 5*sizeof(float));
		AttribPointer tangentAtt(RT_FLOAT, 3, stride, 8*sizeof(float));
		AttribPointer bitangentAtt(RT_FLOAT, 3, stride, 11*sizeof(float));

		m_vao->enableAttribArray(0, vertAtt);
		m_vao->enableAttribArray(1, texCoordAtt);
		m_vao->enableAttribArray(2, normAtt);
		if(useTangent)
		{
			m_vao->enableAttribArray(3, tangentAtt);
			m_vao->enableAttribArray(4, bitangentAtt);
		}

		m_indicesCount=shape.mesh.indices.size();

		m_nodeType="Mesh";
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