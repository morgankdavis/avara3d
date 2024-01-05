
cmake_minimum_required(VERSION 3.8)

project(imgui)

file(GLOB HEADERS *.h)
file(GLOB SOURCES *.c*)

include_directories(.)

add_library(imgui
	${HEADERS}
	${SOURCES}
	backends/imgui_impl_opengl3.h
	backends/imgui_impl_opengl3.cpp
	backends/imgui_impl_glfw.h
	backends/imgui_impl_glfw.cpp)

