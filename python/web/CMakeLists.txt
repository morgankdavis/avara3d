include(ExternalProject)

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

message(STATUS
		"[PYTHON WEB CONTAINER: ${A3D_PYTHON_WEB_CONTAINER_RUNTIME}]")

### Pyodide runtime ###

set(A3D_PYODIDE_VERSION
		"314.0.7")

set(A3D_PYODIDE_COMMIT
		"b1e4fcc2488962f6360a97f19b9982d6fdb5d16f")

set(A3D_PYODIDE_PREFIX_DIR
		"${CMAKE_CURRENT_BINARY_DIR}/pyodide-${A3D_PYODIDE_VERSION}")

set(A3D_PYODIDE_SOURCE_DIR
		"${A3D_PYODIDE_PREFIX_DIR}/source")

set(A3D_PYODIDE_RUNTIME_FILES
		"${A3D_PYODIDE_SOURCE_DIR}/dist/pyodide.mjs"
		"${A3D_PYODIDE_SOURCE_DIR}/dist/pyodide.asm.mjs"
		"${A3D_PYODIDE_SOURCE_DIR}/dist/pyodide.asm.wasm")

ExternalProject_Add(a3d-python-web-pyodide
		PREFIX
		"${A3D_PYODIDE_PREFIX_DIR}"

		SOURCE_DIR
		"${A3D_PYODIDE_SOURCE_DIR}"

		GIT_REPOSITORY
		"https://github.com/pyodide/pyodide.git"

		GIT_TAG
		"${A3D_PYODIDE_COMMIT}"

		GIT_SUBMODULES_RECURSE
		TRUE

		GIT_PROGRESS
		TRUE

		UPDATE_DISCONNECTED
		TRUE

		CONFIGURE_COMMAND
		""

		BUILD_IN_SOURCE
		TRUE

		BUILD_COMMAND
		"<SOURCE_DIR>/run_docker"
		--non-interactive
		"EMSDK_NUM_CORES=${A3D_PYTHON_WEB_JOBS} EMCC_CORES=${A3D_PYTHON_WEB_JOBS} PYODIDE_JOBS=${A3D_PYTHON_WEB_JOBS} make EXTRA_LDFLAGS='--use-port=contrib.glfw3'"

		BUILD_BYPRODUCTS
		${A3D_PYODIDE_RUNTIME_FILES}

		INSTALL_COMMAND
		""

		USES_TERMINAL_DOWNLOAD
		TRUE

		USES_TERMINAL_BUILD
		TRUE

		EXCLUDE_FROM_ALL
		TRUE)

### Avara3D wheel ###

set(A3D_PYTHON_WEB_WHEEL_TAG
		"cp314-cp314-pyemscripten_2026_0_wasm32")

set(A3D_PYTHON_WEB_WHEEL_NAME
		"avara3d-${PROJECT_VERSION}-${A3D_PYTHON_WEB_WHEEL_TAG}.whl")

set(A3D_PYTHON_WEB_STAGED_SOURCE_DIR
		"${A3D_PYODIDE_SOURCE_DIR}/avara3d-source")

set(A3D_PYTHON_WEB_TEMP_WHEEL_DIR
		"${A3D_PYODIDE_SOURCE_DIR}/avara3d-wheels")

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

list(FILTER A3D_PYTHON_WEB_SOURCE_TREE_FILES
		EXCLUDE
		REGEX "/lib/data/")

list(APPEND A3D_PYTHON_WEB_SOURCE_FILES
		${A3D_PYTHON_WEB_SOURCE_TREE_FILES})

