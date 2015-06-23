#ifndef __R3D_SCENE_EMPTY_SCENENODE_HPP_
#define __R3D_SCENE_EMPTY_SCENENODE_HPP_

#include <r3d/Scene/SceneNode.hpp>

namespace r3d
{
	class Renderer;
	class EmptySceneNode: public SceneNode
	{
	public:
		EmptySceneNode(SceneNodePtr parent, 
			ContextWindow *cw,
			const char *name="Untitled", 
			const Transformation &relative=Transformation()
			):
			SceneNode(parent, cw, name, relative)
		{
			m_nodeType="Empty";
		}
		virtual void render(Renderer *renderer, Program *program, Camera *cam, 
			const glm::mat4 &current=glm::mat4(1.0f), const glm::mat4 &currentRotation=glm::mat4(1.0f))
		{ 
			// Don't draw! 
		}
	};
}

#endif