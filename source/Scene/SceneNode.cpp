#include <r3d/Scene/SceneNode.hpp>
#include <cstdio>
namespace r3d
{
	// Start from 1
	uint32_t SceneNode::IDCounter=1;

	SceneNode::SceneNode(SceneNodePtr parent, ContextWindow *cw, const char *name, const Transformation &relative):
		m_parent(parent.get()), m_cw(cw), m_relative(relative), m_name(name), m_id(IDCounter++)
	{

	}

	void SceneNode::removeChild(const char *name)
	{
		for(auto it=m_children.begin(); it!=m_children.end(); ++it)
		{
			if((*it)->m_name==name)
			{
				m_children.erase(it);
				break;
			}
		}
	}
}