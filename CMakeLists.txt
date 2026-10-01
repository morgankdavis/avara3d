cmake_minimum_required(VERSION 3.20)

project(avara3d
		VERSION 0.3.0
		HOMEPAGE_URL https://avara3d.net/)

option(A3D_BUILD_PYTHON # must be before CompileDefinitions, EnvironmentDetect
		"Build experimental A3D Python bindings"
		OFF)

include(cmake/CompileDefinitions.cmake)
include(cmake/EmscriptenHelpers.cmake)
include(cmake/EnvironmentDetect.cmake)
include(cmake/ExternalVirtualInclude.cmake)
include(cmake/UtilityFunctions.cmake)

if (A3D_BUILD_PYTHON)
	set(CMAKE_POSITION_INDEPENDENT_CODE ON)
endif ()

option(A3D_BUILD_PYTHON_WEB "Build A3D Python support for web" OFF)
option(A3D_BUILD_AUTOMATED_TESTS "Build A3D automated tests" ON)
option(A3D_BUILD_SANDBOX_TESTS "Build A3D sandbox tests" ON)
option(A3D_BUILD_DEMOS "Build A3D demos" ON)
option(A3D_BUILD_EDITOR "Build A3D editor" ON)
option(A3D_BUILD_DOCUMENTATION "Build Avara3D API documentation" ON)

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

set(INSTALL_ROOT_DIR "${PROJECT_BINARY_DIR}/packaged/avara3d/${PLATFORM_STRING}/$<LOWER_CASE:$<CONFIG>>")

### add a3d library ###

add_subdirectory(lib)

### add Python bindings ###

if (A3D_BUILD_PYTHON)
	add_subdirectory(python)
endif ()

### add Python web support ###

if (A3D_BUILD_PYTHON_WEB)
	add_subdirectory(python/web python-web)
endif ()

### add documentation ###

if (A3D_BUILD_DOCUMENTATION)
	add_subdirectory(docs)
endif()

### add editor ###

if (A3D_BUILD_EDITOR)
	add_subdirectory(tools/a3de)
endif ()

### add tests and demos ###

include(cmake/Program.cmake)

if (A3D_BUILD_AUTOMATED_TESTS)
	enable_testing()
endif()

if (A3D_BUILD_AUTOMATED_TESTS OR A3D_BUILD_SANDBOX_TESTS)
	add_subdirectory(tests)
endif()

if (A3D_BUILD_DEMOS)
	add_subdirectory(demos)
endif()
