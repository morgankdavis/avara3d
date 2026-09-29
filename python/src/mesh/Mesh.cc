//
//  Mesh.cc
//  avara3d-python
//
//  Created by Morgan Davis on 9/28/26.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#include "Bindings.h"

#include "a3d/mesh/Mesh.h"

namespace py = pybind11;

namespace a3d::python {

void BindMesh(py::module_& module) {

    py::class_<Mesh, py::smart_holder>(module, "Mesh");
}

} // namespace a3d::python
