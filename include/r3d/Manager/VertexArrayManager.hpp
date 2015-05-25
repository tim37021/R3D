#ifndef __R3D_CORE_VERTEX_ARRAY_MANAGER_HPP_
#define __R3D_CORE_VERTEX_ARRAY_MANAGER_HPP_

#include <map>
#include <string>

namespace r3d
{
	class Engine;
	class ContextWindow;
	class VertexArray;
	class VertexArrayManager
	{
	public:
		VertexArrayManager(Engine *, ContextWindow *);
		~VertexArrayManager();

		VertexArray *registerVertexArray(const std::string &name);

		void clear();
	private:
		Engine *m_engine;
		ContextWindow *m_cw;

		std::map<std::string, VertexArray *> m_vaoMap;
	};
}

#endif