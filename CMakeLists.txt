
# cmake 3.8 is required for C++17
cmake_minimum_required(VERSION 3.0)

project(avara-engine)


# print environment for debugging...

#message("ENVIRONMENT:")

#get_cmake_property(_variableNames VARIABLES)
#foreach (_variableName ${_variableNames})
#    message(STATUS "${_variableName}=${${_variableName}}")
#endforeach()

#execute_process(COMMAND "${CMAKE_COMMAND}" "-E" "environment")


if(UNIX AND APPLE)
    message("We think we're APPLE")
    set(MACOS TRUE)
    if(CMAKE_GENERATOR STREQUAL "Xcode")
        set(XCODE TRUE)
    endif()
endif()

if(UNIX AND NOT APPLE)
    message("We think we're LINUX")
    set(LINUX TRUE)
endif()

if(WIN32)
    message("We think we're WIN32")
    set(WINDOWS TRUE)
    set(WIN32 TRUE)
endif()

if(WIN64)
    message("We think we're WIN64")
    set(WINDOWS TRUE)
    set(WIN64 TRUE)
endif()


if(MINGW)
    message("We think we're MINGW")
    set(WINDOWS TRUE)

    if(CMAKE_BUILD_TOOL STREQUAL "C:/MinGW/bin/mingw32-make.exe")
        message("*** MINGW32 ***")
        set(MINGW32 TRUE)
        set(WIN32 TRUE)
    else()
        message("*** MINGW64 ***")
        set(MINGW64 TRUE)
        set(WIN64 TRUE)
    endif()
endif()

if(MSVS)
    message("We think we're MSVS")
endif()

if(MSYS)
    message("We think we're MSYS")
endif()


if(XCODE)
	add_definitions("-DAPPLE" "-DMACOS" "-DXCODE")
elseif(MACOS)
	add_definitions("-DAPPLE" "-DMACOS")
elseif(LINUX)
	add_definitions("-DLINUX")
elseif(WINDOWS)
	add_definitions("-DWINDOWS")
    if(MINGW)
        add_definitions("-DMINGW")
	if(MINGW32)
        	add_definitions("-DMINGW32")
	else()
		add_definitions("-DMINGW64")
    	endif()
    endif()
endif()


# GLM

include_directories(avara-engine/external/glm-0.9.9-a2)

# GLFW

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory(avara-engine/external/glfw-3.2.1)
if(WINDOWS) # donno
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
if(WINDOWS)
	set(ASSIMP_BUILD_ZLIB ON CACHE BOOL "" FORCE) # needed for windows?
endif()
add_subdirectory(avara-engine/external/assimp-3.3.1)
include_directories(avara-engine/external/assimp-3.3.1/include)

# boost

#include_directories(avara-engine/external/boost_1_65_1)
include_directories(avara-engine/external/boost-1.66.0)
add_subdirectory(avara-engine/external/boost-1.66.0)

# spdlog

add_subdirectory(avara-engine/external/spdlog-0.16.2)
include_directories(avara-engine/external/spdlog-0.16.2/include)

# stb

include_directories(avara-engine/external/stb)

# fontstash

include_directories(avara-engine/external/fontstash)

# bullet
include_directories(avara-engine/external/bullet3-2.87/src)


add_subdirectory(avara-engine)
add_subdirectory(tests)
