
add_subdirectory(000-sandbox)
add_subdirectory(001-import)
add_subdirectory(002-scenegraph)
add_subdirectory(003-input-camera)
add_subdirectory(004-geoprimitives)
add_subdirectory(005-materials-lights)
add_subdirectory(006-debug)
add_subdirectory(030-rigidbody)


# install shared test data

set(TEST_DATA_DIR "${CMAKE_SOURCE_DIR}/examples/data")
set(TESTS_INSTALL_DIR "${ROOT_INSTALL_DIR}/examples")

message(STATUS "TEST_DATA_DIR: " ${TEST_DATA_DIR})
#TEST_DATA_DIR: /home/mkd/avara-engine/examples/data

message(STATUS "ROOT_INSTALL_DIR [examples]: " ${ROOT_INSTALL_DIR})

install(DIRECTORY "${TEST_DATA_DIR}"
		DESTINATION "${TESTS_INSTALL_DIR}")
		
