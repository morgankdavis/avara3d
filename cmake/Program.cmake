function(a3d_add_program TARGET_NAME)

	cmake_parse_arguments(
			ARG
			""
			"OUTPUT_NAME;INSTALL_DESTINATION;DATA_DIRECTORY;DATA_DESTINATION"
			"SOURCES"
			${ARGN})

	if (NOT ARG_SOURCES)
		message(FATAL_ERROR
				"a3d_add_program(${TARGET_NAME}) requires SOURCES")
	endif ()

	if (ARG_DATA_DIRECTORY AND NOT ARG_DATA_DESTINATION)
		message(FATAL_ERROR
				"a3d_add_program(${TARGET_NAME}) requires "
				"DATA_DESTINATION when DATA_DIRECTORY is specified")
	endif ()

	if (ARG_DATA_DESTINATION AND NOT ARG_DATA_DIRECTORY)
		message(FATAL_ERROR
				"a3d_add_program(${TARGET_NAME}) requires "
				"DATA_DIRECTORY when DATA_DESTINATION is specified")
	endif ()

	if (ARG_OUTPUT_NAME)
		set(PROGRAM_NAME "${ARG_OUTPUT_NAME}")
	else ()
		set(PROGRAM_NAME "${TARGET_NAME}")
	endif ()

	add_executable(${TARGET_NAME}
			${ARG_SOURCES})

	if (ARG_OUTPUT_NAME)
		set_target_properties(${TARGET_NAME} PROPERTIES
				OUTPUT_NAME "${ARG_OUTPUT_NAME}")
	endif ()

	target_link_libraries(${TARGET_NAME}
			PRIVATE
			a3d::avara3d)

	a3d_set_emscripten_compile_options(${TARGET_NAME})
	a3d_set_emscripten_link_options(${TARGET_NAME})

	# shared engine assets
	a3d_emscripten_preload_directory(
			${TARGET_NAME}
			"${PROJECT_SOURCE_DIR}/lib/data"
			"/data")

	# assets for only this test or demo
	if (ARG_DATA_DIRECTORY)
		a3d_emscripten_preload_directory(
				${TARGET_NAME}
				"${ARG_DATA_DIRECTORY}"
				"${ARG_DATA_DESTINATION}")
	endif ()

	if (ARG_INSTALL_DESTINATION)

		set(PROGRAM_INSTALL_DESTINATION
				"${ARG_INSTALL_DESTINATION}/${PROGRAM_NAME}")

		# Installs the primary output:
		#   Debug web:   program/program.html
		#   Release web: program/program.js
		#   Native:      program/program executable
		install(TARGETS ${TARGET_NAME}
				DESTINATION "${PROGRAM_INSTALL_DESTINATION}")

		a3d_install_emscripten_outputs(
				${TARGET_NAME}
				"${PROGRAM_INSTALL_DESTINATION}")

		if (ARG_DATA_DIRECTORY AND NOT A3D_WEB)
			install(DIRECTORY "${ARG_DATA_DIRECTORY}/"
					DESTINATION "${PROGRAM_INSTALL_DESTINATION}/data"
					OPTIONAL)
		endif ()

	endif ()

endfunction()

function(a3d_add_sandbox)

	get_filename_component(PROGRAM_NAME
			"${CMAKE_CURRENT_SOURCE_DIR}"
			NAME)

	a3d_add_program(
			"a3d-sandbox-${PROGRAM_NAME}"

			OUTPUT_NAME
			"${PROGRAM_NAME}"

			${ARGN})

endfunction()

function(a3d_add_demo)

	get_filename_component(PROGRAM_NAME
			"${CMAKE_CURRENT_SOURCE_DIR}"
			NAME)

	a3d_add_program(
			"a3d-demo-${PROGRAM_NAME}"

			OUTPUT_NAME
			"${PROGRAM_NAME}"

			INSTALL_DESTINATION
			"${INSTALL_DEMOS_EXEC_DIR}"

			${ARGN})

endfunction()
