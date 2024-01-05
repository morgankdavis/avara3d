
cmake_minimum_required(VERSION 3.8)

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

find_package(glfw3 REQUIRED)

add_library(imgui
		${HEADERS}
		${SOURCES}
		backends/imgui_impl_opengl3.h
		backends/imgui_impl_opengl3.cpp
		backends/imgui_impl_glfw.h
		backends/imgui_impl_glfw.cpp)

target_link_libraries(imgui glfw)
