#ifndef __R3D_SCENE_SCENENODE_H_
#define __R3D_SCENE_SCENENODE_H_

#include <memory>
#include <list>
#include <string>
#include <cstdint>

#include <glm/glm.hpp>
#include <r3d/Core/Transformation.hpp>
#include <r3d/Material/Material.hpp>
// #include <r3d/Core/AABB.hpp>

namespace r3d
{
	class ContextWindow;
	class Renderer;
	class SceneNode;
	class Program;
	class Camera;
	class Program;

	typedef std::shared_ptr<SceneNode> SceneNodePtr;

	//! Scene Interface
	/**
		Scene node is a node in the hierarchical scene graph.
		Every scene node may have children, which are also 
		scene nodes. Children move relative to their parent's 
		position.
	*/
	class SceneNode
	{
	public:
		SceneNode(SceneNodePtr parent, ContextWindow *cw, const char *name,
			Transformation relative);
		virtual ~SceneNode(){}

		//!< render this node
		virtual void render(Renderer *renderer, Program *program, Camera *cam, 
			const glm::mat4 &current=glm::mat4(1.0f), const glm::mat4 &currentRotation=glm::mat4(1.0f))=0;

		//!< get the name of the node.
		virtual const char *getName() const
		{ return m_name.c_str(); }

		//!< set the name of the node.
		//!< \param newName New name of the node.
		virtual void setName(const char *newName)
		{ m_name=newName; }

		virtual void setMaterial(MaterialPtr m)
		{ m_material=m; }

		virtual MaterialPtr getMaterial() const
		{ return m_material; }

		//!< OnAnimate is called when drawing the whole scene.
		//!< \param timeMS Current time in miliseconds.
		virtual void OnAnimate(uint32_t timeMS)
		{
			// to be implemented by derived classes
		}

		virtual void setVisible(bool value)
		{ m_isVisible=value; }


		virtual void addChild(SceneNodePtr newChild)
		{
			m_children.push_back(newChild); 
			newChild->m_parent=this;
		}

		virtual void removeChild(const char *name);

		Transformation *getTransformation()
		{ return &m_relative; }

		const std::list<SceneNodePtr> &getChildren() const { return m_children; }

		const uint32_t getID() const { return m_id; }

	protected:
		//!< pointer to its parent node.
		SceneNode *m_parent;

		ContextWindow *m_cw;

		//!< children of the node.
		std::list<SceneNodePtr> m_children;

		//!< Relative transformation to its parent
		Transformation m_relative;

		//!< it of the node
		uint32_t m_id;

	private:
		//!< name of the node.
		std::string m_name;

		//!< visibility of the node, does not affect its child
		bool m_isVisible;

		//!< the node is currently animating or stopped.
		bool m_isFreezed;


		MaterialPtr m_material;

		static uint32_t IDCounter;
	};
}

#endif