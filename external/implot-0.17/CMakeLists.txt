
cmake_minimum_required(VERSION 3.20)

project(implot)

file(GLOB HEADERS *.h)
file(GLOB SOURCES *.c*)

include_directories(.)

add_library(implot
		STATIC
		${HEADERS}
		${SOURCES})

target_include_directories(implot PRIVATE ${CMAKE_SOURCE_DIR}/external/imgui-1.90)
