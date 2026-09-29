//
//  Module.cc
//  avara3d
//

#include <pybind11/pybind11.h>

namespace py = pybind11;

PYBIND11_MODULE(_avara3d, module) {
    module.doc() = "Experimental Python bindings for Avara3D";

    module.def("hello", [] {
        return "Hello from Avara3D";
    });
}
