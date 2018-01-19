
cmake_minimum_required(VERSION 3.0)

project(winpthreads)

file(GLOB HEADERS src/*.h)
file(GLOB SOURCES src/*.c*)


set(CMAKE_CXX_STANDARD 14)

add_library(winpthreads ${HEADERS} ${SOURCES})
