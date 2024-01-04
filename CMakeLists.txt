
# https://api.csswg.org/bikeshed/?force=1&url=https://raw.githubusercontent.com/vector-of-bool/pitchfork/develop/data/spec.bs


# cmake 3.8 is required for C++17
# cmake 3.12 is required for C++20
# 3.13: target_link_libraries() and target_sources(): https://crascit.com/2016/01/31/enhanced-source-file-handling-with-target_sources/
cmake_minimum_required(VERSION 3.8 FATAL_ERROR)


project(avara-engine)

set(CMAKE_CXX_STANDARD 17)

if ((CMAKE_BUILD_TYPE STREQUAL "") OR (NOT DEFINED CMAKE_BUILD_TYPE))
	set(CMAKE_BUILD_TYPE "Debug")
endif ()


# print all variables
# https://stackoverflow.com/questions/9298278/cmake-print-out-all-accessible-variables-in-a-script?answertab=trending#tab-top
#get_cmake_property(_variableNames VARIABLES)
#list (SORT _variableNames)
#foreach (_variableName ${_variableNames})
#    message(STATUS "${_variableName}=${${_variableName}}")
#endforeach()


# set this to look for non-built-in modules
#message("CMAKE_MODULE_PATH: " ${CMAKE_MODULE_PATH})


# Windows ClangCL = WIN32, nothing else

message("*** BUILD TYPE: " ${CMAKE_BUILD_TYPE} " ***")

if (UNIX AND APPLE)
    message("*** MACOS ***")
    set(MACOS TRUE)
    if (CMAKE_GENERATOR STREQUAL "Xcode")
        set(XCODE TRUE)
    endif()
endif()

if (UNIX AND NOT APPLE AND NOT ANDROID AND NOT MINGW)
    message("*** LINUX ***")
    set(LINUX TRUE)
endif()

if (WIN32)
    message("*** WIN32 ***")
    set(WINDOWS TRUE)
    set(WIN32 TRUE)
	add_definitions("-DWINDOWS")
	add_definitions("-DBT_NO_SIMD_OPERATOR_OVERLOADS")
endif()

if (WIN64)
    message("*** WIN64 ***")
    set(WINDOWS TRUE)
    set(WIN64 TRUE)
endif()

if (MINGW)
    message("*** MINGW ***")
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
    message("*** ANDROID ***")
    set(ANDROID TRUE)
endif()

if (MSYS)
    message("*** MSYS ***")
	set(MSYS TRUE)
endif()

if (MSVS)
    message("*** MSVS ***")
endif()

if (MACOS OR WINDOWS OR LINUX)
	message("*** DESKTOP ***")
    set(DESKTOP TRUE)
endif()

if (XCODE)
	message("*** XCODE ***")
	add_definitions("-DXCODE")
endif()

if (MACOS)
	add_definitions("-DAPPLE" "-DMACOS")
elseif (LINUX)
	add_definitions("-DLINUX")
elseif (WINDOWS)
	add_definitions("-DWINDOWS")
	# stops "ERROR" macro conflict with LOG_LEVEL::ERROR
	# https://stackoverflow.com/questions/27064391/unwanted-header-file-wingdi-h
	# update: does stop the conflict with LOG_LEVEL but breaks GLFW as it uses GDI.
	# 	see above LOG_LEVEL in Types.h
	# add_definitions("-DNOGDI")
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
	message("*** UBUNTU_CROSS ***")
	add_definitions("-DUBUNTU_CROSS")
endif ()

if (DESKTOP)
	set(CMAKE_OSX_ARCHITECTURES "x86_64")
endif()

if (RPI)
    message("*** RPI ***")
    set(RPI TRUE)
    add_definitions("-DRPI")
endif()

if (ANDROID OR RPI)
	message("Using OpenGL ES.")
	set(OPENGL_ES TRUE)
	add_definitions("-DOPENGL_ES")
else ()
	message("Using full OpenGL.")
	set(OPENGL_CORE TRUE)
	add_definitions("-DOPENGL_CORE")
endif ()

# libraries (headers...) that the test apps depend on

# GLM

include_directories(external/glm-0.9.9.8)

# fmt

#add_subdirectory(external/fmt-5.2.0)
#include_directories(external/fmt-5.2.0/include)
add_subdirectory(external/fmt-10.1.1)

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


#if (DESKTOP)
	add_subdirectory(examples)
#endif()

# external (?)

include_directories(external)

if (ANDROID)

	# NDKHelper

	add_subdirectory(external/NDKHelper)

else()

	# either use CMake 3.11 or set 'OpenGL_GL_PREFERENCE' to 'GLVND' to use new OpenGL libs
	# https://github.com/Kitware/CMake/commit/2ea17412a9a40bd66342a1761ac8f067f5d2ab09
	set(OpenGL_GL_PREFERENCE GLVND)



	# glfw

