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
        .def_static("black", &Color::Black)
        .def_static("dark_gray", &Color::DarkGray)
        .def_static("gray", &Color::Gray)
        .def_static("light_gray", &Color::LightGray)
        .def_static("white", &Color::White)
        .def_static("maroon", &Color::Maroon)
        .def_static("red", &Color::Red)
        .def_static("orange", &Color::Orange)
        .def_static("yellow", &Color::Yellow)
        .def_static("olive", &Color::Olive)
        .def_static("lime", &Color::Lime)
        .def_static("green", &Color::Green)
        .def_static("cyan", &Color::Cyan)
        .def_static("blue", &Color::Blue)
        .def_static("navy", &Color::Navy)
        .def_static("teal", &Color::Teal)
        .def_static("magenta", &Color::Magenta)
        .def_static("purple", &Color::Purple)
        .def_static("brown", &Color::Brown)
        .def_static("random", &Color::Random);
}

} // namespace a3d::python
