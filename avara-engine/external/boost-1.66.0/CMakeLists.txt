
#cmake_minimum_required(VERSION 3.8) # 3.8 required for CXX_STANDARD 17

#project(boost-things)


#file(GLOB HEADERS src/*.h)
#file(GLOB SOURCES src/*.c*)


#set(CMAKE_CXX_STANDARD 14)


#add_library(boost-filesystem ${HEADERS} ${SOURCES})


include_directories(.)

add_subdirectory(libs/filesystem)
add_subdirectory(libs/system)
