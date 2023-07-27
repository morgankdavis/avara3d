
file(GLOB HEADERS include/*.h)
file(GLOB SOURCES src/*.c*)

include_directories(include)

add_library(manymouse ${HEADERS} ${SOURCES})

target_compile_options(manymouse PRIVATE 
	"-Wno-tautological-pointer-compare"
	"-Wno-unused-function")

if(UNIX AND APPLE)
	target_compile_options(manymouse PRIVATE "-mmacosx-version-min=10.9")
endif()
