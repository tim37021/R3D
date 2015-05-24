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
  - Windows:
  
        cmake . -G "MinGW Makefiles"
        mingw32-make

### TODO
  - Context Resource Management
  - Document