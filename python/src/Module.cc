//
//  Module.cc
//  python
//
//  Created by Morgan Davis on 9/28/26.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//


#include <pybind11/pybind11.h>

#include <pybind11/pybind11.h>

#include "a3d/Color.h"

namespace py = pybind11;

PYBIND11_MODULE(_avara3d, module) {

    module.doc() = "Python bindings for Avara3D";

    py::class_<a3d::Color>(module, "Color")
        .def(py::init<>())
        .def(py::init<float, float, float>(),
             py::arg("r"), py::arg("g"), py::arg("b"))
        .def(py::init<float, float, float, float>(),
             py::arg("r"), py::arg("g"), py::arg("b"), py::arg("a"))
        .def(py::init<const std::string&>(),
             py::arg("hex"))
        .def_property_readonly("r", &a3d::Color::r)
        .def_property_readonly("g", &a3d::Color::g)
        .def_property_readonly("b", &a3d::Color::b)
        .def_property_readonly("a", &a3d::Color::a)
        .def_static("blue", &a3d::Color::Blue);
}
