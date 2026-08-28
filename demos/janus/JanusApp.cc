//
//  JanusApp.cc
//  janus
//
//  Created by Morgan Davis on 8/4/26.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#include "JanusApp.h"

#include <algorithm>
#include <cmath>

#include "a3d/a3d.h"

using namespace a3d;
using namespace a3d::math;
using namespace demo::janus;
using namespace std;

// [Private Constants]

const log::Level                  APP_LOG_LEVEL {log::Level::Debug};
const uvec2                       WINDOW_SIZE {1280, 768};
const RenderContext::Antialiasing ANTIALIASING {RenderContext::Antialiasing::Msaa2X};
const float                       TIME_STEP {1.0 / 120.0};
const std::uint32_t               MAX_CATCH_UP_STEPS {8};

const vec3 GRAVITY_EARTH {0.0f, -9.807f, 0.0f};
const vec3 GRAVITY_MOON {0.0f, -1.62f, 0.0f};
const vec3 GRAVITY_ZERO {0.0f, 0.0f, 0.0f};

// [Private Static Non-Member Prototypes]

static shared_ptr<Node>               MakeSimulationRoot();
static optional<JanusApp::PickResult> Pick(VisualWorld&               visualWorld,
                                           const vec2&                screenPosition,
                                           const vector<const Node*>& ignoredNodes = {});
static optional<JanusApp::PickResult> FindActionTarget(Scene&                     scene,
                                                       const vec2&                screenPosition,
                                                       const vector<const Node*>& ignoredNodes = {});
static vector<shared_ptr<Node>>       DropRocks(Node&         parent,
                                                const vec3&   location,
                                                const vec3&   boxSize,
                                                const u8vec3& stackSize,
                                                float         padding);
static vector<shared_ptr<Node>>       DropCoins(Node&         parent,
                                                const vec3&   location,
                                                const vec3&   boxSize,
                                                const u8vec3& stackSize,
                                                float         padding);
static vector<shared_ptr<Node>>       DropBalls(Node&         parent,
                                                const vec3&   location,
                                                const vec3&   boxSize,
                                                const u8vec3& stackSize,
                                                float         padding);
static shared_ptr<Node>               ThrowHammer(Node& parent, const vec3& location, const vec3& velocity);
static shared_ptr<Node>               ThrowHula(Node& parent, const vec3& location, const vec3& velocity);
static shared_ptr<Node>               ThrowDuck(Node& parent, const vec3& location, const vec3& velocity);
static bool IsIgnored(const shared_ptr<Node>& node, const vector<const Node*>& ignoredNodes);

// [Public Lifecycle Functions]

JanusApp::JanusApp(int argc, char* argv[]):
    Application(argc, argv, APP_LOG_LEVEL),
    _window {nullptr},
    _simulationRoot {nullptr},
    _cameraNode {nullptr},
    _cameraController {},
    _hoveredNode {},
    _selection {},
    _cursorMarker {nullptr},
    _actionTarget {},
    _action {Action::Drop},
    _dropAction {DropAction::Rocks},
    _throwAction {ThrowAction::Hammer},
    _pokiness {Pokiness::Soft},
    _transients {ext::Transients::SweepPolicy::EveryInterval(1.0)},
    _backgroundRotationTime {0.0},
    _orbWanderers {},
    _pickIgnores {},
    _pendingReset {false} {}

JanusApp::~JanusApp() = default;

// [Application Protected Member Functions]

