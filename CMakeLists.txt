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


add_executable(nbody_sim src/main.cpp src/Renderer.cpp src/Simulator.cpp src/Quadtree.cpp)

#add GLM
target_include_directories(nbody_sim PRIVATE external/glm)

#find_package(OpenGL)
#target_link_libraries(nbody_sim PRIVATE glew glfw OpenGL::GL)
target_link_libraries(nbody_sim PRIVATE glew glfw opengl32)
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static-libgcc -static-libstdc++")

