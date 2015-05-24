#ifndef __CORE_BUFFER_MANAGER_HPP_
#define __CORE_BUFFER_MANAGER_HPP_

#include <r3d/Core/Buffer.hpp>
#include <map>
#include <vector>
#include <string>

namespace r3d
{
	class Engine;
	class ContextWindow;
	class BufferManager
	{
	public:
		BufferManager(Engine *, ContextWindow *);
		~BufferManager();

		//!> If name already exists, return existance buffer
		Buffer *registerVertexBuffer(const std::string &name, const void *data, uint32_t n, BufferUsage bu);
		Buffer *registerIndexBuffer(const std::string &name, const std::vector<uint32_t> &indices, BufferUsage bu);

		bool releaseBuffer(Buffer *);
		void clear();
	private:
		Engine *m_engine;
		ContextWindow *m_cw;
		std::map<std::string, Buffer *> m_buffers;
	};
}

#endif