//
//  Module.cc
//  python
//
//  Created by Morgan Davis on 9/28/26.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#include <functional>
#include <memory>
#include <utility>

#include <pybind11/pybind11.h>

#include "a3d/Application.h"
#include "a3d/Color.h"
#include "a3d/Math.h"
#include "a3d/mesh/Mesh.h"
#include "a3d/mesh/primitive/Box.h"
#include "a3d/render/context/RenderContext.h"
#include "a3d/render/context/Window.h"
#include "a3d/scene/Node.h"
#include "a3d/scene/Scene.h"
#include "a3d/visual/VisualWorld.h"
#include "a3d/visual/light/AmbientLight.h"
#include "a3d/visual/light/PointLight.h"
#include "a3d/visual/material/Material.h"

namespace py = pybind11;

namespace {

char  PythonProgramName[] = "python";
char* PythonArguments[] = {PythonProgramName, nullptr};

class PyApplication : public a3d::Application, public py::trampoline_self_life_support {

public:
    PyApplication():
        a3d::Application(1, PythonArguments) {}

protected:
    std::unique_ptr<a3d::Scene> init() override {
        PYBIND11_OVERRIDE_PURE(std::unique_ptr<a3d::Scene>, a3d::Application, init, );
    }

    bool shouldContinue(const a3d::Scene& scene) override {
        PYBIND11_OVERRIDE(bool, a3d::Application, shouldContinue, std::cref(scene));
    }
};

} // namespace

PYBIND11_MODULE(_avara3d, module) {

    module.doc() = "Python bindings for Avara3D";

    py::class_<a3d::math::uvec2>(module, "UVec2")
        .def(py::init<>())
        .def(py::init<a3d::math::u32, a3d::math::u32>(), py::arg("x"), py::arg("y"))
        .def_readwrite("x", &a3d::math::uvec2::x)
        .def_readwrite("y", &a3d::math::uvec2::y);

    py::enum_<a3d::RenderContext::Antialiasing>(module, "Antialiasing")
        .value("None_", a3d::RenderContext::Antialiasing::None)
        .value("Msaa2X", a3d::RenderContext::Antialiasing::Msaa2X)
        .value("Msaa4X", a3d::RenderContext::Antialiasing::Msaa4X)
        .value("Msaa8X", a3d::RenderContext::Antialiasing::Msaa8X)
        .value("Msaa16X", a3d::RenderContext::Antialiasing::Msaa16X);

    py::class_<a3d::RenderContext, py::smart_holder>(module, "RenderContext");

    py::class_<a3d::Window, a3d::RenderContext, py::smart_holder>(module, "Window")
        .def(py::init<const a3d::math::uvec2&, bool, bool, a3d::RenderContext::Antialiasing>(), py::arg("size"),
             py::arg("fullScreen"), py::arg("enableHighDPI") = true,
             py::arg("antialiasing") = a3d::RenderContext::Antialiasing::None)
        .def("open", &a3d::Window::open)
        .def("close", &a3d::Window::close)
        .def("isOpen", &a3d::Window::isOpen)
        .def("center", &a3d::Window::center);

    py::class_<a3d::VisualWorld, py::smart_holder>(module, "VisualWorld")
        .def(py::init<a3d::RenderContext&>(), py::arg("renderContext"));

    py::class_<a3d::Scene, py::smart_holder>(module, "Scene")
        .def(py::init<>())
        .def(py::init<const std::string&>(), py::arg("name"))
        .def(py::init([](std::unique_ptr<a3d::VisualWorld> visualWorld) {
                 auto scene = std::make_unique<a3d::Scene>();
                 scene->visualWorld(std::move(visualWorld));
                 return scene;
             }),
             py::arg("visualWorld"))
        .def_property_readonly("rootNode", [](const a3d::Scene& scene) {
            return scene.rootNode();
        });

    py::class_<a3d::Application, PyApplication, py::smart_holder>(module, "Application")
        .def(py::init_alias<>());

    py::class_<a3d::Color>(module, "Color")
        .def(py::init<>())
        .def(py::init<float, float, float>(), py::arg("r"), py::arg("g"), py::arg("b"))
        .def(py::init<float, float, float, float>(), py::arg("r"), py::arg("g"), py::arg("b"), py::arg("a"))
        .def(py::init<const std::string&>(), py::arg("hex"))
        .def_property_readonly("r", &a3d::Color::r)
        .def_property_readonly("g", &a3d::Color::g)
        .def_property_readonly("b", &a3d::Color::b)
        .def_property_readonly("a", &a3d::Color::a)
        .def_static("blue", &a3d::Color::Blue);

    py::class_<a3d::math::vec3>(module, "Vec3")
        .def(py::init<>())
        .def(py::init<float, float, float>(), py::arg("x"), py::arg("y"), py::arg("z"))
        .def_readwrite("x", &a3d::math::vec3::x)
        .def_readwrite("y", &a3d::math::vec3::y)
        .def_readwrite("z", &a3d::math::vec3::z);

    py::class_<a3d::Material, py::smart_holder>(module, "Material")
        .def_static(
            "diffuseMaterial",
            [](const a3d::Color& color) {
                return a3d::Material::DiffuseMaterial(color);
            },
            py::arg("color"));

    py::class_<a3d::Mesh, py::smart_holder>(module, "Mesh");

    py::class_<a3d::Box>(module, "Box")
        .def_static("mesh", &a3d::Box::Mesh, py::arg("width"), py::arg("height"), py::arg("length"),
                    py::arg("widthSegments") = 1U, py::arg("heightSegments") = 1U,
                    py::arg("lengthSegments") = 1U, py::arg("material") = nullptr);

    py::class_<a3d::AmbientLight, py::smart_holder>(module, "AmbientLight")
        .def(py::init<>())
        .def(py::init<const a3d::Color&>(), py::arg("color"));

    py::class_<a3d::PointLight, py::smart_holder>(module, "PointLight")
        .def(py::init<>())
        .def(py::init<const a3d::Color&>(), py::arg("color"));

    py::class_<a3d::Node, py::smart_holder>(module, "Node")
        .def(py::init<>())
        .def(py::init<const std::string&>(), py::arg("name"))
        .def_static("meshNode", &a3d::Node::MeshNode, py::arg("mesh"))
        .def_static(
            "lightNode",
            [](const std::shared_ptr<a3d::AmbientLight>& light) {
                return a3d::Node::LightNode(light);
            },
            py::arg("light"))
        .def_static(
            "lightNode",
            [](const std::shared_ptr<a3d::PointLight>& light) {
                return a3d::Node::LightNode(light);
            },
            py::arg("light"))
        .def("addChild", &a3d::Node::addChild, py::arg("node"), py::arg("reparent") = false)
        .def_property(
            "position",
            [](const a3d::Node& node) {
                return node.position();
            },
            [](a3d::Node& node, const a3d::math::vec3& position) {
                node.position(position);
            })
        .def_property(
            "eulerAngles",
            [](const a3d::Node& node) {
                return node.eulerAngles();
            },
            [](a3d::Node& node, const a3d::math::vec3& angles) {
                node.eulerAngles(angles);
            });

    module.def(
        "run",
        [](std::unique_ptr<a3d::Application> application) {
            return a3d::Application::Run(std::move(application));
        },
        py::arg("application"));
}
