//
//  Color.cc
//  avara3d-python
//
//  Created by Morgan Davis on 9/28/26.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#include "Bindings.h"

#include "a3d/Color.h"

namespace py = pybind11;

namespace a3d::python {

void BindColor(py::module_& module) {

    py::class_<Color>(module, "Color")
        .def(py::init<>())
        .def(py::init<float, float, float>(), py::arg("r"), py::arg("g"), py::arg("b"))
        .def(py::init<float, float, float, float>(), py::arg("r"), py::arg("g"), py::arg("b"), py::arg("a"))
        .def(py::init<const std::string&>(), py::arg("hex"))
        .def_property_readonly("r", &Color::r)
        .def_property_readonly("g", &Color::g)
        .def_property_readonly("b", &Color::b)
        .def_property_readonly("a", &Color::a)
        .def_static("blue", &Color::Blue);
}

} // namespace a3d::python
