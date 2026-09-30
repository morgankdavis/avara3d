cmake_minimum_required(VERSION 3.20)

project(glfw_poc LANGUAGES CXX)

set(PYBIND11_USE_CROSSCOMPILING ON)

find_package(pybind11 CONFIG REQUIRED)

pybind11_add_module(_glfw_poc MODULE NO_EXTRAS
		src/Module.cc)

target_compile_features(_glfw_poc PRIVATE cxx_std_20)

# Supply GLFW's headers while compiling the side module. GLFW itself and its
# JavaScript support already live in the customized Pyodide main module.
target_compile_options(_glfw_poc PRIVATE
		"--use-port=contrib.glfw3")

install(TARGETS _glfw_poc
		LIBRARY DESTINATION .)
