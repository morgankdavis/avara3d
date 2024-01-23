
cmake_minimum_required(VERSION 3.15)

project(avara-engine
		VERSION 0.0.1
		HOMEPAGE_URL http://avara-engine.net/)

option(AE_BUILD_BULLET "Build Bullet from source" ON)
option(AE_BUILD_GLFW "Build GLFW from source" ON)

include(cmake/EnvironmentDetect.cmake)
include(cmake/UtilityFunctions.cmake)

set(CMAKE_CXX_STANDARD 17)

if ((CMAKE_BUILD_TYPE STREQUAL "") OR (NOT DEFINED CMAKE_BUILD_TYPE))
	set(CMAKE_BUILD_TYPE "Debug")
endif()
message("BUILD TYPE: " ${CMAKE_BUILD_TYPE})


### create BuildInfo.cmake.h ###

string(TOUPPER ${CMAKE_BUILD_TYPE} UPPER_RELEASE_TYPE)
set(AE_BUILD_TYPE ${UPPER_RELEASE_TYPE})

if ((GITLAB_CI_PIPELINE_IID STREQUAL "") OR (NOT DEFINED GITLAB_CI_PIPELINE_IID))
	set(GITLAB_CI_PIPELINE_IID 0)
	set(GITLAB_CI_PIPELINE_CREATED_AT 0000-00--0T00:00:00Z) # temporary
	set(AE_BUILD_ORIGIN "ADHOC")
else()
	set(AE_BUILD_ORIGIN "CI")
endif()

message("avara-engine_VERSION_MAJOR: " ${avara-engine_VERSION_MAJOR})
message("avara-engine_VERSION_MINOR: " ${avara-engine_VERSION_MINOR})
message("avara-engine_VERSION_PATCH: " ${avara-engine_VERSION_PATCH})
message("AE_BUILD_TYPE: " ${AE_BUILD_TYPE})
message("GITLAB_CI_PIPELINE_IID: " ${GITLAB_CI_PIPELINE_IID})
message("GITLAB_CI_PIPELINE_CREATED_AT: " ${GITLAB_CI_PIPELINE_CREATED_AT})
# ex:
# GITLAB_CI_PIPELINE_CREATED_AT: 2024-01-10T03:58:04Z
# GITLAB_CI_PIPELINE_IID: 456

configure_file(BuildInfo.cmake.in BuildInfo.cmake.h)


### add external libraries ###

if (AE_BUILD_BULLET)

	set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
	set(USE_SOFT_BODY_MULTI_BODY_DYNAMICS_WORLD OFF CACHE BOOL "" FORCE)
	set(BUILD_PYBULLET OFF CACHE BOOL "" FORCE)
	set(BUILD_OPENGL3_DEMOS OFF CACHE BOOL "" FORCE)
	set(BUILD_BULLET2_DEMOS OFF CACHE BOOL "" FORCE)
	set(BUILD_EXTRAS OFF CACHE BOOL "" FORCE)
	set(INSTALL_LIBS OFF CACHE BOOL "" FORCE)
	set(BUILD_UNIT_TESTS OFF CACHE BOOL "" FORCE)
	set(INSTALL_CMAKE_FILES OFF CACHE BOOL "" FORCE)

	add_subdirectory(external/bullet-3.17)
	include_directories(external/bullet-3.17/src)

	target_compile_options(Bullet3Collision PUBLIC
			"-Wno-deprecated-volatile")

	set(BULLET_LIBRARIES
			LinearMath
			Bullet3Common
			BulletInverseDynamics
			BulletCollision
			BulletDynamics)

else()

	find_package(Bullet REQUIRED)
	include_directories(${BULLET_INCLUDE_DIRS})

endif()

add_subdirectory(external/generator)

add_subdirectory(external/glew-2.1.0)

if (AE_BUILD_GLFW)

	set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
	set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
	set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
	set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)

	add_subdirectory(external/glfw-3.3.9)

else()

	find_package(glfw3 REQUIRED)

endif()

add_subdirectory(external/imgui-1.90)

add_subdirectory(external/rapidobj-1.0.1)


### add ae ###

