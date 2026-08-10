//
//  App.cc
//  003-inputcamera
//
//  Created by Morgan Davis on 7/11/26.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#include "App.h"

#include <iostream>
#include <memory>
#include <string>
#include <utility>

#include "a3d/a3d.h"
#include "a3d/util/Filesystem.h"

using namespace a3d;
using namespace a3d::math;
using namespace test::inputcamera;
using namespace std;

/// Private Constants ///

const Log::Level                      APP_LOG_LEVEL {Log::Level::Debug};
const uvec2                           WINDOW_SIZE {1280, 768};
const bool                            FULLSCREEN {false};
const bool                            ENABLE_HIGH_DPI {true};
const RenderContext::AntialiasingMode ANTIALIAS_MODE {RenderContext::AntialiasingMode::Msaa4X};
const bool                            ENABLE_VSYNC {false};
const bool                            CAPTURE_CURSOR {false};

/// Public Lifecycle Functions ///

App::App(int argc, char* argv[]):
    Application(argc, argv, APP_LOG_LEVEL) {}

App::~App() = default;

/// Application Protected Member Functions ///

std::unique_ptr<Scene> App::init() {
    try {
        _window = make_unique<Window>(RenderContext::RenderingApi::OpenGL, *util::fs::ExecutableName(),
                                      WINDOW_SIZE, FULLSCREEN, ENABLE_HIGH_DPI, ANTIALIAS_MODE);
        _window->vSyncEnabled(ENABLE_VSYNC);
        _window->cursorCaptured(CAPTURE_CURSOR);

        auto visualWorld = make_unique<VisualWorld>(*_window);
        //visualWorld->usesDefaultLighting(true);
        visualWorld->background(Background {make_shared<Color>(u8vec3 {109, 136, 164})});

        auto scene = util::fs::SceneNamed("import_test/import_test");
        scene->visualWorld(std::move(visualWorld));
        scene->inputContext(Window::InputContext());
        scene->debugOptions(Scene::DebugOptions::ShowStatsOverlay);

        auto cameraConfig = _cameraController.config();
        cameraConfig.moveSpeed = math::max(scene->extent());
        _cameraController.config(cameraConfig);

        _window->center();
        _window->open();

        return scene;
    }
    catch (std::exception& e) {
        log::app::f()("Exception: {}", e.what());
        return nullptr;
    }
}

bool App::shouldContinue(const Scene& scene) {
    return _window->isOpen();
}

void App::inputDidUpdate(Runner&                         runner,
                         Scene&                          scene,
                         InputContext&                   inputContext,
                         const InputContext::UpdateInfo& info) {

    auto window = dynamic_cast<Window*>(scene.visualWorld()->renderContext());

    // get input

    auto input = static_cast<DesktopInputContext*>(&inputContext);

    using Key = DesktopInputContext::Key;

    if (input->keyPressed(Key::Slash)) {
        window->cursorCaptured(!(window->cursorCaptured()));
    }

    if (input->keyPressed(Key::Escape)) {
        _window->close();
    }

    if (auto pov = scene.visualWorld()->pointOfView().lock(); pov && window->cursorCaptured()) {
        _cameraController.update(*pov, *input, info.deltaTime);
    }
}
