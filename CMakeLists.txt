
cmake_minimum_required(VERSION 3.15)

project(avara-engine)

option(AE_BUILD_BULLET "Build Bullet from source" OFF)

set(CMAKE_CXX_STANDARD 17)

function(print_all_variables)
	# https://stackoverflow.com/questions/9298278/cmake-print-out-all-accessible-variables-in-a-script?answertab=trending#tab-top
	get_cmake_property(_variableNames VARIABLES)
	list (SORT _variableNames)
	foreach (_variableName ${_variableNames})
		message(STATUS "${_variableName}=${${_variableName}}")
	endforeach()
endfunction()

message("BUILD TYPE: " ${CMAKE_BUILD_TYPE})

if (UNIX AND APPLE)
    message("[MACOS]")
    set(MACOS TRUE)
	set(CMAKE_OSX_ARCHITECTURES "x86_64")
	add_definitions("-DAPPLE" "-DMACOS")
    if (CMAKE_GENERATOR STREQUAL "Xcode")
        set(XCODE TRUE)
		add_definitions("-DXCODE")
    endif()
endif()

if (UNIX AND NOT APPLE AND NOT ANDROID AND NOT MINGW)
    message("[LINUX]")
    set(LINUX TRUE)
	add_definitions("-DLINUX")
endif()

if (WIN32)
    message("[WIN32]")
    set(WINDOWS TRUE)
    set(WIN32 TRUE)
	add_definitions("-DWINDOWS" "-DBT_NO_SIMD_OPERATOR_OVERLOADS")
endif()

if (WIN64)
    message("[WIN64]")
    set(WINDOWS TRUE)
    set(WIN64 TRUE)
	add_definitions("-DWINDOWS")
endif()

if (ANDROID)
    message("[ANDROID]")
    set(ANDROID TRUE)
	add_definitions("-DANDROID")
endif()

if (RPI)
	message("[RPI]")
	set(RPI TRUE)
	add_definitions("-DRPI")
endif()

if (MACOS OR WINDOWS OR LINUX)
	message("[DESKTOP]")
    set(DESKTOP TRUE)
	add_definitions("-DDESKTOP")
endif()

if (ANDROID OR RPI)
	message("[OPENGL_ES]")
	set(OPENGL_ES TRUE)
	add_definitions("-DOPENGL_ES")
else ()
	message("[OPENGL_CORE]")
	set(OPENGL_CORE TRUE)
	add_definitions("-DOPENGL_CORE")
endif ()

################################
# glm
################################

include_directories(external/glm-0.9.9.8)

################################
# fmt
################################

add_subdirectory(external/fmt-10.1.1)

################################
# android things
################################

if (ANDROID)
	add_subdirectory(android)
endif()

#if (DESKTOP)
	add_subdirectory(examples)
#endif()

if (ANDROID)

	################################
	# NDKHelper
	################################

	add_subdirectory(external/NDKHelper)

else()

	# either use CMake 3.11 or set 'OpenGL_GL_PREFERENCE' to 'GLVND' to use new OpenGL libs
	# https://github.com/Kitware/CMake/commit/2ea17412a9a40bd66342a1761ac8f067f5d2ab09
	set(OpenGL_GL_PREFERENCE GLVND)

	################################
	# glfw
	################################

#	set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
#	set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
#	set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
#	set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)
#	add_subdirectory(external/glfw-3.3.9)
#	include_directories(external/glfw-3.3.9/include)

	find_package(glfw3 REQUIRED)

	################################
	# glew
	################################

	add_subdirectory(external/glew-2.1.0)
	include_directories(external/glew-2.1.0/include)
#	set(GLEW_USE_STATIC_LIBS TRUE)
#	find_package(GLEW REQUIRED)


	################################
	# assimp
	################################

