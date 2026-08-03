cmake_minimum_required(VERSION 3.20)

project(avara3d
		VERSION 0.1.0
		HOMEPAGE_URL https://avara3d.net/)

include(cmake/CompileDefinitions.cmake)
include(cmake/EmscriptenHelpers.cmake)
include(cmake/EnvironmentDetect.cmake)
include(cmake/ExternalVirtualInclude.cmake)
include(cmake/UtilityFunctions.cmake)

option(A3D_BUILD_TESTS "Build A3D tests" ON)
option(A3D_BUILD_DEMOS "Build A3D demos" ON)
option(A3D_BUILD_EDITOR "Build A3D editor" ON)

# set a default build type only for single-config generators (Ninja/Makefiles)
# (VS/Xcode ignore CMAKE_BUILD_TYPE)
if (NOT CMAKE_CONFIGURATION_TYPES AND (NOT CMAKE_BUILD_TYPE OR CMAKE_BUILD_TYPE STREQUAL ""))
	set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "" FORCE)
endif()
if(CMAKE_CONFIGURATION_TYPES)
	message(STATUS "[BUILD TYPE: multi-config (${CMAKE_GENERATOR})]")
else()
	message(STATUS "[BUILD TYPE: ${CMAKE_BUILD_TYPE}]")
endif()

### configure Emscripten output ###

if (A3D_WEB)

	set(A3D_WEB_GENERATE_HTML_DEFAULT OFF)

	if (CMAKE_BUILD_TYPE STREQUAL "Debug")
		set(A3D_WEB_GENERATE_HTML_DEFAULT ON)
	endif()

	option(A3D_WEB_GENERATE_HTML
			"Generate Emscripten launcher HTML"
			${A3D_WEB_GENERATE_HTML_DEFAULT})

	message(STATUS "[WEB GENERATE HTML: ${A3D_WEB_GENERATE_HTML}]")

	unset(A3D_WEB_GENERATE_HTML_DEFAULT)

endif()

### configure install root ###

if (A3D_RPI)
	set(PLATFORM_STRING "raspberry-pi")
elseif (A3D_LINUX)
	set(PLATFORM_STRING "linux")
elseif (A3D_MACOS)
	set(PLATFORM_STRING "macos")
elseif (A3D_WINDOWS)
	set(PLATFORM_STRING "windows")
elseif (A3D_WEB)
	set(PLATFORM_STRING "web")
else()
	message(FATAL_ERROR "Unsupported platform.")
endif()

if(CMAKE_CONFIGURATION_TYPES)
	set(INSTALL_ROOT_DIR "${PROJECT_BINARY_DIR}/packaged/avara3d/${PLATFORM_STRING}")
else()
	string(TOLOWER "${CMAKE_BUILD_TYPE}" LOWER_RELEASE_TYPE)
	set(INSTALL_ROOT_DIR "${PROJECT_BINARY_DIR}/packaged/avara3d/${PLATFORM_STRING}/${LOWER_RELEASE_TYPE}")
endif()

### add a3d library ###

add_subdirectory(lib)

### add editor ###

if (A3D_BUILD_EDITOR)
	add_subdirectory(tools/a3de)
endif ()

### add tests and demos ###

include(cmake/Program.cmake)

if (A3D_BUILD_TESTS)
	enable_testing()
	add_subdirectory(tests)
endif ()

if (A3D_BUILD_DEMOS)
	add_subdirectory(demos)
endif()
