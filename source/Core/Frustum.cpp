#include <glm/glm.hpp>
#include "Frustum.hpp"
#include <cmath>
#include <r3d/Core/AABB.hpp>

#define PI 3.141592

namespace r3d{
	Frustum::Frustum(){

	}

	void Frustum::setFrustum(glm::vec3 m_pos, glm::vec3 m_dir, glm::vec3 m_up, float fov, float ratio, float far, float near){

		float nh, nw, fh, fw;
		float tanOfFov = (float)tan(0.5*fov*PI/180);
		nh = near * tanOfFov;
		nw = nh * ratio; 
		fh = far  * tanOfFov;
		fw = fh * ratio;

		glm::vec3 cam_x, cam_y, cam_z; 
		cam_z = m_dir;
		cam_z = normalize(cam_z);

		cam_x = -glm::cross(m_up, cam_z);
		cam_x = normalize(cam_x);

		cam_y = glm::cross(cam_x, cam_z);
		cam_y = normalize(cam_y); 

		glm::vec3 nc = m_pos + cam_z*near;
		glm::vec3 fc = m_pos + cam_z*far;
		
		glm::vec3 ntl = nc + cam_y * nh - cam_x * nw;
		glm::vec3 ntr = nc + cam_y * nh + cam_x * nw;
		glm::vec3 nbl = nc - cam_y * nh - cam_x * nw;
		glm::vec3 nbr = nc - cam_y * nh + cam_x * nw;
		glm::vec3 ftl = fc + cam_y * fh - cam_x * fw;
		glm::vec3 ftr = fc + cam_y * fh + cam_x * fw;
		glm::vec3 fbl = fc - cam_y * fh - cam_x * fw;
		glm::vec3 fbr = fc - cam_y * fh + cam_x * fw; 

		glm::vec4 tmp;
		p.push_back(calcPlane(ntr, ntl, ftl));//TOP
		p.push_back(calcPlane(nbl, nbr, fbr));//BOTTOM
		p.push_back(calcPlane(ntl, nbl, fbl));//LEFT
		p.push_back(calcPlane(nbr, ntr, fbr));//RIGHT
		p.push_back(calcPlane(ntl, ntr, nbr));//NEAR
		p.push_back(calcPlane(ftr, ftl, fbl));//FAR
	}

	glm::vec4 Frustum::calcPlane(glm::vec3 a, glm::vec3 b, glm::vec3 c){
		glm::vec3 v1, v2;
		v1 =  a - b;
		v2 =  c - b;

		glm::vec3 normal = glm::cross(v1, v2);
		normal = normalize(normal);
		float d = -(glm::dot(normal, a));

		return glm::vec4(normal, d);
	}

	bool Frustum::AABBinFrustum(AABB aabb)
	{
		bool result = true;
		for(int i=0; i < 6; i++) {

			float distP, distN;

			distP = p[i].w + glm::dot(aabb.getVertexP(glm::vec3(p[i].x, p[i].y, p[i].z)), glm::vec3(p[i].x, p[i].y, p[i].z));
			distN = p[i].w + glm::dot(aabb.getVertexN(glm::vec3(p[i].x, p[i].y, p[i].z)), glm::vec3(p[i].x, p[i].y, p[i].z));
			
			if (distP < 0)
				return false;
			else if (distN < 0)
				result = true;
		}
		return result;
	// 	bool result;
	// 	int out,in;
	// 	float dist;
	// 	result = true;
	// 	// for each plane do ...
	// 	for(int i=0; i < 6; i++) {

	// 		// reset counters for corners in and out
	// 		out=0;in=0;
	// 		// for each corner of the box do ...
	// 		// get out of the cycle as soon as a box as corners
	// 		// both inside and out of the frustum
	// 		if(in==0 || out==0) {
	// 			// is the corner outside or inside
	// 			dist = p[i].w + glm::dot(glm::vec3(aabb.m_max.x, aabb.m_max.y, aabb.m_max.z), glm::vec3(p[i].x, p[i].y, p[i].z));
	// 			if (dist < 0)
	// 				out++;
	// 			else
	// 				in++;
	// 		}
	// 		if(in==0 || out==0) {
	// 			// is the corner outside or inside
	// 			dist = p[i].w + glm::dot(glm::vec3(aabb.m_min.x, aabb.m_max.y, aabb.m_max.z), glm::vec3(p[i].x, p[i].y, p[i].z));
	// 			if (dist < 0)
	// 				out++;
	// 			else
	// 				in++;
	// 		}
	// 		if(in==0 || out==0) {
	// 			// is the corner outside or inside
	// 			dist = p[i].w + glm::dot(glm::vec3(aabb.m_max.x, aabb.m_min.y, aabb.m_max.z), glm::vec3(p[i].x, p[i].y, p[i].z));
	// 			if (dist < 0)
	// 				out++;
	// 			else
	// 				in++;
	// 		}
	// 		if(in==0 || out==0) {
	// 			// is the corner outside or inside
	// 			dist = p[i].w + glm::dot(glm::vec3(aabb.m_max.x, aabb.m_max.y, aabb.m_min.z), glm::vec3(p[i].x, p[i].y, p[i].z));
	// 			if (dist < 0)
	// 				out++;
	// 			else
	// 				in++;
	// 		}
	// 		if(in==0 || out==0) {
	// 			// is the corner outside or inside
	// 			dist = p[i].w + glm::dot(glm::vec3(aabb.m_min.x, aabb.m_min.y, aabb.m_max.z), glm::vec3(p[i].x, p[i].y, p[i].z));
	// 			if (dist < 0)
	// 				out++;
	// 			else
	// 				in++;
	// 		}
	// 		if(in==0 || out==0) {
	// 			// is the corner outside or inside
	// 			dist = p[i].w + glm::dot(glm::vec3(aabb.m_max.x, aabb.m_min.y, aabb.m_min.z), glm::vec3(p[i].x, p[i].y, p[i].z));
	// 			if (dist < 0)
	// 				out++;
	// 			else
	// 				in++;
	// 		}
	// 		if(in==0 || out==0) {
	// 			// is the corner outside or inside
	// 			dist = p[i].w + glm::dot(glm::vec3(aabb.m_min.x, aabb.m_max.y, aabb.m_min.z), glm::vec3(p[i].x, p[i].y, p[i].z));
	// 			if (dist < 0)
	// 				out++;
	// 			else
	// 				in++;
	// 		}
	// 		if(in==0 || out==0) {
	// 			// is the corner outside or inside
	// 			dist = p[i].w + glm::dot(glm::vec3(aabb.m_min.x, aabb.m_min.y, aabb.m_min.z), glm::vec3(p[i].x, p[i].y, p[i].z));
	// 			if (dist < 0)
	// 				out++;
	// 			else
	// 				in++;
	// 		}
	// 		//if all corners are out
	// 		if (!in)
	// 			return false;
	// 		// if some corners are out and others are in
	// 		else if (out)
	// 			result = true;
	// 	}
	// 	return result;
	}
}