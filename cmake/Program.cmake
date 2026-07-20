
function(a3d_add_program TARGET_NAME)
	cmake_parse_arguments(
			ARG
			""
			"INSTALL_DESTINATION"
			"SOURCES"
			${ARGN})

	if (NOT ARG_SOURCES)
		message(FATAL_ERROR "a3d_add_program(${TARGET_NAME}) requires SOURCES")
	endif()

	add_executable(${TARGET_NAME} ${ARG_SOURCES})

	target_link_libraries(${TARGET_NAME}
			PRIVATE
			a3d::avara3d)

	a3d_set_emscripten_compile_options(${TARGET_NAME})
	a3d_set_emscripten_link_options(${TARGET_NAME})

	if (ARG_INSTALL_DESTINATION)
		install(TARGETS ${TARGET_NAME}
				DESTINATION "${ARG_INSTALL_DESTINATION}")
	endif()
endfunction()
