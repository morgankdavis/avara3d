
add_subdirectory(000-sandbox)
add_subdirectory(001-import)
add_subdirectory(002-scenegraph)
add_subdirectory(003-input-camera)
add_subdirectory(004-geoprimitives)
add_subdirectory(005-materials-lights)
add_subdirectory(006-debug)
add_subdirectory(030-rigidbody)

# install shared test data

set(TEST_DATA_DIR "${CMAKE_SOURCE_DIR}/tests/data")
set(TESTS_INSTALL_DIR "${ROOT_INSTALL_DIR}/tests")

install(DIRECTORY "${TEST_DATA_DIR}"
		DESTINATION "${TESTS_INSTALL_DIR}")
