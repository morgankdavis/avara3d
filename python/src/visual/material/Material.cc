//
//  Color.cc
//  avara3d-python
//
//  Created by Morgan Davis on 9/28/26.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#include "Bindings.h"

#include "a3d/Color.h"
#include "a3d/visual/material/Material.h"

namespace py = pybind11;

namespace a3d::python {

void BindMaterial(py::module_& module) {

    py::class_<Material, py::smart_holder>(module, "Material")
        .def_static(
            "diffuseMaterial",
            [](const Color& color) {
                return Material::DiffuseMaterial(color);
            },
            py::arg("color"));
}

} // namespace a3d::python
