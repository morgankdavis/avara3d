
set(EXECUTABLE_NAME "test-003-input-camera")
MESSAGE(STATUS "EXECUTABLE_NAME: " ${EXECUTABLE_NAME})


# add headers from avara-engine
include_directories(${CMAKE_SOURCE_DIR}/avara-engine/include)


file(GLOB HEADERS src/*.h)
file(GLOB SOURCES src/*.c*)


add_executable(${EXECUTABLE_NAME} ${HEADERS} ${SOURCES})
target_link_libraries(${EXECUTABLE_NAME} ae)


if(MACOS)
	
	link_directories("/usr/local/lib/" "/System/Library/Frameworks")
	
	# target_compile_options(${EXECUTABLE_NAME} PRIVATE
	#  		"-mmacosx-version-min=10.9")

	set(CMAKE_CXX_FLAGS "-std=c++14 -stdlib=libc++ -arch x86_64")

elseif(LINUX)
	
	include_directories("/usr/include" "/usr/local/include")
	
	set(CMAKE_CXX_FLAGS "-std=c++14")

elseif(WINDOWS)

	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static -static-libgcc -static-libstdc++ -lpthread")

endif()


target_compile_options(${EXECUTABLE_NAME} PRIVATE 
	"-fmessage-length=0"
	"-UGLFW_CDECL"
	"-Wno-float-equal"
	"-Wno-c++11-long-long"
	"-Wno-self-assign"
	"-Wno-reorder"
	"-Wno-address-of-packed-member"
	"-Wno-unused-function")





if (NOT ANDROID)

	# check build type

	if ((CMAKE_BUILD_TYPE STREQUAL "Release") OR (CMAKE_BUILD_TYPE STREQUAL "MinSizeRel"))
		set(INSTALL_DIR ${PROJECT_BINARY_DIR}/packaged/release)
	else()
		set(INSTALL_DIR ${PROJECT_BINARY_DIR}/packaged/debug)
	endif()

	install(TARGETS ${EXECUTABLE_NAME}
	        DESTINATION ${INSTALL_DIR}/tests)
	        
	set(DATA_INSTALL_DIR ${INSTALL_DIR}/tests/${EXECUTABLE_NAME}/data)
			
	install(DIRECTORY "../tests/${EXECUTABLE_NAME}/data"
	        DESTINATION ${DATA_INSTALL_DIR}/../)

endif()
