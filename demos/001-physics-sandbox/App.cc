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

const Log::Level                  APP_LOG_LEVEL {Log::Level::Debug};
const uvec2                       WINDOW_SIZE {1280, 768};
const bool                        FULLSCREEN {false};
const bool                        ENABLE_HIGH_DPI {true};
const RenderContext::Antialiasing ANTIALIASING {RenderContext::Antialiasing::Msaa4X};
const bool                        ENABLE_VSYNC {false};
const bool                        CAPTURE_CURSOR {false};
const float                       TIME_STEP {1.0 / 120.0};
const std::uint32_t               MAX_CATCH_UP_STEPS {4};
const float                       BACKGROUND_ROTATION_SPEED {radians(0.5f)};
const vec3                        BACKGROUND_ROTATION_AXIS {0.5f, 1.0f, 1.0f};
const vec3                        GRAVITY_EARTH {0.0f, -9.807f, 0.0f};
const vec3                        GRAVITY_MOON {0.0f, -1.62f, 0.0f};
const vec3                        GRAVITY_ZERO {0.0f, 0.0f, 0.0f};

/// Private Static Non-Member Prototypes ///

static shared_ptr<Node>          MakeSimulationRoot();
static optional<App::PickResult> Pick(Scene& scene, const vec2& screenPosition);
static void                      ShootSlurm(Node& parent, const vec3& location, const vec3& direction);
static vector<shared_ptr<Node>>  AddBoxStack(Node&             parent,
                                             const vec3&       location,
                                             const vec3&       boxSize,
                                             const u8vec3&     stackSize,
                                             float             padding,
                                             shared_ptr<Color> color);
static string                    FormatVec3(const vec3& value);
static string                    FormatRotation(const vec3& eulerAngles);
static string_view               BodyTypeName(PhysicsBody::Type type);
static string_view               ShapeTypeName(PhysicsShape::Type type);

/// Public Lifecycle Functions ///

App::App(int argc, char* argv[]):
    Application(argc, argv, APP_LOG_LEVEL),
    _window {nullptr},
    _cameraController {},
    _cameraNode {nullptr},
    _simulationRoot {nullptr},
    _selection {},
    _transients {ext::TransientNodeRegistry::SweepPolicy::EveryInterval(1.0)},
    _backgroundRotationTime {0.0},
    _resetRequested {false} {}

App::~App() = default;

/// Application Protected Member Functions ///

