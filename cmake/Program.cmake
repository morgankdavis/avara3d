
function(a3d_add_program TARGET_NAME)

	cmake_parse_arguments(
			ARG
			""
			"INSTALL_DESTINATION;DATA_DIRECTORY;DATA_DESTINATION"
			"SOURCES"
			${ARGN})

	if (NOT ARG_SOURCES)
		message(FATAL_ERROR
				"a3d_add_program(${TARGET_NAME}) requires SOURCES")
	endif()

	if (ARG_DATA_DIRECTORY AND NOT ARG_DATA_DESTINATION)
		message(FATAL_ERROR
				"a3d_add_program(${TARGET_NAME}) requires "
				"DATA_DESTINATION when DATA_DIRECTORY is specified")
	endif()

	if (ARG_DATA_DESTINATION AND NOT ARG_DATA_DIRECTORY)
		message(FATAL_ERROR
				"a3d_add_program(${TARGET_NAME}) requires "
				"DATA_DIRECTORY when DATA_DESTINATION is specified")
	endif()

	add_executable(${TARGET_NAME}
			${ARG_SOURCES})

	target_link_libraries(${TARGET_NAME}
			PRIVATE
			a3d::avara3d)

	a3d_set_emscripten_compile_options(${TARGET_NAME})
	a3d_set_emscripten_link_options(${TARGET_NAME})

	# shared engine assets
	a3d_emscripten_preload_directory(
			${TARGET_NAME}
			"${PROJECT_SOURCE_DIR}/data"
			"/data")

	# assets for only this test or demo
	if (ARG_DATA_DIRECTORY)
		a3d_emscripten_preload_directory(
				${TARGET_NAME}
				"${ARG_DATA_DIRECTORY}"
				"${ARG_DATA_DESTINATION}")
	endif()

	if (ARG_INSTALL_DESTINATION)

		install(TARGETS ${TARGET_NAME}
				DESTINATION "${ARG_INSTALL_DESTINATION}")

		if (ARG_DATA_DIRECTORY)
			install(DIRECTORY "${ARG_DATA_DIRECTORY}/"
					DESTINATION "${ARG_INSTALL_DESTINATION}/data"
					OPTIONAL)
		endif()

	endif()

endfunction()
