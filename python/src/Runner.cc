//
//  Runner.cc
//  avara3d-python
//
//  Created by Morgan Davis on 9/28/26.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#include "Bindings.h"

#include "a3d/Runner.h"

namespace py = pybind11;

namespace a3d::python {

void BindRunner(py::module_& module) {

    py::class_<Runner, py::smart_holder>(module, "Runner");
}

} // namespace a3d::python
