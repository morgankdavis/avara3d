
file(GLOB HEADERS src/*.h)
file(GLOB SOURCES src/*.c*)

add_library(boost_system ${HEADERS} ${SOURCES})

target_compile_options(boost_system PRIVATE "-mmacosx-version-min=10.9")
