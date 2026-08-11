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
#include <thread>

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
const RenderContext::AntialiasingMode ANTIALIAS_MODE {RenderContext::AntialiasingMode::Msaa4X};
const bool                            ENABLE_VSYNC {false};
const bool                            CAPTURE_CURSOR {false};
const float                           TIMESTEP {1.0 / 120.0};
const float                           BACKGROUND_ROTATION_SPEED {radians(0.5f)};
const vec3                            BACKGROUND_ROTATION_AXIS {0.5f, 1.0f, 1.0f};
const vec3                            GRAVITY_EARTH {0.0f, -9.807f, 0.0f};
const vec3                            GRAVITY_MOON {0.0f, -1.62f, 0.0f};
const vec3                            GRAVITY_ZERO {0.0f, 0.0f, 0.0f};

/// Private Static Non-Member Prototypes ///

void ShootSlurm(Scene& scene, const vec3& location, const vec3& direction);

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
                    .reliefStrength = -0.1f,
                },
            .majorGrid =
                InfiniteGround::Grid {
                    .color = make_shared<Color>(vec4 {0.75f, 0.75f, 0.75f, 0.25f}),
                    .spacing = 10.0f,
                    .lineWidthPixels = 1.0f,
                    .reliefStrength = -0.1f,
                },
            .radialFade =
                InfiniteGround::RadialFade {
                    .color = make_shared<Color>(vec4 {0.02f, 0.02f, 0.02f, 1.0f}),
                    .center = {0.0f, 0.0f},
                    .startDistance = 10.0f,
                    .endDistance = 100.0f,
                },
            .curvature =
                InfiniteGround::Curvature {
                    .center = {0.0f, 0.0f},
                    .radius = 5000.0f,
                },
            .horizonHaze =
                InfiniteGround::HorizonHaze {
                    .color = make_shared<Color>(vec4 {0.075f, 0.075f, 0.075f, 0.65f}),
                    .angularWidthDegrees = 4.5f,
                },
            .specularIntensity = 0.15f,
            .specularExponent = 32.0f,
        });

        visualWorld->fog(Fog {.color = Color::Black(), .startDistance = 30.0f, .endDistance = 150.0f});

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

        // {
        //     auto testBoxNode = Node::MeshNode(Box::Mesh(1.0f, 1.0f, 1.0f));
        //     testBoxNode->name("Ground test box");
        //     testBoxNode->position({0.0f, 5.0f, 0.0f});
        //     testBoxNode->physicsBody(PhysicsBody::DynamicBody());
        //     {
        //         auto testBoxMaterial = make_shared<Material>();
        //         testBoxMaterial->emission(Color::White());
        //         testBoxNode->mesh()->addMaterial(testBoxMaterial);
        //     }
        //     scene->rootNode()->addChild(testBoxNode);
        //
        //
        //     auto pointLight = make_shared<PointLight>(Color::White());
        //     pointLight->attenuation(Attenuation {
        //         .quadratic = 0.05f,
        //     });
        //     auto pointLightNode = Node::LightNode(pointLight);
        //     pointLightNode->position({0.0f, 5.0f, 0.0f});
        //     //scene->rootNode()->addChild(pointLightNode);
        //     testBoxNode->addChild(pointLightNode);
        // }

        //scene->visualWorld()->usesDefaultLighting(true);

        auto ambientLight = make_shared<AmbientLight>(make_shared<Color>(0.15f));
        auto ambientLightNode = Node::LightNode(ambientLight);
        scene->rootNode()->addChild(ambientLightNode);

        {
            auto pointLight = make_shared<PointLight>(Color::White());
            pointLight->attenuation(Attenuation {
                .quadratic = 0.05f,
            });
            auto pointLightNode = Node::LightNode(pointLight);
            pointLightNode->position({0.0f, 4.0f, 0.0f});
            scene->rootNode()->addChild(pointLightNode);
        }


        // janus

        // auto janusNode = Node::MeshNode(util::fs::MeshNamed("janus/janus"));
        // scene->rootNode()->addChild(janusNode);

        auto janusNode = Node::MeshNode(util::fs::MeshNamed("janus_lod/janus_lod"));
        scene->rootNode()->addChild(janusNode);






        // angel

        // auto anielNode = Node::MeshNode(util::fs::MeshNamed("aniel/aniel"));
        // scene->rootNode()->addChild(anielNode);
        // auto anielExtent = anielNode->extent();


        // constexpr float ANGEL_HEIGHT = 2.0f;
        // auto            angelNode = Node::MeshNode(util::fs::MeshNamed("aniel/aniel"));
        // angelNode->scale(angelNode->scale() * (ANGEL_HEIGHT / angelNode->extent(true).y));
        // angelNode->physicsBody(PhysicsBody::StaticBody());
        // scene->rootNode()->addChild(angelNode);


        // constexpr float ANGEL_HEIGHT = 2.0f;
        // auto            angelMesh = util::fs::MeshNamed("aniel_lod/aniel_lod");
        // const float     scaleFactor = ANGEL_HEIGHT / angelMesh->localExtent().y;
        // angelMesh->burnTransform(math::scale(mat4(1.0f), vec3(scaleFactor)), true);
        // auto angelNode = Node::MeshNode(angelMesh);
        // angelNode->name("Angel");
        // angelNode->rotation({0.0f, 1.0f, 0.0f}, radians(180.0f));
        // angelNode->physicsBody(PhysicsBody::StaticBody());
        // scene->rootNode()->addChild(angelNode);


        // {
        //     auto topLight = Light::Spot(Color::White());
        //     topLight->innerAngle(radians(18.0f));
        //     topLight->outerAngle(radians(28.0f));
        //
        //     auto topLightNode = Node::LightNode(topLight);
        //     topLightNode->position({0.0f, 2.5, -0.5});
        //     topLightNode->eulerAngles({
        //         radians(-65.0f), // pitch: straight down
        //         radians(0.0f),
        //         radians(0.0f)
        //     });
        //
        //     scene->rootNode()->addChild(topLightNode);
        // }

        // {
        //     auto bottomLight = Light::Spot(Color::White());
        //     bottomLight->innerAngle(radians(18.0f));
        //     bottomLight->outerAngle(radians(28.0f));
        //
        //     auto bottomLightNode = Node::LightNode(bottomLight);
        //     bottomLightNode->position({0.0f, 0.15f, -0.45f});
        //     bottomLightNode->eulerAngles({
        //         radians(65.0f), // pitch: straight up
        //         radians(0.0f),
        //         radians(0.0f)
        //     });
        //
        //     scene->rootNode()->addChild(bottomLightNode);
        // }



        // banana

        // _bananaNode = Node::MeshNode(util::fs::MeshNamed("banana_lod/banana_lod"));
        // auto rx = math::quaternion({1.0f, 0.0f, 0.0f}, radians(90.0f));
        // auto ry = math::quaternion({0.0f, 1.0f, 0.0f}, radians(90.0f));
        // _bananaNode->orientation(rx * ry);
        // scene->rootNode()->addChild(_bananaNode);


        // teapot

        // {
        //     constexpr float TEAPOT_HEIGHT = 0.35f;
        //     auto            teapotMesh = util::fs::MeshNamed("teapot/teapot");
        //     const float     teapotScale = TEAPOT_HEIGHT / teapotMesh->localExtent().y;
        //     teapotMesh->burnTransform(math::scale(mat4(1.0f), vec3(teapotScale)), true);
        //     teapotMesh->replaceMaterial(0, Material::DiffuseMaterial(Color::DarkGray()));
        //
        //     auto teapotNode = Node::MeshNode(teapotMesh);
        //     teapotNode->name("Teapot");
        //
        //     // put its bottom ~1 meter above the ground so it drops in
        //     teapotNode->position({1.5f, 1.0f - teapotMesh->localAABB().min.y, 0.0f});
        //
        //     // dynamic physics; this will auto-create a convex-hull shape
        //     auto teapotBody = PhysicsBody::DynamicBody();
        //     teapotBody->mass(1.5f);
        //     teapotBody->friction(0.6f);
        //     teapotBody->restitution(0.15f);
        //     teapotBody->linearDamping(0.03f);
        //     teapotBody->angularDamping(0.05f);
        //
        //     teapotNode->physicsBody(std::move(teapotBody));
        //
        //     scene->rootNode()->addChild(teapotNode);
        // }


        // camera

        auto camera = make_shared<PerspectiveCamera>(0.1f, 1000.0f, radians(45.0f));
        _cameraNode = Node::CameraNode(camera);
        _cameraNode->name("Turntable camera");
        scene->rootNode()->addChild(_cameraNode);
        scene->visualWorld()->pointOfView(_cameraNode);

        auto cameraConfig = _cameraController.config();
        cameraConfig.controls.primaryButton = DesktopInputContext::MouseButton::Two;
        cameraConfig.invertPitch = true;
        _cameraController.config(cameraConfig);

        _cameraController.view({
            .target = vec3 {0.0f, 1.0f, 0.0f},
            .yaw = radians(35.0f),
            .pitch = radians(20.0f),
            .distance = 20.0f,
        });

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

