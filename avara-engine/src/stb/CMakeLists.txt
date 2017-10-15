
file(GLOB HEADERS *.h)
file(GLOB SOURCES *.c*)

add_library(stb ${HEADERS} ${SOURCES})
