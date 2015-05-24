# R3D Engine
Real-time 3D Engine for our graduated project, it is still under early development.

### Feature
  - Deferred Rendering (working)
  - Physically Based Rendering (working)
  - Level Editor
  - Lua scripting
  - Multiple Context Management (working)
  - Low/High Level Rendering API Available

### Dependency and third-party
  - GLFW
  - GLM
  - Tiny Obj Loader
  - libRocket
  - Lua5.1(LuaJIT) / Lua 5.3(Offical Lua)

### How to Build

  - If you wish to use Lua 53, unzip libRocketLua53.zip, and type the following commands. (You may need to rebuild libRocket on your own)
  - NOTE: We are using LUAJIT for Lua 5.1, which has faster implementation than the offical one.
  
        cmake . -G "MinGW Makefiles" -DLUA_VERSION=53
        make

### Directory Description
  - Any thing put in include directory is for user to export functionalities for library user
  - Any thing put in source directory is to hide detail information to library user 

### TODO
  - Context Resource Management
  - Document