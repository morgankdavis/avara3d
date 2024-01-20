
set(INSTALL_TESTS_ROOT_DIR "${INSTALL_ROOT_DIR}/tests")
set(INSTALL_TESTS_EXEC_DIR "${INSTALL_TESTS_ROOT_DIR}")
set(INSTALL_TESTS_DATA_DIR "${INSTALL_TESTS_ROOT_DIR}/data")

install(DIRECTORY "${CMAKE_SOURCE_DIR}/tests/data/"
		DESTINATION "${INSTALL_TESTS_DATA_DIR}")

add_subdirectory(000-sandbox)
add_subdirectory(001-import)
add_subdirectory(002-scenegraph)
add_subdirectory(003-input-camera)
add_subdirectory(004-geoprimitives)
add_subdirectory(005-materials-lights)
add_subdirectory(006-debug)
add_subdirectory(030-rigidbody)
