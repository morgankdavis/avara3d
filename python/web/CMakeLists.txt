cmake_host_system_information(
		RESULT _a3d_python_web_physical_cores
		QUERY NUMBER_OF_PHYSICAL_CORES)

if (NOT _a3d_python_web_physical_cores)
	set(_a3d_python_web_physical_cores 1)
endif ()

set(A3D_PYTHON_WEB_JOBS
		"${_a3d_python_web_physical_cores}"
		CACHE STRING
		"Parallel jobs used to build A3D Python web support")

if (NOT A3D_PYTHON_WEB_JOBS MATCHES "^[1-9][0-9]*$")
	message(FATAL_ERROR
			"A3D_PYTHON_WEB_JOBS must be a positive integer.")
endif ()

find_program(A3D_PYTHON_WEB_CONTAINER_RUNTIME
		NAMES docker podman
		REQUIRED)

set(A3D_PYODIDE_VERSION "314.0.7")
set(A3D_PYODIDE_BUILD_VERSION "0.36.1.dev18+g79fb82b")

set(A3D_PYODIDE_SOURCE_DIR
		"${CMAKE_CURRENT_SOURCE_DIR}/external/pyodide-${A3D_PYODIDE_VERSION}")

set(A3D_PYODIDE_WORK_DIR
		"${CMAKE_CURRENT_BINARY_DIR}/work/pyodide-${A3D_PYODIDE_VERSION}")

set(A3D_PYODIDE_STAMP_DIR
		"${CMAKE_CURRENT_BINARY_DIR}/stamps")

set(A3D_PYODIDE_BUILD_STAMP
		"${A3D_PYODIDE_STAMP_DIR}/pyodide-${A3D_PYODIDE_VERSION}.stamp")

file(MAKE_DIRECTORY
		"${A3D_PYODIDE_WORK_DIR}"
		"${A3D_PYODIDE_STAMP_DIR}")

file(GLOB_RECURSE A3D_PYODIDE_SOURCE_FILES
		CONFIGURE_DEPENDS
		LIST_DIRECTORIES FALSE
		"${A3D_PYODIDE_SOURCE_DIR}/*")

add_custom_command(
		OUTPUT
		"${A3D_PYODIDE_BUILD_STAMP}"

		COMMAND
		"${CMAKE_COMMAND}"
		"-DSOURCE_DIRECTORY=${A3D_PYODIDE_SOURCE_DIR}"
		"-DDESTINATION_DIRECTORY=${A3D_PYODIDE_WORK_DIR}"
		-P "${CMAKE_CURRENT_SOURCE_DIR}/cmake/CopyDirectory.cmake"

		COMMAND
		"${A3D_PYODIDE_WORK_DIR}/run_docker"
		--non-interactive
		"SETUPTOOLS_SCM_PRETEND_VERSION=${A3D_PYODIDE_BUILD_VERSION} EMSDK_NUM_CORES=${A3D_PYTHON_WEB_JOBS} EMCC_CORES=${A3D_PYTHON_WEB_JOBS} PYODIDE_JOBS=${A3D_PYTHON_WEB_JOBS} make EXTRA_LDFLAGS='--use-port=contrib.glfw3'"

		COMMAND
		"${CMAKE_COMMAND}" -E touch
		"${A3D_PYODIDE_BUILD_STAMP}"

		DEPENDS
		${A3D_PYODIDE_SOURCE_FILES}
		"${CMAKE_CURRENT_SOURCE_DIR}/cmake/CopyDirectory.cmake"

		BYPRODUCTS
		"${A3D_PYODIDE_WORK_DIR}/dist/pyodide.mjs"
		"${A3D_PYODIDE_WORK_DIR}/dist/pyodide.asm.mjs"
		"${A3D_PYODIDE_WORK_DIR}/dist/pyodide.asm.wasm"

		WORKING_DIRECTORY
		"${A3D_PYODIDE_WORK_DIR}"

		COMMENT
		"Building Pyodide ${A3D_PYODIDE_VERSION} with GLFW support"

		USES_TERMINAL
		VERBATIM)

add_custom_target(a3d-python-web-pyodide
		DEPENDS
		"${A3D_PYODIDE_BUILD_STAMP}")

