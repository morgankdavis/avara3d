a3d_add_demo(
		SOURCES
		main.cc
		JanusApp.h
		JanusApp.cc
		Constants.h
		JanusAppUI.cc
		TransientsBuilder.h
		TransientsBuilder.cc
		TransientsCache.h
		TransientsCache.cc

		DATA_DIRECTORY
		"${CMAKE_CURRENT_SOURCE_DIR}/data"

		DATA_DESTINATION
		"/demos/data")
