if (NOT DEFINED SOURCE_DIRECTORY OR SOURCE_DIRECTORY STREQUAL "")
	message(FATAL_ERROR "SOURCE_DIRECTORY is required.")
endif ()

if (NOT DEFINED DESTINATION_DIRECTORY OR DESTINATION_DIRECTORY STREQUAL "")
	message(FATAL_ERROR "DESTINATION_DIRECTORY is required.")
endif ()

if (NOT IS_DIRECTORY "${SOURCE_DIRECTORY}")
	message(FATAL_ERROR
			"Source directory does not exist: ${SOURCE_DIRECTORY}")
endif ()

file(MAKE_DIRECTORY
		"${DESTINATION_DIRECTORY}"
		"${DESTINATION_DIRECTORY}/python")

# Replace only the staged source. Leave python/.pyodide_build intact so
# Pyodide can retain its package-build state between wheel builds.
file(REMOVE_RECURSE
		"${DESTINATION_DIRECTORY}/cmake"
		"${DESTINATION_DIRECTORY}/lib"
		"${DESTINATION_DIRECTORY}/python/avara3d"
		"${DESTINATION_DIRECTORY}/python/src")

file(REMOVE
		"${DESTINATION_DIRECTORY}/CMakeLists.txt"
		"${DESTINATION_DIRECTORY}/python/CMakeLists.txt"
		"${DESTINATION_DIRECTORY}/python/pyproject.toml"
		"${DESTINATION_DIRECTORY}/python/requirements-build.txt")

file(COPY
		"${SOURCE_DIRECTORY}/CMakeLists.txt"
		DESTINATION "${DESTINATION_DIRECTORY}"
		USE_SOURCE_PERMISSIONS)

file(COPY
		"${SOURCE_DIRECTORY}/cmake"
		"${SOURCE_DIRECTORY}/lib"
		DESTINATION "${DESTINATION_DIRECTORY}"
		USE_SOURCE_PERMISSIONS)

foreach (_file
		CMakeLists.txt
		pyproject.toml
		requirements-build.txt)

	if (EXISTS "${SOURCE_DIRECTORY}/python/${_file}")
		file(COPY
				"${SOURCE_DIRECTORY}/python/${_file}"
				DESTINATION "${DESTINATION_DIRECTORY}/python"
				USE_SOURCE_PERMISSIONS)
	endif ()

endforeach ()

file(COPY
		"${SOURCE_DIRECTORY}/python/avara3d"
		"${SOURCE_DIRECTORY}/python/src"
		DESTINATION "${DESTINATION_DIRECTORY}/python"
		USE_SOURCE_PERMISSIONS)
