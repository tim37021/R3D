#ifndef __R3D_TEST_LUA_INTERFACE_H_
#define __R3D_TEST_LUA_INTERFACE_H_


extern "C"
{
	#include <lua.h>
}
#include <cstdint>

namespace r3d
{
	class Engine;
}

class LuaInterface
{
public:
    static void Initialise(r3d::Engine *engine);
    static void OnFilesDropIn(uint32_t, const char *files[]);
};

#endif