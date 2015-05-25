#ifndef __R3D_MANAGER_MANAGERS_HPP_
#define __R3D_MANAGER_MANAGERS_HPP_

#include <r3d/Manager/VertexArrayManager.hpp>
#include <r3d/Manager/BufferManager.hpp>
#include <r3d/Manager/SceneManager.hpp>
#include <r3d/Manager/TextureManager.hpp>

namespace r3d
{
	struct Managers
	{
		VertexArrayManager *vaoMgr;
		BufferManager *bMgr;
		SceneManager *sMgr;
		TextureManager *tMgr;

		void freeAll()
		{
			delete vaoMgr;
			delete bMgr;
			delete sMgr;
			delete tMgr;
		}

		Managers(): vaoMgr(nullptr), bMgr(nullptr){}
	};
}

#endif