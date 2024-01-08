
set(UNIT_NUMBER "004")
set(UNIT_DESCRIPTION "geoprimitives")

set(TEST_NAME "${UNIT_NUMBER}-${UNIT_DESCRIPTION}")
message(STATUS "TEST_NAME: " ${TEST_NAME})

# add headers from avara-engine
include_directories("${CMAKE_SOURCE_DIR}/include")

file(GLOB HEADERS "include/*.h")
file(GLOB SOURCES "src/*.c*")

include_directories("include")

add_executable(${TEST_NAME} ${HEADERS} ${SOURCES})
target_link_libraries(${TEST_NAME} ae)

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
