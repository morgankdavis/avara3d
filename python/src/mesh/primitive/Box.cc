//
//  Box.cc
//  avara3d-python
//
//  Created by Morgan Davis on 9/28/26.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#include "Bindings.h"

#include "a3d/mesh/Mesh.h"
#include "a3d/mesh/primitive/Box.h"
#include "a3d/visual/material/Material.h"

namespace py = pybind11;

namespace a3d::python {

void BindBox(py::module_& module) {

    py::class_<Box>(module, "Box")
        .def_static("mesh", &Box::Mesh, py::arg("width"), py::arg("height"), py::arg("length"),
                    py::arg("width_segments") = 1U, py::arg("height_segments") = 1U,
                    py::arg("length_segments") = 1U, py::arg("material") = nullptr);
}

} // namespace a3d::python
