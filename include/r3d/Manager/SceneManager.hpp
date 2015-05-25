#ifndef __R3D_MANAGER_SCENE_MANAGER_HPP_
#define __R3D_MANAGER_SCENE_MANAGER_HPP_

#include <stdexcept>
#include <memory>
#include <vector>
#include <r3d/Scene/Light.hpp>

namespace r3d
{
	class SceneNode;
	class Engine;
	class Program;
	class Camera;

	typedef std::shared_ptr<SceneNode> SceneNodePtr;
	typedef std::shared_ptr<Program> ProgramPtr;
	typedef std::shared_ptr<Camera> CameraPtr;

	class SceneLoadError: std::runtime_error
	{
	public:
		explicit SceneLoadError(const std::string &what_): runtime_error(what_){}
	};

	class SceneManager
	{
	public:
		SceneManager(Engine *engine);
		void drawAll();
		//!> load Obj scene and necessary texture and buffer
		//   and put it under SceneNode
		SceneNode *loadObjScene(SceneNodePtr, const char *filename, const char *base=nullptr);
		SceneNodePtr addMeshSceneNode();
		SceneNodePtr addEmptySceneNode();

		void addLight(Light *light)
		{ lights.push_back(light); }

		void setMainCamera(CameraPtr cam){ m_camera=cam; }

		SceneNodePtr getRootNode() const { return m_rootNode; }
		std::vector<Light *> &getLights() { return lights; }
	private:
		Engine *m_engine;
		SceneNodePtr m_rootNode;
		CameraPtr m_camera;
		std::vector<Light *> lights;

		ProgramPtr m_program;
	};
}

#endif
