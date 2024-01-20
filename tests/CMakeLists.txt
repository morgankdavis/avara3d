
set(INSTALL_TESTS_ROOT_DIR "${INSTALL_ROOT_DIR}/tests")
set(INSTALL_TESTS_EXEC_DIR "${INSTALL_TESTS_ROOT_DIR}")
set(INSTALL_TESTS_DATA_DIR "${INSTALL_TESTS_ROOT_DIR}/data")

install(DIRECTORY "${CMAKE_SOURCE_DIR}/tests/data/"
		DESTINATION "${INSTALL_TESTS_DATA_DIR}")

#add_subdirectory(000-sandbox)
#add_subdirectory(001-import)
#add_subdirectory(002-scenegraph)
#add_subdirectory(003-input-camera)
#add_subdirectory(004-geoprimitives)
#add_subdirectory(005-materials-lights)
#add_subdirectory(006-debug)
#add_subdirectory(030-rigidbody)


set(TEST_006_RIGIDBODY "030-rigidbody")
set(SOURCES_030 030-rigidbody.cc)
add_executable(${TEST_030_RIGIDBODY} ${SOURCES_030})
target_include_directories(${TEST_030_RIGIDBODY} PUBLIC # add headers from avara-engine
		"${CMAKE_SOURCE_DIR}/include")
target_link_libraries(${TEST_030_RIGIDBODY} ae)


set(TEST_030_RIGIDBODY "030-rigidbody")
set(SOURCES_030 030-rigidbody.cc)
add_executable(${TEST_030_RIGIDBODY} ${SOURCES_030})
target_include_directories(${TEST_030_RIGIDBODY} PUBLIC # add headers from avara-engine
		"${CMAKE_SOURCE_DIR}/include")
target_link_libraries(${TEST_030_RIGIDBODY} ae)





if (NOT ANDROID)

	install(TARGETS ${TEST_NAME}
			DESTINATION "${INSTALL_TESTS_EXEC_DIR}")

endif()