file(GLOB_RECURSE HEADERS include/*.h)
file(GLOB_RECURSE SOURCES src/*.c*)

add_library(ae STATIC
		${HEADERS}
		${SOURCES})


### setup ae environment defines ###

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
	target_compile_definitions(ae PUBLIC "AE_DEBUG")
endif()
if (DESKTOP)
	target_compile_definitions(ae PUBLIC "DESKTOP")
	target_compile_definitions(ae PUBLIC "OPENGL_DESKTOP")
endif()
if (LINUX)
	target_compile_definitions(ae PUBLIC "LINUX")
	target_compile_definitions(ae PUBLIC "POSIX")
	if (RPI)
		target_compile_definitions(ae PUBLIC "RPI")
		target_compile_definitions(ae PUBLIC "OPENGL_ES")
	endif()
elseif (MACOS)
	target_compile_definitions(ae PUBLIC "MACOS")
	target_compile_definitions(ae PUBLIC "POSIX")
elseif (WINDOWS)
	target_compile_definitions(ae PUBLIC "WINDOWS")
	#	target_compile_definitions(ae PUBLIC "BT_NO_SIMD_OPERATOR_OVERLOADS") // needed for Bullet?
elseif (ANDROID)
	target_compile_definitions(ae PUBLIC "ANDROID")
	target_compile_definitions(ae PUBLIC "POSIX")
	target_compile_definitions(ae PUBLIC "OPENGL_ES")
endif()


### add library headers ###

target_include_directories(ae PUBLIC include)
target_include_directories(ae PRIVATE external/generator/include)
target_include_directories(ae PRIVATE external/gif-h)
target_include_directories(ae PRIVATE external/glew-2.1.0/include)
target_include_directories(ae PRIVATE external/glfw-3.3.9/include)
target_include_directories(ae PRIVATE external/imgui-1.90)
target_include_directories(ae PRIVATE external/imgui-1.90/backends)
target_include_directories(ae PRIVATE external/magic_enum-0.9.3/include)
target_include_directories(ae PRIVATE external/stb)
target_include_directories(ae PRIVATE external/tinygltf-2.8.19)
#target_include_directories(ae PRIVATE external/tinyobjloader-1.0.6)
target_include_directories(ae PRIVATE external/v-hacd-4.1.0)


### add special defines ###

target_compile_definitions(ae PUBLIC FMT_HEADER_ONLY)


### link ae to external libraries ###

target_link_libraries(ae PUBLIC
		${BULLET_LIBRARIES}
		generator
		imgui
		rapidobj)

# for BuildInfoDefines.h
target_include_directories(ae PUBLIC
		"${PROJECT_BINARY_DIR}")

if (DESKTOP)

	target_link_libraries(ae PUBLIC
			glew_s
			glfw)

endif()

if (LINUX)

	target_link_libraries(ae PUBLIC
			dl
			pthread
			X11
			Xrandr
			Xxf86vm
			Xi
			z)

	if (RPI)

		find_library(EGL_LIBRARY EGL)
		find_library(OPENGLES3_LIBRARY GLESv2)

		target_link_libraries(ae PUBLIC
				${EGL_LIBRARY}
				${OPENGLES3_LIBRARY})

	else()

		target_link_libraries(ae PUBLIC
				GL)

	endif()

elseif (MACOS)

	find_library(FRAMEWORK_COCOA Cocoa)
	find_library(FRAMEWORK_CORE_VIDEO CoreVideo)
	find_library(FRAMEWORK_IOKIT IOKit)
	find_library(FRAMEWORK_OPENGL OpenGL)

	set(FRAMEWORKS
			${FRAMEWORK_COCOA}
			${FRAMEWORK_CORE_VIDEO}
			${FRAMEWORK_IOKIT}
			${FRAMEWORK_OPENGL})

	target_link_libraries(ae PUBLIC
			${FRAMEWORKS}
			z)

	# https://stackoverflow.com/questions/43551483/how-to-set-rpath-in-cmake
	set(CMAKE_MACOSX_RPATH 1) # silence "MACOSX_RPATH is not specified [...]"

endif()


### set root install dir before adding tests ###

string(TOLOWER ${CMAKE_BUILD_TYPE} LOWER_RELEASE_TYPE)
set(INSTALL_ROOT_DIR "${PROJECT_BINARY_DIR}/packaged/avara-engine/linux/${LOWER_RELEASE_TYPE}")


### add tests ###

add_subdirectory(tests)


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

install(TARGETS ae
		DESTINATION ${INSTALL_LIB_DIR})
