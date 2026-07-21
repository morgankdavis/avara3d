get_filename_component(TARGET_NAME
		"${CMAKE_CURRENT_SOURCE_DIR}"
		NAME)

a3d_add_program(${TARGET_NAME}
		SOURCES
		App.cc
		App.h
		main.cc

		DATA_DIRECTORY
		"${CMAKE_CURRENT_SOURCE_DIR}/data"

		DATA_DESTINATION
		"/demos/data"

		INSTALL_DESTINATION
		"${INSTALL_DEMOS_EXEC_DIR}")
