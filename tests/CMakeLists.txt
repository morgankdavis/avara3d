if (A3D_BUILD_AUTOMATED_TESTS)
	add_subdirectory(automated)
endif()

if (A3D_BUILD_SANDBOX_TESTS)
	add_subdirectory(sandbox)
endif()
