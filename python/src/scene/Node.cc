//
//  Node.cc
//  avara3d-python
//
//  Created by Morgan Davis on 9/28/26.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#include "Bindings.h"

#include <memory>

#include "a3d/Math.h"
#include "a3d/mesh/Mesh.h"
#include "a3d/scene/Node.h"
#include "a3d/visual/light/AmbientLight.h"
#include "a3d/visual/light/PointLight.h"

namespace py = pybind11;

namespace a3d::python {

void BindNode(py::module_& module) {

    py::class_<Node, py::smart_holder>(module, "Node")
        .def(py::init<>())
        .def(py::init<const std::string&>(), py::arg("name"))
        .def_static("mesh_node", &Node::MeshNode, py::arg("mesh"))
        .def_static(
            "light_node",
            [](const std::shared_ptr<AmbientLight>& light) {
                return Node::LightNode(light);
            },
            py::arg("light"))
        .def_static(
            "light_node",
            [](const std::shared_ptr<PointLight>& light) {
                return Node::LightNode(light);
            },
            py::arg("light"))
        .def("add_child", &Node::addChild, py::arg("node"), py::arg("reparent") = false)
        .def_property(
            "position",
            [](const Node& node) {
                return node.position();
            },
            [](Node& node, const math::vec3& position) {
                node.position(position);
            })
        .def_property(
            "euler_angles",
            [](const Node& node) {
                return node.eulerAngles();
            },
            [](Node& node, const math::vec3& angles) {
                node.eulerAngles(angles);
            });
}

} // namespace a3d::python
