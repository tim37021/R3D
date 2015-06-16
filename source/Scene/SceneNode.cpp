#include <r3d/Scene/SceneNode.hpp>

namespace r3d
{
	SceneNode::SceneNode(SceneNodePtr parent, ContextWindow *cw, const char *name, Transformation relative):
		m_parent(parent.get()), m_cw(cw), m_relative(relative), m_name(name)
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