
set(TEST_PREFIX "test")
set(DEMO_PREFIX "demo")
set(UNIT_NUMBER "005")
set(UNIT_DESCRIPTION "materials-lights")

set(TEST_NAME "${TEST_PREFIX}-${UNIT_NUMBER}-${UNIT_DESCRIPTION}")
#set(DEMO_NAME "${DEMO_PREFIX}-${UNIT_NUMBER}-${UNIT_DESCRIPTION}")

message(STATUS "TEST_NAME: " ${TEST_NAME})
#message(STATUS "DEMO_NAME: " ${DEMO_NAME})


# add headers from avara-engine
include_directories("${CMAKE_SOURCE_DIR}/avara-engine/include")


file(GLOB HEADERS "src/*.h")
file(GLOB SOURCES "src/*.c*")


add_executable(${TEST_NAME} ${HEADERS} ${SOURCES})
target_link_libraries(${TEST_NAME} ae)


if(MACOS)

elseif(LINUX)

elseif(WINDOWS)

	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static -static-libgcc -static-libstdc++ -lpthread")

endif()


target_compile_options(${TEST_NAME} PRIVATE 
	"-fmessage-length=0"
	"-UGLFW_CDECL"
	"-Wno-float-equal"
	"-Wno-c++11-long-long"
	"-Wno-self-assign"
	"-Wno-reorder"
	"-Wno-address-of-packed-member"
	"-Wno-unused-function")


if (NOT ANDROID)

	# install test executable
	
	set(TESTS_INSTALL_DIR "${ROOT_INSTALL_DIR}/tests")

	install(TARGETS ${TEST_NAME}
	        DESTINATION "${TESTS_INSTALL_DIR}")

endif()
