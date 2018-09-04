
# from meshoptimizer’s original CMakeLists
#if(MSVC)
#  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4 /WX")
#endif(MSVC)

file(GLOB HEADERS include/*.h)
file(GLOB SOURCES src/*.c*)

include_directories(include)

add_library(meshoptimizer ${HEADERS} ${SOURCES})
