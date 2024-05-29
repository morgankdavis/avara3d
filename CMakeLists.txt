
cmake_minimum_required(VERSION 3.15)

project(avara3d
		VERSION 0.0.1
		HOMEPAGE_URL https://avara3d.net/)

include(cmake/EnvironmentDetect.cmake)
include(cmake/UtilityFunctions.cmake)

set(CMAKE_CXX_STANDARD 17)

if ((CMAKE_BUILD_TYPE STREQUAL "") OR (NOT DEFINED CMAKE_BUILD_TYPE))
	set(CMAKE_BUILD_TYPE "Debug")
endif ()
message("[BUILD TYPE: " ${CMAKE_BUILD_TYPE} "]")


### create BuildInfo.cmake.h ###

set(A3D_BUILD_TYPE ${CMAKE_BUILD_TYPE})

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

add_subdirectory(external/fastgltf-0.6.1)

add_subdirectory(external/generator)

#add_subdirectory(external/glew-2.1.0)
add_subdirectory(external/glad-3.3)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)
add_subdirectory(external/glfw-3.4.0)

add_subdirectory(external/imgui-1.90)

add_subdirectory(external/manymouse)


### add a3d ###

file(GLOB_RECURSE HEADERS include/*.h)
file(GLOB_RECURSE SOURCES src/*.c*)

add_library(avara3d STATIC
		${HEADERS}
		${SOURCES})


### setup a3d environment defines ###

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
	target_compile_definitions(avara3d PUBLIC "A3D_DEBUG")
endif ()
if (DESKTOP)
	target_compile_definitions(avara3d PUBLIC "DESKTOP")
	target_compile_definitions(avara3d PUBLIC "OPENGL_DESKTOP")
endif ()
if (LINUX)
	target_compile_definitions(avara3d PUBLIC "LINUX")
	target_compile_definitions(avara3d PUBLIC "POSIX")
	if (RPI)
		target_compile_definitions(avara3d PUBLIC "RPI")
		target_compile_definitions(avara3d PUBLIC "OPENGL_ES")
	endif()
elseif (MACOS)
	target_compile_definitions(avara3d PUBLIC "MACOS")
	target_compile_definitions(avara3d PUBLIC "POSIX")
elseif (WINDOWS)
	target_compile_definitions(avara3d PUBLIC "WINDOWS")
endif ()


### add library headers ###

target_include_directories(avara3d PUBLIC include)
target_include_directories(avara3d PRIVATE "${PROJECT_BINARY_DIR}") # BuildInfo.cmake.h
target_include_directories(avara3d PRIVATE external/generator/include)
target_include_directories(avara3d PRIVATE external/gif-h)
#target_include_directories(avara3d PRIVATE external/glew-2.1.0/include)
target_include_directories(avara3d PRIVATE external/glad-3.3/include)
target_include_directories(avara3d PRIVATE external/glfw-3.4.0/include)
target_include_directories(avara3d PRIVATE external/imgui-1.90)
target_include_directories(avara3d PRIVATE external/imgui-1.90/backends)
target_include_directories(avara3d PRIVATE external/magic_enum-0.9.3/include)
target_include_directories(avara3d PRIVATE external/manymouse/include)
target_include_directories(avara3d PRIVATE external/stb)
target_include_directories(avara3d PRIVATE external/v-hacd-4.1.0)


### add special defines ###

target_compile_definitions(avara3d PUBLIC FMT_HEADER_ONLY)


### link a3d to common libraries ###

target_link_libraries(avara3d PUBLIC
		${BULLET_LIBRARIES}
		fastgltf
		generator
		imgui)


### link a3d to platform-specific libraries ###

if (DESKTOP)

	target_link_libraries(avara3d PUBLIC
			#glew_s
			glad
			glfw
			manymouse)

endif()

if (LINUX)

	target_link_libraries(avara3d PUBLIC
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

		target_link_libraries(avara3d PUBLIC
				${EGL_LIBRARY}
				${OPENGLES3_LIBRARY})

	else ()

		target_link_libraries(avara3d PUBLIC
				GL)

	endif ()

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

	target_link_libraries(avara3d PUBLIC
			${FRAMEWORKS}
			z)

	# https://stackoverflow.com/questions/43551483/how-to-set-rpath-in-cmake
	set(CMAKE_MACOSX_RPATH 1) # silence "MACOSX_RPATH is not specified [...]"

endif ()


### set root install dir before adding tests ###

if (RPI)
	set(PLATFORM_STRING "raspberry-pi")
elseif (LINUX)
	set(PLATFORM_STRING "linux")
elseif (MACOS)
	set(PLATFORM_STRING "macos")
elseif (WINDOWS)
	set(PLATFORM_STRING "windows")
endif ()

string(TOLOWER ${CMAKE_BUILD_TYPE} LOWER_RELEASE_TYPE)
set(INSTALL_ROOT_DIR "${PROJECT_BINARY_DIR}/packaged/avara3d/${PLATFORM_STRING}/${LOWER_RELEASE_TYPE}")


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

install(TARGETS avara3d
		DESTINATION ${INSTALL_LIB_DIR})
