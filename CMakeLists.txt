cmake_minimum_required(VERSION 3.10)
project(nbody_sim)

set(CMAKE_CXX_STANDARD 17)

find_package(OpenGL REQUIRED)
find_package(PkgConfig REQUIRED)
find_package(glfw3 REQUIRED)

pkg_search_module(GLEW REQUIRED glew)

add_executable(nbody_sim main.cpp)
target_include_directories(nbody_sim PRIVATE /usr/include)
target_link_libraries(nbody_sim PRIVATE glfw OpenGL::GL)