std::unique_ptr<Scene> App::init() {
    try {
        // create the window / render context

        _window = make_unique<Window>(RenderContext::RenderingApi::OpenGL, *util::fs::ExecutableName(),
                                      WINDOW_SIZE, FULLSCREEN, ENABLE_HIGH_DPI, ANTIALIASING);
        _window->vSyncEnabled(ENABLE_VSYNC);
        _window->cursorCaptured(CAPTURE_CURSOR);

        // create and configure the visual world

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
                    .reliefStrength = -0.125f,
                },
            .majorGrid =
                InfiniteGround::Grid {
                    .color = make_shared<Color>(vec4 {0.75f, 0.75f, 0.75f, 0.25f}),
                    .spacing = 10.0f,
                    .lineWidthPixels = 1.0f,
                    .reliefStrength = -0.125f,
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
            .specularIntensity = 0.05f,
            .specularExponent = 8.0f,
        });

        visualWorld->fog(Fog {.color = Color::Black(), .startDistance = 30.0f, .endDistance = 150.0f});

        // create the physics world

        auto physicsWorld = make_unique<PhysicsWorld>();

        auto scene =
            make_unique<Scene>(std::move(visualWorld), std::move(physicsWorld), Window::InputContext());
        scene->debugOptions(Scene::DebugOptions::ShowStatsOverlay);

        // create and configure the ground

        auto groundNode = Node::NamedNode("Ground");
        groundNode->orientation(math::quaternion({1.0f, 0.0f, 0.0f}, radians(-90.0f)));
        auto groundShape = make_shared<InfinitePlanePhysicsShape>();
        auto groundBody = make_unique<PhysicsBody>(PhysicsBody::Type::Static, groundShape);
        groundNode->physicsBody(std::move(groundBody));
        scene->rootNode()->addChild(groundNode);

        // setup lighting

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

        // setup transiet node groups

        // _transients.policy({.maxCount = {}, .maxAge = {}, .distanceLimit = {}});

        // _transients.groupPolicy("box",
        //                         {.maxCount = 100,
        //                          .distanceLimit =
        //                              ext::TransientNodeRegistry::DistanceLimit {.center = {0.0f, 0.0f, 0.0f},
        //                                                                         .radius = 50.0f}});

        _transients.groupPolicy("box",
                                {.maxCount = {100},
                                 .distanceLimit =
                                     ext::TransientNodeRegistry::DistanceLimit {.center = {0.0f, 0.0f, 0.0f},
                                                                                .radius = 25.0f}});

        // create and configure the camer and camera controller

        auto camera = make_shared<PerspectiveCamera>(0.1f, 1000.0f, radians(45.0f));
        _cameraNode = Node::CameraNode(camera);
        _cameraNode->name("Turntable camera");
        scene->rootNode()->addChild(_cameraNode);
        scene->visualWorld()->pointOfView(_cameraNode);

        auto cameraConfig = _cameraController.config();
        cameraConfig.controls.primaryButton = DesktopInputContext::MouseButton::Two;
        //cameraConfig.minPitch = math::radians(0.0f);
        cameraConfig.invertPitch = true;
        _cameraController.config(cameraConfig);

        _cameraController.view({
            .target = vec3 {0.0f, 1.0f, 0.0f},
            .yaw = radians(35.0f),
            .pitch = radians(20.0f),
            .distance = 20.0f,
        });

        // create the resettable simulation root node

        _simulationRoot = MakeSimulationRoot();
        scene->rootNode()->addChild(_simulationRoot);

        // open the window

        _window->center();
        _window->open();

        // ! TEMPORARY !
        log::app::i()("Wireframe rendering supported: {}",
                      scene->visualWorld()->capabilities().wireframeRendering);

        return scene;
    }
    catch (std::exception& e) {
        log::app::f()("Exception: {}", e.what());
        return nullptr;
    }
}

SimulationConfig App::simulationConfig() const {
    return {
        .timeStep = TIME_STEP,
        .maxCatchUpSteps = MAX_CATCH_UP_STEPS,
    };
}

bool App::shouldContinue(const Scene& scene) {
    return _window->isOpen();
}

void App::hostUpdate(Runner& runner, Scene&, const Runner::UpdateInfo&) {

    if (_resetRequested) {
        _resetRequested = false;
        resetSimulation();
        if (runner.simulationPaused()) {
            runner.simulationPaused(false);
        }
    }
}

void App::inputDidUpdate(Runner&       runner,
                         Scene&        scene,
                         InputContext& inputContext,
                         const InputContext::UpdateInfo&) {

    auto& input = static_cast<DesktopInputContext&>(inputContext);

    using Key = DesktopInputContext::Key;
    using MouseButton = DesktopInputContext::MouseButton;

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

    if (input.mouseButtonPressed(MouseButton::One)) {
        if (!result.pointerDragging) {
            select(Pick(scene, input.mousePosition()));
            // if (_selection) {
            //     if (auto node = _selection->node.lock()) {
            //         auto name = node->name();
            //         if (name) {
            //             log::app::i()("Selection: {}", *name);
            //         }
            //         else {
            //             log::app::i()("Selection: {:P}", static_cast<void*>(node.get()));
            //         }
            //     }
            // }
        }
    }
}

