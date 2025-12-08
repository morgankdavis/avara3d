
cmake_minimum_required(VERSION 3.20)

project(imgui)

file(GLOB HEADERS *.h)
file(GLOB SOURCES *.c*)

include_directories(. backends)

# WTF?
# if we use the system GLFW installation (not building source locally)
# #include <GLFW/glfw3.h> works in Arch/Ubuntu but fails in macOS and Windows with:
#	'GLFW/glfw3.h' file not found
# 	(imgui_impl_glfw3.cpp)
# so we have to do this BS:
#if (MACOS)
#	include_directories("/usr/local/include/")
#elseif (WINDOWS)
#	include_directories("C:/Program Files/vcpkg/installed/x64-windows/include/")
#endif()

#find_package(glfw3 REQUIRED)

# mkd: worked with static libs {
#add_library(imgui
#		STATIC
#		${HEADERS}
#		${SOURCES}
#		backends/imgui_impl_opengl3.h
#		backends/imgui_impl_opengl3.cpp
#		backends/imgui_impl_glfw.h
#		backends/imgui_impl_glfw.cpp)
#
#target_link_libraries(imgui glfw)
#}



#add_library(imgui
#		SHARED
#		${HEADERS}
#		${SOURCES}
#		backends/imgui_impl_opengl3.h
#		backends/imgui_impl_opengl3.cpp
#		backends/imgui_impl_glfw.h
#		backends/imgui_impl_glfw.cpp)

add_library(imgui
		STATIC
		${HEADERS}
		${SOURCES}
		backends/imgui_impl_opengl3.h
		backends/imgui_impl_opengl3.cpp
		backends/imgui_impl_glfw.h
		backends/imgui_impl_glfw.cpp)

target_link_libraries(imgui glfw)

# mkd: added when switching to shared libraries
#target_compile_options(glew_s PUBLIC
#		"-fPIC")
#target_compile_options(glad PUBLIC
#		"-fPIC")
