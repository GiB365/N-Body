cmake_minimum_required(VERSION 3.10)
project(nbody_sim)

set(CMAKE_CXX_STANDARD 17)
set(OpenGL_GL_PREFERENCE GLVND)

find_package(OpenGL REQUIRED)
find_package(PkgConfig REQUIRED)
find_package(glfw3 REQUIRED)
find_package(GLEW REQUIRED)

pkg_check_modules(GLFW REQUIRED glfw3)
pkg_check_modules(GLEW REQUIRED glew)

include_directories(${GLFW_INCLUDE_DIRS} ${GLEW_INCLUDE_DIRS})
link_directories(${GLFW_LIBRARY_DIRS} ${GLEW_LIBRARY_DIRS})

message(STATUS "GLEW libraries: ${GLEW_LIBRARIES}")
set(CMAKE_BUILD_TYPE Debug)

add_executable(nbody_sim main.cpp Renderer.cpp Simulator.cpp Quadtree.cpp)
target_link_libraries(nbody_sim PRIVATE ${GLFW_LIBRARIES} ${GLEW_LIBRARIES} OpenGL::GL)
