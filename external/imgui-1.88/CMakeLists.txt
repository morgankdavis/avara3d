
cmake_minimum_required(VERSION 3.8)

project(imgui)

file(GLOB HEADERS *.h)
file(GLOB SOURCES *.c*)

include_directories(.)

# mkd: HACK!
if (macOS)
	include_directories(/usr/local/include/)
endif()

add_library(imgui
	${HEADERS} ${SOURCES}
	# !!! HACK !!!
	backends/imgui_impl_opengl3.cpp
	backends/imgui_impl_glfw.cpp)

