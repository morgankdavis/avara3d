
set(EXECUTABLE_NAME "test06-debug")


# add headers from avara-engine
include_directories(${CMAKE_SOURCE_DIR}/avara-engine/src)

# https://github.com/nothings/stb/blob/master/docs/stb_howto.txt
#include_directories(${CMAKE_SOURCE_DIR}/avara-engine/external/stb)

#add_subdirectory(include)
#add_subdirectory(src)


file(GLOB HEADERS src/*.h)
file(GLOB SOURCES src/*.c*)


include_directories("/usr/include" "/usr/local/include")


set(CMAKE_CXX_STANDARD 14)


if(MACOS)
	link_directories("/usr/local/lib/" "/System/Library/Frameworks")

	#-pedantic
	#set(CMAKE_CXX_FLAGS "-std=c++ -stdlib=libc++ \
	#	-Wall -mmacosx-version-min=10.8 -arch x86_64 \
	#	-fmessage-length=0 -UGLFW_CDECL \
	#	-Wfloat-equal -Wno-c++11-long-long -Wself-assign -Wno-reorder")
	
	set(CMAKE_CXX_FLAGS "-stdlib=libc++ \
		-Wall -mmacosx-version-min=10.8 -arch x86_64 \
		-fmessage-length=0 -UGLFW_CDECL \
		-Wfloat-equal -Wno-c++11-long-long -Wself-assign -Wno-reorder")
	
elseif(LINUX)
	##-pedantic
	##-Wno-c++11-long-long
	#set(CMAKE_CXX_FLAGS "-std=c++ \
	#	-Wall -fmessage-length=0 -UGLFW_CDECL \
	#	-Wfloat-equal -Wno-reorder -Wfloat-equal")
	
	set(CMAKE_CXX_FLAGS "-Wall -fmessage-length=0 -UGLFW_CDECL \
		-Wfloat-equal -Wno-reorder -Wfloat-equal")

elseif(WINDOWS)

	#set(CMAKE_CXX_FLAGS "-static-libgcc")
	set(CMAKE_CXX_FLAGS "-static-libgcc -static-libstdc++ -static -lpthread -lwinpthread")

endif()


MESSAGE(STATUS "EXECUTABLE_NAME: " ${EXECUTABLE_NAME})

add_executable(${EXECUTABLE_NAME} ${HEADERS} ${SOURCES})
target_link_libraries(${EXECUTABLE_NAME} ae)

