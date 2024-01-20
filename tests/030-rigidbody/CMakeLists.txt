
set(UNIT_NUMBER "030")
set(UNIT_DESCRIPTION "rigidbody")

set(TEST_NAME "${UNIT_NUMBER}-${UNIT_DESCRIPTION}")
message(STATUS "TEST_NAME: " ${TEST_NAME})

#file(GLOB HEADERS "*.h")
#file(GLOB SOURCES "*.c*")
file(GLOB SOURCES main.cc)

add_executable(${TEST_NAME} ${HEADERS} ${SOURCES})

target_include_directories(${TEST_NAME} PUBLIC # add headers from avara-engine
		"${CMAKE_SOURCE_DIR}/include")
target_link_libraries(${TEST_NAME} ae)

if (NOT ANDROID)

	install(TARGETS ${TEST_NAME}
			DESTINATION "${INSTALL_TESTS_EXEC_DIR}")

endif()
