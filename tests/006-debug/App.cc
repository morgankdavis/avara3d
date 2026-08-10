//
//  App.cc
//  006-debug
//
//  Created by Morgan Davis on 7/11/26.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#include "App.h"

#include "a3d/a3d.h"

using namespace a3d;
using namespace a3d::math;
using namespace test::debug;
using namespace std;

/// Private Constants ///

const Log::Level                      APP_LOG_LEVEL {Log::Level::Debug};
const uvec2                           WINDOW_SIZE {1280, 768};
const bool                            FULLSCREEN {false};
const bool                            ENABLE_HIGH_DPI {true};
const RenderContext::AntialiasingMode ANTIALIAS_MODE {RenderContext::AntialiasingMode::None};
const bool                            ENABLE_VSYNC {false};

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
        _window->cursorCaptured(false);

        auto visualWorld = make_unique<VisualWorld>(*_window);
        visualWorld->fog(Fog {
            .color = Color::LightGray(),
            .startDistance = 500.0f,
            .endDistance = 5000.0f,
            .transitionExponent = 1.0f,
        });
        visualWorld->background(Background {
            make_shared<Texture>(std::move(util::fs::CubeImageNamed("sky1", "png")))});

    //     visualWorld->backgroundOrientation(
    // quaternion(vec3(0.0f, 1.0f, 0.0f), radians(90.0f)));

        auto scene = make_unique<Scene>(std::move(visualWorld), nullptr, Window::InputContext());
        auto debugOptions = Scene::DebugOptions::ShowStatsOverlay | Scene::DebugOptions::ShowBoundingBoxes;
        scene->debugOptions(debugOptions);

        auto ambientLight = make_shared<AmbientLight>(Color::Gray());
        auto ambientLightNode = make_shared<Node>("Ambient light");
        ambientLightNode->light(ambientLight);
        scene->rootNode()->addChild(ambientLightNode);

        auto pointLight = make_shared<PointLight>(Color::White());
        pointLight->attenuation(Attenuation {.quadratic = 0.0001f});
        auto pointLightNode = make_shared<Node>();
        pointLightNode->light(pointLight);
        scene->rootNode()->addChild(pointLightNode);
        pointLightNode->position({100.0, 20.0, 20.0});

        auto materialProperty = pointLight->color();
        auto material = make_shared<Material>();
        material->name("LIGHT material");
        material->emission(materialProperty);
        auto mesh = shared_ptr(Sphere::Mesh(3.5, 4, material));
        pointLightNode->mesh(mesh);

        auto teapotNode = Node::MeshNode(util::fs::MeshNamed("teapot/teapot"));
        teapotNode->rotation({1, 0, 0}, radians(30.0));
        teapotNode->scale(teapotNode->scale() * 50.0f);
        scene->rootNode()->addChild(teapotNode);

        auto dragonNode = Node::MeshNode(util::fs::MeshNamed("dragon/dragon"));
        dragonNode->scale({2.5, 2.5, 2.5});
        dragonNode->position({50, 0, 0});

        scene->rootNode()->addChild(dragonNode);

        auto boxNode = Node::MeshNode(Box::Mesh(1.0, 1.0, 1.0));
        scene->rootNode()->addChild(boxNode);

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

    auto im = static_cast<DesktopInputContext*>(&inputContext);

    auto keysDown = im->keysDown();
    auto mousePositionDelta = im->mousePositionDelta();

    using Key = DesktopInputContext::Key;

    if (im->keyPressed(Key::Escape)) {
        window->close();
    }

    using DebugOptions = Scene::DebugOptions;

    if (im->keyPressed(Key::F)) {
        if (util::bitmask::contains(scene.debugOptions(), DebugOptions::ShowWireframes)) {
            scene.debugOptions(util::bitmask::remove(scene.debugOptions(), DebugOptions::ShowWireframes));
        }
        else {
            scene.debugOptions(util::bitmask::add(scene.debugOptions(), DebugOptions::ShowWireframes));
        }
    }
    if (im->keyPressed(Key::B)) {
        if (util::bitmask::contains(scene.debugOptions(), DebugOptions::ShowBoundingBoxes)) {
            scene.debugOptions(util::bitmask::remove(scene.debugOptions(), DebugOptions::ShowBoundingBoxes));
        }
        else {
            scene.debugOptions(util::bitmask::add(scene.debugOptions(), DebugOptions::ShowBoundingBoxes));
        }
    }
    if (im->keyPressed(Key::I)) {
        if (util::bitmask::contains(scene.debugOptions(), DebugOptions::ShowStatsOverlay)) {
            scene.debugOptions(util::bitmask::remove(scene.debugOptions(), DebugOptions::ShowStatsOverlay));
        }
        else {
            scene.debugOptions(util::bitmask::add(scene.debugOptions(), DebugOptions::ShowStatsOverlay));
        }
    }

    if (im->keyPressed(Key::V)) {
        window->vSyncEnabled(!(window->vSyncEnabled()));
    }

    if (im->keyPressed(Key::Backslash)) {
        util::snapshot::SaveSnapshot(*window);
    }

    if (im->keyPressed(Key::R)) {
        if (!window->recordingGIF()) {
            util::snapshot::StartGIFRecording(*window, {320, 240}, 8);
        }
        else {
            util::snapshot::StopGIFRecording(*window);
        }
    }
}
