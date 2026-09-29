find_package(Python REQUIRED COMPONENTS Interpreter Development.Module)
find_package(pybind11 3.1 CONFIG REQUIRED)

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
