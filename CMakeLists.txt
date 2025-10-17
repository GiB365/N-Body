cmake_minimum_required(VERSION 3.10)
project(nbody_sim)

set(CMAKE_CXX_STANDARD 17)
set(OpenGL_GL_PREFERENCE GLVND)

#add GLFW
add_subdirectory(external/glfw)

#add GLEW
add_library(glew STATIC external/glew/src/glew.c)
target_include_directories(glew PUBLIC external/glew/include)
target_compile_definitions(glew PUBLIC GLEW_STATIC)

#add Freetype
add_subdirectory(external/freetype)

add_executable(nbody_sim src/main.cpp src/Renderer.cpp src/Simulator.cpp src/Quadtree.cpp src/UIElements.cpp)

#add GLM
target_include_directories(nbody_sim PRIVATE external/glm)

#For Linux
#find_package(OpenGL REQUIRED)
#target_link_libraries(nbody_sim PRIVATE glew glfw OpenGL::GL freetype)

#For windows
target_link_libraries(nbody_sim PRIVATE glew glfw opengl32 freetype)
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static-libgcc -static-libstdc++")

