
cmake_minimum_required(VERSION 3.15)

project(avara-engine
		VERSION 0.0.1
		HOMEPAGE_URL http://avara-engine.net/)

#option(AE_BUILD_TESTS "Build the AE test applications" ON)
#option(AE_BUILD_DEMOS "Build the AE demo applications" ON)

option(AE_BUILD_ASSIMP "Build Assimp from source" ON)
option(AE_BUILD_BULLET "Build Bullet from source" ON)
option(AE_BUILD_GLFW "Build GLFW from source" ON)

include(cmake/EnvironmentDetect.cmake)
include(cmake/UtilityFunctions.cmake)

set(CMAKE_CXX_STANDARD 17)

if ((CMAKE_BUILD_TYPE STREQUAL "") OR (NOT DEFINED CMAKE_BUILD_TYPE))
	set(CMAKE_BUILD_TYPE "Debug")
endif()
message("BUILD TYPE: " ${CMAKE_BUILD_TYPE})


# create BuildInfo.h
message("avara-engine_VERSION_MAJOR: " ${avara-engine_VERSION_MAJOR})
message("avara-engine_VERSION_MINOR: " ${avara-engine_VERSION_MINOR})
message("avara-engine_VERSION_PATCH: " ${avara-engine_VERSION_PATCH})
# BUILD_SOURCE = {CI, ADHOC}
message("GITLAB_CI_PIPELINE_CREATED_AT: " ${GITLAB_CI_PIPELINE_CREATED_AT}) # RELEASE_DATE
message("GITLAB_CI_PIPELINE_IID: " ${GITLAB_CI_PIPELINE_IID}) # RELEASE_BUILD
# ex:
# GITLAB_CI_PIPELINE_CREATED_AT: 2024-01-10T03:58:04Z
# GITLAB_CI_PIPELINE_IID: 456
# ^^ check and replace if not defined



# https://github.com/Kitware/CMake/commit/2ea17412a9a40bd66342a1761ac8f067f5d2ab09
set(OpenGL_GL_PREFERENCE GLVND)









### assimp ###

if (AE_BUILD_ASSIMP)

	# assimp 3.3.1

	set(ENABLE_BOOST_WORKAROUND ON CACHE BOOL "" FORCE)
	set(ASSIMP_BUILD_ASSIMP_TOOLS OFF CACHE BOOL "" FORCE)
	set(ASSIMP_BUILD_SAMPLES OFF CACHE BOOL "" FORCE)
	set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
	set(ASSIMP_NO_EXPORT ON CACHE BOOL "" FORCE)
	if (WINDOWS)
		set(ASSIMP_BUILD_ZLIB ON CACHE BOOL "" FORCE) # needed for windows? -- maybe onto MinGW?
	endif()

	set(ASSIMP_LIBRARIES "assimp")
	add_subdirectory(external/assimp-3.3.1)
	include_directories(external/assimp-3.3.1/include)
	set_property(TARGET assimp PROPERTY CXX_STANDARD 14)

else()

	find_package(assimp REQUIRED)
	#include_directories(${ASSIMP_INCLUDE_DIR})

endif()


### bullet ###

if (AE_BUILD_BULLET)

	# bullet 3.17

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
	include_directories(${BULLET_INCLUDE_DIRS}) # BULLET_INCLUDE_DIRS=/usr/include/bullet

endif()


### generator ###

add_subdirectory(external/generator)
include_directories(external/generator/include) # MOVE


### gif-h ###

include_directories(external/gif-h) # MOVE


### glew ###

add_subdirectory(external/glew-2.1.0)
include_directories(external/glew-2.1.0/include) # MOVE



### glfw ###

if (AE_BUILD_GLFW)

	set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
	set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
	set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
	set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)

	add_subdirectory(external/glfw-3.3.9)
	include_directories(external/glfw-3.3.9/include) # MOVE

else()

	find_package(glfw3 REQUIRED)

endif()


if (OPENGL_CORE)

	### imgui ###

	add_subdirectory(external/imgui-1.90)
	include_directories(external/imgui-1.90) # MOVE
	include_directories(external/imgui-1.90/backends) # MOVE

endif()


### magic_enmum ###

include_directories(external/magic_enum-0.9.3/include) # MOVE



### stb ###

include_directories(external/stb) # MOVE

### v-hacd ###

include_directories(external/v-hacd-4.1.0) # MOVE




file(GLOB_RECURSE HEADERS include/*.h)
file(GLOB_RECURSE SOURCES src/*.c*)

add_library(ae STATIC
		${HEADERS}
		${SOURCES})

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
	target_compile_definitions(ae PUBLIC "AE_DEBUG")
endif()
if (DESKTOP)
	target_compile_definitions(ae PUBLIC "DESKTOP")
	target_compile_definitions(ae PUBLIC "OPENGL_CORE")
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

target_include_directories(ae PUBLIC include)
target_compile_definitions(ae PUBLIC FMT_HEADER_ONLY)

target_link_libraries(ae PUBLIC
		generator
		${BULLET_LIBRARIES})

if (DESKTOP)

	target_link_libraries(ae PUBLIC
			glfw
			glew_s
			${ASSIMP_LIBRARIES})

endif()

if (OPENGL_CORE)

	target_link_libraries(ae PUBLIC
			imgui)

endif()

if (LINUX)

	target_link_libraries(ae PUBLIC
			z
			X11
			Xxf86vm
			Xrandr
			pthread
			Xi
			dl)

	if (RPI)

		find_library(OPENGLES3_LIBRARY GLESv2)
		find_library(EGL_LIBRARY EGL)

		target_link_libraries(ae PUBLIC
				${OPENGLES3_LIBRARY}
				${EGL_LIBRARY})

	else()

		target_link_libraries(ae PUBLIC
				GL)

	endif()

elseif (MACOS)

	find_library(FRAMEWORK_COCOA Cocoa)
	find_library(FRAMEWORK_OPENGL OpenGL)
	find_library(FRAMEWORK_IOKIT IOKit)
	find_library(FRAMEWORK_CORE_VIDEO CoreVideo)

	set(FRAMEWORKS
			${FRAMEWORK_COCOA}
			${FRAMEWORK_OPENGL}
			${FRAMEWORK_IOKIT}
			${FRAMEWORK_CORE_VIDEO})

	target_link_libraries(ae PUBLIC
			z
			${FRAMEWORKS})

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
		DESTINATION "${INSTALL_INCLUDE_DIR}")

install(DIRECTORY "${CMAKE_SOURCE_DIR}/data/"
		DESTINATION "${INSTALL_DATA_DIR}")

install(DIRECTORY "${CMAKE_SOURCE_DIR}/external/generator"
		DESTINATION "${INSTALL_LEGAL_DIR}")

install(TARGETS ae
		DESTINATION "${INSTALL_LIB_DIR}")