std::unique_ptr<Scene> JanusApp::init() {
    try {
        // create and configure the window
        _window = make_unique<Window>(WINDOW_SIZE, false, true, ANTIALIASING);

        auto visualWorld = make_unique<VisualWorld>(*_window);

        visualWorld->background(Background {
            make_shared<Texture>(std::move(util::fs::CubeImageAt("nebula.webp")))});

        visualWorld->surface(SphereSurface {.center = {0.0f, -5000.0f, 0.0f}, .radius = 5000.0f});

        //tiles_ceramic_white_diff.jpg

        // auto groundImage = util::fs::ImageAt("white_ceramic_tiles_diff.webp");
        // auto groundTexture = make_shared<Texture>(std::move(groundImage));
        // auto groundMaterial = make_shared<Material>(std::monostate {}, groundTexture, Color::Gray());
        // groundMaterial->specularExponent(16.0f);
        // groundMaterial->uvScale(5.0f);

        auto minor = Ground::Procedural::GridComponent {.color = {0.5f, 0.5f, 0.5f, 0.25f},
                                                        .spacing = 1.0f,
                                                        .lineWidthPixels = 1.0f,
                                                        .reliefStrength = -0.125f};

        auto major = Ground::Procedural::GridComponent {.color = {0.75f, 0.75f, 0.75f, 0.25f},
                                                        .spacing = 10.0f,
                                                        .lineWidthPixels = 1.0f,
                                                        .reliefStrength = -0.125f};

        auto grid = Ground::Procedural::Grid {.color = Color::DarkGray(), //.color = Color{0.15f},
                                              .minor = minor,
                                              .major = major,
                                              .specularIntensity = 0.05f,
                                              .specularExponent = 8.0f};

        auto radialFade = Ground::RadialFade {.color = {0.01f, 0.01f, 0.01f, 1.0f},
                                              .center = {0.0f, 0.0f},
                                              .startDistance = 10.0f,
                                              .endDistance = 100.0f};

        auto horizonHaze =
            Ground::HorizonHaze {.color = {0.2f, 0.2f, 0.2f, 0.3f}, .angularWidth = math::radians(4.0f)};

        visualWorld->ground(Ground {.fill = Ground::Procedural {.content = grid},
                                    .radialFade = radialFade,
                                    .horizonHaze = horizonHaze});

        auto atmosphericHaze = Atmosphere::Haze {.color = {0.10f, 0.11f, 0.12f, 0.3}, .density = .35};

        auto limbGlow = Atmosphere::LimbGlow {.color = {0.30f, 0.38f, 0.48f, 0.5f}, .intensity = 0.25f};

        visualWorld->atmosphere(Atmosphere {.scaleHeight = 1.00f,
                                            .haze = atmosphericHaze,
                                            .limbGlow = limbGlow});

        // visualWorld->fog(Fog {
        //     .color = {0.2f, 0.2f, 0.2f, 0.25f},
        //     .startDistance = 10.0f,
        //     .endDistance = 50.0f,
        //     .transitionExponent = 1.0f,
        // });

        // create the physics world

        auto physicsWorld = make_unique<PhysicsWorld>();

        // create the scene

        // auto scene =
        //     make_unique<Scene>(std::move(visualWorld), std::move(physicsWorld), Window::InputContext());
        // //scene->debugOptions(Scene::DebugOptions::ShowStatsOverlay | Scene::DebugOptions::ShowPhysicsContactPoints);
        // scene->debugOptions(Scene::DebugOptions::ShowStatsOverlay);

        auto options = Scene::ImportOptions::ImportMeshes | Scene::ImportOptions::ImportMaterials
                       | Scene::ImportOptions::ImportLights;
        auto scene = util::fs::SceneAt("janus/janus.gltf", options);
        scene->visualWorld(std::move(visualWorld));
        scene->physicsWorld(std::move(physicsWorld));
        scene->inputContext(Window::InputContext());
        scene->debugOptions(Scene::DebugOptions::ShowStatsOverlay);
        auto rootNode = scene->rootNode();
        if (auto evoraNode = rootNode->childNamed("evora")) {

            _pickIgnores.push_back({.node = evoraNode});

            auto evoraPhysScene = util::fs::SceneAt("janus/phys.gltf", Scene::ImportOptions::ImportMeshes);
            if (auto physNode = evoraPhysScene->rootNode()->childNamed("evora_phys")) {

                auto shape = PhysicsShape::ConcavePolyhedronShape(physNode->mesh());
                auto body = PhysicsBody::StaticBody(shape);
                evoraNode->physicsBody(std::move(body));

                physNode->removeFromParent();
                rootNode->addChild(physNode);

                physNode->hidden(true);
                _pickIgnores.push_back({.node = physNode});
            }
        }

        if (auto node = rootNode->childNamed("janus")) {
            node->physicsBody(PhysicsBody::DynamicBody());
            node->physicsBody()->mass(10.0f);
            node->physicsBody()->friction(0.6f);
            node->physicsBody()->restitution(0.15f);
            node->physicsBody()->linearDamping(0.03f);
            node->physicsBody()->angularDamping(0.05f);
        }

        if (auto node = rootNode->childNamed("teapot")) {
            node->physicsBody(PhysicsBody::DynamicBody());
            node->physicsBody()->mass(1.5f);
            node->physicsBody()->friction(0.6f);
            node->physicsBody()->restitution(0.15f);
            node->physicsBody()->linearDamping(0.03f);
            node->physicsBody()->angularDamping(0.05f);
        }

        if (auto node = rootNode->childNamed("plinth1")) {
            node->physicsBody(PhysicsBody::DynamicBody());
            node->physicsBody()->mass(17.5f);
            node->physicsBody()->friction(0.6f);
            node->physicsBody()->restitution(0.15f);
            node->physicsBody()->linearDamping(0.03f);
            node->physicsBody()->angularDamping(0.05f);
        }

        if (auto node = rootNode->childNamed("plinth2")) {
            node->physicsBody(PhysicsBody::DynamicBody());
            node->physicsBody()->mass(12.5f);
            node->physicsBody()->friction(0.6f);
            node->physicsBody()->restitution(0.15f);
            node->physicsBody()->linearDamping(0.03f);
            node->physicsBody()->angularDamping(0.05f);
        }

        // create and configure the ground

        auto groundNode = Node::NamedNode("Ground");
        groundNode->orientation(math::quaternion({1.0f, 0.0f, 0.0f}, radians(-90.0f)));
        auto groundShape = make_shared<InfinitePlanePhysicsShape>();
        auto groundBody = PhysicsBody::StaticBody(groundShape);
        groundBody->friction(1.0f);
        groundBody->restitution(0.1f);
        groundNode->physicsBody(std::move(groundBody));
        scene->rootNode()->addChild(groundNode);

        // setup lighting

        auto ambientLight = make_shared<AmbientLight>(Color {0.15f});
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

        // setup transient node groups

        _transients.groupPolicy("drop", {.maxCount = (3 * 3 * 3) * 3});
        _transients.groupPolicy("throw", {.maxCount = 10,
                                          .distanceLimit = ext::Transients::DistanceLimit {.radius = 100.0f}});

        // create and configure the camera and camera controller

        auto camera = make_shared<PerspectiveCamera>(0.1f, 1000.0f, radians(45.0f));
        _cameraNode = Node::CameraNode(camera);
        _cameraNode->name("Camera");
        scene->rootNode()->addChild(_cameraNode);
        scene->visualWorld()->pointOfView(_cameraNode);

        auto cameraConfig = _cameraController.config();
        cameraConfig.controls.orbitButton = DesktopInputContext::MouseButton::One;
        cameraConfig.controls.panButton = DesktopInputContext::MouseButton::Two;
        // cameraConfig.minPitch = math::radians(-2.5f);
        cameraConfig.invertPitch = true;
        cameraConfig.minDistance = 1.0f;
        cameraConfig.maxDistance = 100.0f;
        _cameraController.config(cameraConfig);

        _cameraController.view({.target = vec3 {0.0f, 1.0f, 0.0f},
                                .yaw = radians(-35.0f),
                                .pitch = radians(0.0f),
                                .distance = 20.0f});

        // create the resettable simulation root node

        _simulationRoot = MakeSimulationRoot();
        scene->rootNode()->addChild(_simulationRoot);

        // create the action target marker
        {
            const bool ENABLE_CURSOR_MARKER {false};
            if (ENABLE_CURSOR_MARKER) {

                const float CURSOR_MARKER_RADIUS {0.1f};
                auto        material = Material::EmissionMaterial(Color::Yellow());
                material->depthTestEnabled(false);
                material->depthWriteEnabled(false);

                auto mesh = Sphere::Mesh(CURSOR_MARKER_RADIUS, 8, material);

                _cursorMarker = Node::MeshNode(mesh);
                _cursorMarker->name("Marker");
                _cursorMarker->renderOrder(100);
                _cursorMarker->hidden(true);

                scene->rootNode()->addChild(_cursorMarker);
            }
        }

        // wandering lights

        {
            const vec3 ORB_GROUP_POSITION {-4.5f, 10.0f, -2.5f};

            const vec3 ORB_POSITION_MIN {-1.0f, -0.5f, -1.0f};
            const vec3 ORB_POSITION_MAX {1.0f, 0.5f, 1.0f};

            const vec3   ORB_WANDER_EXTENTS {1.5f, 0.75f, 1.5f};
            const size_t ORB_COUNT {4};

            auto orbGroup = Node::NamedNode("Orbs");
            orbGroup->position(ORB_GROUP_POSITION);
            scene->rootNode()->addChild(orbGroup);

            auto orbMaterial = Material::EmissionMaterial(Color::White());
            auto orbMesh = Sphere::Mesh(0.1, 3, orbMaterial);

            _orbWanderers.reserve(ORB_COUNT);

            for (size_t i = 0; i < ORB_COUNT; ++i) {
                auto light = make_shared<PointLight>(Color::White());
                // light->attenuation(Attenuation {
                //     .quadratic = 0.5f
                // });
                light->attenuation(Attenuation::FromRange(3.0f, 0.02f));

                auto orb = Node::LightNode(light);
                orb->name(std::format("Orb {}", i + 1));
                orb->mesh(orbMesh);

                orb->position(math::uniform_linear(ORB_POSITION_MIN, ORB_POSITION_MAX));

                orb->physicsBody(PhysicsBody::KinematicBody());

                orbGroup->addChild(orb);

                ext::Wanderer::Config config {.halfExtents = ORB_WANDER_EXTENTS,
                                              .segmentDuration = math::uniform_linear(5.0f, 7.0f),
                                              .seed = 1000u + static_cast<uint32_t>(i)};
                _orbWanderers.push_back(ext::Wanderer(orb, config));
            }
        }

        // open the window

        _window->center();
        _window->open();

        return scene;
    }
    catch (std::exception& e) {
        log::app::f()("Exception: {}", e.what());
        return nullptr;
    }
}

SimulationConfig JanusApp::simulationConfig() const {
    return {
        .timeStep = TIME_STEP,
        .maxCatchUpSteps = MAX_CATCH_UP_STEPS,
    };
}

bool JanusApp::shouldContinue(const Scene& scene) {
    return _window->isOpen();
}

void JanusApp::runnerUpdate(Runner& runner, Scene&, const Runner::UpdateInfo&) {

    if (_pendingReset) {
        _pendingReset = false;

        reset();
    }
}

void JanusApp::inputDidUpdate(Runner&       runner,
                              Scene&        scene,
                              InputContext& inputContext,
                              const InputContext::UpdateInfo&) {

    auto& input = static_cast<DesktopInputContext&>(inputContext);

    using Key = DesktopInputContext::Key;

    if (env::is(Platform::Desktop) && input.keyPressed(Key::Escape)) {
        _window->close();
    }

    const auto camera = static_pointer_cast<PerspectiveCamera>(_cameraNode->camera());
    const auto viewportSize = _window->viewportLogicalSize();
    const auto result = _cameraController.update(input, camera->yFov(), static_cast<float>(viewportSize.y));

    _window->cursorCaptured(result.pointerDragging);

    if (result.orbitButtonClick) {

        if (input.keyDown(Key::LeftControl)) {
            queueAction(result.orbitButtonClick->position);
        }
        else {
            select(Pick(*scene.visualWorld(), result.orbitButtonClick->position, {_cursorMarker.get()}));
        }
    }

    if (result.panButtonClick) {
        queueAction(result.panButtonClick->position);
    }
}

