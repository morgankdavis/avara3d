
# cmake 3.8 is required for C++17
cmake_minimum_required(VERSION 3.0)

project(avara-engine)

set(CMAKE_CXX_STANDARD 14)

if ((CMAKE_BUILD_TYPE STREQUAL "") OR (NOT DEFINED CMAKE_BUILD_TYPE))
	set(CMAKE_BUILD_TYPE "Debug")
endif ()


# print environment for debugging...

#message("ENVIRONMENT:")

#get_cmake_property(_variableNames VARIABLES)
#foreach (_variableName ${_variableNames})
#    message(STATUS "${_variableName}=${${_variableName}}")
#endforeach()

#execute_process(COMMAND "${CMAKE_COMMAND}" "-E" "environment")

message("*** BUILD TYPE: " ${CMAKE_BUILD_TYPE} " ***")


if (UNIX AND APPLE)
    message("We're MACOS")
    set(MACOS TRUE)
    if (CMAKE_GENERATOR STREQUAL "Xcode")
        set(XCODE TRUE)
    endif()
endif()

if (UNIX AND NOT APPLE AND NOT ANDROID AND NOT MINGW)
    message("We're LINUX")
    set(LINUX TRUE)
endif()

if (WIN32)
    message("We're WIN32")
    set(WINDOWS TRUE)
    set(WIN32 TRUE)
endif()

if (WIN64)
    message("We're WIN64")
    set(WINDOWS TRUE)
    set(WIN64 TRUE)
endif()

if (MINGW)
    message("We're MINGW")
    set(WINDOWS TRUE)

    if (CMAKE_BUILD_TOOL STREQUAL "C:/MinGW/bin/mingw32-make.exe")
        message("*** MINGW32 ***")
        set(MINGW32 TRUE)
        set(WIN32 TRUE)
    else()
        message("*** MINGW64 ***")
        set(MINGW64 TRUE)
        set(WIN64 TRUE)
    endif()
endif()

if (ANDROID)
    message("We're ANDROID")
    set(ANDROID TRUE)
endif()

if (MSYS)
    message("We're MSYS")
	set(MSYS TRUE)
endif()

if (MSVS)
    message("We're MSVS")
endif()

if (MACOS OR WINDOWS OR LINUX)
	message("We're DESKTOP")
    set(DESKTOP TRUE)
endif()


if (XCODE)
	message("We're XCODE")
	add_definitions("-DXCODE")
endif()
if (MACOS)
	add_definitions("-DAPPLE" "-DMACOS")
elseif (LINUX)
	add_definitions("-DLINUX")
elseif (WINDOWS)
	add_definitions("-DWINDOWS")
    if (MINGW)
        add_definitions("-DMINGW")
	if (MINGW32)
        	add_definitions("-DMINGW32")
	else()
		add_definitions("-DMINGW64")
    	endif()
    endif()
elseif (ANDROID)
	add_definitions("-DANDROID")
endif()
if (DESKTOP)
	add_definitions("-DDESKTOP")
endif()

if (UBUNTU_CROSS)
	message("We're UBUNTU_CROSS")
	add_definitions("-DUBUNTU_CROSS")
endif ()



# libraries (headers...) that the test apps depend on

# GLM

include_directories(avara-engine/external/glm-0.9.9)

# boost

include_directories(avara-engine/external/boost-1.67.0)
add_subdirectory(avara-engine/external/boost-1.67.0)

# spdlog

add_subdirectory(avara-engine/external/spdlog-0.16.2)
include_directories(avara-engine/external/spdlog-0.16.2/include)


# android things

if (ANDROID)
	add_subdirectory(android)
endif()


# install location

if ((CMAKE_BUILD_TYPE STREQUAL "Release") OR (CMAKE_BUILD_TYPE STREQUAL "MinSizeRel"))
	set(ROOT_INSTALL_DIR "${PROJECT_BINARY_DIR}/packaged/release")
else()
	set(ROOT_INSTALL_DIR "${PROJECT_BINARY_DIR}/packaged/debug")
endif()


# ae & tests

add_subdirectory(avara-engine)

if (DESKTOP)
	add_subdirectory(tests)
endif()

