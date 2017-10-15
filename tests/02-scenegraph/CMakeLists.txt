
set(EXECUTABLE_NAME "test02-scenegraph")


# add headers from avara-engine
include_directories(${CMAKE_SOURCE_DIR}/avara-engine/src)


#add_subdirectory(include)
#add_subdirectory(src)


file(GLOB HEADERS src/*.h)
file(GLOB SOURCES src/*.c*)


include_directories("/usr/include" "/usr/local/include")


if(MACOS)
	link_directories("/usr/local/lib/" "/System/Library/Frameworks")

	#-pedantic
	set(CMAKE_CXX_FLAGS "-std=c++14 -stdlib=libc++ \
		-Wall -mmacosx-version-min=10.8 -arch x86_64 \
		-fmessage-length=0 -UGLFW_CDECL \
		-Wfloat-equal -Wno-c++11-long-long -Wself-assign -Wno-reorder")
elseif(LINUX)
	#-pedantic
	#-Wno-c++11-long-long
	set(CMAKE_CXX_FLAGS "-std=c++14 \
		-Wall -fmessage-length=0 -UGLFW_CDECL \
		-Wfloat-equal -Wno-reorder -Wfloat-equal")
endif()


MESSAGE(STATUS "EXECUTABLE_NAME: " ${EXECUTABLE_NAME})

add_executable(${EXECUTABLE_NAME} ${HEADERS} ${SOURCES})
target_link_libraries(${EXECUTABLE_NAME} ae)

