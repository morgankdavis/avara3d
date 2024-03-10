
cmake_minimum_required(VERSION 3.8)

project(generator)

add_definitions(-DGLM_ENABLE_EXPERIMENTAL -DGENERATOR_USE_GLM)
set(GLM_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/include/a3d/glm")
include_directories(${GLM_INCLUDE_DIR})

include_directories(include)
add_subdirectory(src)
