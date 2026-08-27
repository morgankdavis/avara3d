a3d_add_sandbox(
		SOURCES
		RigidBody.cc
		RigidBody.h
		main.cc

		DATA_DIRECTORY
		"${CMAKE_CURRENT_SOURCE_DIR}/data"

		DATA_DESTINATION
		"/tests/data")
