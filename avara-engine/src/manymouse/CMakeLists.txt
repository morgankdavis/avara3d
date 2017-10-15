
file(GLOB HEADERS *.h)
file(GLOB SOURCES *.c*)

add_library(manymouse ${HEADERS} ${SOURCES})

