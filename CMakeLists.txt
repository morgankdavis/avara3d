cmake_minimum_required(VERSION 3.20)

project(avara3d
		VERSION 0.0.1
		HOMEPAGE_URL https://avara3d.net/)

include(cmake/ExternalVirtualInclude.cmake)
include(cmake/EnvironmentDetect.cmake)
include(cmake/UtilityFunctions.cmake)

option(A3D_BUILD_TESTS "Build A3D tests" ON)
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

### force C++20 for all targets ###

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(avara3d_build_options INTERFACE)

# require C++20 for anything that links this
target_compile_features(avara3d_build_options INTERFACE cxx_std_20)

# make __cplusplus reflect reality
if (MSVC)
	target_compile_options(avara3d_build_options INTERFACE /std:c++20 /Zc:__cplusplus)
endif()

### add external libraries ###

set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
set(USE_SOFT_BODY_MULTI_BODY_DYNAMICS_WORLD OFF CACHE BOOL "" FORCE)
set(BUILD_PYBULLET OFF CACHE BOOL "" FORCE)
set(BUILD_OPENGL3_DEMOS OFF CACHE BOOL "" FORCE)
set(BUILD_BULLET2_DEMOS OFF CACHE BOOL "" FORCE)
set(BUILD_EXTRAS OFF CACHE BOOL "" FORCE)
set(INSTALL_LIBS OFF CACHE BOOL "" FORCE)
set(BUILD_UNIT_TESTS OFF CACHE BOOL "" FORCE)
set(INSTALL_CMAKE_FILES OFF CACHE BOOL "" FORCE)
set(BUILD_BULLET3 OFF CACHE BOOL "" FORCE)
set(BUILD_OPENCL OFF CACHE BOOL "" FORCE)
set(BUILD_BULLET3_OPENCL OFF CACHE BOOL "" FORCE)
add_subdirectory(external/bullet-3.17)
set(BULLET_LIBRARIES
		LinearMath
		Bullet3Common
		BulletInverseDynamics
		BulletCollision
		BulletDynamics)
target_compile_definitions(BulletDynamics PUBLIC BT_THREADSAFE=1)
target_compile_definitions(BulletCollision PUBLIC BT_THREADSAFE=1)
target_compile_definitions(LinearMath PUBLIC BT_THREADSAFE=1)

add_subdirectory(external/fastgltf-0.9.0)

add_subdirectory(external/generator)

add_subdirectory(external/glad-3.3)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)
add_subdirectory(external/glfw-3.4.0)

add_subdirectory(external/imgui-1.92.5)

#add_subdirectory(external/implot-0.17)

add_subdirectory(external/stb)

add_subdirectory(external/v-hacd-4.1.0)

### add a3d ###

file(GLOB_RECURSE HEADERS CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/include/*.h")
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/src/*.c*")

add_library(avara3d STATIC
		${HEADERS}
		${SOURCES})

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

if (A3D_DESKTOP)
	target_compile_definitions(avara3d PUBLIC "A3D_DESKTOP")
	target_compile_definitions(avara3d PUBLIC "A3D_GL_DESKTOP")
endif ()
if (A3D_LINUX)
	target_compile_definitions(avara3d PUBLIC "A3D_LINUX")
	target_compile_definitions(avara3d PUBLIC "A3D_POSIX")
	if (A3D_RPI)
		target_compile_definitions(avara3d PUBLIC "A3D_RPI")
		target_compile_definitions(avara3d PUBLIC "A3D_GL_ES")
	endif()
elseif (A3D_MACOS)
	target_compile_definitions(avara3d PUBLIC "A3D_MACOS")
	target_compile_definitions(avara3d PUBLIC "A3D_POSIX")
elseif (A3D_WINDOWS)
	target_compile_definitions(avara3d PUBLIC "A3D_WINDOWS")
	target_compile_definitions(avara3d PRIVATE NOMINMAX WIN32_LEAN_AND_MEAN)
endif ()

### add library headers ###

# expose the virtual include root to your library/app targets
a3d_use_external_virtual_includes(avara3d)

target_include_directories(avara3d PUBLIC include)
target_include_directories(avara3d PRIVATE "${PROJECT_BINARY_DIR}") # BuildInfo.cmake.h
a3d_external_virtual_include_dir("bullet" "${CMAKE_SOURCE_DIR}/external/bullet-3.17/src")
# special case: bullet internal source includes
target_include_directories(avara3d SYSTEM PUBLIC
		"$<BUILD_INTERFACE:${A3D_EXTERNAL_VINCLUDE_ROOT}/bullet>"
)
target_include_directories(avara3d PRIVATE external/generator/include)
a3d_external_virtual_include_dir("gif-h" "${CMAKE_SOURCE_DIR}/external/gif-h")
if (A3D_GL_DESKTOP)
target_include_directories(avara3d PUBLIC external/glad-3.3/include) # ! temporary PUBLIC !
endif ()
target_include_directories(avara3d PRIVATE external/glfw-3.4.0/include)
a3d_external_virtual_include_dir("imgui" "${CMAKE_SOURCE_DIR}/external/imgui-1.92.5")
# special case: imgui internal source includes
target_include_directories(avara3d SYSTEM PUBLIC
		"$<BUILD_INTERFACE:${A3D_EXTERNAL_VINCLUDE_ROOT}/imgui>"
		"$<BUILD_INTERFACE:${A3D_EXTERNAL_VINCLUDE_ROOT}/imgui/backends>"
)
#target_include_directories(avara3d PRIVATE external/implot-0.17)
target_include_directories(avara3d PRIVATE external/magic_enum-0.9.3/include)
#a3d_external_virtual_include_dir("v-hacd" "${CMAKE_SOURCE_DIR}/external/v-hacd-4.1.0")

### setup clang-tidy ###
# TODO: use clang-style?

#find_program(CLANG_TIDY_EXE NAMES clang-tidy)
#if (CLANG_TIDY_EXE)
#	message(STATUS "clang-tidy found: ${CLANG_TIDY_EXE}")
#	set_target_properties(avara3d PROPERTIES
#			CXX_CLANG_TIDY "${CLANG_TIDY_EXE};--format-style=file")
#endif ()

### link a3d to common libraries ###

target_link_libraries(avara3d
		PRIVATE
		avara3d_build_options
		PUBLIC
		${BULLET_LIBRARIES}
		fastgltf
		generator
		imgui
		stb
		vhacd) # implot

### link a3d to platform-specific libraries ###

if (A3D_DESKTOP)

	target_link_libraries(avara3d
			PUBLIC
			glad
			glfw)

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

### add tests ###

if (A3D_BUILD_TESTS)
	add_subdirectory(tests)
endif ()

### install ###

set(INSTALL_DATA_DIR "${INSTALL_ROOT_DIR}/data")
set(INSTALL_INCLUDE_DIR "${INSTALL_ROOT_DIR}/include")
set(INSTALL_LEGAL_DIR "${INSTALL_ROOT_DIR}/legal")
set(INSTALL_LIB_DIR "${INSTALL_ROOT_DIR}/lib")

# TODO: public vs. private headers
install(DIRECTORY "${CMAKE_SOURCE_DIR}/include/"
		DESTINATION ${INSTALL_INCLUDE_DIR})

install(DIRECTORY "${CMAKE_SOURCE_DIR}/data/"
		DESTINATION ${INSTALL_DATA_DIR})

install(DIRECTORY "${CMAKE_SOURCE_DIR}/external/generator"
		DESTINATION ${INSTALL_LEGAL_DIR})

install(TARGETS avara3d
		DESTINATION ${INSTALL_LIB_DIR})