void App::inputDidUpdate(Runner&, Scene&, InputContext& inputContext, const InputContext::UpdateInfo&) {

    auto& input = static_cast<DesktopInputContext&>(inputContext);

    using Key = DesktopInputContext::Key;

    if (input.keyPressed(Key::Escape)) {
        _window->close();
    }

    if (!_cameraNode) {
        //_window->cursorHidden(false);
        _window->cursorCaptured(false);
        return;
    }

    const auto camera = static_pointer_cast<PerspectiveCamera>(_cameraNode->camera());
    const auto viewportSize = _window->viewportLogicalSize();
    const auto result = _cameraController.update(input, camera->yFov(), static_cast<float>(viewportSize.y));

    //_window->cursorHidden(result.pointerDragging);
    _window->cursorCaptured(result.pointerDragging);
}

void App::sceneWillStep(Runner& runner, Scene& scene, const Scene::StepInfo& info) {

    if (_pendingHiccup > 0ms) {
        const auto duration = std::exchange(_pendingHiccup, 0ms);

        std::this_thread::sleep_for(duration);
    }

    auto& input = static_cast<DesktopInputContext&>(*scene.inputContext());

    // if (_bananaNode) {
    //     // rotate the banana at 30 degrees per second
    //     const float rotation = static_cast<float>(info.deltaTime) * radians(-30.0f);
    //     const auto rotationY = math::quaternion({0.0f, 1.0f, 0.0f}, rotation);
    //     _bananaNode->orientation(rotationY * _bananaNode->orientation());
    // }

    using MouseButton = DesktopInputContext::MouseButton;

    // if (input.mouseButtonPressed(MouseButton::One)) {
    //
    //     const auto mouse = input.mousePosition();
    //     const auto from = scene.visualWorld()->unprojectPoint({mouse.x, mouse.y, 0.0f});
    //     const auto to = scene.visualWorld()->unprojectPoint({mouse.x, mouse.y, 1.0f});
    //
    //     const auto hits = scene.physicsWorld()->rayTest(from, to);
    //
    //     if (!hits.empty()) {
    //
    //         const auto& hit = hits.front();
    //
    //         if (auto node = hit.node()) {
    //             if (auto body = node->physicsBody(); body && body->type() == PhysicsBody::Type::Dynamic) {
    //
    //                 const vec3  direction = normalize(to - from);
    //                 const float impulse = 5.0f;
    //
    //                 body->applyForce(direction * impulse, hit.worldCoordinates(), true);
    //             }
    //         }
    //     }
    // }

    const auto visualWorld = scene.visualWorld();
    if (auto pov = visualWorld->pointOfView().lock(); input.mouseButtonPressed(MouseButton::One)) {
        ShootSlurm(scene, pov->worldPosition(), pov->worldForward());
    }
}

