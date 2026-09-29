add_custom_target(a3d-python-sandbox-rotating-cube

		COMMAND
		"${CMAKE_COMMAND}" -E env
		"PYTHONPATH=$<TARGET_FILE_DIR:_avara3d>/.."
		"${Python_EXECUTABLE}"
		"${CMAKE_CURRENT_SOURCE_DIR}/rotating_cube.py"

		DEPENDS
		_avara3d

		WORKING_DIRECTORY
		"${PROJECT_SOURCE_DIR}"

		COMMENT
		"Running Python rotating-cube sandbox"

		USES_TERMINAL
		VERBATIM

		SOURCES
		rotating_cube.py)
