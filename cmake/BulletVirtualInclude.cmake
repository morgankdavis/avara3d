# cmake/BulletVirtualInclude.cmake

if(NOT DEFINED A3D_THIRDPARTY_INCLUDE_ROOT)
	set(A3D_THIRDPARTY_INCLUDE_ROOT "${PROJECT_BINARY_DIR}/_thirdparty_include")
endif()

set(BULLET_SRC_DIR "${PROJECT_SOURCE_DIR}/external/bullet-3.17/src")
set(BULLET_PREFIX_DIR "${A3D_THIRDPARTY_INCLUDE_ROOT}/bullet")

file(MAKE_DIRECTORY "${A3D_THIRDPARTY_INCLUDE_ROOT}")

# If it exists but isn't what we want, blow it away (prevents stale/bad copies)
set(_need_rebuild TRUE)
if(EXISTS "${BULLET_PREFIX_DIR}")
	if(IS_SYMLINK "${BULLET_PREFIX_DIR}")
		file(READ_SYMLINK "${BULLET_PREFIX_DIR}" _link_target)
		if(_link_target STREQUAL "${BULLET_SRC_DIR}")
			set(_need_rebuild FALSE)
		endif()
	endif()

	if(_need_rebuild)
		file(REMOVE_RECURSE "${BULLET_PREFIX_DIR}")
	endif()
endif()

if(_need_rebuild)
	execute_process(
			COMMAND "${CMAKE_COMMAND}" -E create_symlink "${BULLET_SRC_DIR}" "${BULLET_PREFIX_DIR}"
			RESULT_VARIABLE _symlink_result
	)

	# Fallback: copy headers if symlink fails (Windows often)
	if(NOT _symlink_result EQUAL 0)
		file(MAKE_DIRECTORY "${BULLET_PREFIX_DIR}")
		file(COPY "${BULLET_SRC_DIR}/" DESTINATION "${BULLET_PREFIX_DIR}"
				FILES_MATCHING
				PATTERN "*.h" PATTERN "*.hpp" PATTERN "*.inl"
				PATTERN "*.c" EXCLUDE
				PATTERN "*.cpp" EXCLUDE
		)
	endif()
endif()

if(NOT TARGET a3d_bullet_headers)
	add_library(a3d_bullet_headers INTERFACE)
	add_library(a3d::bullet_headers ALIAS a3d_bullet_headers)

	target_include_directories(a3d_bullet_headers SYSTEM INTERFACE
			"$<BUILD_INTERFACE:${A3D_THIRDPARTY_INCLUDE_ROOT}>"
			"$<BUILD_INTERFACE:${A3D_THIRDPARTY_INCLUDE_ROOT}/bullet>"
	)

endif()