#	set(ENABLE_BOOST_WORKAROUND ON CACHE BOOL "" FORCE)
#	set(ASSIMP_BUILD_ASSIMP_TOOLS OFF CACHE BOOL "" FORCE)
#	set(ASSIMP_BUILD_SAMPLES OFF CACHE BOOL "" FORCE)
#	set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
#	set(ASSIMP_NO_EXPORT ON CACHE BOOL "" FORCE)
#	if (WINDOWS)
#		set(ASSIMP_BUILD_ZLIB ON CACHE BOOL "" FORCE) # needed for windows?
#	endif()
#	add_subdirectory(external/assimp-3.3.1)
#	include_directories(external/assimp-3.3.1/include)
#	set_property(TARGET assimp PROPERTY CXX_STANDARD 14)
	# causes GLEW linker errors
	find_package(assimp REQUIRED)
	#include_directories(${ASSIMP_INCLUDE_DIR})

#	ASSIMP_BUILD_SHARED_LIBS ??

	################################
	# manymouse
	################################

	add_subdirectory(external/manymouse)
	include_directories(external/manymouse/include)

endif() # NOT ANDROID

if (OPENGL_CORE)

	# imgui

	add_subdirectory(external/imgui-1.90)
	include_directories(external/imgui-1.90)
	include_directories(external/imgui-1.90/backends)

endif()

################################
# generator
################################

add_subdirectory(external/generator)
include_directories(external/generator/include)

################################
# gif-h
################################

include_directories(external/gif-h)

################################
# stb
################################

include_directories(external/stb)

################################
# v-hacd
################################

include_directories(external/v-hacd-4.1.0)

################################
# bullet
################################

if (AE_BUILD_BULLET)

	set(USE_SOFT_BODY_MULTI_BODY_DYNAMICS_WORLD OFF CACHE BOOL "" FORCE)
	set(BUILD_PYBULLET OFF CACHE BOOL "" FORCE)
	set(BUILD_OPENGL3_DEMOS OFF CACHE BOOL "" FORCE)
	set(BUILD_BULLET2_DEMOS OFF CACHE BOOL "" FORCE)
	set(BUILD_EXTRAS OFF CACHE BOOL "" FORCE)
	set(INSTALL_LIBS OFF CACHE BOOL "" FORCE)
	set(BUILD_UNIT_TESTS OFF CACHE BOOL "" FORCE)
	set(INSTALL_CMAKE_FILES OFF CACHE BOOL "" FORCE)

	set(BULLET_LIBRARIES
			LinearMath
			Bullet3Common
			BulletInverseDynamics
			BulletCollision
			BulletDynamics)
			#BulletSoftBody)

	add_subdirectory(external/bullet-3.17)
	include_directories(external/bullet-3.17/src)
	set_property(TARGET Bullet3Common PROPERTY CXX_STANDARD 14)
	set_property(TARGET LinearMath PROPERTY CXX_STANDARD 14)
	target_compile_options(Bullet3Collision PUBLIC
			"-Wno-deprecated-volatile")



else ()

#	find_package(Bullet CONFIG REQUIRED)
#	include_directories(${BULLET_INCLUDE_DIRS}) # BULLET_INCLUDE_DIRS=include/bullet - wtf?

	find_package(Bullet REQUIRED)
	include_directories(${BULLET_INCLUDE_DIRS}) # BULLET_INCLUDE_DIRS=/usr/include/bullet

#	BULLET_LIBRARIES=LinearMath;Bullet3Common;BulletInverseDynamics;BulletCollision;BulletDynamics;BulletSoftBody
endif ()

################################
# magic_enmum
################################

include_directories(external/magic_enum-0.9.3/include)

################################
# ae
################################