void App::frameDidBegin(Runner&                        runner,
                        Scene&                         scene,
                        VisualWorld&                   visualWorld,
                        const VisualWorld::RenderInfo& info) {

    const float  delta = static_cast<float>(info.updateDeltaTime);
    static float angle = radians(180.0);
    if (!runner.simulationPaused()) {
        angle += delta * BACKGROUND_ROTATION_SPEED;
    }
    if (auto& background = visualWorld.background()) {
        background->orientation(quaternion(BACKGROUND_ROTATION_AXIS, angle));
    }

    if (_cameraNode) {
        _cameraController.apply(*_cameraNode);
    }

    ui::Panel panel("controls", {
                                    .width = 180.0f,
                                    .margin = 12.0f,
                                });

    panel.section("simulation");

    const bool paused = runner.simulationPaused();

    panel.value("state", paused ? "paused" : "running");
    panel.value("time scale", std::format("{:.2f}x", runner.timeScale()));

    panel.row(paused ? 3 : 2);

    if (panel.button(paused ? "Resume" : "Pause")) {
        if (paused) {
            runner.resumeSimulation();
        }
        else {
            runner.pauseSimulation();
        }
    }

    if (paused) {
        if (panel.button("Step")) {
            runner.requestSimulationStep();
        }
    }

    if (paused) {
        if (panel.button("Reset")) {
            // resetSimulation();
        }
    }

    if (!paused) {
        if (panel.button("Hiccup")) {
            _pendingHiccup = std::chrono::milliseconds {uniform_linear(50, 250)};
        }
    }

    panel.spacer(12.0f);

    panel.section("environment");

    if (auto physicsWorld = scene.physicsWorld()) {
        const auto gravity = physicsWorld->gravity();

        panel.value("gravity", std::format("{:.2f}, {:.2f}, {:.2f}", gravity.x, gravity.y, gravity.z));

        const auto isGravity = [&gravity](const vec3& value) {
            return length(gravity - value) < 0.001f;
        };

        panel.row(3);

        if (panel.option("Earth", isGravity(GRAVITY_EARTH))) {
            physicsWorld->gravity(GRAVITY_EARTH);
        }

        if (panel.option("Moon", isGravity(GRAVITY_MOON))) {
            physicsWorld->gravity(GRAVITY_MOON);
        }

        if (panel.option("Zero", isGravity(GRAVITY_ZERO))) {
            physicsWorld->gravity(GRAVITY_ZERO);
        }
    }

    panel.spacer(12.0f);

    panel.section("debug");

    using DebugOptions = Scene::DebugOptions;

    auto debugOptions = scene.debugOptions();

    bool stats = util::bitmask::contains(debugOptions, DebugOptions::ShowStatsOverlay);
    if (panel.toggle("stats", stats)) {
        debugOptions = stats ? util::bitmask::add(debugOptions, DebugOptions::ShowStatsOverlay)
                             : util::bitmask::remove(debugOptions, DebugOptions::ShowStatsOverlay);
        scene.debugOptions(debugOptions);
    }

    bool defaultLighting = visualWorld.usesDefaultLighting();
    if (panel.toggle("default lighting", defaultLighting)) {
        visualWorld.usesDefaultLighting(defaultLighting);
    }

    bool meshBounds = util::bitmask::contains(debugOptions, DebugOptions::ShowBoundingBoxes);
    if (panel.toggle("mesh bounds", meshBounds)) {
        scene.debugOptions(meshBounds ? util::bitmask::add(debugOptions, DebugOptions::ShowBoundingBoxes)
                                      : util::bitmask::remove(debugOptions, DebugOptions::ShowBoundingBoxes));
    }

    bool physBounds = util::bitmask::contains(debugOptions, DebugOptions::ShowPhysicsBoundingBoxes);
    if (panel.toggle("physics bounds", physBounds)) {
        scene.debugOptions(physBounds
                               ? util::bitmask::add(debugOptions, DebugOptions::ShowPhysicsBoundingBoxes)
                               : util::bitmask::remove(debugOptions, DebugOptions::ShowPhysicsBoundingBoxes));
    }

    bool physWireframes = util::bitmask::contains(debugOptions, DebugOptions::ShowPhysicsWireframes);
    if (panel.toggle("physics wireframes", physWireframes)) {
        scene.debugOptions(physWireframes
                               ? util::bitmask::add(debugOptions, DebugOptions::ShowPhysicsWireframes)
                               : util::bitmask::remove(debugOptions, DebugOptions::ShowPhysicsWireframes));
    }
}

