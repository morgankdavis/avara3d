
add_subdirectory(test-001-import)
add_subdirectory(test-002-scenegraph)
add_subdirectory(test-003-input-camera)
add_subdirectory(test-004-geoprimitives)
add_subdirectory(test-005-materials-lights)
add_subdirectory(test-006-debug)
add_subdirectory(test-030-rigidbody)


# install shared test data

set(TEST_DATA_DIR "${CMAKE_SOURCE_DIR}/tests/data")
set(TESTS_INSTALL_DIR "${ROOT_INSTALL_DIR}/tests")

install(DIRECTORY "${TEST_DATA_DIR}"
		DESTINATION "${TESTS_INSTALL_DIR}")
		