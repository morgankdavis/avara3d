
file(GLOB HEADERS src/*.h)
file(GLOB SOURCES src/*.c*)

add_library(boost_exception ${HEADERS} ${SOURCES})

if(UNIX AND APPLE)
	target_compile_options(boost_exception PRIVATE "-mmacosx-version-min=10.9")
endif()
