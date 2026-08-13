set(INSTALL_TESTS_ROOT_DIR "${INSTALL_ROOT_DIR}/tests")
set(INSTALL_TESTS_EXEC_DIR "${INSTALL_TESTS_ROOT_DIR}")

if (A3D_BUILD_AUTOMATED_TESTS)
	add_subdirectory(automated)
endif()

if (A3D_BUILD_SANDBOX_TESTS)
	add_subdirectory(sandbox)
endif()
