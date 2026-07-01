function(a3d_set_emscripten_compile_options target)

	if (A3D_EMSCRIPTEN)
		target_compile_options(${target}
				PRIVATE
				"-fexceptions"
				"-sDISABLE_EXCEPTION_CATCHING=0")
	endif()

endfunction()

function(a3d_set_emscripten_link_options target)

	if (A3D_EMSCRIPTEN)
		target_link_options(${target}
				PRIVATE
				"--use-port=contrib.glfw3"
				"-sMIN_WEBGL_VERSION=2"
				"-sMAX_WEBGL_VERSION=2"
				"-sFULL_ES3=1"
				"-sASSERTIONS=1"
				"-sALLOW_MEMORY_GROWTH=1"
				"-sEXIT_RUNTIME=0"
				"-fexceptions"
				"-sDISABLE_EXCEPTION_CATCHING=0"
				"--preload-file=${CMAKE_SOURCE_DIR}/data@/data" # host path @ virtual path
				"--preload-file=${CMAKE_SOURCE_DIR}/tests/data@/tests/data") # ! temporary (like 250mb...)

		set_target_properties(${target} PROPERTIES
				SUFFIX ".html")
	endif()

endfunction()
