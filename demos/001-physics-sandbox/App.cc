//
//  App.cc
//  001-physics-sandbox
//
//  Created by Morgan Davis on 8/4/26.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#include "App.h"

#include <algorithm>
#include <cmath>

#include "a3d/a3d.h"

using namespace a3d;
using namespace a3d::math;
using namespace demo::physicssandbox;
using namespace std;

/// Private Constants ///

const Log::Level                      APP_LOG_LEVEL {Log::Level::Debug};
const uvec2                           WINDOW_SIZE {1280, 768};
const bool                            FULLSCREEN {false};
const bool                            ENABLE_HIGH_DPI {true};
const RenderContext::AntialiasingMode ANTIALIAS_MODE {RenderContext::AntialiasingMode::Msaa2X};
const bool                            ENABLE_VSYNC {false};
const bool                            CAPTURE_CURSOR {false};
const float                           MOUSE_SENSITIVITY {0.5};
#if defined(A3D_WEB)
const float TIMESTEP {1.0 / 60.0};
#else
const float TIMESTEP {1.0 / 120.0};
#endif

const float BACKGROUND_ROTATION_SPEED {radians(0.5f)};
const vec3  BACKGROUND_ROTATION_AXIS {0.5f, 1.0f, 1.0f};

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

        visualWorld->background(Background {
            make_shared<Texture>(std::move(util::fs::CubeImageNamed("nebula", "png")))});

        visualWorld->infiniteGround(InfiniteGround {
            .color = Color::DarkGray(),
            .height = 0.0f,

            .minorGrid =
                InfiniteGround::Grid {
                    .color = make_shared<Color>(vec4 {0.5f, 0.5f, 0.5f, 0.25f}),
                    .spacing = 1.0f,
                    .lineWidthPixels = 1.0f,
                },

            .majorGrid =
                InfiniteGround::Grid {
                    .color = make_shared<Color>(vec4 {0.75f, 0.75f, 0.75f, 0.25f}),
                    .spacing = 10.0f,
                    .lineWidthPixels = 1.5f,
                },

            // .radialFade =
            InfiniteGround::RadialFade {
                .color = Color::Black(),
                .center = {0.0f, 0.0f},
                .startDistance = 8.0f,
                .endDistance = 30.0f,
            },

            .specularIntensity = 0.15f,
            .specularExponent = 32.0f,
        });

        auto physicsWorld = make_unique<PhysicsWorld>();

        auto scene =
            make_unique<Scene>(std::move(visualWorld), std::move(physicsWorld), Window::InputContext());
        scene->debugOptions(Scene::DebugOptions::ShowStatsOverlay | Scene::DebugOptions::ShowBoundingBoxes);

        auto groundNode = Node::NamedNode("Ground");
        groundNode->orientation(math::quaternion({1.0f, 0.0f, 0.0f}, radians(-90.0f)));
        auto groundShape = make_shared<InfinitePlanePhysicsShape>();
        auto groundBody = make_unique<PhysicsBody>(PhysicsBody::Type::Static, groundShape);
        groundNode->physicsBody(std::move(groundBody));
        scene->rootNode()->addChild(groundNode);

        auto testBoxNode = Node::MeshNode(Box::Mesh(1.0f, 1.0f, 1.0f));
        testBoxNode->name("Ground test box");
        testBoxNode->position({0.0f, 5.0f, 0.0f});
        testBoxNode->physicsBody(PhysicsBody::DynamicBody());
        scene->rootNode()->addChild(testBoxNode);

        auto ambientLight = make_shared<AmbientLight>(make_shared<Color>(0.15f));
        auto ambientLightNode = Node::LightNode(ambientLight);
        scene->rootNode()->addChild(ambientLightNode);

        auto pointLight = make_shared<PointLight>(Color::White());
        pointLight->attenuation(Attenuation {
            .quadratic = 0.05f,
        });

        auto pointLightNode = Node::LightNode(pointLight);
        pointLightNode->position({0.0f, 4.0f, 0.0f});
        scene->rootNode()->addChild(pointLightNode);

        _bananaNode = Node::MeshNode(util::fs::MeshNamed("banana_lod/banana_lod"));
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

void App::sceneWillStep(Runner& runner, Scene& scene, const Scene::StepInfo& info) {

    if (!_bananaNode) {
        return;
    }

    // // rotate the banana at 30 degrees per second
    // const float rotation = static_cast<float>(info.deltaTime) * radians(-30.0f);
    // const auto rotationY = math::quaternion({0.0f, 1.0f, 0.0f}, rotation);
    // _bananaNode->orientation(rotationY * _bananaNode->orientation());
}

void App::frameDidBegin(Runner&, Scene&, VisualWorld& visualWorld, const VisualWorld::RenderInfo& info) {

    const float  delta = static_cast<float>(info.updateDeltaTime);
    static float angle = radians(90.0);
    angle += delta * BACKGROUND_ROTATION_SPEED;
    if (auto& background = visualWorld.background()) {
        background->orientation(quaternion(BACKGROUND_ROTATION_AXIS, angle));
    }
}