#	set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
#	set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
#	set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
#	set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)
#	add_subdirectory(external/glfw-3.3.8)
#	include_directories(external/glfw-3.3.8/include)

	if (macOS)

		set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

		option(FindGLFW_GLFW_ROOT "GLFW Directory" "")
		set(GLFW_ROOT ${FindGLFW_GLFW_ROOT})
		find_package(GLFW REQUIRED)
		include_directories(${GLFW_INCLUDE_DIR})

		# sets:
		#-- glfw3_CONFIG=/usr/local/lib/cmake/glfw3/glfw3Config.cmake
		#-- glfw3_CONSIDERED_CONFIGS=/usr/local/lib/cmake/glfw3/glfw3Config.cmake
		#-- glfw3_CONSIDERED_VERSIONS=3.3.9
		#-- glfw3_DIR=/usr/local/lib/cmake/glfw3
		#-- glfw3_FOUND=1
		#-- glfw3_VERSION=3.3.9
		#-- glfw3_VERSION_COUNT=3
		#-- glfw3_VERSION_MAJOR=3
		#-- glfw3_VERSION_MINOR=3
		#-- glfw3_VERSION_PATCH=9
		#-- glfw3_VERSION_TWEAK=0

		# actual macOS include dir:
		# /usr/local/opt/glfw/include
		# on runner VM: /usr/local/include/GLFW -> ../Cellar/glfw/3.3.9/include/GLFW


	else()

		find_package(glfw3 REQUIRED)

	endif()




	# glew

	add_subdirectory(external/glew-2.1.0)
	include_directories(external/glew-2.1.0/include)
#	find_package(GLEW REQUIRED)
#	include_directories(${GLEW_INCLUDE_DIRS})
#	set(GLEW_USE_STATIC_LIBS TRUE)
#	message("GLEW_VERSION: " ${GLEW_VERSION})

	# assimp

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

	# manymouse

	add_subdirectory(external/manymouse)
	include_directories(external/manymouse/include)

endif() # NOT ANDROID

if (OPENGL_CORE)

	# imgui

	add_subdirectory(external/imgui-1.88)
	include_directories(external/imgui-1.88)
	include_directories(external/imgui-1.88/backends)

endif()

#if (MINGW)# AND UBUNTU_CROSS)

	# bzip2

#	add_subdirectory(external/bzip2)
#	include_directories(external/bzip2/include)

#endif() # MINGW

# generator

add_subdirectory(external/generator)
include_directories(external/generator/include)

# gif-h

include_directories(external/gif-h)

# stb

include_directories(external/stb)

# v-hacd

include_directories(external/v-hacd-4.1.0)

# bullet

#add_subdirectory(external/bullet-3.17)
#include_directories(external/bullet-3.17/src)
#set_property(TARGET Bullet3Common PROPERTY CXX_STANDARD 14)
#set_property(TARGET LinearMath PROPERTY CXX_STANDARD 14)
#target_compile_options(Bullet3Collision PUBLIC
#		"-Wno-deprecated-volatile")
find_package(Bullet REQUIRED)
include_directories(${BULLET_INCLUDE_DIRS})

# magic_enmum

include_directories(external/magic_enum-0.9.3/include)

# ae

#include_directories(include)
#file(GLOB HEADERS include/*.h)
#file(GLOB SOURCES src/*.c*)

include_directories(include)

#include_directories(
#		include
#
#		include/diagnostic
#		include/diagnostic/logging
#
#		include/geometry
#		include/geometry/primitives
#
#		include/input
#		include/input/platform
#		include/input/platform/android
#		include/input/platform/desktop
#
#		include/physics
#		include/physics/bullet
#
#		include/rendering
#		include/rendering/camera
#		include/rendering/context/platform/android
#		include/rendering/context/platform/desktop
#		include/rendering/materials
#		include/rendering/opengl
#
#		include/scene
#
#		include/utilities)

#target_sources(
#		ae
#		PUBLIC
#		include/*.h
#		src/*.c*
#		include/diagnostic
#		src/diagnostic
#		include/diagnostic/logging
#		src/diagnostic/logging
#)