file(GLOB_RECURSE HEADERS include/*.h)
file(GLOB_RECURSE SOURCES src/*.c*)

include_directories(include)

add_library(ae STATIC
		${HEADERS}
		${SOURCES})

target_link_libraries(ae
		fmt::fmt
		generator
		${BULLET_LIBRARIES})


if (DESKTOP)

	target_link_libraries(ae
			manymouse
			glfw
			glew_s
			# imgui
			${ASSIMP_LIBRARIES})

endif()

if (MACOS)

	find_library(FRAMEWORK_COCOA Cocoa)
	find_library(FRAMEWORK_OPENGL OpenGL)
	find_library(FRAMEWORK_IOKIT IOKit)
	find_library(FRAMEWORK_CORE_VIDEO CoreVideo)

	set(FRAMEWORKS
			${FRAMEWORK_COCOA}
			${FRAMEWORK_OPENGL}
			${FRAMEWORK_IOKIT}
			${FRAMEWORK_CORE_VIDEO})

	target_link_libraries(ae
			imgui
			z
			${FRAMEWORKS})

	set(CMAKE_MACOSX_RPATH 1) # silence "MACOSX_RPATH is not specified [...]"

elseif (RPI)

	find_library(OPENGLES3_LIBRARY GLESv2)
	find_library(EGL_LIBRARY EGL)

	target_link_libraries(ae
			z
			X11
			Xxf86vm
			Xrandr
			pthread
			Xi
			dl
			${OPENGLES3_LIBRARY}
			${EGL_LIBRARY})

elseif (LINUX) # not RPI

	target_link_libraries(ae
		imgui
		z
		GL
		X11
		Xxf86vm
		Xrandr
		pthread
		Xi
		dl)

elseif (WINDOWS)

	target_link_libraries(ae
		imgui)

	# https://stackoverflow.com/questions/14172856/compile-with-mt-instead-of-md-using-cmake
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MTd")

	#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static -static-libgcc -l:libstdc++.a")

elseif (ANDROID)

	target_link_libraries(ae
			android
			NDKHelper
			z
			EGL
			GLESv3)

	# export ANativeActivity_onCreate(). Refer to: https://github.com/android-ndk/ndk/issues/381.
	set_target_properties(ae PROPERTIES LINK_FLAGS "-u ANativeActivity_onCreate")

endif()

if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")

	target_compile_options(ae PUBLIC
			#"-stdlib=libc++" # use LLVM libc++
			"-fmessage-length=0"
			"-UGLFW_CDECL"
			"-Wno-unknown-warning-option"
			"-Wno-float-equal"
			"-Wno-c++11-long-long"
			"-Wno-self-assign"
			"-Wno-reorder"
			"-Wno-address-of-packed-member"
			"-Wno-unused-function"
			"-Wno-deprecated-declarations"
			"-Wno-argument-outside-range")

endif()

if (NOT ANDROID)

	# set install directories

	string(TOLOWER CMAKE_BUILD_TYPE LOWER_RELEASE_TYPE)
#	if ((CMAKE_BUILD_TYPE STREQUAL "Release") OR (CMAKE_BUILD_TYPE STREQUAL "MinSizeRel"))
#		set(ROOT_INSTALL_DIR "${PROJECT_BINARY_DIR}/packaged/release")
#	else()
#		set(ROOT_INSTALL_DIR "${PROJECT_BINARY_DIR}/packaged/debug")
#	endif()
	set(ROOT_INSTALL_DIR "${PROJECT_BINARY_DIR}/packaged/${LOWER_RELEASE_TYPE}")

	# install lib and headers

	set(LIB_INSTALL_DIR "${ROOT_INSTALL_DIR}/${INSTALL_DIR}/${PROJECT_NAME}")

	install(TARGETS ae
	        DESTINATION "${LIB_INSTALL_DIR}/lib")

	# TODO: make "public headers"
	install(FILES ${HEADERS}
	        DESTINATION "${LIB_INSTALL_DIR}/include")

	# install engine resources

	file(GLOB SHADERS "${CMAKE_SOURCE_DIR}/${PROJECT_NAME}/data/shaders/*")
	install(FILES ${SHADERS}
			DESTINATION "${LIB_INSTALL_DIR}/data/shaders")

	file(GLOB IMAGES "${CMAKE_SOURCE_DIR}/${PROJECT_NAME}/data/images/*")
	install(FILES ${IMAGES}
			DESTINATION "${LIB_INSTALL_DIR}/data/images")

	file(GLOB FONTS "${CMAKE_SOURCE_DIR}/${PROJECT_NAME}/data/fonts/*")
	install(FILES ${FONTS}
			DESTINATION "${LIB_INSTALL_DIR}/data/fonts")

endif()