add_custom_command(
		OUTPUT
		"${A3D_PYTHON_WEB_WHEEL}"

		COMMAND
		"${CMAKE_COMMAND}"
		"-DSOURCE_DIRECTORY=${PROJECT_SOURCE_DIR}"
		"-DDESTINATION_DIRECTORY=${A3D_PYTHON_WEB_STAGED_SOURCE_DIR}"
		-P "${CMAKE_CURRENT_SOURCE_DIR}/cmake/StageAvara3D.cmake"

		COMMAND
		"${CMAKE_COMMAND}" -E rm -rf
		"${A3D_PYTHON_WEB_TEMP_WHEEL_DIR}"
		"${A3D_PYTHON_WEB_WHEEL_DIR}"

		COMMAND
		"${CMAKE_COMMAND}" -E make_directory
		"${A3D_PYTHON_WEB_TEMP_WHEEL_DIR}"
		"${A3D_PYTHON_WEB_WHEEL_DIR}"

		COMMAND
		"${A3D_PYODIDE_SOURCE_DIR}/run_docker"
		--non-interactive
		"cd /src/avara3d-source/python && pyodide build . --outdir /src/avara3d-wheels"

		COMMAND
		"${CMAKE_COMMAND}" -E copy_if_different
		"${A3D_PYTHON_WEB_TEMP_WHEEL_DIR}/${A3D_PYTHON_WEB_WHEEL_NAME}"
		"${A3D_PYTHON_WEB_WHEEL}"

		DEPENDS
		${A3D_PYODIDE_RUNTIME_FILES}
		"${CMAKE_CURRENT_SOURCE_DIR}/cmake/StageAvara3D.cmake"
		${A3D_PYTHON_WEB_SOURCE_FILES}

		WORKING_DIRECTORY
		"${A3D_PYODIDE_SOURCE_DIR}"

		COMMENT
		"Building Avara3D Python web wheel"

		USES_TERMINAL
		VERBATIM)

add_custom_target(a3d-python-web-wheel
		DEPENDS
		"${A3D_PYTHON_WEB_WHEEL}")

add_dependencies(a3d-python-web-wheel
		a3d-python-web-pyodide)

### Staged output ###

set(A3D_PYTHON_WEB_OUTPUT_DIR
		"${CMAKE_CURRENT_BINARY_DIR}/output")

set(A3D_PYTHON_WEB_OUTPUT_RUNTIME_DIR
		"${A3D_PYTHON_WEB_OUTPUT_DIR}/runtime")

set(A3D_PYTHON_WEB_OUTPUT_WHEEL_DIR
		"${A3D_PYTHON_WEB_OUTPUT_DIR}/wheels")

set(A3D_PYTHON_WEB_OUTPUT_RESOURCE_DIR
		"${A3D_PYTHON_WEB_OUTPUT_DIR}/resources")

set(A3D_PYTHON_WEB_POC_SOURCE_DIR
		"${CMAKE_CURRENT_SOURCE_DIR}/poc/rotating-cube")

set(A3D_PYTHON_WEB_POC_OUTPUT_DIR
		"${A3D_PYTHON_WEB_OUTPUT_DIR}/poc/rotating-cube")

set(A3D_PYTHON_WEB_CONFIGURED_POC_DIR
		"${CMAKE_CURRENT_BINARY_DIR}/configured/poc/rotating-cube")

set(A3D_PYTHON_WEB_STAMP_DIR
		"${CMAKE_CURRENT_BINARY_DIR}/stamps")

set(A3D_PYTHON_WEB_OUTPUT_STAMP
		"${A3D_PYTHON_WEB_STAMP_DIR}/output.stamp")

file(MAKE_DIRECTORY
		"${A3D_PYTHON_WEB_CONFIGURED_POC_DIR}"
		"${A3D_PYTHON_WEB_STAMP_DIR}")

configure_file(
		"${A3D_PYTHON_WEB_POC_SOURCE_DIR}/runner.html.in"
		"${A3D_PYTHON_WEB_CONFIGURED_POC_DIR}/runner.html"
		@ONLY)

file(GLOB_RECURSE A3D_PYTHON_WEB_RESOURCE_FILES
		CONFIGURE_DEPENDS
		LIST_DIRECTORIES FALSE
		"${PROJECT_SOURCE_DIR}/lib/data/*")

