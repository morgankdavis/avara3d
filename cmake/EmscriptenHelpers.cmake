
function(a3d_set_emscripten_compile_options target)

	if (A3D_WEB)
		target_compile_options(${target}
				PRIVATE
				"-fexceptions"
				"-sDISABLE_EXCEPTION_CATCHING=0")
	endif()

endfunction()

function(a3d_emscripten_preload_directory
		target
		source_directory
		virtual_directory)

	if (NOT A3D_WEB)
		return()
	endif()

	if (NOT IS_ABSOLUTE "${source_directory}")
		get_filename_component(
				source_directory
				"${source_directory}"
				ABSOLUTE
				BASE_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
	endif()

	if (NOT IS_DIRECTORY "${source_directory}")
		message(VERBOSE
				"Skipping missing Emscripten preload directory: "
				"${source_directory}")
		return()
	endif()

	if (NOT virtual_directory MATCHES "^/")
		message(FATAL_ERROR
				"Emscripten virtual directory must be absolute: "
				"${virtual_directory}")
	endif()

	target_link_options(${target}
			PRIVATE
			"--preload-file=${source_directory}@${virtual_directory}")

endfunction()

function(a3d_set_emscripten_link_options target)

	if (NOT A3D_WEB)
		return()
	endif()

	target_link_options(${target}
			PRIVATE
			"--use-port=contrib.glfw3"
			"-sMIN_WEBGL_VERSION=2"
			"-sMAX_WEBGL_VERSION=2"
			"-sFULL_ES3=1"
#			"-sASSERTIONS=1"
			"-sALLOW_MEMORY_GROWTH=1"
			"-sEXIT_RUNTIME=0"
			"-fexceptions"
			"-sDISABLE_EXCEPTION_CATCHING=0")

	if (A3D_WEB_GENERATE_HTML)
		set_target_properties(${target} PROPERTIES SUFFIX ".html")
	else()
		set_target_properties(${target} PROPERTIES SUFFIX ".js")
	endif()

endfunction()

function(a3d_install_emscripten_outputs
		target
		destination)

	if (NOT A3D_WEB)
		return()
	endif()

	set(output_base
			"$<TARGET_FILE_DIR:${target}>/$<TARGET_FILE_BASE_NAME:${target}>")

	# WebAssembly binary.
	install(FILES
			"${output_base}.wasm"
			DESTINATION "${destination}")

	# Packed preload assets. This may not exist if nothing was preloaded.
	install(FILES
			"${output_base}.data"
			DESTINATION "${destination}"
			OPTIONAL)

	# When HTML is the primary target, JavaScript is a sidecar.
	# In non-HTML builds, install(TARGETS) already installs the .js file.
	if (A3D_WEB_GENERATE_HTML)
		install(FILES
				"${output_base}.js"
				DESTINATION "${destination}")
	endif()

endfunction()
