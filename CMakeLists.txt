cmake_minimum_required (VERSION 2.6)
project (R3D)
find_package(OpenGL)

SET(PROJECT_DIR .)
SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ./lib)


set(CMAKE_CXX_FLAGS "-std=c++0x -Wall -DGLEW_STATIC")
set(CMAKE_C_FLAGS "-DGLEW_STATIC")
set(CMAKE_BUILD_TYPE Debug)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_subdirectory(source)
add_subdirectory(lib)

add_library(glew ./lib/glew.c)
add_library(jpeg ${JPEG_SOURCE})
add_library(png ${PNG_SOURCE})
add_library(z SHARED  ${ZLIB_SOURCE})
set_target_properties(z PROPERTIES OUTPUT_NAME zlib1)
set_target_properties(z PROPERTIES PREFIX "")


include_directories(./include ./lib ./lib/lua/include)
link_directories(./lib)
add_library(r3d ${SOURCE})

add_executable(R3DTest ./source/Test/test.cpp ./source/Test/r3drocket.cpp ./source/Test/LuaInterface.cpp)
TARGET_LINK_LIBRARIES(R3DTest r3d)
TARGET_LINK_LIBRARIES(R3DTest glew)
TARGET_LINK_LIBRARIES(R3DTest jpeg)
TARGET_LINK_LIBRARIES(R3DTest png)
TARGET_LINK_LIBRARIES(R3DTest z)
if(WIN32)
	TARGET_LINK_LIBRARIES(R3DTest -lRocketCore.dll -lRocketCoreLua.dll -lRocketControls.dll -lRocketControlsLua.dll -lRocketDebugger.dll -lglfw3 -pthread -lopengl32 -lgdi32 -lm -llua53)
else()
	TARGET_LINK_LIBRARIES(R3DTest -lglfw3 -pthread -lX11 -lXrandr -lXi -lXxf86vm -lXinerama -lXcursor -lGL -lm)
endif()