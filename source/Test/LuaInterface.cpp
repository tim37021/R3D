#include "LuaInterface.h"
#include <r3d/r3d.hpp>
#include <glm/glm.hpp>
#include <Rocket/Core.h>
#include <Rocket/Core/Lua/Interpreter.h>
#include "SceneNodeList.cpp"
#include <cstdio>

static r3d::Engine *engine;
static lua_State *L;
static int LUASetRotation(lua_State *L){
	glm::vec3 r;
	r.x=lua_tonumber(L, 2);
	r.y=lua_tonumber(L, 3);
	r.z=lua_tonumber(L, 4);

	lua_getfield(L, 1, "ptr");

	auto *node=static_cast<r3d::SceneNode *>(lua_touserdata(L,-1));
	node->getTransformation()->setRotation(r);
	return 0;
}

static int LUAGetRotation(lua_State *L){
	const char* ch=lua_tostring(L,2);
	lua_getfield(L, 1, "ptr");
	auto *node=static_cast<r3d::SceneNode *>(lua_touserdata(L,-1));
	if(ch[0]=='x'){
		lua_pushnumber(L,node->getTransformation()->getRotation().x);
	}else if(ch[0]=='y'){
		lua_pushnumber(L,node->getTransformation()->getRotation().y);
	}else{
		lua_pushnumber(L,node->getTransformation()->getRotation().z);
	}
	return 1;
}

static int LUASetTranslation(lua_State *L){
	glm::vec3 r;
	r.x=lua_tonumber(L, 2);
	r.y=lua_tonumber(L, 3);
	r.z=lua_tonumber(L, 4);

	lua_getfield(L, 1, "ptr");

	auto *node=static_cast<r3d::SceneNode *>(lua_touserdata(L,-1));
	node->getTransformation()->setTranslation(r);
	return 0;
}

static int LUAGetTranslation(lua_State *L){
	const char* ch=lua_tostring(L,2);
	lua_getfield(L, 1, "ptr");
	auto *node=static_cast<r3d::SceneNode *>(lua_touserdata(L,-1));
	if(ch[0]=='x'){
		lua_pushnumber(L,node->getTransformation()->getTranslation().x);
	}else if(ch[0]=='y'){
		lua_pushnumber(L,node->getTransformation()->getTranslation().y);
	}else{
		lua_pushnumber(L,node->getTransformation()->getTranslation().z);
	}
	return 1;
}

static int LUASetScale(lua_State *L){
	glm::vec3 r;
	r.x=r.y=r.z=lua_tonumber(L, 2);

	lua_getfield(L, 1, "ptr");

	auto *node=static_cast<r3d::SceneNode *>(lua_touserdata(L,-1));
	node->getTransformation()->setScale(r);
	return 0;
}

static int LUAGetScale(lua_State *L){
	lua_getfield(L, 1, "ptr");
	auto *node=static_cast<r3d::SceneNode *>(lua_touserdata(L,-1));
	lua_pushnumber(L,node->getTransformation()->getScale().x);
	return 1;
}

static int LUALoadObjScene(lua_State *L)
{
	const char *filename=lua_tostring(L, 1);
	
	auto cw=engine->getCurrentContext();
	auto sMgr=cw->getSceneManager();
	auto node=sMgr->loadObjScene(sMgr->getRootNode(), filename);
	scene.addnode(node->getName(),"light",node);
	
	lua_createtable(L, 0, 7);
	lua_pushstring(L, "ptr");
	lua_pushlightuserdata(L, node);
	lua_settable(L, -3);
	lua_pushstring(L, "SetRotation");
	lua_pushcfunction (L, LUASetRotation);
	lua_settable(L, -3);
	lua_pushstring(L, "GetRotation");
	lua_pushcfunction (L, LUAGetRotation);
	lua_settable(L, -3);
	lua_pushstring(L, "SetTranslation");
	lua_pushcfunction (L, LUASetTranslation);
	lua_settable(L, -3);
	lua_pushstring(L, "GetTranslation");
	lua_pushcfunction (L, LUAGetTranslation);
	lua_settable(L, -3);
	lua_pushstring(L, "SetScale");
	lua_pushcfunction (L, LUASetScale);
	lua_settable(L, -3);
	lua_pushstring(L, "GetScale");
	lua_pushcfunction (L, LUAGetScale);
	lua_settable(L, -3);
	return 1;
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
	lua_pushnumber(L, x);
	lua_pushnumber(L, y);
	Rocket::Core::Lua::Interpreter::ExecuteCall(2, 0);
}

void LuaInterface::SetSelectObject(r3d::SceneNode *node)
{
	lua_getglobal(L, "obj_sel");
	if (lua_istable(L, -1))
	{
      lua_pushstring(L, "ptr");
      lua_pushlightuserdata(L, node);
      lua_settable(L, -3);
	}
}

void LuaInterface::Initialise(r3d::Engine *engine)
{
	::engine=engine;
	L=Rocket::Core::Lua::Interpreter::GetLuaState();

	lua_register(L, "LoadObjScene", LUALoadObjScene);  
}