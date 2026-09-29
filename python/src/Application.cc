//
//  Application.cc
//  avara3d-python
//
//  Created by Morgan Davis on 9/28/26.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#include "Bindings.h"

#include <functional>
#include <memory>
#include <utility>

#include "a3d/Application.h"
#include "a3d/Runner.h"
#include "a3d/scene/Scene.h"
#include "a3d/visual/VisualWorld.h"

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
        PYBIND11_OVERRIDE_NAME(
                bool,
                a3d::Application,
                "should_continue",
                shouldContinue,
                std::cref(scene));
    }

    void frameDidBegin(
        a3d::Runner&                         runner,
        a3d::Scene&                          scene,
        a3d::VisualWorld&                    visualWorld,
        const a3d::VisualWorld::RenderInfo& info) override {
        PYBIND11_OVERRIDE_NAME(
                void,
                a3d::Application,
                "frame_did_begin",
                frameDidBegin,
                std::ref(runner),
                std::ref(scene),
                std::ref(visualWorld),
                a3d::VisualWorld::RenderInfo {info});
    }
};

} // namespace

namespace a3d::python {

void BindApplication(py::module_& module) {

    py::class_<Application, PyApplication, py::smart_holder>(module, "Application").def(py::init_alias<>());

    module.def(
        "run",
        [](std::unique_ptr<Application> application) {
            return Application::Run(std::move(application));
        },
        py::arg("application"));
}

} // namespace a3d::python