set(A3D_PYTHON_WEB_WHEEL_TAG
		"cp314-cp314-pyemscripten_2026_0_wasm32")

set(A3D_PYTHON_WEB_WHEEL_NAME
		"avara3d-${PROJECT_VERSION}-${A3D_PYTHON_WEB_WHEEL_TAG}.whl")

set(A3D_PYTHON_WEB_WHEEL_SOURCE_DIR
		"${A3D_PYODIDE_WORK_DIR}/avara3d-source")

set(A3D_PYTHON_WEB_TEMP_WHEEL_DIR
		"${A3D_PYODIDE_WORK_DIR}/avara3d-wheels")

set(A3D_PYTHON_WEB_WHEEL_DIR
		"${CMAKE_CURRENT_BINARY_DIR}/wheels")

set(A3D_PYTHON_WEB_WHEEL
		"${A3D_PYTHON_WEB_WHEEL_DIR}/${A3D_PYTHON_WEB_WHEEL_NAME}")

set(A3D_PYTHON_WEB_SOURCE_FILES
		"${PROJECT_SOURCE_DIR}/CMakeLists.txt"
		"${PROJECT_SOURCE_DIR}/python/CMakeLists.txt"
		"${PROJECT_SOURCE_DIR}/python/pyproject.toml"
		"${PROJECT_SOURCE_DIR}/python/requirements-build.txt")

file(GLOB_RECURSE A3D_PYTHON_WEB_SOURCE_TREE_FILES
		CONFIGURE_DEPENDS
		LIST_DIRECTORIES FALSE
		"${PROJECT_SOURCE_DIR}/cmake/*"
		"${PROJECT_SOURCE_DIR}/lib/*"
		"${PROJECT_SOURCE_DIR}/python/avara3d/*"
		"${PROJECT_SOURCE_DIR}/python/src/*")

list(APPEND A3D_PYTHON_WEB_SOURCE_FILES
		${A3D_PYTHON_WEB_SOURCE_TREE_FILES})

add_custom_command(
		OUTPUT
		"${A3D_PYTHON_WEB_WHEEL}"

		COMMAND
		"${CMAKE_COMMAND}"
		"-DSOURCE_DIRECTORY=${PROJECT_SOURCE_DIR}"
		"-DDESTINATION_DIRECTORY=${A3D_PYTHON_WEB_WHEEL_SOURCE_DIR}"
		-P "${CMAKE_CURRENT_SOURCE_DIR}/cmake/StageAvara3D.cmake"

		COMMAND
		"${CMAKE_COMMAND}" -E rm -rf
		"${A3D_PYTHON_WEB_TEMP_WHEEL_DIR}"

		COMMAND
		"${CMAKE_COMMAND}" -E make_directory
		"${A3D_PYTHON_WEB_TEMP_WHEEL_DIR}"
		"${A3D_PYTHON_WEB_WHEEL_DIR}"

		COMMAND
		"${A3D_PYODIDE_WORK_DIR}/run_docker"
		--non-interactive
		"cd /src/avara3d-source/python && pyodide build . --outdir /src/avara3d-wheels"

		COMMAND
		"${CMAKE_COMMAND}" -E copy
		"${A3D_PYTHON_WEB_TEMP_WHEEL_DIR}/${A3D_PYTHON_WEB_WHEEL_NAME}"
		"${A3D_PYTHON_WEB_WHEEL}"

		DEPENDS
		"${A3D_PYODIDE_BUILD_STAMP}"
		"${CMAKE_CURRENT_SOURCE_DIR}/cmake/StageAvara3D.cmake"
		${A3D_PYTHON_WEB_SOURCE_FILES}

		WORKING_DIRECTORY
		"${A3D_PYODIDE_WORK_DIR}"

		COMMENT
		"Building Avara3D Python web wheel"

		USES_TERMINAL
		VERBATIM)

add_custom_target(a3d-python-web-wheel
		DEPENDS
		"${A3D_PYTHON_WEB_WHEEL}")

add_dependencies(a3d-python-web-wheel
		a3d-python-web-pyodide)

add_custom_target(a3d-python-web
		DEPENDS
		a3d-python-web-pyodide
		a3d-python-web-wheel)
