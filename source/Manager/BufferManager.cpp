#include <r3d/Manager/BufferManager.hpp>
#include <r3d/Core/Core.hpp>

namespace r3d
{
	BufferManager::BufferManager(Engine *engine, ContextWindow *cw)
		: m_engine(engine), m_cw(cw)
	{
	}

	BufferManager::~BufferManager()
	{
		clear();
	}

	Buffer *BufferManager::registerVertexBuffer(const std::string &name, 
		const void *data, uint32_t n, BufferUsage bu)
	{
		auto it=m_buffers.find(name+"_VBO");
		if(it==m_buffers.cend())
		{
			ContextWindow *current=m_engine->getCurrentContext();
			m_cw->makeCurrent();
			Buffer *newed=m_engine->newVertexBuffer(data, n, bu);
			current->makeCurrent();
			m_buffers[name+"_VBO"]=newed;
			return newed;
		}else
			return it->second;
	}
	Buffer *BufferManager::registerIndexBuffer(const std::string &name, 
		const std::vector<uint32_t> &indices, BufferUsage bu)
	{
		auto it=m_buffers.find(name+"_IBO");
		if(it==m_buffers.cend())
		{
			ContextWindow *current=m_engine->getCurrentContext();
			m_cw->makeCurrent();
			Buffer *newed=m_engine->newIndexBuffer(indices, bu);
			current->makeCurrent();
			m_buffers[name+"_IBO"]=newed;
			return newed;
		}else
			return it->second;
	}

	bool BufferManager::releaseBuffer(Buffer *buf)
	{
		for(auto it=m_buffers.begin(); it!=m_buffers.end(); ++it)
		{
			if(it->second==buf)
			{
				delete it->second;
				m_buffers.erase(it);
				return true;
			}
		}
		return false;
	}

	void BufferManager::clear()
	{
		ContextWindow *current=m_engine->getCurrentContext();
		m_cw->makeCurrent();
		for(auto &element: m_buffers)
			delete element.second;
		m_buffers.clear();
		current->makeCurrent();
	}
}