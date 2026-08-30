a3d_add_demo(
		SOURCES
		main.cc
		JanusApp.h
		JanusApp.cc
		TransientsCache.h
		TransientsCache.cc

		DATA_DIRECTORY
		"${CMAKE_CURRENT_SOURCE_DIR}/data"

		DATA_DESTINATION
		"/demos/data")
