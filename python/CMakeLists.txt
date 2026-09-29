find_package(Python REQUIRED COMPONENTS Interpreter Development.Module)

execute_process(
		COMMAND "${Python_EXECUTABLE}" -m pybind11 --cmakedir
		RESULT_VARIABLE _pybind11_result
		OUTPUT_VARIABLE _pybind11_cmake_dir
		ERROR_VARIABLE _pybind11_error
		OUTPUT_STRIP_TRAILING_WHITESPACE
)

if (NOT "${_pybind11_result}" STREQUAL "0")
	message(FATAL_ERROR
			"pybind11 is not installed for ${Python_EXECUTABLE}.\n"
			"Install python/requirements-build.txt into that environment.\n"
			"${_pybind11_error}"
	)
endif ()

find_package(pybind11 3.1 CONFIG REQUIRED
		PATHS "${_pybind11_cmake_dir}"
		NO_DEFAULT_PATH
)

pybind11_add_module(_avara3d MODULE
		src/Module.cc)

target_compile_features(_avara3d PRIVATE cxx_std_20)

target_link_libraries(_avara3d
		PRIVATE
		a3d::avara3d
)

set(A3D_PYTHON_PACKAGE_DIR
		"${CMAKE_CURRENT_BINARY_DIR}/avara3d")

file(MAKE_DIRECTORY
		"${A3D_PYTHON_PACKAGE_DIR}")

set_target_properties(_avara3d PROPERTIES
		LIBRARY_OUTPUT_DIRECTORY "${A3D_PYTHON_PACKAGE_DIR}"
		RUNTIME_OUTPUT_DIRECTORY "${A3D_PYTHON_PACKAGE_DIR}"
)

configure_file(
		avara3d/__init__.py
		"${A3D_PYTHON_PACKAGE_DIR}/__init__.py"
		COPYONLY
)
