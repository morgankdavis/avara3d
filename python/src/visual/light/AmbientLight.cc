//
//  AmbientLight.cc
//  avara3d-python
//
//  Created by Morgan Davis on 9/28/26.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#include "Bindings.h"

#include "a3d/Color.h"
#include "a3d/visual/light/AmbientLight.h"

namespace py = pybind11;

namespace a3d::python {

void BindAmbientLight(py::module_& module) {

    py::class_<AmbientLight, py::smart_holder>(module, "AmbientLight")
        .def(py::init<>())
        .def(py::init<const Color&>(), py::arg("color"));
}

} // namespace a3d::python