void App::sceneWillStep(Runner& runner, Scene& scene, const Scene::StepInfo& info) {

    auto& input = static_cast<DesktopInputContext&>(*scene.inputContext());

    // if (_bananaNode) {
    //     // rotate the banana at 30 degrees per second
    //     const float rotation = static_cast<float>(info.deltaTime) * radians(-30.0f);
    //     const auto rotationY = math::quaternion({0.0f, 1.0f, 0.0f}, rotation);
    //     _bananaNode->orientation(rotationY * _bananaNode->orientation());
    // }

    using Key = DesktopInputContext::Key;
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

    // shoot slurm

    // const auto visualWorld = scene.visualWorld();
    // if (auto pov = visualWorld->pointOfView().lock(); input.mouseButtonPressed(MouseButton::One)) {
    //     ShootSlurm(scene, pov->worldPosition(), pov->worldForward());
    // }

    // drop boxes

    if (input.keyPressed(Key::GraveAccent)) {
        _transients.track(AddBoxStack(*_simulationRoot, {0.0f, 10.0f, 0.0f}, {0.25f, 0.25f, 0.25f}, {3, 3, 3},
                                      0.025f, Color::White()),
                          "box");
    }
}

void App::sceneDidStep(Runner& runner, Scene& scene, const Scene::StepInfo& info) {

    _transients.update(info);
}

