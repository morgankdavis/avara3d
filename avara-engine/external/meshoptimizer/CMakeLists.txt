
# from meshoptimizer’s original CMakeLists
#if(MSVC)
#  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4 /WX")
#endif(MSVC)

file(GLOB HEADERS *.h)
file(GLOB SOURCES *.c*)

add_library(meshoptimizer ${HEADERS} ${SOURCES})
