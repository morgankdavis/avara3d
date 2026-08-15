//
//  App.cc
//  import
//
//  Created by Morgan Davis on 7/11/26.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#include "App.h"

#include <iostream>
#include <memory>
#include <utility>

#include "a3d/a3d.h"
#include "a3d/util/Filesystem.h"

using namespace a3d;
using namespace a3d::math;
using namespace test::import;
using namespace std;

/// Private Constants ///

const Log::Level                  APP_LOG_LEVEL {Log::Level::Debug};
const uvec2                       WINDOW_SIZE {1280, 768};
const bool                        FULLSCREEN {false};
const bool                        ENABLE_HIGH_DPI {true};
const RenderContext::Antialiasing ANTIALIASING {RenderContext::Antialiasing::Msaa4X};
const bool                        ENABLE_VSYNC {false};
const bool                        CAPTURE_CURSOR {false};

/// Public Lifecycle Functions ///

App::App(int argc, char* argv[]):
    Application(argc, argv, APP_LOG_LEVEL) {}

App::~App() = default;

/// Application Protected Member Functions ///

std::unique_ptr<Scene> App::init() {
    try {
        _window = make_unique<Window>(RenderContext::RenderingApi::OpenGL, *util::fs::ExecutableName(),
                                      WINDOW_SIZE, FULLSCREEN, ENABLE_HIGH_DPI, ANTIALIASING);
        _window->vSyncEnabled(ENABLE_VSYNC);
        _window->cursorCaptured(CAPTURE_CURSOR);

        auto visualWorld = make_unique<VisualWorld>(*_window);
        visualWorld->background(Background {make_shared<Color>(u8vec3 {109, 136, 164})});

        auto scene = make_unique<Scene>();
        scene->visualWorld(std::move(visualWorld));
        scene->inputContext(Window::InputContext());

        auto options = Scene::ImportOptions::ImportMeshes | Scene::ImportOptions::ImportMaterials
                       | Scene::ImportOptions::ImportLights;
        auto testScene = util::fs::SceneAt("import_test/import_test.gltf", options);
        auto testSceneNodes = testScene->rootNode()->children();
        auto importLightsCamerasRoot = make_shared<Node>("importLightsCamerasRoot");
        auto importMeshRoot = make_shared<Node>("importMeshRoot");

        for (auto& node : testSceneNodes) {

            if (node->light() || node->camera()) {
                importLightsCamerasRoot->addChild(node);
            }
            else {
                importMeshRoot->addChild(node);
            }
        }

        scene->rootNode()->addChild(importLightsCamerasRoot);
        scene->rootNode()->addChild(importMeshRoot);

        scene->debugOptions(Scene::DebugOptions::ShowStatsOverlay);

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

void App::inputDidUpdate(Runner&       runner,
                         Scene&        scene,
                         InputContext& inputContext,
                         const InputContext::UpdateInfo&) {

    auto& input = static_cast<DesktopInputContext&>(inputContext);

    using Key = DesktopInputContext::Key;

    if (input.keyPressed(Key::Escape)) {
        _window->close();
    }
}