void JanusApp::sceneWillStep(Runner& runner, Scene& scene, const Scene::StepInfo& info) {

    for (auto& wander : _orbWanderers) {
        wander.update(info.deltaTime);
    }
}

void JanusApp::sceneDidStep(Runner& runner, Scene& scene, const Scene::StepInfo& info) {

    _transients.update(info);

    for (auto& ignore : _pickIgnores) {
        if (ignore.remainingTime) {
            *ignore.remainingTime -= info.deltaTime;
        }
    }

    std::erase_if(_pickIgnores, [](const PickIgnore& ignore) {
        return ignore.remainingTime && (ignore.remainingTime <= 0.0 || ignore.node.expired());
    });
}

void JanusApp::frameDidBegin(Runner&                        runner,
                             Scene&                         scene,
                             VisualWorld&                   visualWorld,
                             const VisualWorld::RenderInfo& info) {

    if (!runner.simulationPaused()) {
        _backgroundRotationTime += info.updateDeltaTime * runner.timeScale();
    }

    const float BACKGROUND_ROTATION_SPEED {radians(1.0 / 8.0f)};
    const vec3  BACKGROUND_ROTATION_AXIS {0.5f, 1.0f, 1.0f};

    const float angle =
        radians(180.0f) + static_cast<float>(_backgroundRotationTime) * BACKGROUND_ROTATION_SPEED;

    if (const auto& current = visualWorld.background()) {
        auto background = *current;
        background.orientation = quaternion(BACKGROUND_ROTATION_AXIS, angle);
        visualWorld.background(background);
    }

    if (_cameraNode) {
        _cameraController.apply(*_cameraNode);
    }

    bool hovered = drawPanel();

    if (_window->cursorCaptured() || hovered) {

        hover(nullptr);

        _actionTarget.reset();
        if (_cursorMarker) {
            _cursorMarker->hidden(true);
        }
    }
    else {

        auto& input = static_cast<DesktopInputContext&>(*scene.inputContext());

        vector<const Node*> ignoredNodes;
        ignoredNodes.reserve(_pickIgnores.size() + 1);
        if (_cursorMarker) {
            ignoredNodes.push_back(_cursorMarker.get());
        }
        for (const auto& entry : _pickIgnores) {
            if (auto node = entry.node.lock()) {
                ignoredNodes.push_back(node.get());
            }
        }
        _actionTarget = FindActionTarget(scene, input.mousePosition(), ignoredNodes);

        if (_actionTarget) {

            hover(_actionTarget->node.lock());

            if (_cursorMarker) {
                _cursorMarker->position(_actionTarget->hitPosition);
                _cursorMarker->hidden(false);
            }
        }
        else {

            hover(nullptr);

            if (_cursorMarker) {
                _cursorMarker->hidden(true);
            }
        }
    }

    //_window->cursorHidden(!_cursorMarker->hidden());
}

// [Private Member Functions]

