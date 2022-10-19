
# https://api.csswg.org/bikeshed/?force=1&url=https://raw.githubusercontent.com/vector-of-bool/pitchfork/develop/data/spec.bs

### FROM TOP {{{



# cmake 3.8 is required for C++17
cmake_minimum_required(VERSION 3.0)

project(avara-engine)

set(CMAKE_CXX_STANDARD 14)

if ((CMAKE_BUILD_TYPE STREQUAL "") OR (NOT DEFINED CMAKE_BUILD_TYPE))
	set(CMAKE_BUILD_TYPE "Debug")
endif ()



# print all variables
# https://stackoverflow.com/questions/9298278/cmake-print-out-all-accessible-variables-in-a-script?answertab=trending#tab-top
get_cmake_property(_variableNames VARIABLES)
list (SORT _variableNames)
foreach (_variableName ${_variableNames})
    message(STATUS "${_variableName}=${${_variableName}}")
endforeach()



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

if (DESKTOP)
	set(CMAKE_OSX_ARCHITECTURES "x86_64")
endif()

if (RPI)
    message("We're RPI")
    set(RPI TRUE)
    add_definitions("-DRPI")
endif()

if (ANDROID OR RPI)
	message("Using OpenGL ES.")
	set(GL_ES TRUE)
	add_definitions("-DGL_ES")
else ()
	message("Using OpenGL full.")
	set(GL_FULL TRUE)
	add_definitions("-DGL_FULL")
endif ()


# libraries (headers...) that the test apps depend on

# GLM

include_directories(external/glm-0.9.9)

# boost

add_subdirectory(external/boost-1.67.0)
include_directories(external/boost-1.67.0)

# fmt

add_subdirectory(external/fmt-5.2.0)
include_directories(external/fmt-5.2.0/include)

# spdlog

#add_subdirectory(avara-engine/external/spdlog-0.16.2)
#include_directories(avara-engine/external/spdlog-0.16.2/include)


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

#add_subdirectory(avara-engine)

#if (DESKTOP)
	add_subdirectory(examples)
#endif()



### }}} END FROM TOP






include_directories(external)

if (ANDROID)

	# NDKHelper

	add_subdirectory(external/NDKHelper)

else()

	# either use CMake 3.11 or set 'OpenGL_GL_PREFERENCE' to 'GLVND' to use new OpenGL libs
	# https://github.com/Kitware/CMake/commit/2ea17412a9a40bd66342a1761ac8f067f5d2ab09
	set(OpenGL_GL_PREFERENCE GLVND)

	# GLFW

	set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
	set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
	set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
	set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)
	add_subdirectory(external/glfw-3.3)
	include_directories(external/glfw-3.3/include)

	# GLEW

	add_subdirectory(external/glew-2.1.0)
	include_directories(external/glew-2.1.0/include)

	# assimp

	set(ENABLE_BOOST_WORKAROUND ON CACHE BOOL "" FORCE)
	set(ASSIMP_BUILD_ASSIMP_TOOLS OFF CACHE BOOL "" FORCE)
	set(ASSIMP_BUILD_SAMPLES OFF CACHE BOOL "" FORCE)
	set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
	set(ASSIMP_NO_EXPORT ON CACHE BOOL "" FORCE)
	if (WINDOWS)
		set(ASSIMP_BUILD_ZLIB ON CACHE BOOL "" FORCE) # needed for windows?
	endif()
	add_subdirectory(external/assimp-3.3.1)
	include_directories(external/assimp-3.3.1/include)

	# many mouse

	add_subdirectory(external/manymouse)
	include_directories(external/manymouse/include)

endif() # NOT ANDROID

if (GL_FULL)

	# imgui

	add_subdirectory(external/imgui-1.62)
	include_directories(external/imgui-1.62/include)

endif ()


if (MINGW)# AND UBUNTU_CROSS)

	# bzip2

	add_subdirectory(external/bzip2)
	include_directories(external/bzip2/include)

endif() # MINGW


# mesh optimizer

add_subdirectory(external/meshoptimizer)
include_directories(external/meshoptimizer/include)

# generator

add_subdirectory(external/generator)
include_directories(external/generator/include)

# gif-h

include_directories(external/gif-h)

# stb

include_directories(external/stb)

# bullet

