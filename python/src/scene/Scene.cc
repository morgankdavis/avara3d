//
//  Scene.cc
//  avara3d-python
//
//  Created by Morgan Davis on 9/28/26.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#include "Bindings.h"

#include <memory>
#include <utility>

#include "a3d/scene/Node.h"
#include "a3d/scene/Scene.h"
#include "a3d/visual/VisualWorld.h"

namespace py = pybind11;

namespace a3d::python {

void BindScene(py::module_& module) {

    py::class_<Scene, py::smart_holder>(module, "Scene")
        .def(py::init<>())
        .def(py::init<const std::string&>(), py::arg("name"))
        .def(py::init([](std::unique_ptr<VisualWorld> visualWorld) {
                 auto scene = std::make_unique<Scene>();
                 scene->visualWorld(std::move(visualWorld));
                 return scene;
             }),
             py::arg("visualWorld"))
        .def_property_readonly("rootNode", [](const Scene& scene) {
            return scene.rootNode();
        });
}

} // namespace a3d::python
