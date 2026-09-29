//
//  RenderContext.cc
//  avara3d-python
//
//  Created by Morgan Davis on 9/28/26.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#include "Bindings.h"

#include "a3d/render/context/RenderContext.h"

namespace py = pybind11;

namespace a3d::python {

void BindRenderContext(py::module_& module) {

    py::enum_<RenderContext::Antialiasing>(module, "Antialiasing")
        .value("None_", RenderContext::Antialiasing::None)
        .value("Msaa2X", RenderContext::Antialiasing::Msaa2X)
        .value("Msaa4X", RenderContext::Antialiasing::Msaa4X)
        .value("Msaa8X", RenderContext::Antialiasing::Msaa8X)
        .value("Msaa16X", RenderContext::Antialiasing::Msaa16X);

    py::class_<RenderContext, py::smart_holder>(module, "RenderContext");
}

} // namespace a3d::python