add_subdirectory(external/bullet3-2.87)
include_directories(external/bullet3-2.87/src)


file(GLOB HEADERS include/*.h)
file(GLOB SOURCES src/*.c*)

include_directories(include)

add_library(ae STATIC ${HEADERS} ${SOURCES})


target_link_libraries(ae
	generator
	meshoptimizer
	Bullet3Common
	BulletCollision
	Bullet3Collision
	BulletDynamics
	Bullet3Dynamics
	BulletInverseDynamics
	Bullet3Geometry
	Bullet3OpenCL_clew
	LinearMath
	Bullet2FileLoader
	boost_atomic
	boost_chrono
	boost_date_time
	boost_exception
	boost_filesystem
	boost_regex
	boost_system
	boost_test
	boost_thread
	boost_timer)

if (DESKTOP)
	target_link_libraries(ae
		manymouse
		glfw
		glew_s
		assimp)
endif()

if (MACOS)

	target_link_libraries(ae imgui z)

	find_library(FRAMEWORK_COCOA Cocoa)
	find_library(FRAMEWORK_OPENGL OpenGL)
	find_library(FRAMEWORK_IOKIT IOKit)
	find_library(FRAMEWORK_CORE_VIDEO CoreVideo)
	set(FRAMEWORKS ${FRAMEWORK_COCOA} ${FRAMEWORK_OPENGL} ${FRAMEWORK_IOKIT} ${FRAMEWORK_CORE_VIDEO})
	target_link_libraries(ae ${FRAMEWORKS})

	set(CMAKE_MACOSX_RPATH 1) # silence "MACOSX_RPATH is not specified [...]"

#        target_compile_options(ae PUBLIC
#                        "-Wargument-outside-range") # for Bullet 3.87 osxcross 'error: argument value 10880 is outside the valid range [0, 255]'


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
		imgui
		bz2)

	# wtf?
	if(MSYS OR MINGW OR UBUNTU_CROSS)
		target_link_libraries(ae winpthread)
	else()
		target_link_libraries(ae winpthreads)
	endif()

	# https://stackoverflow.com/questions/11783932/how-do-i-add-a-linker-or-compile-flag-in-a-cmake-file
	#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static -static-libgcc -static-libstdc++ '-Wl,-rpath,$ORIGIN/lib'") # I don't think RPATH works in windows
	#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static -static-libgcc -static-libstdc++")
	#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static -lgcc -lstdc++")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static -static-libgcc -l:libstdc++.a")

	# https://stackoverflow.com/questions/18138635/mingw-exe-requires-a-few-gcc-dlls-regardless-of-the-code
	#set(CMAKE_CXX_STANDARD_LIBRARIES "-static-libgcc -static-libstdc++ ${CMAKE_CXX_STANDARD_LIBRARIES}")
	#set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-Bstatic,--whole-archive")

#	'-Wl,-rpath,$ORIGIN/../lib'

elseif (ANDROID)

	target_link_libraries(
			ae
			android
			NDKHelper
			z
			EGL
			GLESv3)

	# Export ANativeActivity_onCreate(). Refer to: https://github.com/android-ndk/ndk/issues/381.
	set_target_properties(ae PROPERTIES LINK_FLAGS "-u ANativeActivity_onCreate")

endif()

if (RPI)

	find_library(OPENGLES3_LIBRARY GLESv2 "OpenGL ES v3.0 library")
	find_library(EGL_LIBRARY EGL "EGL 1.4 library")

	target_link_libraries(
			ae
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


# silence stupid warnings

#message("COMPILER IS CLANG")
#message(CMAKE_CXX_COMPILER_ID="${CMAKE_CXX_COMPILER_ID})

if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")

	message("COMPILER IS CLANG")
	message(CMAKE_CXX_COMPILER_ID="${CMAKE_CXX_COMPILER_ID}")

	target_compile_options(ae PUBLIC
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
			"-Wno-argument-outside-range") # for Bullet 3.87 osxcross 'error: argument value 10880 is outside the valid range [0, 255]'
			#"-Wno-macro-redefined")

elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")

# seems as if these are not needed for some reason... (?)

#else()

#	message("COMPILER IS OTHER")
#	message(CMAKE_CXX_COMPILER_ID="${CMAKE_CXX_COMPILER_ID})

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
