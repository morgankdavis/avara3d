cmake_minimum_required(VERSION 3.20)

project(avara3d
		VERSION 0.0.1
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

### add external libraries ###

add_subdirectory(external)

### add a3d ###

file(GLOB_RECURSE HEADERS CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/include/*.h")
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/src/*.c*")

add_library(avara3d STATIC
		${HEADERS}
		${SOURCES})

add_library(a3d::avara3d ALIAS avara3d)

target_compile_features(avara3d PUBLIC cxx_std_20)

if (MSVC)
	target_compile_options(avara3d PUBLIC /Zc:__cplusplus)
endif()

### create BuildInfo.cmake.h ###

# compile-time string for both single- and multi-config
target_compile_definitions(avara3d
		PUBLIC
		$<$<CONFIG:Debug>:A3D_DEBUG>
		PRIVATE
		A3D_BUILD_TYPE_STR=\"$<CONFIG>\"
)
if ((GITLAB_CI_PIPELINE_IID STREQUAL "") OR (NOT DEFINED GITLAB_CI_PIPELINE_IID))
	set(A3D_BUILD_NUMBER 0)
	set(A3D_BUILD_TIME "0000-00-00T00:00:00Z")
	set(A3D_BUILD_ORIGIN "AdHoc")
else ()
	set(A3D_BUILD_NUMBER ${GITLAB_CI_PIPELINE_IID})
	set(A3D_BUILD_TIME ${GITLAB_CI_PIPELINE_CREATED_AT})
	set(A3D_BUILD_ORIGIN "CI")
endif ()

configure_file(cmake/BuildInfo.cmake.in BuildInfo.cmake.h)

### setup a3d environment defines ###

a3d_set_compile_definitions(avara3d)

### add library headers ###

target_include_directories(avara3d
		PUBLIC
		"${PROJECT_SOURCE_DIR}/include"

		PRIVATE
		"${PROJECT_BINARY_DIR}") # BuildInfo.cmake.h

### setup clang-tidy ###
# TODO: use clang-style?

target_link_libraries(avara3d
		PRIVATE
		a3d::generator
		a3d::gif_h
		a3d::imgui
		a3d::magic_enum
		a3d::pugixml
		a3d::stb
		a3d::vhacd

		PUBLIC
		a3d::bullet
		a3d::fastgltf)

### link a3d to platform-specific libraries ###

if (A3D_GL_DESKTOP)

	target_link_libraries(avara3d
			PUBLIC
			a3d::glad)

endif()

if (A3D_DESKTOP)

	target_link_libraries(avara3d
			PRIVATE
			a3d::glfw)

endif()

if (A3D_LINUX)

	find_package(Threads REQUIRED)
	target_link_libraries(avara3d
			PUBLIC
			Threads::Threads
			${CMAKE_DL_LIBS})

	if (A3D_RPI)

		find_library(EGL_LIBRARY EGL)
		find_library(OPENGLES3_LIBRARY GLESv2)

		target_link_libraries(avara3d
				PUBLIC
				${EGL_LIBRARY}
				${OPENGLES3_LIBRARY})

	else ()

		target_link_libraries(avara3d
				PUBLIC
				GL)

	endif ()

elseif (A3D_MACOS)

	find_library(FRAMEWORK_COCOA Cocoa)
	find_library(FRAMEWORK_CORE_VIDEO CoreVideo)
	find_library(FRAMEWORK_IOKIT IOKit)
	find_library(FRAMEWORK_OPENGL OpenGL)

	set(FRAMEWORKS
			${FRAMEWORK_COCOA}
			${FRAMEWORK_CORE_VIDEO}
			${FRAMEWORK_IOKIT}
			${FRAMEWORK_OPENGL})

	target_link_libraries(avara3d
			PUBLIC
			${FRAMEWORKS}
			z)

	# https://stackoverflow.com/questions/43551483/how-to-set-rpath-in-cmake
	set(CMAKE_MACOSX_RPATH 1) # silence "MACOSX_RPATH is not specified [...]"

endif ()

### set root install dir before adding tests ###

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
endif ()

if(CMAKE_CONFIGURATION_TYPES)
	set(INSTALL_ROOT_DIR "${PROJECT_BINARY_DIR}/packaged/avara3d/${PLATFORM_STRING}")
else()
	string(TOLOWER "${CMAKE_BUILD_TYPE}" LOWER_RELEASE_TYPE)
	set(INSTALL_ROOT_DIR "${PROJECT_BINARY_DIR}/packaged/avara3d/${PLATFORM_STRING}/${LOWER_RELEASE_TYPE}")
endif()

### add editor ###

if (A3D_BUILD_EDITOR)
	add_subdirectory(tools/a3de)
endif ()

### add tests and demos ###

include(cmake/Program.cmake)

if (A3D_BUILD_TESTS)
	add_subdirectory(tests)
endif ()

if (A3D_BUILD_DEMOS)
	add_subdirectory(demos)
endif()

### install ###

set(INSTALL_DATA_DIR "${INSTALL_ROOT_DIR}/data")
set(INSTALL_INCLUDE_DIR "${INSTALL_ROOT_DIR}/include")
set(INSTALL_LEGAL_DIR "${INSTALL_ROOT_DIR}/legal")
set(INSTALL_LIB_DIR "${INSTALL_ROOT_DIR}/lib")

# TODO: public vs. private headers
install(DIRECTORY "${CMAKE_SOURCE_DIR}/include/"
		DESTINATION ${INSTALL_INCLUDE_DIR})

if (NOT A3D_WEB)
	install(DIRECTORY "${CMAKE_SOURCE_DIR}/data/"
			DESTINATION ${INSTALL_DATA_DIR})
endif()

install(DIRECTORY "${CMAKE_SOURCE_DIR}/external/generator"
		DESTINATION ${INSTALL_LEGAL_DIR})

install(TARGETS avara3d
		DESTINATION ${INSTALL_LIB_DIR})
