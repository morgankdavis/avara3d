//
//  VisualWorld.cc
//  avara3d-python
//
//  Created by Morgan Davis on 9/28/26.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#include "Bindings.h"

#include "a3d/render/context/RenderContext.h"
#include "a3d/visual/VisualWorld.h"

namespace py = pybind11;

namespace a3d::python {

void BindVisualWorld(py::module_& module) {

    auto visualWorldClass = py::class_<VisualWorld, py::smart_holder>(module, "VisualWorld");

    py::class_<VisualWorld::RenderInfo>(visualWorldClass, "RenderInfo")
        .def_readonly("frame_index", &VisualWorld::RenderInfo::frameIndex)
        .def_readonly("update_index", &VisualWorld::RenderInfo::updateIndex)
        .def_readonly("update_time", &VisualWorld::RenderInfo::updateTime)
        .def_readonly("update_delta_time", &VisualWorld::RenderInfo::updateDeltaTime)
        .def_readonly("simulation_time", &VisualWorld::RenderInfo::simulationTime)
        .def_readonly("simulation_step_count", &VisualWorld::RenderInfo::simulationStepCount);

    visualWorldClass.def(py::init<RenderContext&>(), py::arg("render_context"));
}

} // namespace a3d::python
