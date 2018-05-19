
file(GLOB HEADERS src/*.h)
file(GLOB SOURCES src/*.c*)

add_library(boost_test ${HEADERS} ${SOURCES})

if(UNIX AND APPLE)
	target_compile_options(boost_test PRIVATE "-mmacosx-version-min=10.9")
endif()
