if (A3D_WEB)
	message(STATUS "Automated tests are not supported for Web builds; skipping.")
	return()
endif()

find_package(Catch2 3 QUIET)

if (NOT Catch2_FOUND)
	message(WARNING
			"Catch2 3 was not found; automated tests will not be built. "
			"Set A3D_BUILD_AUTOMATED_TESTS=OFF to suppress this warning.")
	return()
endif()

add_executable(a3d-tests
		scene/SceneTests.cc
		runner/RunnerTests.cc)

target_link_libraries(a3d-tests
		PRIVATE
		a3d::avara3d
		Catch2::Catch2WithMain)

include(Catch)
catch_discover_tests(a3d-tests)
