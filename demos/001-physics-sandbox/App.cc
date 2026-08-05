//
//  App.cc
//  001-physics-sandbox
//
//  Created by Morgan Davis on 8/4/26.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#include "App.h"

#include "a3d/a3d.h"

using namespace a3d;
using namespace a3d::math;
using namespace test::physicssandbox;
using namespace std;

/// Private Constants ///

const Log::Level                      APP_LOG_LEVEL {Log::Level::Debug};
const uvec2                           WINDOW_SIZE {1280, 768};
const bool                            FULLSCREEN {false};
const bool                            ENABLE_HIGH_DPI {true};
const RenderContext::AntialiasingMode ANTIALIAS_MODE {RenderContext::AntialiasingMode::Msaa4X};
const bool                            ENABLE_VSYNC {false};
const bool                            CAPTURE_CURSOR {false};
const float                           MOUSE_SENSITIVITY {0.5};
const double                          TIMESTEP {1.0 / 120.0};
const bool                            DARK {false};

/// Public Lifecycle Functions ///

App::App(int argc, char* argv[]):
    Application(argc, argv, APP_LOG_LEVEL) {}

App::~App() = default;

/// Public Member Functions ///

std::unique_ptr<Scene> App::init() {
    try {
        _window = make_unique<Window>(RenderContext::RenderingApi::OpenGL, *util::filesystem::ExecutableName(),
                                      WINDOW_SIZE, FULLSCREEN, ENABLE_HIGH_DPI, ANTIALIAS_MODE);
        _window->vSyncEnabled(ENABLE_VSYNC);
        _window->cursorCaptured(CAPTURE_CURSOR);

        auto visualWorld = make_unique<VisualWorld>(*_window);
        auto backgroundColor = make_shared<Color>(u8vec3 {109, 136, 164});
        visualWorld->background(backgroundColor);

        auto physicsWorld = make_unique<PhysicsWorld>();

        auto scene =
            make_unique<Scene>(std::move(visualWorld), std::move(physicsWorld), Window::InputContext());
        scene->debugOptions(Scene::DebugOptions::ShowStatsOverlay);

        _bananaNode = Node::MeshNode(util::filesystem::MeshNamed("banana_lod/banana_lod"));
        auto rot90X = math::quaternion({1.0f, 0.0f, 0.0f}, radians(90.0f));
        auto rot90Y = math::quaternion({0.0f, 1.0f, 0.0f}, radians(90.0f));
        _bananaNode->orientation(rot90X * rot90Y);
        scene->rootNode()->addChild(_bananaNode);

        _window->center();
        _window->open();

        return scene;
    }
    catch (std::exception& e) {
        log::app::f()("Exception: {}", e.what());
        return nullptr;
    }
}

SimulationConfig App::simulationConfig() const {
    return {.timeStep = TIMESTEP};
}

bool App::shouldContinue(const Scene& scene) {
    return _window->isOpen();
}

void App::didShutdown() {}

/// InputContext Callbacks ///

void App::inputContextDidUpdate(InputContext& inputContext, const InputContext::UpdateInfo&) {

    auto& input = static_cast<DesktopInputContext&>(inputContext);

    using Key = DesktopInputContext::Key;

    if (input.keyPressed(Key::Escape)) {
        _window->close();
    }
}

/// Scene Callbacks ///

void App::sceneWillStep(Scene&, const Scene::StepInfo& info) {

    if (!_bananaNode) {
        return;
    }

    // Rotate the banana at 30 degrees per second.
    const float rotation = static_cast<float>(info.deltaTime) * radians(-30.0f);

    const auto rotationY = math::quaternion({0.0f, 1.0f, 0.0f}, rotation);

    _bananaNode->orientation(rotationY * _bananaNode->orientation());
}
