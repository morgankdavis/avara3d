
set(A3D_EXTERNAL_VINCLUDE_ROOT
		"${CMAKE_BINARY_DIR}/_external_virtual_include"
		CACHE PATH "Build-time virtual include root for third-party headers")

function(a3d_external_virtual_include_dir alias src_dir)
	if(NOT IS_ABSOLUTE "${src_dir}")
		get_filename_component(src_dir "${src_dir}" ABSOLUTE BASE_DIR "${CMAKE_SOURCE_DIR}")
	endif()

	if(NOT EXISTS "${src_dir}")
		message(FATAL_ERROR "a3d_external_virtual_include_dir: source dir does not exist: ${src_dir}")
	endif()

	file(MAKE_DIRECTORY "${A3D_EXTERNAL_VINCLUDE_ROOT}")
	set(dst "${A3D_EXTERNAL_VINCLUDE_ROOT}/${alias}")

	# If it's already a symlink to the right place, keep it (edits propagate automatically).
	if(IS_SYMLINK "${dst}")
		file(READ_SYMLINK "${dst}" cur_target)
		# normalize to absolute for comparison
		if(NOT IS_ABSOLUTE "${cur_target}")
			get_filename_component(cur_target "${cur_target}" ABSOLUTE BASE_DIR "${A3D_EXTERNAL_VINCLUDE_ROOT}")
		endif()
		get_filename_component(cur_target "${cur_target}" REALPATH)
		get_filename_component(src_dir_real "${src_dir}" REALPATH)

		if(cur_target STREQUAL src_dir_real)
			return()
		endif()
	endif()

	# Otherwise (wrong link, or copied dir, or regular dir), rebuild it.
	if(EXISTS "${dst}" OR IS_SYMLINK "${dst}")
		file(REMOVE_RECURSE "${dst}")
	endif()

	if(WIN32)
		execute_process(
				COMMAND "${CMAKE_COMMAND}" -E create_symlink "${src_dir}" "${dst}"
				RESULT_VARIABLE rv
		)
		if(rv EQUAL 0)
			return()
		endif()

		message(WARNING
				"a3d_external_virtual_include_dir('${alias}'): create_symlink failed; falling back to COPY of '${src_dir}' into '${dst}'. "
				"This may copy non-header sources and can be slow. Consider enabling Developer Mode / symlinks, or restricting COPY to headers."
		)

		file(MAKE_DIRECTORY "${dst}")
		file(COPY "${src_dir}/" DESTINATION "${dst}")
	else()
		file(CREATE_LINK "${src_dir}" "${dst}" SYMBOLIC)
	endif()
endfunction()

function(a3d_external_virtual_include_file alias src_file)
	if(NOT IS_ABSOLUTE "${src_file}")
		get_filename_component(src_file "${src_file}" ABSOLUTE BASE_DIR "${CMAKE_SOURCE_DIR}")
	endif()

	if(NOT EXISTS "${src_file}")
		message(FATAL_ERROR "a3d_external_virtual_include_file: source file does not exist: ${src_file}")
	endif()

	file(MAKE_DIRECTORY "${A3D_EXTERNAL_VINCLUDE_ROOT}/${alias}")

	get_filename_component(fname "${src_file}" NAME)
	set(dst "${A3D_EXTERNAL_VINCLUDE_ROOT}/${alias}/${fname}")

	if(EXISTS "${dst}" OR IS_SYMLINK "${dst}")
		file(REMOVE "${dst}")
	endif()

	if(WIN32)
		execute_process(
				COMMAND "${CMAKE_COMMAND}" -E create_symlink "${src_file}" "${dst}"
				RESULT_VARIABLE rv
		)
		if(NOT rv EQUAL 0)
			message(WARNING
					"a3d_external_virtual_include_file('${alias}'): create_symlink failed; falling back to COPY of '${src_file}'."
			)
			file(COPY "${src_file}" DESTINATION "${A3D_EXTERNAL_VINCLUDE_ROOT}/${alias}")
		endif()
	else()
		file(CREATE_LINK "${src_file}" "${dst}" SYMBOLIC)
	endif()
endfunction()

function(a3d_use_external_virtual_includes target)
	target_include_directories(${target}
			SYSTEM
			PUBLIC
			$<BUILD_INTERFACE:${A3D_EXTERNAL_VINCLUDE_ROOT}>
	)
endfunction()
