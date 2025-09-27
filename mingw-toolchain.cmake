# Tell CMake we are cross-compiling for Windows
set(CMAKE_SYSTEM_NAME Windows)
set(CMAKE_SYSTEM_VERSION 1)

# Compilers
set(CMAKE_C_COMPILER x86_64-w64-mingw32-gcc)
set(CMAKE_CXX_COMPILER x86_64-w64-mingw32-g++)
set(CMAKE_RC_COMPILER x86_64-w64-mingw32-windres)

# Prevent GLFW from using pthread/dl/X11
set(GLFW_USE_LIBDL OFF CACHE BOOL "Do not use libdl")
set(GLFW_USE_PTHREADS OFF CACHE BOOL "Do not use pthreads")
set(GLFW_USE_X11 OFF CACHE BOOL "Do not use X11")