/// Private Static Non-Member Functions ///

void ShootSlurm(Scene& scene, const vec3& location, const vec3& direction) {

    static auto mesh = util::fs::MeshNamed("slurm/slurm");
    // mesh->materials()[0]->emission(mesh->materials()[0]->diffuse());
    // mesh->materials()[1]->emission(mesh->materials()[1]->diffuse());

    auto node = Node::MeshNode(mesh);
    node->name("Slurm");

    node->position(location);

    static auto extent = node->mesh()->localExtent();
    static auto physicsShape = make_shared<CylinderPhysicsShape>(extent.x / 2.0, extent.y);
    auto        physicsBody = make_unique<PhysicsBody>(PhysicsBody::Type::Dynamic, physicsShape);
    physicsBody->mass(.354); // 12fl oz water @ 70F
    physicsBody->restitution(1.0);
    physicsBody->friction(0.35);
    physicsBody->rollingFriction(0.05);

    const float radius = extent.x * 0.5f;
    physicsBody->ccdMotionThreshold(radius * 0.25f);
    physicsBody->ccdSweptSphereRadius(radius * 0.8f);
    physicsBody->ccdEnabled(true);

    // static auto light = Light::Point();
    // light->attenuation(Attenuation {.quadratic = 0.04f});
    // node->light(light);

    // add random factor

    node->eulerAngles({uniform_linear(0.0f, two_pi()), uniform_linear(0.0f, two_pi()),
                       uniform_linear(0.0f, two_pi())});

    static const float ANGULAR_VARIANCE = radians(260.0); // deg/sec
    physicsBody->angularVelocity({uniform_linear(-ANGULAR_VARIANCE, ANGULAR_VARIANCE),
                                  uniform_linear(-ANGULAR_VARIANCE, ANGULAR_VARIANCE),
                                  uniform_linear(-ANGULAR_VARIANCE, ANGULAR_VARIANCE)});

    const float VELOCITY = uniform_linear(40.0f, 60.0f);
    // const float        VELOCITY = uniform_linear(20.0f, 40.0f);
    static const float DIRECTION_VARIATION = 0.01;
    const vec3         variedDirection = normalize(normalize(direction) + uniform_ball(DIRECTION_VARIATION));
    physicsBody->linearVelocity(variedDirection * VELOCITY);

    node->physicsBody(std::move(physicsBody));

    scene.rootNode()->addChild(node);
}
