
file(GLOB HEADERS src/*.h)
file(GLOB SOURCES src/*.c*)

add_library(boost_filesystem ${HEADERS} ${SOURCES})
