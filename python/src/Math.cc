//
//  Math.h
//  avara3d-python
//
//  Created by Morgan Davis on 9/28/26.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#include "Bindings.h"

#include "a3d/Math.h"

namespace py = pybind11;

namespace a3d::python {

void BindMath(py::module_& module) {

    py::class_<math::uvec2>(module, "uvec2")
        .def(py::init<>())
        .def(py::init<math::u32, math::u32>(), py::arg("x"), py::arg("y"))
        .def_readwrite("x", &math::uvec2::x)
        .def_readwrite("y", &math::uvec2::y);

    py::class_<math::vec3>(module, "vec3")
        .def(py::init<>())
        .def(py::init<float, float, float>(), py::arg("x"), py::arg("y"), py::arg("z"))
        .def_readwrite("x", &math::vec3::x)
        .def_readwrite("y", &math::vec3::y)
        .def_readwrite("z", &math::vec3::z);

    module.def("radians", &math::radians, py::arg("degrees"));
    module.def("degrees", &math::degrees, py::arg("radians"));
}

} // namespace a3d::python
