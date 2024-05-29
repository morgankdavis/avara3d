
file(GLOB HEADERS include/glad/*.h)
file(GLOB HEADERS include/KHR/*.h)
file(GLOB SOURCES src/*.c*)

include_directories(include)

add_library(glad STATIC ${HEADERS} ${SOURCES})

