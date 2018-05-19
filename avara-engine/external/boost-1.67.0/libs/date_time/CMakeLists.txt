
file(GLOB GREGORIAN_HEADERS src/gregorian/*.h)
file(GLOB GREGORIAN_SOURCES src/gregorian/*.c*)

file(GLOB POXIX_HEADERS src/posix_time/*.h)
file(GLOB POXIX_SOURCES src/posix_time/*.c*)

add_library(boost_date_time ${GREGORIAN_HEADERS} ${GREGORIAN_SOURCES} ${POXIX_HEADERS} ${POXIX_SOURCES})

if(UNIX AND APPLE)
	target_compile_options(boost_date_time PRIVATE "-mmacosx-version-min=10.9")
endif()
