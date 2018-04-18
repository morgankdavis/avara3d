
file(GLOB HEADERS src/*.h)
file(GLOB SOURCES src/*.c*)

add_library(boost_filesystem ${HEADERS} ${SOURCES})

target_link_libraries(boost_filesystem boost_system)

if(UNIX AND APPLE)
	target_compile_options(boost_filesystem PRIVATE "-mmacosx-version-min=10.9")
endif()