void App::frameDidBegin(Runner&                        runner,
                        Scene&                         scene,
                        VisualWorld&                   visualWorld,
                        const VisualWorld::RenderInfo& info) {

    const float frameDelta = static_cast<float>(info.updateDeltaTime);

    // const float angle = radians(180.0f) + static_cast<float>(info.simulationTime) * BACKGROUND_ROTATION_SPEED;
    // if (auto& background = visualWorld.background()) {
    //     background->orientation(quaternion(BACKGROUND_ROTATION_AXIS, angle));
    // }

    if (!runner.simulationPaused()) {
        _backgroundRotationTime += info.updateDeltaTime * runner.timeScale();
    }

    const float angle =
        radians(180.0f) + static_cast<float>(_backgroundRotationTime) * BACKGROUND_ROTATION_SPEED;

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
    // panel.value("time scale", std::format("{:.1f}x", runner.timeScale()));

    // float timeStep = runner.timeStep();
    // if (panel.slider("time step", timeStep, 0.1f, 2.0f, "%.2fx")) {
    //     runner.timeScale(timeStep);
    // }

    int stepRate = static_cast<int>(std::lround(1.0 / runner.timeStep()));
    if (panel.slider("time step", stepRate, 30, 512, "1/%ds")) {
        runner.timeStep(1.0 / stepRate);
    }

    int maxCatchUpSteps = runner.maxCatchUpSteps();
    if (panel.slider("catch-up steps", maxCatchUpSteps, 1, 16, "%d")) {
        runner.maxCatchUpSteps(maxCatchUpSteps);
    }

    // panel.row(2);

    float timeScale = runner.timeScale();
    if (panel.slider("time scale", timeScale, 0.1f, 2.0f, "%.2fx")) {
        runner.timeScale(timeScale);
    }

    // if (panel.button("1x")) {
    //     runner.timeScale(1.0f);
    // }

    panel.row(paused ? 2 : 1);

    if (panel.button(paused ? "Resume" : "Pause")) {
        runner.simulationPaused(!paused);
    }
    if (paused) {
        if (panel.button("Step")) {
            runner.requestSimulationStep();
        }
    }

    if (panel.button("Reset")) {
        _resetRequested = true;
    }

    panel.spacer(12.0f);

    panel.section("environment");

    if (auto physicsWorld = scene.physicsWorld()) {
        const auto gravity = physicsWorld->gravity();

        //panel.value("gravity", std::format("{:.1f}, {:.1f}, {:.1f}", gravity.x, gravity.y, gravity.z));
        panel.text("gravity");

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

    panel.section("selected node");

    if (!_selection) {
        panel.text("click an object to inspect");
    }
    else if (auto node = _selection->node.lock()) {

        // node

        panel.value("name", node->name().value_or("(unnamed)"));
        // panel.value("position", FormatVec3(node->worldPosition()));
        // panel.value("rotation", FormatRotation(node->worldEulerAngles()));

        // physics body

        if (auto body = node->physicsBody()) {

            panel.spacer(8.0f);

            panel.value("body", BodyTypeName(body->type()));

            if (const auto& shape = body->shape()) {
                panel.value("shape", ShapeTypeName(shape->type()));
            }
            else {
                panel.value("shape", "none");
            }

            panel.value("mass", std::format("{:.1f}", body->mass()));

            panel.spacer(6.0f);

            panel.value("velocity", FormatVec3(body->linearVelocity()));
            panel.value("angular", FormatVec3(body->angularVelocity()));

            // panel.spacer(6.0f);
            //
            // panel.value("inertia", FormatVec3(body->momentOfInertia()));
            // panel.value("COM", FormatVec3(body->centerOfMass()));
            //
            // panel.value("linear damping", std::format("{:.3f}", body->linearDamping()));
            // panel.value("angular damping", std::format("{:.3f}", body->angularDamping()));

            panel.value("friction", std::format("{:.1f}", body->friction()));
            panel.value("restitution", std::format("{:.1f}", body->restitution()));

            panel.value("resting", body->resting() ? "yes" : "no");

            // panel.spacer(6.0f);
            //
            // panel.value("force", FormatVec3(body->totalForce()));
            // panel.value("torque", FormatVec3(body->totalTorque()));
        }
        else {
            panel.spacer(8.0f);
            panel.value("body", "none");
        }

        // mesh

        if (const auto& mesh = node->mesh()) {

            panel.spacer(8.0f);

            panel.value("mesh", mesh->name().value_or("(unnamed)"));
            uint64_t polygons = 0;
            for (const auto& e : mesh->elements()) {
                polygons += e->indexCount() / 3u;
            }
            panel.value("polygons", std::format("{:.1f}k", float(polygons) / 1000.0f));
            // panel.value("elements", std::format("{}", mesh->elements().size()));
            panel.value("materials", std::format("{}", mesh->materials().size()));
        }
    }
    else {
        // The selected node was removed from the scene.
        select({});
        panel.text("click an object to inspect");
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

    bool defaultLighting = visualWorld.defaultLightingEnabled();
    if (panel.toggle("default lighting", defaultLighting)) {
        visualWorld.defaultLightingEnabled(defaultLighting);
    }

    bool meshBounds = util::bitmask::contains(debugOptions, DebugOptions::ShowBoundingBoxes);
    if (panel.toggle("mesh bounds", meshBounds)) {
        scene.debugOptions(meshBounds ? util::bitmask::add(debugOptions, DebugOptions::ShowBoundingBoxes)
                                      : util::bitmask::remove(debugOptions, DebugOptions::ShowBoundingBoxes));
    }

    if (visualWorld.capabilities().wireframeRendering) {
        bool meshWireframes = util::bitmask::contains(debugOptions, DebugOptions::ShowWireframes);
        if (panel.toggle("mesh wireframes", meshWireframes)) {
            scene.debugOptions(meshWireframes
                                   ? util::bitmask::add(debugOptions, DebugOptions::ShowWireframes)
                                   : util::bitmask::remove(debugOptions, DebugOptions::ShowWireframes));
        }
    }
    else {
        panel.value("mesh wireframes", "n/a", {0.0f, 1.0f, 0.0f, 0.0f});
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

/// Private Member Functions ///

void App::select(optional<PickResult> selection) {

    using DebugOptions = Node::DebugOptions;

    if (_selection) {
        if (auto node = _selection->node.lock()) {
            node->debugOptions(util::bitmask::remove(node->debugOptions(), DebugOptions::ShowHighlightBox));
            node->debugOptions(util::bitmask::remove(node->debugOptions(), DebugOptions::ShowHighlightTint));
        }
    }

    _selection = std::move(selection);

    if (_selection) {
        if (auto node = _selection->node.lock()) {
            node->debugOptions(util::bitmask::add(node->debugOptions(), DebugOptions::ShowHighlightBox));
            node->debugOptions(util::bitmask::add(node->debugOptions(), DebugOptions::ShowHighlightTint));
        }
        else {
            _selection.reset();
        }
    }
}

void App::resetSimulation() {
    select({});

    _simulationRoot->removeFromParent();

    _transients.clear();

    scene().physicsWorld()->gravity(GRAVITY_EARTH);

    _simulationRoot = MakeSimulationRoot();
    scene().rootNode()->addChild(_simulationRoot);

    runner().resetSimulation();
}

/// Private Static Non-Member Functions ///

shared_ptr<Node> MakeSimulationRoot() {
    auto root = Node::NamedNode("Simulation root");

    // janus

    // auto janusNode = Node::MeshNode(util::fs::MeshNamed("janus/janus"));
    // root->addChild(janusNode);

    static auto janusMesh = [] {
        constexpr float JANUS_HEIGHT = 1.0f;
        auto            mesh = util::fs::MeshNamed("janus_lod/janus_lod");
        const float     scaleFactor = JANUS_HEIGHT / mesh->localExtent().y;
        mesh->burnTransform(math::scale(mat4(1.0f), vec3(scaleFactor)), true);
        return mesh;
    }();

    auto janusNode = Node::MeshNode(janusMesh);
    janusNode->name("Janus");
    janusNode->position({-1.5f, 0.0f, 0.0f});
    root->addChild(janusNode);

    // {
    //     constexpr float JANUS_HEIGHT = 1.0f;
    //     auto            janusMesh = util::fs::MeshNamed("janus_lod/janus_lod");
    //     const float     scaleFactor = JANUS_HEIGHT / janusMesh->localExtent().y;
    //     janusMesh->burnTransform(math::scale(mat4(1.0f), vec3(scaleFactor)), true);
    //     auto janusNode = Node::MeshNode(janusMesh);
    //     janusNode->position({-1.5f, 0.0f, 0.0f});
    //     root->addChild(janusNode);
    // }

    // angel

    // auto anielNode = Node::MeshNode(util::fs::MeshNamed("aniel/aniel"));
    // root->addChild(anielNode);
    // auto anielExtent = anielNode->extent();

    // constexpr float ANGEL_HEIGHT = 2.0f;
    // auto            angelNode = Node::MeshNode(util::fs::MeshNamed("aniel/aniel"));
    // angelNode->scale(angelNode->scale() * (ANGEL_HEIGHT / angelNode->extent(true).y));
    // angelNode->physicsBody(PhysicsBody::StaticBody());
    // root->addChild(angelNode);

    // {
    //     constexpr float ANGEL_HEIGHT = 2.0f;
    //     auto            angelMesh = util::fs::MeshNamed("aniel_lod/aniel_lod");
    //     const float     scaleFactor = ANGEL_HEIGHT / angelMesh->localExtent().y;
    //     angelMesh->burnTransform(math::scale(mat4(1.0f), vec3(scaleFactor)), true);
    //     auto angelNode = Node::MeshNode(angelMesh);
    //     angelNode->name("Angel");
    //     angelNode->rotation({0.0f, 1.0f, 0.0f}, radians(180.0f));
    //     angelNode->physicsBody(PhysicsBody::StaticBody());
    //     root->addChild(angelNode);
    // }

    static auto angelMesh = [] {
        constexpr float ANGEL_HEIGHT = 2.0f;
        auto            angelMesh = util::fs::MeshNamed("aniel_lod/aniel_lod");
        const float     scaleFactor = ANGEL_HEIGHT / angelMesh->localExtent().y;
        angelMesh->burnTransform(math::scale(mat4(1.0f), vec3(scaleFactor)), true);
        return angelMesh;
    }();

    static auto angelShape = make_shared<PhysicsShape>(PhysicsShape::Type::ConcavePolyhedron, angelMesh);
    auto        angelNode = Node::MeshNode(angelMesh);
    angelNode->name("Angel");
    angelNode->rotation({0.0f, 1.0f, 0.0f}, radians(180.0f));
    auto angelBody = make_unique<PhysicsBody>(PhysicsBody::Type::Static, angelShape);
    angelNode->physicsBody(std::move(angelBody));
    root->addChild(angelNode);

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
    //     root->addChild(topLightNode);
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
    //     root->addChild(bottomLightNode);
    // }

    // banana

    // _bananaNode = Node::MeshNode(util::fs::MeshNamed("banana_lod/banana_lod"));
    // auto rx = math::quaternion({1.0f, 0.0f, 0.0f}, radians(90.0f));
    // auto ry = math::quaternion({0.0f, 1.0f, 0.0f}, radians(90.0f));
    // _bananaNode->orientation(rx * ry);
    // root->addChild(_bananaNode);

    // teapot

    static auto teapotMesh = [] {
        constexpr float TEAPOT_HEIGHT = 0.35f;
        auto            teapotMesh = util::fs::MeshNamed("teapot/teapot");
        const float     teapotScale = TEAPOT_HEIGHT / teapotMesh->localExtent().y;
        teapotMesh->burnTransform(math::scale(mat4(1.0f), vec3(teapotScale)), true);
        teapotMesh->replaceMaterial(0, Material::DiffuseMaterial(Color::DarkGray()));
        return teapotMesh;
    }();

    static auto teapotShape = make_shared<PhysicsShape>(PhysicsShape::Type::ConvexHull, teapotMesh);

    auto teapotNode = Node::MeshNode(teapotMesh);
    teapotNode->name("Teapot");
    teapotNode->position({1.5f, 1.0f - teapotMesh->localAABB().min.y, 0.0f});
    auto teapotBody = make_unique<PhysicsBody>(PhysicsBody::Type::Dynamic, teapotShape);
    teapotBody->mass(1.5f);
    teapotBody->friction(0.6f);
    teapotBody->restitution(0.15f);
    teapotBody->linearDamping(0.03f);
    teapotBody->angularDamping(0.05f);
    teapotNode->physicsBody(std::move(teapotBody));
    root->addChild(teapotNode);

    return root;
}

optional<App::PickResult> Pick(Scene& scene, const vec2& screenPosition) {

    auto visualWorld = scene.visualWorld();
    auto physicsWorld = scene.physicsWorld();

    if (!visualWorld || !physicsWorld) {
        return {};
    }

    const auto isSelectable = [](const Node& node) {
        return node.mesh() != nullptr;
    };

    const auto from = visualWorld->unprojectPoint({screenPosition.x, screenPosition.y, 0.0f});
    const auto to = visualWorld->unprojectPoint({screenPosition.x, screenPosition.y, 1.0f});
    const auto hits = physicsWorld->rayTest(from, to);

    for (const auto& hit : hits) {

        auto node = hit.node();

        if (!node || !isSelectable(*node)) {
            continue;
        }

        return App::PickResult {
            .node = node,
            .worldHitPosition = hit.worldCoordinates(),
            .worldHitNormal = hit.worldNormal(),
        };
    }

    return {};
}

void ShootSlurm(Node& parent, const vec3& location, const vec3& direction) {

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

    node->eulerAngles({uniform_linear(0.0f, TWO_PI), uniform_linear(0.0f, TWO_PI),
                       uniform_linear(0.0f, TWO_PI)});

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

    parent.addChild(node);
}

vector<shared_ptr<Node>> AddBoxStack(Node&             parent,
                                     const vec3&       location,
                                     const vec3&       boxSize,
                                     const u8vec3&     stackSize,
                                     float             padding,
                                     shared_ptr<Color> color) {

    if (!isfinite(padding) || padding < 0.0f) {
        throw invalid_argument("Box stack padding must be finite and non-negative.");
    }

    const unsigned countX = stackSize.x;
    const unsigned countZ = stackSize.y;
    const unsigned countY = stackSize.z;

    if (countX == 0 || countZ == 0 || countY == 0) {
        return {};
    }

    vector<shared_ptr<Node>> added;
    added.reserve(countX * countZ * countY);

    auto physicsShape = make_shared<BoxPhysicsShape>(boxSize.x, boxSize.y, boxSize.z);

    shared_ptr<Mesh> sharedMesh;

    if (color) {
        sharedMesh = Box::Mesh(boxSize.x, boxSize.y, boxSize.z);
        auto material = make_shared<Material>(monostate {}, monostate {}, monostate {}, color);
        sharedMesh->addMaterial(material);
    }

    const float stepX = boxSize.x + padding;
    const float stepY = boxSize.y + padding;
    const float stepZ = boxSize.z + padding;

    const float totalLength = boxSize.x * static_cast<float>(countX) + padding * static_cast<float>(countX - 1);
    const float totalWidth = boxSize.z * static_cast<float>(countZ) + padding * static_cast<float>(countZ - 1);
    const float startX = location.x - totalLength * 0.5f + boxSize.x * 0.5f;
    const float startZ = location.z - totalWidth * 0.5f + boxSize.z * 0.5f;
    const float startY = location.y + boxSize.y * 0.5f;

    for (unsigned y = 0; y < countY; ++y) {
        for (unsigned z = 0; z < countZ; ++z) {
            for (unsigned x = 0; x < countX; ++x) {

                auto boxColor = color ? color : Color::Random();
                auto mesh = sharedMesh;

                if (!mesh) {
                    mesh = Box::Mesh(boxSize.x, boxSize.y, boxSize.z);

                    auto material = make_shared<Material>(monostate {}, monostate {}, monostate {}, boxColor);

                    mesh->addMaterial(material);
                }

                auto       node = Node::MeshNode(mesh);
                static int boxNum = 0;
                node->name(std::format("Box {}", ++boxNum));

                node->position({
                    startX + static_cast<float>(x) * stepX,
                    startY + static_cast<float>(y) * stepY,
                    startZ + static_cast<float>(z) * stepZ,
                });

                auto physicsBody = PhysicsBody::DynamicBody();
                physicsBody->mass(1.0f);
                physicsBody->restitution(0.1f);
                physicsBody->friction(0.25f);
                physicsBody->shape(physicsShape);

                const float angularVariance = radians(90.0f);
                physicsBody->angularVelocity({
                    uniform_linear(-angularVariance, angularVariance),
                    uniform_linear(-angularVariance, angularVariance),
                    uniform_linear(-angularVariance, angularVariance),
                });

                node->physicsBody(std::move(physicsBody));

                auto light = Light::Point(boxColor);
                // light->attenuation(Attenuation::FromRange(2.5f, 0.02f));
                light->attenuation(Attenuation::FromRange(3.0f, 0.02f));
                node->light(light);

                added.push_back(node);
                parent.addChild(node);
            }
        }
    }
    return added;
}

string FormatVec3(const vec3& value) {

    return std::format("{:.1f}, {:.1f}, {:.1f}", value.x, value.y, value.z);
}

string FormatRotation(const vec3& eulerAngles) {

    return std::format("{:.1f}, {:.1f}, {:.1f}", degrees(eulerAngles.x), degrees(eulerAngles.y),
                       degrees(eulerAngles.z));
}

string_view BodyTypeName(PhysicsBody::Type type) {

    switch (type) {
        case PhysicsBody::Type::Static:
            return "static";

        case PhysicsBody::Type::Dynamic:
            return "dynamic";

        case PhysicsBody::Type::Kinematic:
            return "kinematic";
    }

    return "unknown";
}

string_view ShapeTypeName(PhysicsShape::Type type) {

    switch (type) {
        case PhysicsShape::Type::Primitive:
            return "primitive";

        case PhysicsShape::Type::BoundingBox:
            return "bounding box";

        case PhysicsShape::Type::ConvexHull:
            return "convex hull";

        case PhysicsShape::Type::ConcavePolyhedron:
            return "concave polyhedron";
    }

    return "unknown";
}
