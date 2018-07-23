
cmake_minimum_required(VERSION 3.0)

project(imgui)



# # GLFW
#
# set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
# set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
# set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
# add_subdirectory(external/glfw-3.2.1)
# if (WINDOWS) # donno
# 	include_directories(external/glfw-3.2.1/include)
# endif()
#
# # GLEW
#
# add_subdirectory(external/glew-2.1.0/build/cmake)
# include_directories(external/glew-2.1.0/include)




file(GLOB HEADERS include/*.h)
file(GLOB SOURCES src/*.c*)

include_directories(include)


# is there a better way to do this..?
include_directories(../glfw-3.2.1/include)
include_directories(../glew-2.1.0/include)


add_library(imgui ${HEADERS} ${SOURCES})

