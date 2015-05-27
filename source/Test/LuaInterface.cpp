#include "LuaInterface.h"
#include <r3d/r3d.hpp>
#include <glm/glm.hpp>
#include <Rocket/Core.h>
#include <Rocket/Core/Lua/Interpreter.h>

static r3d::Engine *engine;
static lua_State *L;
static lua_State *luacmd;

static int LUALoadObjScene(lua_State *L)
{
	const char *filename=lua_tostring(L, 1);
	glm::vec3 t, r, z;
	t.x=lua_tonumber(L, 2);
	t.y=lua_tonumber(L, 3);
	t.z=lua_tonumber(L, 4);

	r.x=lua_tonumber(L, 5);
	r.y=lua_tonumber(L, 6);
	r.z=lua_tonumber(L, 7);

	z.x=z.y=z.z=lua_tonumber(L, 8);

	auto cw=engine->getCurrentContext();
	auto sMgr=cw->getSceneManager();
	auto node=sMgr->loadObjScene(sMgr->getRootNode(), filename);

	node->getTransformation()->setTranslation(t);
	node->getTransformation()->setRotation(r);
	node->getTransformation()->setScale(z);
	return 0;
}

void LuaInterface::OnFilesDropIn(uint32_t count, const char *files[])
{
	lua_getglobal(L, "OnFilesDropIn");
	lua_createtable(L, 0, count);
	for(uint32_t i=0; i<count; i++)
	{
		// Lua array start from 1
		lua_pushnumber(L, i+1);
		lua_pushstring(L, files[i]);
		lua_settable(L, -3);
	}

	// We wish Rocket
	Rocket::Core::Lua::Interpreter::ExecuteCall(1, 0);
}

void LuaInterface::updatemspos(int x,int y)
{
	lua_getglobal(L, "updatems");
		// Lua array start from 1
	lua_pushnumber(L, x);
	lua_pushnumber(L, y);

	// We wish Rocket
	Rocket::Core::Lua::Interpreter::ExecuteCall(2, 0);
}

void LuaInterface::Initialise(r3d::Engine *engine)
{
	::engine=engine;
	L=Rocket::Core::Lua::Interpreter::GetLuaState();

	lua_register(L, "LoadObjScene", LUALoadObjScene);  
}