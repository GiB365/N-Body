cmake_minimum_required(VERSION 3.10)
project(nbody_sim)

add_subdirectory(external/glfw)

find_package(GLEW REQUIRED)
include_directories(${GLEW_INCLUDE_DIRS})
link_libraries(${GLEW_LIBRARIES})

add_executable(nbody_sim src/main.cpp src/Renderer.cpp src/Simulator.cpp src/Quadtree.cpp)
target_link_libraries(nbody_sim PRIVATE glfw)
