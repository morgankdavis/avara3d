
cmake_minimum_required(VERSION 3.0)

project(avara-engine)


if(UNIX AND APPLE)
    set(MACOS TRUE)
    if(CMAKE_GENERATOR STREQUAL "Xcode")
        set(XCODE TRUE)
    endif()
endif()

if(UNIX AND NOT APPLE)
    set(LINUX TRUE)
endif()

if(WIN32)
    set(WINDOWS TRUE)
endif()

#if(MSVS OR MSYS OR MINGW)
#    set(Visual studio?)
#endif()


if(XCODE)
	add_definitions("-DAPPLE" "-DMACOS" "-DXCODE")
elseif(MACOS)
	add_definitions("-DAPPLE" "-DMACOS")
elseif(LINUX)
	add_definitions("-DLINUX")
elseif(WINDOWS)
	add_definitions("-DWINDOWS")
endif()



# GLM

include_directories(avara-engine/external/glm-0.9.9.9)

# GLFW

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory(avara-engine/external/glfw-3.2.1)
if(WINDOWS)
	include_directories(avara-engine/external/glfw-3.2.1/include)
endif()

# GLEW

add_subdirectory(avara-engine/external/glew-2.1.0/build/cmake)
include_directories(avara-engine/external/glew-2.1.0/include)

# assimp

set(ENABLE_BOOST_WORKAROUND ON CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_ASSIMP_TOOLS OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_SAMPLES OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(ASSIMP_NO_EXPORT ON CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_ZLIB ON CACHE BOOL "" FORCE) # needed for windows?

# we always seem to have trouble importing the headers for assimp 4.0.1... 
#add_subdirectory(avara-engine/external/assimp-4.0.1)
#include_directories(avara-engine/external/assimp-4.0.1/include)

add_subdirectory(avara-engine/external/assimp-3.3.1)
include_directories(avara-engine/external/assimp-3.3.1/include)

# boost

#if(WINDOWS)
#	include_directories(avara-engine/external/boost_1_65_1-win)
#else()
	include_directories(avara-engine/external/boost_1_65_1)
#endif()


add_subdirectory(avara-engine)
add_subdirectory(tests)



