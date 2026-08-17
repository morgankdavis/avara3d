//
//  App.cc
//  scratch
//
//  Created by Morgan Davis on 7/11/26.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#include "App.h"

#include "a3d/a3d.h"
#include "../../extras/rug/alf/ALFImporter.h"

using namespace a3d;
using namespace a3d::math;
using namespace test::scratch;
using namespace std;

/// Private Constants ///

const Log::Level                  APP_LOG_LEVEL {Log::Level::Debug};
const uvec2                       WINDOW_SIZE {1280, 768};
const bool                        FULLSCREEN {false};
const bool                        ENABLE_HIGH_DPI {true};
const RenderContext::Antialiasing ANTIALIASING {RenderContext::Antialiasing::Msaa4X};
const bool                        ENABLE_VSYNC {false};
const bool                        CAPTURE_CURSOR {false};
const double                      TIMESTEP {1.0 / 120.0};
const bool                        DARK {false};

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

        visualWorld->background(Background {Color(u8vec3 {109, 136, 164})});

        visualWorld->surface(VisualWorld::Sphere {
            .center = {0.0f, -5000.0f, 0.0f},
            .radius = 5000.0f,
        });

        visualWorld->ground(Ground {
            .color = Color::DarkGray(),
            .minorGrid =
                Ground::Grid {
                    .color = Color(vec4 {0.5f, 0.5f, 0.5f, 0.25f}),
                    .spacing = 1.0f,
                    .lineWidthPixels = 1.0f,
                    .reliefStrength = -0.15f,
                },
            .majorGrid =
                Ground::Grid {
                    .color = Color(vec4 {0.75f, 0.75f, 0.75f, 0.25f}),
                    .spacing = 10.0f,
                    .lineWidthPixels = 1.0f,
                    .reliefStrength = -0.15f,
                },
            .horizonHaze =
                Ground::HorizonHaze {
                    .color = Color(vec4 {0.1f, 0.1f, 0.1f, 0.5f}),
                    .angularWidthDegrees = 2.5f,
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

        auto teapotNode = Node::MeshNode(util::fs::MeshAt("teapot/teapot.gltf"));
        teapotNode->scale(teapotNode->scale() * 10.0f);
        scene->rootNode()->addChild(teapotNode);
        _teapotNode = teapotNode;

        auto ambientLight = make_shared<AmbientLight>(Color(0.15f));
        auto ambientLightNode = Node::LightNode(ambientLight);
        scene->rootNode()->addChild(ambientLightNode);

        auto pointLight = make_shared<PointLight>(Color::White());
        pointLight->attenuation(Attenuation {
            .quadratic = 0.05f,
        });

        auto pointLightNode = Node::LightNode(pointLight);
        pointLightNode->position({5.0f, 5.0f, 5.0f});
        scene->rootNode()->addChild(pointLightNode);
        // auto pointLightMaterial = make_shared<Material>();
        // pointLightMaterial->emission(Color::White());
        // auto geometry = Sphere::Mesh(0.1, 4, pointLightMaterial);
        // pointLightNode->mesh(geometry);

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

SimulationConfig App::simulationConfig() const {
    return {.timeStep = TIMESTEP};
}

bool App::shouldContinue(const Scene& scene) {
    return _window->isOpen();
}

void App::inputDidUpdate(Runner&                         runner,
                         Scene&                          scene,
                         InputContext&                   inputContext,
                         const InputContext::UpdateInfo& info) {

    Window* window = nullptr;
    if (scene.visualWorld()) {
        window = dynamic_cast<Window*>(scene.visualWorld()->renderContext());
    }

    // get input

    auto& input = static_cast<DesktopInputContext&>(inputContext);

    using Key = DesktopInputContext::Key;
    using MouseButton = DesktopInputContext::MouseButton;

    if (input.keyPressed(Key::Escape)) {
        window->close();
    }

    if (input.keyPressed(Key::T)) {
        log::app::i()("TREE:\n{}", util::string::Tree(*(scene.rootNode())));
    }

    if (input.keyPressed(Key::P)) {
        if (auto pov = scene.visualWorld()->pointOfView().lock()) {
            auto world = pov->worldPosition() + pov->worldForward() * 10.0f;
            auto screen = scene.visualWorld()->projectPoint(world);
            auto roundTrip = scene.visualWorld()->unprojectPoint(screen);

            log::app::i()("world: {}, {}, {}", world.x, world.y, world.z);
            log::app::i()("screen: {}, {}, {}", screen.x, screen.y, screen.z);
            log::app::i()("roundTrip: {}, {}, {}", roundTrip.x, roundTrip.y, roundTrip.z);

            auto visualWorld = scene.visualWorld();
            auto viewportSize = visualWorld->renderContext()->viewportLogicalSize();

            const float centerX = float(viewportSize.x) * 0.5f;
            const float centerY = float(viewportSize.y) * 0.5f;

            auto nearPoint = visualWorld->unprojectPoint({centerX, centerY, 0.0f});
            auto farPoint = visualWorld->unprojectPoint({centerX, centerY, 1.0f});
            auto rayDirection = normalize(farPoint - nearPoint);

            if (auto pov = visualWorld->pointOfView().lock()) {
                auto cameraForward = pov->worldForward();
                log::app::i()("ray: {}, {}, {}", rayDirection.x, rayDirection.y, rayDirection.z);
                log::app::i()("forward: {}, {}, {}", cameraForward.x, cameraForward.y, cameraForward.z);
                log::app::i()("dot: {}", dot(rayDirection, cameraForward));
            }
        }
    }

    if (input.keyPressed(Key::One)) {

        //auto cameraNodes = vector<Node*>();
        auto cameraNodes = vector<std::shared_ptr<Node>>();
        for (auto& node : scene.rootNode()->children(true)) {
            auto camera = node->camera();
            if (camera) {
                cameraNodes.push_back(node);
            }
        }

        scene.visualWorld()->pointOfView(cameraNodes[0]);
    }

    if (input.keyPressed(Key::Two)) {

        auto cameraNodes = vector<std::shared_ptr<Node>>();
        for (auto& node : scene.rootNode()->children(true)) {
            auto camera = node->camera();
            if (camera) {
                cameraNodes.push_back(node);
            }
        }

        scene.visualWorld()->pointOfView(cameraNodes[1]);
    }

    if (input.keyPressed(Key::Three)) {

        auto cameraNodes = vector<std::shared_ptr<Node>>();
        for (auto& node : scene.rootNode()->children(true)) {
            auto camera = node->camera();
            if (camera) {
                cameraNodes.push_back(node);
            }
        }

        scene.visualWorld()->pointOfView(cameraNodes[2]);
    }

    using DebugOptions = Scene::DebugOptions;

    if (input.keyPressed(Key::F)) {
        if (util::bitmask::contains(scene.debugOptions(), DebugOptions::ShowWireframes)) {
            scene.debugOptions(util::bitmask::remove(scene.debugOptions(), DebugOptions::ShowWireframes));
        }
        else {
            scene.debugOptions(util::bitmask::add(scene.debugOptions(), DebugOptions::ShowWireframes));
        }
    }
    if (input.keyPressed(Key::B)) {
        if (util::bitmask::contains(scene.debugOptions(), DebugOptions::ShowBoundingBoxes)) {
            scene.debugOptions(util::bitmask::remove(scene.debugOptions(), DebugOptions::ShowBoundingBoxes));
        }
        else {
            scene.debugOptions(util::bitmask::add(scene.debugOptions(), DebugOptions::ShowBoundingBoxes));
        }
    }
    if (input.keyPressed(Key::I)) {
        if (util::bitmask::contains(scene.debugOptions(), DebugOptions::ShowStatsOverlay)) {
            scene.debugOptions(util::bitmask::remove(scene.debugOptions(), DebugOptions::ShowStatsOverlay));
        }
        else {
            scene.debugOptions(util::bitmask::add(scene.debugOptions(), DebugOptions::ShowStatsOverlay));
        }
    }
    if (input.keyPressed(Key::P)) {
        if (util::bitmask::contains(scene.debugOptions(), DebugOptions::ShowPhysicsBoundingBoxes)) {
            scene.debugOptions(util::bitmask::remove(scene.debugOptions(),
                                                     DebugOptions::ShowPhysicsBoundingBoxes));
        }
        else {
            scene.debugOptions(util::bitmask::add(scene.debugOptions(),
                                                  DebugOptions::ShowPhysicsBoundingBoxes));
        }
    }
    if (input.keyPressed(Key::G)) {
        if (util::bitmask::contains(scene.debugOptions(), DebugOptions::ShowPhysicsWireframes)) {
            scene.debugOptions(util::bitmask::remove(scene.debugOptions(),
                                                     DebugOptions::ShowPhysicsWireframes));
        }
        else {
            scene.debugOptions(util::bitmask::add(scene.debugOptions(), DebugOptions::ShowPhysicsWireframes));
        }
    }
    if (input.keyPressed(Key::C)) {
        if (util::bitmask::contains(scene.debugOptions(), DebugOptions::ShowPhysicsContactPoints)) {
            scene.debugOptions(util::bitmask::remove(scene.debugOptions(),
                                                     DebugOptions::ShowPhysicsContactPoints));
        }
        else {
            scene.debugOptions(util::bitmask::add(scene.debugOptions(),
                                                  DebugOptions::ShowPhysicsContactPoints));
        }
    }
    if (input.keyPressed(Key::N)) {
        if (util::bitmask::contains(scene.debugOptions(), DebugOptions::ShowPhysicsNormals)) {
            scene.debugOptions(util::bitmask::remove(scene.debugOptions(), DebugOptions::ShowPhysicsNormals));
        }
        else {
            scene.debugOptions(util::bitmask::add(scene.debugOptions(), DebugOptions::ShowPhysicsNormals));
        }
    }

    if (input.keyPressed(Key::V)) {
        window->vSyncEnabled(!window->vSyncEnabled());
    }

    if (input.keyPressed(Key::Backslash)) {
        util::snapshot::SaveSnapshot(*window);
    }

    if (input.keyPressed(Key::Slash)) {
        window->cursorCaptured(!(window->cursorCaptured()));
    }

    if (input.keyPressed(Key::R)) {
        if (!window->recordingGIF()) {
            util::snapshot::StartGIFRecording(*window, {320, 240}, 8);
        }
        else {
            util::snapshot::StopGIFRecording(*window);
        }
    }

    if (auto pov = scene.visualWorld()->pointOfView().lock(); pov && _window->cursorCaptured()) {
        _cameraController.update(*pov, input, info.deltaTime);
    }
}

void App::sceneWillStep(Runner& runner, Scene& scene, const Scene::StepInfo& info) {

    if (auto teapotNode = _teapotNode.lock()) {
        // rotate the teapot at 30 degrees per second
        const float rotation = static_cast<float>(info.deltaTime) * radians(-30.0f);
        const auto  rotationY = math::quaternion({0.0f, 1.0f, 0.0f}, rotation);
        teapotNode->orientation(rotationY * teapotNode->orientation());
    }
}