bool JanusApp::drawPanel() {

    const float PANEL_WIDTH {180.0f};

    auto& runner = JanusApp::runner();
    auto& scene = JanusApp::scene();
    auto& visualWorld = *scene.visualWorld();
    auto& physicsWorld = *scene.physicsWorld();

    ui::Panel panel("controls", {.width = PANEL_WIDTH, .margin = 12.0f});

    panel.section("simulation", {.line = true}, {.top = 0.0f, .bottom = 4.0f});

    const bool paused = runner.simulationPaused();

    panel.value("state", paused ? "paused" : "running");

    int stepRate = static_cast<int>(math::round(1.0 / runner.timeStep()));
    if (panel.slider("time step", stepRate, 30, 480, "1/%ds")) {
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
        _pendingReset = true;
    }

    panel.section("environment");

    const auto gravity = physicsWorld.gravity();

    panel.text("gravity");

    const auto isGravity = [&gravity](const vec3& value) {
        return length(gravity - value) < 0.001f;
    };

    panel.row(3);

    if (panel.option("Earth", isGravity(GRAVITY_EARTH))) {
        physicsWorld.gravity(GRAVITY_EARTH);
    }

    if (panel.option("Moon", isGravity(GRAVITY_MOON))) {
        physicsWorld.gravity(GRAVITY_MOON);
    }

    if (panel.option("Zero", isGravity(GRAVITY_ZERO))) {
        physicsWorld.gravity(GRAVITY_ZERO);
    }

    //panel.spacer(12.0f);
    panel.section("action");

    panel.row(3);
    if (panel.option("Drop", _action == Action::Drop)) {
        _action = Action::Drop;
    }
    if (panel.option("Throw", _action == Action::Throw)) {
        _action = Action::Throw;
    }
    if (panel.option("Poke", _action == Action::Poke)) {
        _action = Action::Poke;
    }

    switch (_action) {
        case Action::Drop:
            panel.row(3);
            if (panel.subOption("Rocks", _dropAction == DropAction::Rocks)) {
                _dropAction = DropAction::Rocks;
            }
            if (panel.subOption("Coins", _dropAction == DropAction::Coins)) {
                _dropAction = DropAction::Coins;
            }
            if (panel.subOption("Balls", _dropAction == DropAction::Balls)) {
                _dropAction = DropAction::Balls;
            }
            break;
        case Action::Throw:
            panel.row(3);
            if (panel.subOption("Hammer", _throwAction == ThrowAction::Hammer)) {
                _throwAction = ThrowAction::Hammer;
            }
            if (panel.subOption("Hula", _throwAction == ThrowAction::Hula)) {
                _throwAction = ThrowAction::Hula;
            }
            if (panel.subOption("Duck", _throwAction == ThrowAction::Duck)) {
                _throwAction = ThrowAction::Duck;
            }
            break;
        case Action::Poke:
            panel.row(3);
            if (panel.subOption("Soft", _pokiness == Pokiness::Soft)) {
                _pokiness = Pokiness::Soft;
            }
            if (panel.subOption("Hard", _pokiness == Pokiness::Hard)) {
                _pokiness = Pokiness::Hard;
            }
            if (panel.subOption("Flip", _pokiness == Pokiness::Flip)) {
                _pokiness = Pokiness::Flip;
            }
            break;
    }

    //panel.spacer(12.0f);
    panel.section("selected node");

    if (!_selection) {
        panel.text("click to select");
    }
    else if (auto node = _selection->node.lock()) {

        // node

        panel.value("name", node->name().value_or("(unnamed)"));
        // panel.value("position", FormatVec3(node->worldPosition()));
        // panel.value("rotation", FormatRotation(node->worldEulerAngles()));

        // physics body

        if (auto body = node->physicsBody()) {

            static auto formatVec3 = [](const vec3& value) {
                return std::format("{:.1f}, {:.1f}, {:.1f}", value.x, value.y, value.z);
            };

            static auto bodyTypeName = [](PhysicsBody::Type type) -> string_view {
                switch (type) {
                    case PhysicsBody::Type::Static:
                        return "static";
                    case PhysicsBody::Type::Dynamic:
                        return "dynamic";
                    case PhysicsBody::Type::Kinematic:
                        return "kinematic";
                }

                return "unknown";
            };

            static auto shapeTypeName = [](PhysicsShape::Type type) -> string_view {
                switch (type) {
                    case PhysicsShape::Type::Primitive:
                        return "primitive";
                    case PhysicsShape::Type::BoundingBox:
                        return "bounding box";
                    case PhysicsShape::Type::ConvexHull:
                        return "convex";
                    case PhysicsShape::Type::ConcavePolyhedron:
                        return "concave";
                }

                return "unknown";
            };

            panel.spacer(6.0f);

            panel.value("body", bodyTypeName(body->type()));

            if (const auto& shape = body->shape()) {
                panel.value("shape", shapeTypeName(shape->type()));
            }
            else {
                panel.value("shape", "none");
            }

            panel.spacer(6.0f);

            panel.value("mass", std::format("{:.1f}", body->mass()));

            // panel.value("velocity", formatVec3(body->linearVelocity()));
            // panel.value("angular", formatVec3(body->angularVelocity()));

            // panel.spacer(6.0f);
            //
            // panel.value("inertia", FormatVec3(body->momentOfInertia()));
            // panel.value("COM", FormatVec3(body->centerOfMass()));
            //
            // panel.value("linear damping", std::format("{:.3f}", body->linearDamping()));
            // panel.value("angular damping", std::format("{:.3f}", body->angularDamping()));

            panel.value("friction", std::format("{:.1f}", body->friction()));
            panel.value("restitution", std::format("{:.1f}", body->restitution()));

            const auto contacts = physicsWorld.contactTest(*body);
            panel.value("contacts", std::format("{}", contacts.size()));

            panel.value("resting", body->resting() ? "yes" : "no");

            // panel.spacer(6.0f);
            //
            // panel.value("force", FormatVec3(body->totalForce()));
            // panel.value("torque", FormatVec3(body->totalTorque()));
        }
        else {
            panel.spacer(6.0f);
            panel.value("body", "none", {.top = 6.0f} /*{0.0f, 0.0f, 0.0f, 0.0f}*/);
        }

        // mesh

        if (const auto& mesh = node->mesh()) {

            panel.spacer(6.0f);

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
        // the selected node was removed from the scene
        select({});
        panel.text("click an object to inspect");
    }

    //panel.spacer(12.0f);
    panel.section("debug");

    using DebugOptions = Scene::DebugOptions;

    auto debugOptions = scene.debugOptions();

    // {
    //     bool stats = util::bitmask::contains(debugOptions, DebugOptions::ShowStatsOverlay);
    //     if (panel.toggle("stats", stats)) {
    //         debugOptions = stats ? util::bitmask::add(debugOptions, DebugOptions::ShowStatsOverlay)
    //                              : util::bitmask::remove(debugOptions, DebugOptions::ShowStatsOverlay);
    //         scene.debugOptions(debugOptions);
    //     }
    //
    //     bool defaultLighting = visualWorld.defaultLightingEnabled();
    //     if (panel.toggle("default lighting", defaultLighting)) {
    //         visualWorld.defaultLightingEnabled(defaultLighting);
    //     }
    //
    //     bool meshBounds = util::bitmask::contains(debugOptions, DebugOptions::ShowMeshBounds);
    //     if (panel.toggle("mesh bounds", meshBounds)) {
    //         scene.debugOptions(meshBounds ? util::bitmask::add(debugOptions, DebugOptions::ShowMeshBounds)
    //                                       : util::bitmask::remove(debugOptions, DebugOptions::ShowMeshBounds));
    //     }
    //
    //     bool meshFrames = util::bitmask::contains(debugOptions, DebugOptions::ShowMeshFrames);
    //     if (panel.toggle("mesh frames", meshFrames)) {
    //         scene.debugOptions(meshFrames ? util::bitmask::add(debugOptions, DebugOptions::ShowMeshFrames)
    //                                       : util::bitmask::remove(debugOptions, DebugOptions::ShowMeshFrames));
    //     }
    //
    //     if (visualWorld.capabilities().wireframeRendering) {
    //         bool meshWireframes = util::bitmask::contains(debugOptions, DebugOptions::ShowMeshWireframes);
    //         if (panel.toggle("mesh wireframes", meshWireframes)) {
    //             scene.debugOptions(meshWireframes
    //                                    ? util::bitmask::add(debugOptions, DebugOptions::ShowMeshWireframes)
    //                                    : util::bitmask::remove(debugOptions, DebugOptions::ShowMeshWireframes));
    //         }
    //     }
    //     else {
    //         panel.value("mesh wireframes", "n/a", {0.0f, 1.0f, 0.0f, 0.0f});
    //     }
    //
    //     bool physBounds = util::bitmask::contains(debugOptions, DebugOptions::ShowPhysicsBounds);
    //     if (panel.toggle("physics bounds", physBounds)) {
    //         scene.debugOptions(physBounds ? util::bitmask::add(debugOptions, DebugOptions::ShowPhysicsBounds)
    //                                       : util::bitmask::remove(debugOptions, DebugOptions::ShowPhysicsBounds));
    //     }
    //
    //     bool physFrames = util::bitmask::contains(debugOptions, DebugOptions::ShowPhysicsFrames);
    //     if (panel.toggle("physics frames", physFrames)) {
    //         scene.debugOptions(physFrames ? util::bitmask::add(debugOptions, DebugOptions::ShowPhysicsFrames)
    //                                       : util::bitmask::remove(debugOptions, DebugOptions::ShowPhysicsFrames));
    //     }
    //
    //     bool physWireframes = util::bitmask::contains(debugOptions, DebugOptions::ShowPhysicsWireframes);
    //     if (panel.toggle("physics wireframes", physWireframes)) {
    //         scene.debugOptions(physWireframes
    //                                ? util::bitmask::add(debugOptions, DebugOptions::ShowPhysicsWireframes)
    //                                : util::bitmask::remove(debugOptions, DebugOptions::ShowPhysicsWireframes));
    //     }
    // }

    // {
    //     struct DebugToggle {
    //         const char*  title;
    //         DebugOptions option;
    //         bool         available {true};
    //     };
    //
    //     const std::array debugToggles {
    //         DebugToggle {"stats", DebugOptions::ShowStatsOverlay},
    //         DebugToggle {"mesh bounds", DebugOptions::ShowMeshBounds},
    //         DebugToggle {"mesh frames", DebugOptions::ShowMeshFrames},
    //         DebugToggle {"mesh wireframes", DebugOptions::ShowMeshWireframes,
    //                      visualWorld.capabilities().wireframeRendering},
    //         DebugToggle {"physics bounds", DebugOptions::ShowPhysicsBounds},
    //         DebugToggle {"physics frames", DebugOptions::ShowPhysicsFrames},
    //         DebugToggle {"physics wireframes", DebugOptions::ShowPhysicsWireframes},
    //     };
    //
    //     bool debugOptionsChanged = false;
    //
    //     for (const auto& toggle : debugToggles) {
    //
    //         if (!toggle.available) {
    //             panel.value(toggle.title, "n/a", {0.0f, 1.0f, 0.0f, 0.0f});
    //             continue;
    //         }
    //
    //         bool enabled = util::bitmask::contains(debugOptions, toggle.option);
    //         if (panel.toggle(toggle.title, enabled)) {
    //             if (enabled) {
    //                 util::bitmask::add_inplace(debugOptions, toggle.option);
    //             }
    //             else {
    //                 util::bitmask::remove_inplace(debugOptions, toggle.option);
    //             }
    //             debugOptionsChanged = true;
    //         }
    //     }
    //
    //     if (debugOptionsChanged) {
    //         scene.debugOptions(debugOptions);
    //     }
    //
    //     bool defaultLighting = visualWorld.defaultLightingEnabled();
    //     if (panel.toggle("default lighting", defaultLighting)) {
    //         visualWorld.defaultLightingEnabled(defaultLighting);
    //     }
    // }

    {
        auto debugToggle = [&](const char* title, DebugOptions option) {
            bool enabled = util::bitmask::contains(debugOptions, option);
            if (panel.toggle(title, enabled)) {
                if (enabled) {
                    util::bitmask::add_inplace(debugOptions, option);
                }
                else {
                    util::bitmask::remove_inplace(debugOptions, option);
                }
                scene.debugOptions(debugOptions);
            }
        };

        debugToggle("stats", DebugOptions::ShowStatsOverlay);

        debugToggle("mesh bounds", DebugOptions::ShowMeshBounds);
        debugToggle("mesh frames", DebugOptions::ShowMeshFrames);

        if (visualWorld.capabilities().wireframeRendering) {
            debugToggle("mesh wireframes", DebugOptions::ShowMeshWireframes);
        }
        else {
            panel.value("mesh wireframes", "n/a", {0.0f, 1.0f, 0.0f, 0.0f});
        }

        debugToggle("physics bounds", DebugOptions::ShowPhysicsBounds);
        debugToggle("physics frames", DebugOptions::ShowPhysicsFrames);
        debugToggle("physics wireframes", DebugOptions::ShowPhysicsWireframes);

        bool defaultLighting = visualWorld.defaultLightingEnabled();
        if (panel.toggle("default lighting", defaultLighting)) {
            visualWorld.defaultLightingEnabled(defaultLighting);
        }
    }

    return panel.hovered();
}

void JanusApp::hover(shared_ptr<Node> node) {

    using DebugOptions = Node::DebugOptions;

    if (auto previous = _hoveredNode.lock()) {

        const bool selected = _selection && _selection->node.lock() == previous;
        if (!selected) {
            previous->debugOptions(util::bitmask::remove(previous->debugOptions(),
                                                         DebugOptions::ShowHighlightTint));
        }
    }

    _hoveredNode = node;

    if (node) {
        node->debugOptions(util::bitmask::add(node->debugOptions(), DebugOptions::ShowHighlightTint));
    }
}

void JanusApp::select(optional<PickResult> pickResult) {

    using DebugOptions = Node::DebugOptions;

    if (_selection) {
        if (auto node = _selection->node.lock()) {
            node->debugOptions(util::bitmask::remove(node->debugOptions(), DebugOptions::ShowHighlightBox));

            if (_hoveredNode.lock() != node) {
                node->debugOptions(util::bitmask::remove(node->debugOptions(),
                                                         DebugOptions::ShowHighlightTint));
            }
        }
    }

    _selection = std::move(pickResult);
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

void JanusApp::queueAction(const vec2& screenPosition) {

    auto& scene = JanusApp::scene();

    auto target = FindActionTarget(scene, screenPosition, {_cursorMarker.get()});

    _actionTarget = target;

    if (!target) {

        if (_cursorMarker) {
            _cursorMarker->hidden(true);
        }

        return;
    }

    if (_cursorMarker) {
        _cursorMarker->position(target->hitPosition);
        _cursorMarker->hidden(false);
    }

    auto visualWorld = scene.visualWorld();

    if (!visualWorld || !_cameraNode || !_simulationRoot) {
        return;
    }

    const vec3 from = visualWorld->unprojectPoint({screenPosition.x, screenPosition.y, 0.0f});
    const vec3 to = visualWorld->unprojectPoint({screenPosition.x, screenPosition.y, 1.0f});
    const vec3 ray = to - from;

    if (length(ray) <= F32_COMPARE_EPSILON) {
        return;
    }

    PendingAction pendingAction {.action = _action,
                                 .target = *target,
                                 .simulationRoot = _simulationRoot,
                                 .cameraPosition = _cameraNode->worldPosition(),
                                 .rayDirection = normalize(ray)};

    queueScenePreStepCommand([this, pendingAction = std::move(pendingAction)](Scene& scene) {
        performAction(pendingAction);
    });
}

void JanusApp::performAction(const PendingAction& action) {

    const float DROP_HEIGHT {5.0f};

    const vec3   DROP_BOX_SIZE {vec3 {1.0f} * 0.35f};
    const u8vec3 DROP_STACK_SIZE {3, 3, 3};
    const float  DROP_PADDING {0.065f};

    const float THROW_SPAWN_DISTANCE {0.5f};
    const float THROW_SPEED {15.0f};
    const float THROW_MIN_FLIGHT_TIME {0.25f};
    const float THROW_MAX_FLIGHT_TIME {1.5f};

    const float POKE_IMPULSE_SOFT = 2.5f;
    const float POKE_IMPULSE_HARD = 10.0f;

    const double PROJECTILE_PICK_IGNORE_DURATION {1.5};

    auto& scene = JanusApp::scene();

    auto simulationRoot = action.simulationRoot.lock();

    // simulation may have been reset while this action was waiting for a step boundary
    if (!simulationRoot || simulationRoot != _simulationRoot) {
        return;
    }

    switch (action.action) {

        case Action::Drop: {

            const vec3 spawnLocation = action.target.hitPosition + vec3 {0.0f, DROP_HEIGHT, 0.0f};

            switch (_dropAction) {
                case DropAction::Rocks: {
                    auto rocks =
                        DropRocks(*simulationRoot, spawnLocation, DROP_BOX_SIZE, DROP_STACK_SIZE, DROP_PADDING);
                    _transients.track(rocks, "drop");
                    break;
                }
                case DropAction::Coins: {
                    auto coins = DropCoins(*simulationRoot, spawnLocation, DROP_BOX_SIZE, DROP_STACK_SIZE,
                                           DROP_PADDING + .1);
                    _transients.track(coins, "drop");
                    break;
                }
                case DropAction::Balls: {
                    auto balls = DropBalls(*simulationRoot, spawnLocation, DROP_BOX_SIZE, DROP_STACK_SIZE,
                                           DROP_PADDING + .1);
                    _transients.track(balls, "drop");
                    break;
                }
            }

            break;
        }

        case Action::Throw: {

            auto physicsWorld = scene.physicsWorld();

            const vec3  cameraPosition = action.cameraPosition;
            const vec3  targetPosition = action.target.hitPosition;
            const vec3  cameraToTarget = targetPosition - cameraPosition;
            const float targetDistance = length(cameraToTarget);

            const vec3 aimDirection = cameraToTarget / targetDistance;
            const vec3 spawnPosition =
                cameraPosition + aimDirection * math::min(THROW_SPAWN_DISTANCE, targetDistance * 0.25f);
            const vec3  displacement = targetPosition - spawnPosition;
            const float flightTime =
                math::clamp(length(displacement) / THROW_SPEED, THROW_MIN_FLIGHT_TIME, THROW_MAX_FLIGHT_TIME);
            const vec3 gravity = physicsWorld->gravity();
            const vec3 velocity = displacement / flightTime - 0.5f * gravity * flightTime;

            switch (_throwAction) {
                case ThrowAction::Hammer: {
                    auto projectile = ThrowHammer(*simulationRoot, spawnPosition, velocity);
                    _pickIgnores.push_back({.node = projectile,
                                            .remainingTime = PROJECTILE_PICK_IGNORE_DURATION});
                    _transients.track(projectile, "throw");
                    break;
                }
                case ThrowAction::Hula: {
                    auto projectile = ThrowHula(*simulationRoot, spawnPosition, velocity);
                    _pickIgnores.push_back({.node = projectile,
                                            .remainingTime = PROJECTILE_PICK_IGNORE_DURATION});
                    _transients.track(projectile, "throw");
                    break;
                }
                case ThrowAction::Duck: {
                    auto projectile = ThrowDuck(*simulationRoot, spawnPosition, velocity);
                    _pickIgnores.push_back({.node = projectile,
                                            .remainingTime = PROJECTILE_PICK_IGNORE_DURATION});
                    _transients.track(projectile, "throw");
                    break;
                }
            }

            break;
        }

        case Action::Poke: {

            auto node = action.target.node.lock();
            auto body = node->physicsBody();

            if (!body || body->type() != PhysicsBody::Type::Dynamic) {
                return;
            }

            switch (_pokiness) {
                case Pokiness::Soft: {
                    body->applyForce(action.rayDirection * POKE_IMPULSE_SOFT, action.target.hitPosition, true);
                    break;
                }
                case Pokiness::Hard: {
                    body->applyForce(action.rayDirection * POKE_IMPULSE_HARD, action.target.hitPosition, true);
                    break;
                }

                case Pokiness::Flip: {
                    body->applyForce(vec3 {0.0f, (POKE_IMPULSE_SOFT + POKE_IMPULSE_HARD) / 2.0f, 0.0f},
                                     action.target.hitPosition, true);
                    break;
                }
            }

            break;
        }
    }
}

void JanusApp::reset() {

    select({});

    _actionTarget.reset();

    if (_cursorMarker) {
        _cursorMarker->hidden(true);
    }

    _simulationRoot->removeFromParent();

    _pickIgnores.clear();
    _transients.clear();

    scene().physicsWorld()->gravity(GRAVITY_EARTH);

    _simulationRoot = MakeSimulationRoot();
    scene().rootNode()->addChild(_simulationRoot);

    runner().resetSimulation();

    if (runner().simulationPaused()) {
        runner().simulationPaused(false);
    }
}

// [Private Static Non-Member Functions]

shared_ptr<Node> MakeSimulationRoot() {
    auto root = Node::NamedNode("Simulation root");

    // janus

    // {
    //     static auto [mesh, shape] = [] {
    //         constexpr float HEIGHT = 1.0f;
    //
    //         auto visMesh = util::fs::MeshAt("janus/janus.gltf");
    //         //static auto physMesh = util::fs::MeshAt("janus/phys.gltf", Mesh::ImportOptions::None);
    //
    //         const float scaleFactor = HEIGHT / visMesh->localExtent().y;
    //         const auto  transform = math::scale(mat4(1.0f), vec3(scaleFactor));
    //
    //         visMesh->burnTransform(transform, true);
    //         //physMesh->burnTransform(transform, true);
    //
    //         //auto shape = make_shared<PhysicsShape>(PhysicsShape::Type::ConcavePolyhedron, physMesh);
    //         auto shape = PhysicsShape::ConvexHullShape(visMesh);
    //
    //         return std::pair {visMesh, shape};
    //     }();
    //
    //     auto node = Node::MeshNode(mesh);
    //     node->name("Janus");
    //     node->position({-1.5f, 0.0f, 0.0f});
    //
    //     auto body = PhysicsBody::DynamicBody(shape);
    //     body->mass(10.0f);
    //     body->friction(0.6f);
    //     body->restitution(0.15f);
    //     body->linearDamping(0.03f);
    //     body->angularDamping(0.05f);
    //     node->physicsBody(std::move(body));
    //
    //     root->addChild(node);
    // }

    // // angel
    // {
    //     static auto [mesh, shape] = [] {
    //         constexpr float HEIGHT = 2.0f;
    //
    //         auto        visMesh = util::fs::MeshAt("aniel/aniel.gltf");
    //         static auto physMesh = util::fs::MeshAt("aniel/phys.gltf", Mesh::ImportOptions::None);
    //
    //         const float scaleFactor = HEIGHT / visMesh->localExtent().y;
    //         const auto  transform = math::scale(mat4(1.0f), vec3(scaleFactor));
    //
    //         visMesh->burnTransform(transform, true);
    //         physMesh->burnTransform(transform, true);
    //
    //         auto shape = PhysicsShape::ConcavePolyhedronShape(physMesh);
    //
    //         return std::pair {visMesh, shape};
    //     }();
    //
    //     auto node = Node::MeshNode(mesh);
    //     node->name("Angel");
    //     node->rotation({0.0f, 1.0f, 0.0f}, radians(180.0f));
    //     node->position({0.0f, 0.0f, 0.0f});
    //
    //     auto body = PhysicsBody::StaticBody(shape);
    //     // body->mass(1000.0f);
    //     // body->friction(0.65f);
    //     // body->rollingFriction(0.02f);
    //     // body->restitution(0.03f);
    //     node->physicsBody(std::move(body));
    //
    //     root->addChild(node);
    // }

    // teapot

    // {
    //     static auto mesh = [] {
    //         constexpr float HEIGHT = 0.35f;
    //         auto            mesh = util::fs::MeshAt("marble_teapot/marble_teapot.gltf");
    //         const float     teapotScale = HEIGHT / mesh->localExtent().y;
    //         mesh->burnTransform(math::scale(mat4(1.0f), vec3(teapotScale)), true);
    //         return mesh;
    //     }();
    //
    //     auto node = Node::MeshNode(mesh);
    //     node->name("Teapot");
    //     node->position({1.5f, 2.5f - mesh->localAABB().min.y, 0.0f});
    //
    //     static auto shape = PhysicsShape::ConvexHullShape(mesh);
    //     auto        body = PhysicsBody::DynamicBody(shape);
    //     body->mass(1.5f);
    //     body->friction(0.6f);
    //     body->restitution(0.15f);
    //     body->linearDamping(0.03f);
    //     body->angularDamping(0.05f);
    //     node->physicsBody(std::move(body));
    //
    //     root->addChild(node);
    // }

    return root;
}

optional<JanusApp::PickResult> Pick(VisualWorld&               visualWorld,
                                    const vec2&                screenPosition,
                                    const vector<const Node*>& ignoredNodes) {

    const auto hits =
        visualWorld.hitTest(screenPosition, {.searchMode = HitTestSearchMode::All, .elementBoundsOnly = true});

    for (const auto& hit : hits) {

        auto node = hit.node();

        if (!node) {
            continue;
        }

        if (IsIgnored(node, ignoredNodes)) {
            continue;
        }

        return JanusApp::PickResult {.node = node,
                                     .hitPosition = hit.worldCoordinates(),
                                     .hitNormal = hit.worldNormal()};
    }

    return {};
}

optional<JanusApp::PickResult> FindActionTarget(Scene&                     scene,
                                                const vec2&                screenPosition,
                                                const vector<const Node*>& ignoredNodes) {

    auto visualWorld = scene.visualWorld();

    if (auto result = Pick(*visualWorld, screenPosition, ignoredNodes)) {
        return result;
    }

    const vec3 from = visualWorld->unprojectPoint({screenPosition.x, screenPosition.y, 0.0f});
    const vec3 to = visualWorld->unprojectPoint({screenPosition.x, screenPosition.y, 1.0f});

    for (const auto hits = scene.physicsWorld()->rayTest(from, to); const auto& hit : hits) {

        auto node = hit.node();

        if (IsIgnored(node, ignoredNodes)) {
            continue;
        }

        return JanusApp::PickResult {.node = node,
                                     .hitPosition = hit.worldCoordinates(),
                                     .hitNormal = hit.worldNormal()};
    }

    return {};
}

vector<shared_ptr<Node>> DropRocks(Node&         parent,
                                   const vec3&   location,
                                   const vec3&   boxSize,
                                   const u8vec3& stackSize,
                                   float         padding) {
    // auto rockScene =
    //     util::fs::SceneAt("rocks_convex/rocks_convex.gltf",
    //                       Scene::ImportOptions::ImportMeshes | Scene::ImportOptions::ImportMaterials);

    auto rockScene =
        util::fs::SceneAt("rocks_concave/rocks_concave.gltf",
                          Scene::ImportOptions::ImportMeshes | Scene::ImportOptions::ImportMaterials);

    struct Rock {
        shared_ptr<Mesh>         mesh;
        shared_ptr<PhysicsShape> physicsShape;
    };

    vector<Rock> rocks;

    for (const auto& node : rockScene->rootNode()->children()) {

        auto mesh = node->mesh();
        if (!mesh) {
            continue;
        }

        mesh->burnTransform(util::geom::fit_inside(mesh->localAABB(), boxSize), true);
        auto physicsShape = PhysicsShape::ConvexHullShape(mesh);
        rocks.push_back({std::move(mesh), std::move(physicsShape)});
    }

    const unsigned sizeX = stackSize.x;
    const unsigned sizeY = stackSize.y;
    const unsigned sizeZ = stackSize.z;

    vector<shared_ptr<Node>> added;
    added.reserve(sizeX * sizeZ * sizeY);

    const float stepX = boxSize.x + padding;
    const float stepY = boxSize.y + padding;
    const float stepZ = boxSize.z + padding;

    const float totalLength = boxSize.x * static_cast<float>(sizeX) + padding * static_cast<float>(sizeX - 1);
    const float totalWidth = boxSize.z * static_cast<float>(sizeZ) + padding * static_cast<float>(sizeZ - 1);

    const float startX = location.x - totalLength * 0.5f + boxSize.x * 0.5f;
    const float startZ = location.z - totalWidth * 0.5f + boxSize.z * 0.5f;
    const float startY = location.y + boxSize.y * 0.5f;

    size_t rockIndex = 0;

    for (unsigned y = 0; y < sizeY; ++y) {
        for (unsigned z = 0; z < sizeZ; ++z) {
            for (unsigned x = 0; x < sizeX; ++x) {

                const auto& rock = rocks[rockIndex++ % rocks.size()];

                auto node = Node::MeshNode(rock.mesh);

                static int rockNum = 0;
                node->name(std::format("Rock {}", ++rockNum));

                node->position({startX + static_cast<float>(x) * stepX, startY + static_cast<float>(y) * stepY,
                                startZ + static_cast<float>(z) * stepZ});

                auto physicsBody = PhysicsBody::DynamicBody(rock.physicsShape);
                physicsBody->mass(3.5f);
                physicsBody->restitution(0.02f);
                physicsBody->friction(0.8f);

                const float ANGULAR_VARIANCE = radians(30.0f);
                physicsBody->angularVelocity(uniform_linear(vec3 {-ANGULAR_VARIANCE}, vec3 {ANGULAR_VARIANCE}));

                node->physicsBody(std::move(physicsBody));

                added.push_back(node);
                parent.addChild(node);
            }
        }
    }

    return added;
}

vector<shared_ptr<Node>> DropCoins(Node&         parent,
                                   const vec3&   location,
                                   const vec3&   boxSize,
                                   const u8vec3& stackSize,
                                   float         padding) {

    const unsigned sizeX = stackSize.x;
    const unsigned sizeY = stackSize.y;
    const unsigned sizeZ = stackSize.z;

    vector<shared_ptr<Node>> added;
    added.reserve(sizeX * sizeY * sizeZ);

    static auto [mesh, shape] = [] {
        constexpr float MAX_DIM = 0.35f;

        auto mesh = util::fs::MeshAt("roman_coin/roman_coin.gltf");

        const float scaleFactor = MAX_DIM / math::max(mesh->localExtent());

        auto transform = math::rotate(mat4(1.0f), radians(90.0f), vec3 {1.0f, 0.0f, 0.0f});

        transform = math::scale(transform, vec3(scaleFactor));

        mesh->burnTransform(transform, true);

        // after rotation:
        // X = diameter
        // Y = thickness
        // Z = diameter
        const auto  extent = mesh->localExtent();
        const float radius = math::max(extent.x, extent.z) * 0.5f;
        const float height = extent.y;
        auto        shape = make_shared<CylinderPhysicsShape>(radius, height);

        mesh->firstMaterial()->specular(Color::LightGray());
        mesh->firstMaterial()->specularExponent(16.0f);

        return std::pair {mesh, shape};
    }();

    const float stepX = boxSize.x + padding;
    const float stepY = boxSize.y + padding;
    const float stepZ = boxSize.z + padding;

    const float totalLength = boxSize.x * static_cast<float>(sizeX) + padding * static_cast<float>(sizeX - 1);
    const float totalWidth = boxSize.z * static_cast<float>(sizeY) + padding * static_cast<float>(sizeY - 1);

    const float startX = location.x - totalLength * 0.5f + boxSize.x * 0.5f;
    const float startZ = location.z - totalWidth * 0.5f + boxSize.z * 0.5f;
    const float startY = location.y + boxSize.y * 0.5f;

    for (unsigned y = 0; y < sizeZ; ++y) {
        for (unsigned z = 0; z < sizeY; ++z) {
            for (unsigned x = 0; x < sizeX; ++x) {

                auto node = Node::MeshNode(mesh);

                static int boxNum = 0;
                node->name(std::format("Coin {}", ++boxNum));

                const vec3 positionVariance {padding / 2.0f, padding / 8.0f, padding / 2.0f};
                node->position(vec3 {startX + static_cast<float>(x) * stepX,
                                     startY + static_cast<float>(y) * stepY,
                                     startZ + static_cast<float>(z) * stepZ}
                               + uniform_linear(-positionVariance, positionVariance));

                node->eulerAngles(uniform_linear(vec3 {0.0f}, vec3 {TWO_PI}));

                auto physicsBody = PhysicsBody::DynamicBody(shape);
                physicsBody->mass(20.0f);
                physicsBody->restitution(0.25f);
                physicsBody->friction(0.5f);
                physicsBody->rollingFriction(0.15f);
                physicsBody->spinningFriction(0.10f);
                physicsBody->angularDamping(0.08f);

                // const auto extent = mesh->localExtent();
                // const float minExtent = math::min(extent);
                // physicsBody->ccdMotionThreshold(minExtent * 0.25f);
                // physicsBody->ccdSweptSphereRadius(minExtent * 0.20f);
                // physicsBody->ccdEnabled(true);

                const float ANGULAR_VARIANCE = radians(180.0f);
                physicsBody->angularVelocity(uniform_linear(vec3 {-ANGULAR_VARIANCE}, vec3 {ANGULAR_VARIANCE}));

                node->physicsBody(std::move(physicsBody));

                added.push_back(node);
                parent.addChild(node);
            }
        }
    }

    return added;
}

vector<shared_ptr<Node>> DropBalls(Node&         parent,
                                   const vec3&   location,
                                   const vec3&   boxSize,
                                   const u8vec3& stackSize,
                                   float         padding) {

    const unsigned sizeX = stackSize.x;
    const unsigned sizeY = stackSize.y;
    const unsigned sizeZ = stackSize.z;

    vector<shared_ptr<Node>> added;
    added.reserve(sizeX * sizeY * sizeZ);

    static auto [mesh, shape] = [] {
        constexpr float MAX_DIM = 0.45f;

        auto mesh = util::fs::MeshAt("beachball/beachball.gltf");

        const float scaleFactor = MAX_DIM / math::max(mesh->localExtent());
        const auto  transform = math::scale(mat4(1.0f), vec3(scaleFactor));
        mesh->burnTransform(transform, true);

        auto shape = make_shared<SpherePhysicsShape>(MAX_DIM / 2.0);

        mesh->firstMaterial()->specular(Color::LightGray());
        mesh->firstMaterial()->specularExponent(16.0f);

        return std::pair {mesh, shape};
    }();

    const float stepX = boxSize.x + padding;
    const float stepY = boxSize.y + padding;
    const float stepZ = boxSize.z + padding;

    const float totalLength = boxSize.x * static_cast<float>(sizeX) + padding * static_cast<float>(sizeX - 1);
    const float totalWidth = boxSize.z * static_cast<float>(sizeY) + padding * static_cast<float>(sizeY - 1);

    const float startX = location.x - totalLength * 0.5f + boxSize.x * 0.5f;
    const float startZ = location.z - totalWidth * 0.5f + boxSize.z * 0.5f;
    const float startY = location.y + boxSize.y * 0.5f;

    for (unsigned y = 0; y < sizeZ; ++y) {
        for (unsigned z = 0; z < sizeY; ++z) {
            for (unsigned x = 0; x < sizeX; ++x) {

                auto node = Node::MeshNode(mesh);

                static int boxNum = 0;
                node->name(std::format("Beachball {}", ++boxNum));

                const vec3 positionVariance {padding / 2.0f, padding / 8.0f, padding / 2.0f};
                node->position(vec3 {startX + static_cast<float>(x) * stepX,
                                     startY + static_cast<float>(y) * stepY,
                                     startZ + static_cast<float>(z) * stepZ}
                               + uniform_linear(-positionVariance, positionVariance));

                node->eulerAngles(uniform_linear(vec3 {0.0f}, vec3 {TWO_PI}));

                auto physicsBody = PhysicsBody::DynamicBody(shape);
                physicsBody->mass(0.10f);
                physicsBody->restitution(0.8f);
                physicsBody->friction(0.4f);
                physicsBody->rollingFriction(0.01);
                physicsBody->angularDamping(0.5);

                const float ANGULAR_VARIANCE = radians(30.0f);
                physicsBody->angularVelocity(uniform_linear(vec3 {-ANGULAR_VARIANCE}, vec3 {ANGULAR_VARIANCE}));

                node->physicsBody(std::move(physicsBody));

                added.push_back(node);
                parent.addChild(node);
            }
        }
    }

    return added;
}

shared_ptr<Node> ThrowHammer(Node& parent, const vec3& location, const vec3& velocity) {

    static auto [mesh, shape] = [] {
        constexpr float LENGTH = 1.0f;
        auto            mesh = util::fs::MeshAt("hammer/hammer.gltf");
        const float     scaleFactor = LENGTH / mesh->localExtent().z;
        auto            transform = math::rotate(mat4(1.0f), radians(90.0f), vec3 {1.0f, 0.0f, 0.0f});
        transform = math::scale(transform, scaleFactor);
        mesh->burnTransform(transform, true);
        auto shape = PhysicsShape::ConvexHullShape(mesh);
        // auto shape = PhysicsShape::ConcavePolyhedronShape(mesh);
        return std::pair {mesh, shape};
    }();

    auto       node = Node::MeshNode(mesh);
    static int hammerNum = 0;
    node->name(std::format("Hammer {}", ++hammerNum));
    node->position(location);

    auto physicsBody = PhysicsBody::DynamicBody(shape);
    physicsBody->mass(5.0f);
    physicsBody->restitution(0.15f);
    physicsBody->friction(0.8f);

    static const auto extent = mesh->localExtent();
    physicsBody->centerOfMass(physicsBody->centerOfMass() + extent * vec3 {0.0f, 0.3f, 0.0f});

    const vec3 throwForward = math::normalize(vec3 {velocity.x, 0.0f, velocity.z});
    // node forward is -Z, so yaw -Z toward the horizontal throw direction.
    const float        yaw = math::atan2(-throwForward.x, -throwForward.z);
    static const float START_PITCH = radians(20.0f);
    static const float START_PITCH_VARIANCE = radians(10.0f);
    static const float SIDE_TILT_VARIANCE = radians(8.0f);
    node->eulerAngles({START_PITCH + uniform_linear(-START_PITCH_VARIANCE, START_PITCH_VARIANCE), yaw,
                       uniform_linear(-SIDE_TILT_VARIANCE, SIDE_TILT_VARIANCE)});
    static const float SPIN_RATE = radians(540.0f);
    static const float SPIN_RATE_VARIANCE = radians(90.0f);
    const float        spinRate = SPIN_RATE + uniform_linear(-SPIN_RATE_VARIANCE, SPIN_RATE_VARIANCE);
    physicsBody->angularVelocity(-node->right() * spinRate);

    physicsBody->linearVelocity(velocity);

    node->physicsBody(std::move(physicsBody));

    parent.addChild(node);

    return node;
}

shared_ptr<Node> ThrowHula(Node& parent, const vec3& location, const vec3& velocity) {

    static auto [mesh, shape] = [] {
        constexpr float DIAMETER = 1.1f;
        auto            mesh = util::fs::MeshAt("hula/hula.gltf");
        const float     scaleFactor = DIAMETER / mesh->localExtent().y;
        mesh->burnTransform(math::scale(mat4(1.0f), vec3(scaleFactor)), true);
        auto shape = PhysicsShape::ConcavePolyhedronShape(mesh);
        return std::pair {mesh, shape};
    }();

    auto node = Node::MeshNode(mesh);

    static int hulaNum = 0;
    node->name(std::format("Hula {}", ++hulaNum));
    node->position(location);

    auto physicsBody = PhysicsBody::DynamicBody(shape);

    physicsBody->mass(1.0f);
    physicsBody->restitution(0.15f);
    physicsBody->friction(0.7f);

    static const auto extent = mesh->localExtent();
    physicsBody->centerOfMass(physicsBody->centerOfMass() + extent * vec3 {0.0f, .1f, 0.0f});

    static const float minExtent = math::min(extent);
    physicsBody->ccdMotionThreshold(minExtent * 0.25f);
    physicsBody->ccdSweptSphereRadius(minExtent * 0.20f);
    physicsBody->ccdEnabled(true);

    const vec3 up {0.0f, 1.0f, 0.0f};
    vec3       forward {0.0f, 0.0f, -1.0f};
    const vec3 horizontalVelocity {velocity.x, 0.0f, velocity.z};
    if (length(horizontalVelocity) > F32_COMPARE_EPSILON) {
        forward = normalize(horizontalVelocity);
    }
    const vec3  right = normalize(cross(forward, up));
    const float tilt = radians(uniform_linear(10.0f, 20.0f));
    const float bank = radians(uniform_linear(-5.0f, 5.0f));
    const auto  flatOrientation = quaternion({1.0f, 0.0f, 0.0f}, radians(-90.0f));
    const auto  tiltOrientation = quaternion(right, tilt);
    const auto  bankOrientation = quaternion(forward, bank);
    const auto  orientation = bankOrientation * tiltOrientation * flatOrientation;
    node->orientation(orientation);

    const vec3  spinAxis = -normalize(orientation * vec3 {0.0f, 0.0f, 1.0f});
    const float SPIN_RATE = radians(360.0f * 1.5f);
    physicsBody->angularVelocity(spinAxis * SPIN_RATE);

    physicsBody->linearVelocity(velocity);

    node->physicsBody(std::move(physicsBody));

    parent.addChild(node);

    return node;
}

shared_ptr<Node> ThrowDuck(Node& parent, const vec3& location, const vec3& velocity) {

    static auto [mesh, shape] = [] {
        constexpr float HEIGHT = 0.5f;
        auto            mesh = util::fs::MeshAt("duck/duck.gltf");
        const float     scaleFactor = HEIGHT / mesh->localExtent().y;
        mesh->burnTransform(math::scale(mat4(1.0f), vec3(scaleFactor)), true);
        auto shape = PhysicsShape::ConvexHullShape(mesh);
        return std::pair {mesh, shape};
    }();

    auto       node = Node::MeshNode(mesh);
    static int quackNum = 0;
    node->name(std::format("Quack {}", ++quackNum));
    node->position(location);

    auto physicsBody = PhysicsBody::DynamicBody(shape);
    physicsBody->mass(1.0f);
    physicsBody->restitution(0.35f);
    physicsBody->friction(0.8f);

    static const auto extent = mesh->localExtent();
    physicsBody->centerOfMass(physicsBody->centerOfMass() + extent * vec3 {0.0f, -0.1f, 0.0f});

    static const float minExtent = math::min(extent);
    physicsBody->ccdMotionThreshold(minExtent * 0.25f);
    physicsBody->ccdSweptSphereRadius(minExtent * 0.25f);
    physicsBody->ccdEnabled(true);

    node->eulerAngles(uniform_linear(vec3 {0.0f}, vec3 {TWO_PI}));

    static const float ANGULAR_VARIANCE = radians(360.0f);
    physicsBody->angularVelocity(uniform_linear(vec3 {-ANGULAR_VARIANCE}, vec3 {ANGULAR_VARIANCE}));

    physicsBody->linearVelocity(velocity);

    node->physicsBody(std::move(physicsBody));

    parent.addChild(node);

    return node;
}

bool IsIgnored(const shared_ptr<Node>& node, const vector<const Node*>& ignoredNodes) {
    return !node || find(ignoredNodes.begin(), ignoredNodes.end(), node.get()) != ignoredNodes.end();
}