add_custom_command(
		OUTPUT
		"${A3D_PYTHON_WEB_OUTPUT_STAMP}"

		COMMAND
		"${CMAKE_COMMAND}" -E rm -rf
		"${A3D_PYTHON_WEB_OUTPUT_DIR}"

		COMMAND
		"${CMAKE_COMMAND}" -E make_directory
		"${A3D_PYTHON_WEB_OUTPUT_RUNTIME_DIR}"
		"${A3D_PYTHON_WEB_OUTPUT_WHEEL_DIR}"
		"${A3D_PYTHON_WEB_OUTPUT_RESOURCE_DIR}"
		"${A3D_PYTHON_WEB_POC_OUTPUT_DIR}"

		COMMAND
		"${CMAKE_COMMAND}" -E copy_directory
		"${A3D_PYODIDE_SOURCE_DIR}/dist"
		"${A3D_PYTHON_WEB_OUTPUT_RUNTIME_DIR}"

		COMMAND
		"${CMAKE_COMMAND}" -E copy_if_different
		"${A3D_PYTHON_WEB_WHEEL}"
		"${A3D_PYTHON_WEB_OUTPUT_WHEEL_DIR}/${A3D_PYTHON_WEB_WHEEL_NAME}"

		COMMAND
		"${CMAKE_COMMAND}" -E copy_directory
		"${PROJECT_SOURCE_DIR}/lib/data"
		"${A3D_PYTHON_WEB_OUTPUT_RESOURCE_DIR}"

		COMMAND
		"${CMAKE_COMMAND}" -E copy_if_different
		"${A3D_PYTHON_WEB_POC_SOURCE_DIR}/index.html"
		"${A3D_PYTHON_WEB_POC_OUTPUT_DIR}/index.html"

		COMMAND
		"${CMAKE_COMMAND}" -E copy_if_different
		"${A3D_PYTHON_WEB_CONFIGURED_POC_DIR}/runner.html"
		"${A3D_PYTHON_WEB_POC_OUTPUT_DIR}/runner.html"

		COMMAND
		"${CMAKE_COMMAND}" -E copy_if_different
		"${PROJECT_SOURCE_DIR}/python/tests/sandbox/rotating_cube.py"
		"${A3D_PYTHON_WEB_POC_OUTPUT_DIR}/rotating_cube.py"

		COMMAND
		"${CMAKE_COMMAND}" -E touch
		"${A3D_PYTHON_WEB_OUTPUT_STAMP}"

		DEPENDS
		${A3D_PYODIDE_RUNTIME_FILES}
		"${A3D_PYTHON_WEB_WHEEL}"
		"${A3D_PYTHON_WEB_POC_SOURCE_DIR}/index.html"
		"${A3D_PYTHON_WEB_POC_SOURCE_DIR}/runner.html.in"
		"${A3D_PYTHON_WEB_CONFIGURED_POC_DIR}/runner.html"
		"${PROJECT_SOURCE_DIR}/python/tests/sandbox/rotating_cube.py"
		${A3D_PYTHON_WEB_RESOURCE_FILES}

		BYPRODUCTS
		"${A3D_PYTHON_WEB_OUTPUT_RUNTIME_DIR}/pyodide.mjs"
		"${A3D_PYTHON_WEB_OUTPUT_WHEEL_DIR}/${A3D_PYTHON_WEB_WHEEL_NAME}"
		"${A3D_PYTHON_WEB_OUTPUT_RESOURCE_DIR}/shaders/skybox.vert"
		"${A3D_PYTHON_WEB_POC_OUTPUT_DIR}/index.html"
		"${A3D_PYTHON_WEB_POC_OUTPUT_DIR}/runner.html"
		"${A3D_PYTHON_WEB_POC_OUTPUT_DIR}/rotating_cube.py"

		COMMENT
		"Staging Avara3D Python web output"

		VERBATIM)

add_custom_target(a3d-python-web-output
		DEPENDS
		"${A3D_PYTHON_WEB_OUTPUT_STAMP}")

add_dependencies(a3d-python-web-output
		a3d-python-web-wheel)

### Public target ###

add_custom_target(a3d-python-web)

add_dependencies(a3d-python-web
		a3d-python-web-output)
