
set(UNIT_NUMBER "005")
set(UNIT_DESCRIPTION "materials-lights")
set(TEST_NAME "${UNIT_NUMBER}-${UNIT_DESCRIPTION}")

file(GLOB HEADERS *.h*)
file(GLOB SOURCES *.c*)
add_executable(${TEST_NAME} ${HEADERS} ${SOURCES})

target_link_libraries(${TEST_NAME} avara3d)

if (NOT ANDROID)
	install(TARGETS ${TEST_NAME}
			DESTINATION "${INSTALL_TESTS_EXEC_DIR}")
endif()
