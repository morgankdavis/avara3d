
cmake_minimum_required(VERSION 3.8)

project(imgui)

file(GLOB HEADERS *.h)
file(GLOB SOURCES *.c*)

include_directories(. backends)

# WTF? #include <GLFW/glfw3.h> works in Arch/Ubuntu but fails with:
#	'GLFW/glfw3.h' file not found
# 	(imgui_impl_glfw3.cpp)
# in macOS and Windows.

if (MACOS)
	include_directories("/usr/local/include/")
elseif (WINDOWS)
	include_directories("C:/Program Files/vcpkg/installed/x64-windows/include/")
endif()

add_library(imgui
	${HEADERS}
	${SOURCES}
	backends/imgui_impl_opengl3.h
	backends/imgui_impl_opengl3.cpp
	backends/imgui_impl_glfw.h
	backends/imgui_impl_glfw.cpp)