file(GLOB_RECURSE HEADERS include/*.h)
file(GLOB_RECURSE SOURCES src/*.c*)

add_library(ae STATIC ${HEADERS} ${SOURCES}
		src/physics/model_proxy/PhysicsBodyModelProxy.cc
		include/physics/model_proxy/PhysicsBodyModelProxy.h)
#target_compile_features(ae PUBLIC cxx_std_20)


# this is causing external library headers to not be found. ?
#add_library(ae STATIC "")
#add_subdirectory(include)
#add_subdirectory(src)
#include_directories(include)


# everything
#target_link_libraries(ae
#	fmt::fmt
#	generator
#	Bullet3Common
#	BulletCollision
#	Bullet3Collision
#	BulletDynamics
#	Bullet3Dynamics
#	#BulletInverseDynamics
#	Bullet3Geometry
#	Bullet3OpenCL_clew
#	LinearMath)
#	#Bullet2FileLoader)

#macos
target_link_libraries(ae
		fmt::fmt
		generator
		${ASSIMP_LIBRARIES}
		${BULLET_LIBRARIES})


if (DESKTOP)

	target_link_libraries(ae
			manymouse
			glfw
			glew_s
			assimp)

endif()

if (MACOS)

#get_cmake_property(_variableNames VARIABLES)
#list (SORT _variableNames)
#foreach (_variableName ${_variableNames})
#message(STATUS "${_variableName}=${${_variableName}}")
#endforeach()

#-- ASSIMP_BUILD_SHARED_LIBS=ON
#-- ASSIMP_INCLUDE_DIRS=/usr/local/include;/usr/local/include
#-- ASSIMP_LIBRARIES=assimp::assimp
#-- ASSIMP_LIBRARY_DIRS=
#-- ASSIMP_ROOT_DIR=/usr/local

#-- BULLET_COLLISION_LIBRARY=/usr/local/lib/libBulletCollision.dylib
#-- BULLET_COLLISION_LIBRARY_DEBUG=BULLET_COLLISION_LIBRARY_DEBUG-NOTFOUND
#-- BULLET_DYNAMICS_LIBRARY=/usr/local/lib/libBulletDynamics.dylib
#-- BULLET_DYNAMICS_LIBRARY_DEBUG=BULLET_DYNAMICS_LIBRARY_DEBUG-NOTFOUND
#-- BULLET_FOUND=TRUE
#-- BULLET_INCLUDE_DIR=/usr/local/include/bullet
#-- BULLET_INCLUDE_DIRS=/usr/local/include/bullet
#-- BULLET_LIBRARIES=/usr/local/lib/libBulletDynamics.dylib;/usr/local/lib/libBulletCollision.dylib;/usr/local/lib/libLinearMath.dylib;/usr/local/lib/libBulletSoftBody.dylib
#-- BULLET_MATH_LIBRARY=/usr/local/lib/libLinearMath.dylib
#-- BULLET_MATH_LIBRARY_DEBUG=BULLET_MATH_LIBRARY_DEBUG-NOTFOUND
#-- BULLET_SOFTBODY_LIBRARY=/usr/local/lib/libBulletSoftBody.dylib
#-- BULLET_SOFTBODY_LIBRARY_DEBUG=BULLET_SOFTBODY_LIBRARY_DEBUG-NOTFOUND
#-- Bullet_FOUND=TRUE

#link_directories(
#		"/usr/local/lib/bullet/single")

	find_library(FRAMEWORK_COCOA Cocoa)
	find_library(FRAMEWORK_OPENGL OpenGL)
	find_library(FRAMEWORK_IOKIT IOKit)
	find_library(FRAMEWORK_CORE_VIDEO CoreVideo)

	set(FRAMEWORKS ${FRAMEWORK_COCOA} ${FRAMEWORK_OPENGL} ${FRAMEWORK_IOKIT} ${FRAMEWORK_CORE_VIDEO})

	target_link_libraries(ae
			imgui
			z
			${FRAMEWORKS})

	set(CMAKE_MACOSX_RPATH 1) # silence "MACOSX_RPATH is not specified [...]"

elseif (LINUX AND NOT RPI)

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
		#bz2)

	# wtf?
#	if(MSYS OR MINGW OR UBUNTU_CROSS)
#		target_link_libraries(ae
#				winpthread)
#	else()
#		target_link_libraries(ae
#				winpthreads)
#	endif()

	# for MinGW / cross compile?
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

if (RPI)

	find_library(OPENGLES3_LIBRARY GLESv2 "OpenGL ES v3.0 library")
	find_library(EGL_LIBRARY EGL "EGL 1.4 library")

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

endif ()


if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")

	message("COMPILER IS CLANG")
	message(CMAKE_CXX_COMPILER_ID="${CMAKE_CXX_COMPILER_ID}")

#	if (MACOS)
#		target_compile_options(ae PUBLIC
#				#"-stdlib=libstdc++" # use GCC libcstd++
#				"-I/usr/local/opt/llvm@16/include"
#				"-fexperimental-library"
#				"-fmessage-length=0"
#				"-UGLFW_CDECL"
#				"-Wno-unknown-warning-option"
#				"-Wno-float-equal"
#				"-Wno-c++11-long-long"
#				"-Wno-self-assign"
#				"-Wno-reorder"
#				"-Wno-address-of-packed-member"
#				"-Wno-unused-function"
#				"-Wno-deprecated-declarations"
#				"-Wno-argument-outside-range")
#		target_link_options(ae PUBLIC
#				"-L/usr/local/opt/llvm@16/lib")
#	else()
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
#	endif()
			
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")


endif()


if (NOT ANDROID)

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

