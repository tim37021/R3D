#include <r3d/Manager/VertexArrayManager.hpp>
#include <r3d/Core/Core.hpp>
#include <r3d/Window/ContextWindow.hpp>

namespace r3d
{
	VertexArrayManager::VertexArrayManager(Engine *engine, ContextWindow *cw):
		m_engine(engine), m_cw(cw)
	{

	}

	VertexArrayManager::~VertexArrayManager()
	{
		clear();
	}

	VertexArray *VertexArrayManager::registerVertexArray(const std::string &name)
	{
		auto it=m_vaoMap.find(name);
		if(it==m_vaoMap.cend())
		{
			ContextWindow *current=m_engine->getCurrentContext();
			m_cw->makeCurrent();
			VertexArray *newed=m_engine->newVertexArray();
			current->makeCurrent();
			m_vaoMap[name]=newed;
			return newed;
		}else
			return it->second;
	}

	void VertexArrayManager::clear()
	{
		ContextWindow *current=m_engine->getCurrentContext();
		m_cw->makeCurrent();
		for(auto &element: m_vaoMap)
			delete element.second;
		m_vaoMap.clear();
		current->makeCurrent();
	}
}

