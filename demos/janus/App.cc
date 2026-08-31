//
//  App.cc
//  janus
//
//  Created by Morgan Davis on 8/4/26.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#include "App.h"
#include "Constants.h"
#include "TransientsBuilder.h"

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

const u8vec3 ROCK_GRID_SIZE {3, 3, 3};
const u8vec3 COIN_GRID_SIZE {3, 3, 3};
const u8vec3 BALL_GRID_SIZE {3, 3, 3};

// [Private Static Non-Member Prototypes]

static optional<App::PickResult> Pick(VisualWorld&               visualWorld,
                                           const vec2&                screenPosition,
                                           const vector<const Node*>& ignoredNodes = {},
                                           bool                       elementBoundsOnly = true);
static pair<vec3, vec3>               CalculateThrowTrajectory(const vec3& cameraPosition,
                                                               const vec3& targetPosition,
                                                               const vec3& gravity);

// [Public Lifecycle Functions]

App::App(int argc, char* argv[]):
    Application(argc, argv, APP_LOG_LEVEL),
    _window {nullptr},
    _dynamicsRoot {nullptr},
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
    _transientTracker {ext::TransientTracker::SweepPolicy::EveryInterval(1.0)},
    _backgroundRotationTime {0.0},
    _orbWanderers {},
    _pickIgnores {},
    _dynamicsTransforms {},
    _simulationGeneration {0},
    _pendingReset {false} {}

App::~App() = default;

// [Application Protected Member Functions]

std::unique_ptr<Scene> App::init() {
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

        auto radialFade = Ground::RadialFade {.color = {0.005f, 0.005f, 0.005f, 1.0f},
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

        // load the base scene

        auto scene = util::fs::SceneAt("janus/janus.gltf", Scene::ImportOptions::ImportMeshes
                                                               | Scene::ImportOptions::ImportMaterials
                                                               | Scene::ImportOptions::ImportLights);
        scene->visualWorld(std::move(visualWorld));
        scene->physicsWorld(std::move(physicsWorld));
        scene->inputContext(Window::InputContext());
        scene->debugOptions(Scene::DebugOptions::ShowStatsOverlay);

        /*  root
                environment
                    environment.phys
                        evora.phys
                        lion.phys
                    evora
                    lights
                        ...
                    lion
                dynamics
                    dynamics.phys
                        augustus.phys
                        diana.phys
                    augustus
                    diana
                    janus
                    plinth_janus
                    plinth_teapot
                    teapot
                transient_assets
                    ball
                    coin
                    hammer
                    hula
                    quack
                    rocks
                        ...
         */

        auto environmentRoot = scene->rootNode()->childNamed("environment");

        if (auto node = environmentRoot->childNamed("evora")) {

            _pickIgnores.push_back({.node = node, .purposes = PickPurpose::Hover});

            auto physNode = environmentRoot->childNamed("evora.phys", true);
            auto shape = PhysicsShape::ConcavePolyhedronShape(physNode->mesh());
            auto body = PhysicsBody::StaticBody(shape);
            body->friction(STONE_FRICTION);
            body->restitution(STONE_RESTITUTION);
            node->physicsBody(std::move(body));
        }

        if (auto node = environmentRoot->childNamed("lion")) {

            _pickIgnores.push_back({.node = node, .purposes = PickPurpose::Hover});

            auto physNode = environmentRoot->childNamed("lion.phys", true);
            auto shape = PhysicsShape::ConcavePolyhedronShape(physNode->mesh());
            auto body = PhysicsBody::StaticBody(shape);
            body->friction(STONE_FRICTION);
            body->restitution(STONE_RESTITUTION);
            node->physicsBody(std::move(body));
        }

        environmentRoot->childNamed("environment.phys")->hidden(true);

        _dynamicsRoot = scene->rootNode()->childNamed("dynamics");

        if (auto node = _dynamicsRoot->childNamed("janus")) {

            auto body = PhysicsBody::DynamicBody();
            // auto shape = PhysicsShape::ConcavePolyhedronShape(node->mesh());
            // node->physicsBody(PhysicsBody::DynamicBody(shape));
            body->mass(20.0f);
            body->friction(STONE_FRICTION);
            body->restitution(STONE_RESTITUTION);

            auto extent = node->mesh()->localExtent();
            body->centerOfMass(body->centerOfMass() + extent * vec3 {0.0f, -0.15f, 0.0f});

            node->physicsBody(std::move(body));
        }

        if (auto node = _dynamicsRoot->childNamed("teapot")) {

            auto body = PhysicsBody::DynamicBody();
            // auto shape = PhysicsShape::ConcavePolyhedronShape(node->mesh());
            // node->physicsBody(PhysicsBody::DynamicBody(shape));
            // node->physicsBody()->mass(3.0f);
            body->friction(0.5f);
            body->restitution(0.2f);
            body->rollingFriction(0.1f);
            body->spinningFriction(0.05);
            body->angularSleepingThreshold(0.1f);

            auto extent = node->mesh()->localExtent();
            body->centerOfMass(body->centerOfMass() + extent * vec3 {-0.1f, 0.0f, 0.0f});

            // const auto moi = body->momentOfInertia();
            // log::i()("Teapot MOI: {}, {}, {}", moi.x, moi.y, moi.z);

            // body->autocalculatesMomentOfInertia(false);
            // body->momentOfInertia(body->momentOfInertia() * 1.5f);


            const auto moi = body->momentOfInertia();

            log::i()("Teapot auto MOI: {}, {}, {}", moi.x, moi.y, moi.z);

            body->autocalculatesMomentOfInertia(false);
            body->momentOfInertia(moi * 1.5f);

            const auto manualMoi = body->momentOfInertia();

            log::i()("Teapot manual MOI: {}, {}, {}",
                     manualMoi.x,
                     manualMoi.y,
                     manualMoi.z);




            // body->rollingFriction(0.15f);
            // body->spinningFriction(0.1);
            // body->angularDamping(0.4f);


            // body->angularDamping(0.05f);
            node->physicsBody(std::move(body));
        }

        if (auto node = _dynamicsRoot->childNamed("plinth_janus")) {

            auto body = PhysicsBody::DynamicBody();
            // node->physicsBody(PhysicsBody::DynamicBody());
            body->mass(40.0f);
            body->friction(STONE_FRICTION);
            body->restitution(STONE_RESTITUTION);
            node->physicsBody(std::move(body));
        }

        if (auto node = _dynamicsRoot->childNamed("plinth_teapot")) {

            auto body = PhysicsBody::DynamicBody();
            // node->physicsBody(PhysicsBody::DynamicBody());
            body->mass(30.0f);
            body->friction(STONE_FRICTION);
            body->restitution(STONE_RESTITUTION);
            node->physicsBody(std::move(body));
        }

        // if (auto node = _dynamicsRoot->childNamed("plinth3")) {
        //     node->physicsBody(PhysicsBody::DynamicBody());
        //     node->physicsBody()->mass(40.0f);
        //     node->physicsBody()->friction(0.6f);
        //     node->physicsBody()->restitution(0.15f);
        // }

        if (auto node = _dynamicsRoot->childNamed("diana")) {

            // auto physNode = _dynamicsRoot->childNamed("diana.phys", true);
            // auto shape = PhysicsShape::ConcavePolyhedronShape(physNode->mesh());
            // node->physicsBody(PhysicsBody::DynamicBody(shape));
            //
            // // auto shape = PhysicsShape::ConcavePolyhedronShape(node->mesh());
            // // node->physicsBody(PhysicsBody::DynamicBody(shape));
            //
            // node->physicsBody()->mass(50.0f);
            // node->physicsBody()->friction(STONE_FRICTION);
            // node->physicsBody()->restitution(STONE_RESTITUTION);
            // node->physicsBody()->angularSleepingThreshold(0.25); // default = 1
            //
            // auto extent = node->mesh()->localExtent();
            // node->physicsBody()->centerOfMass(node->physicsBody()->centerOfMass()
            //                                   + extent * vec3 {0.0f, -0.1f, 0.0f});

            node->hidden(true);
        }

        if (auto node = _dynamicsRoot->childNamed("artemis")) {

            auto physNode = _dynamicsRoot->childNamed("artemis.phys", true);
            auto shape = PhysicsShape::ConcavePolyhedronShape(physNode->mesh());
            auto body = PhysicsBody::DynamicBody(shape);
            //node->physicsBody(PhysicsBody::DynamicBody(shape));

            // auto shape = PhysicsShape::ConcavePolyhedronShape(node->mesh());
            // node->physicsBody(PhysicsBody::DynamicBody(shape));

            // node->physicsBody(PhysicsBody::DynamicBody());

            body->mass(50.0f);
            body->friction(STONE_FRICTION);
            body->restitution(STONE_RESTITUTION);
            body->angularSleepingThreshold(0.25); // default = 1

            auto extent = node->mesh()->localExtent();
            body->centerOfMass(body->centerOfMass() + extent * vec3 {0.0f, -0.15f, 0.0f});

            node->physicsBody(std::move(body));
        }

        if (auto node = _dynamicsRoot->childNamed("augustus")) {

            auto physNode = _dynamicsRoot->childNamed("augustus.phys", true);
            auto shape = PhysicsShape::ConcavePolyhedronShape(physNode->mesh());
            auto body = PhysicsBody::DynamicBody(shape);
            // node->physicsBody(PhysicsBody::DynamicBody(shape));

            // auto shape = PhysicsShape::ConcavePolyhedronShape(node->mesh());
            // node->physicsBody(PhysicsBody::DynamicBody(shape));

            body->mass(55.0f);
            body->friction(STONE_FRICTION);
            body->restitution(STONE_RESTITUTION);
            body->angularSleepingThreshold(0.25); // default = 1

            auto extent = node->mesh()->localExtent();
            body->centerOfMass(body->centerOfMass() + extent * vec3 {0.0f, -0.1f, 0.0f});

            node->physicsBody(std::move(body));
        }

        _dynamicsRoot->childNamed("dynamics.phys")->hidden(true);

        saveDynamicsTransforms();

        auto transientAssetsNode = scene->rootNode()->childNamed("transient_assets");
        _transientsCache.init(*transientAssetsNode);
        transientAssetsNode->removeFromParent();

        _transientsRoot = Node::NamedNode("transients");
        scene->rootNode()->addChild(_transientsRoot);

        // create and configure the ground
        {
            auto node = Node::NamedNode("Ground");
            node->orientation(math::quaternion({1.0f, 0.0f, 0.0f}, radians(-90.0f)));
            auto shape = make_shared<InfinitePlanePhysicsShape>();
            auto body = PhysicsBody::StaticBody(shape);
            body->friction(STONE_FRICTION);
            body->restitution(0.25f);
            node->physicsBody(std::move(body));
            scene->rootNode()->addChild(node);
        }

        // setup lighting

        auto ambientLight = make_shared<AmbientLight>(Color {0.15f});
        auto ambientLightNode = Node::LightNode(ambientLight);
        scene->rootNode()->addChild(ambientLightNode);

        // setup transient node groups

        _transientTracker.groupPolicy("rocks",
                                      {.maxCount =
                                           (ROCK_GRID_SIZE.x * ROCK_GRID_SIZE.y * ROCK_GRID_SIZE.z) * 1.5});
        _transientTracker.groupPolicy("coins",
                                      {.maxCount =
                                           (COIN_GRID_SIZE.x * COIN_GRID_SIZE.y * COIN_GRID_SIZE.z) * 2.5});
        _transientTracker.groupPolicy("balls",
                                      {.maxCount =
                                           (BALL_GRID_SIZE.x * BALL_GRID_SIZE.y * BALL_GRID_SIZE.z) * 2.5});
        _transientTracker.groupPolicy("hammers", {.maxCount = 10,
                                                  .distanceLimit =
                                                      ext::TransientTracker::DistanceLimit {.radius = 100.0f}});
        _transientTracker.groupPolicy("hulas", {.maxCount = 10,
                                                .distanceLimit =
                                                    ext::TransientTracker::DistanceLimit {.radius = 100.0f}});
        _transientTracker.groupPolicy("ducks", {.maxCount = 10,
                                                .distanceLimit =
                                                    ext::TransientTracker::DistanceLimit {.radius = 100.0f}});

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
        cameraConfig.minDistance = 0.5f;
        cameraConfig.maxDistance = 100.0f;
        _cameraController.config(cameraConfig);

        _cameraController.view({.target = vec3 {-0.1346f, 4.2466f, 0.4131},
                                .yaw = radians(-10.66f),
                                .pitch = radians(-1.89f),
                                .distance = 17.4275f});

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

        // wandering orbs

        {
            const vec3 ORB_GROUP_POSITION {-4.5f, 10.0f, -2.5f};

            const vec3 ORB_POSITION_MIN {-1.0f, -0.5f, -1.0f};
            const vec3 ORB_POSITION_MAX {1.0f, 0.5f, 1.0f};

            const vec3   ORB_WANDER_EXTENTS {1.5f, 0.75f, 1.5f};
            const size_t ORB_COUNT {4};

            auto orbGroup = Node::NamedNode("Orbs");
            orbGroup->position(ORB_GROUP_POSITION);
            scene->rootNode()->childNamed("environment")->addChild(orbGroup);

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

SimulationConfig App::simulationConfig() const {
    return {
        .timeStep = TIME_STEP,
        .maxCatchUpSteps = MAX_CATCH_UP_STEPS,
    };
}

bool App::shouldContinue(const Scene& scene) {
    return _window->isOpen();
}

void App::runnerUpdate(Runner& runner, Scene&, const Runner::UpdateInfo&) {

    if (_pendingReset) {
        _pendingReset = false;

        reset();
    }
}

void App::inputDidUpdate(Runner&       runner,
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
            select(*scene.visualWorld(), result.orbitButtonClick->position);
        }
    }

    if (result.panButtonClick) {
        queueAction(result.panButtonClick->position);
    }
}

void App::sceneWillStep(Runner& runner, Scene& scene, const Scene::StepInfo& info) {

    for (auto& wander : _orbWanderers) {
        wander.update(info.deltaTime);
    }
}

void App::sceneDidStep(Runner& runner, Scene& scene, const Scene::StepInfo& info) {

    _transientTracker.update(info);

    for (auto& ignore : _pickIgnores) {
        if (ignore.remainingTime) {
            *ignore.remainingTime -= info.deltaTime;
        }
    }

    erase_if(_pickIgnores, [](const PickIgnore& ignore) {
        return ignore.node.expired() || (ignore.remainingTime && *ignore.remainingTime <= 0.0);
    });
}

void App::frameDidBegin(Runner&                        runner,
                             Scene&                         scene,
                             VisualWorld&                   visualWorld,
                             const VisualWorld::RenderInfo& info) {

    if (!runner.simulationPaused()) {
        _backgroundRotationTime += info.updateDeltaTime * runner.timeScale();
    }

    //const float BACKGROUND_ROTATION_SPEED {radians(1.0 / 8.0f)};
    const float BACKGROUND_ROTATION_SPEED {radians(1.0 / 6.0f)};
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

        const auto screenPosition = input.mousePosition();

        hover(visualWorld, screenPosition);

        _actionTarget = target(scene, screenPosition);

        if (_actionTarget) {

            if (_cursorMarker) {
                _cursorMarker->position(_actionTarget->hitPosition);
                _cursorMarker->hidden(false);
            }
        }
        else {

            if (_cursorMarker) {
                _cursorMarker->hidden(true);
            }
        }
    }

    //_window->cursorHidden(!_cursorMarker->hidden());
}

// [Private Member Functions]

void App::hover(VisualWorld& visualWorld, const vec2& screenPosition) {

    auto result = Pick(visualWorld, screenPosition, pickIgnoredNodes(PickPurpose::Hover));

    if (result) {
        hover(result->node.lock());
    }
    else {
        hover(nullptr);
    }
}

void App::hover(shared_ptr<Node> node) {

    using DebugOptions = Node::DebugOptions;

    if (auto previous = _hoveredNode.lock()) {
        previous->debugOptions(util::bitmask::remove(previous->debugOptions(), DebugOptions::ShowHighlightBox));
    }

    _hoveredNode = node;

    if (node) {
        node->debugOptions(util::bitmask::add(node->debugOptions(), DebugOptions::ShowHighlightBox));
    }
}

void App::select(VisualWorld& visualWorld, const vec2& screenPosition) {

    select(Pick(visualWorld, screenPosition, pickIgnoredNodes(PickPurpose::Select), false));
}

void App::select(optional<PickResult> pickResult) {

    using DebugOptions = Node::DebugOptions;

    if (_selection) {
        if (auto node = _selection->node.lock()) {
            node->debugOptions(util::bitmask::remove(node->debugOptions(), DebugOptions::ShowHighlightTint));
        }
    }

    _selection = std::move(pickResult);

    if (_selection) {
        if (auto node = _selection->node.lock()) {
            node->debugOptions(util::bitmask::add(node->debugOptions(), DebugOptions::ShowHighlightTint));
        }
        else {
            _selection.reset();
        }
    }
}

optional<App::PickResult> App::target(Scene& scene, const vec2& screenPosition) const {

    auto visualWorld = scene.visualWorld();
    auto physicsWorld = scene.physicsWorld();

    if (!visualWorld || !physicsWorld) {
        return {};
    }

    const vec3 from = visualWorld->unprojectPoint({screenPosition.x, screenPosition.y, 0.0f});
    const vec3 to = visualWorld->unprojectPoint({screenPosition.x, screenPosition.y, 1.0f});

    const auto ignoredNodes = pickIgnoredNodes(PickPurpose::Target);

    const auto hits = physicsWorld->rayTest(from, to, {.searchMode = HitTestSearchMode::All});

    for (const auto& hit : hits) {

        auto node = hit.node();

        if (!node || find(ignoredNodes.begin(), ignoredNodes.end(), node.get()) != ignoredNodes.end()) {
            continue;
        }

        return PickResult {.node = node, .hitPosition = hit.worldCoordinates(), .hitNormal = hit.worldNormal()};
    }

    return {};
}

void App::queueAction(const vec2& screenPosition) {

    if (runner().simulationPaused()) {
        return;
    }

    auto& scene = App::scene();

    auto actionTarget = target(scene, screenPosition);

    _actionTarget = actionTarget;

    if (!actionTarget) {

        if (_cursorMarker) {
            _cursorMarker->hidden(true);
        }

        return;
    }

    if (_cursorMarker) {
        _cursorMarker->position(actionTarget->hitPosition);
        _cursorMarker->hidden(false);
    }

    auto visualWorld = scene.visualWorld();

    if (!visualWorld || !_cameraNode || !_dynamicsRoot) {
        return;
    }

    const vec3 from = visualWorld->unprojectPoint({screenPosition.x, screenPosition.y, 0.0f});
    const vec3 to = visualWorld->unprojectPoint({screenPosition.x, screenPosition.y, 1.0f});
    const vec3 ray = to - from;

    if (length(ray) <= F32_COMPARE_EPSILON) {
        return;
    }

    PendingAction pendingAction {.action = _action,
                                 .target = *actionTarget,
                                 .generation = _simulationGeneration,
                                 .dynamicsRoot = _dynamicsRoot,
                                 .cameraPosition = _cameraNode->worldPosition(),
                                 .rayDirection = normalize(ray)};

    queueScenePreStepCommand([this, pendingAction = std::move(pendingAction)](Scene& scene) {
        performAction(pendingAction);
    });
}

void App::performAction(const PendingAction& action) {

    const float DROP_HEIGHT {7.5f};

    const float ROCK_GAP {0.065f};
    const float COIN_GAP {0.165f};
    const float BALL_GAP {0.165f};

    const float POKE_IMPULSE_SOFT = 2.5f;
    const float POKE_IMPULSE_HARD = 10.0f;

    const double PROJECTILE_PICK_IGNORE_DURATION {1.5};

    if (action.generation != _simulationGeneration) {
        return;
    }

    auto& scene = App::scene();

    auto simulationRoot = action.dynamicsRoot.lock();

    // simulation may have been reset while this action was waiting for a step boundary
    if (!simulationRoot || simulationRoot != _dynamicsRoot) {
        return;
    }

    switch (action.action) {

        case Action::Drop: {

            const vec3 spawnLocation = action.target.hitPosition + vec3 {0.0f, DROP_HEIGHT, 0.0f};

            switch (_dropAction) {
                case DropAction::Rocks: {
                    auto rockNodes =
                        TransientsBuilder::BuildRocks(_transientsCache.rocks(), spawnLocation, ROCK_GRID_SIZE, ROCK_GAP);
                    _transientsRoot->addChildren(rockNodes);
                    _transientTracker.track(rockNodes, "rocks");
                    break;
                }
                case DropAction::Coins: {
                    auto coinNodes =
                        TransientsBuilder::BuildCoins(_transientsCache.coin(), spawnLocation, COIN_GRID_SIZE, COIN_GAP);
                    _transientsRoot->addChildren(coinNodes);
                    _transientTracker.track(coinNodes, "coins");
                    break;
                }
                case DropAction::Balls: {
                    auto ballNodes =
                        TransientsBuilder::BuildBalls(_transientsCache.ball(), spawnLocation, BALL_GRID_SIZE, BALL_GAP);
                    _transientsRoot->addChildren(ballNodes);
                    _transientTracker.track(ballNodes, "balls");
                    break;
                }
            }

            break;
        }

        case Action::Throw: {

            const auto [spawnPosition, velocity] =
                CalculateThrowTrajectory(action.cameraPosition, action.target.hitPosition,
                                         scene.physicsWorld()->gravity());

            switch (_throwAction) {
                case ThrowAction::Hammer: {
                    auto hammerNode = TransientsBuilder::BuildHammer(_transientsCache.hammer(), spawnPosition, velocity);
                    _transientsRoot->addChild(hammerNode);
                    _pickIgnores.push_back({.node = hammerNode,
                                            .remainingTime = PROJECTILE_PICK_IGNORE_DURATION});
                    _transientTracker.track(hammerNode, "hammers");
                    break;
                }
                case ThrowAction::Hula: {
                    auto hulaNode = TransientsBuilder::BuildHula(_transientsCache.hula(), spawnPosition, velocity);
                    _pickIgnores.push_back({.node = hulaNode,
                                            .remainingTime = PROJECTILE_PICK_IGNORE_DURATION});
                    _transientsRoot->addChild(hulaNode);
                    _transientTracker.track(hulaNode, "hulas");
                    break;
                }
                case ThrowAction::Duck: {
                    auto duckNode = TransientsBuilder::BuildDuck(_transientsCache.duck(), spawnPosition, velocity);
                    _transientsRoot->addChild(duckNode);
                    _pickIgnores.push_back({.node = duckNode,
                                            .remainingTime = PROJECTILE_PICK_IGNORE_DURATION});
                    _transientTracker.track(duckNode, "ducks");
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

vector<const Node*> App::pickIgnoredNodes(PickPurpose purpose) const {

    vector<const Node*> nodes;
    nodes.reserve(_pickIgnores.size() + 1);

    if (_cursorMarker) {
        nodes.push_back(_cursorMarker.get());
    }

    for (const auto& entry : _pickIgnores) {

        if (!util::bitmask::contains(entry.purposes, purpose)) {
            continue;
        }

        if (auto node = entry.node.lock()) {
            nodes.push_back(node.get());
        }
    }

    return nodes;
}

void App::saveDynamicsTransforms() {

    _dynamicsTransforms.clear();

    for (const auto& node : _dynamicsRoot->children(true)) {
        _dynamicsTransforms.push_back({.node = node.get(), .transform = node->transform()});
    }
}

void App::restoreDynamicsTransforms() const {

    for (const auto& state : _dynamicsTransforms) {
        state.node->transform(state.transform);
    }
}

void App::reset() {

    select({});

    _transientTracker.clear();

    _transientsRoot->removeFromParent();
    _transientsRoot = Node::NamedNode("transients");
    scene().rootNode()->addChild(_transientsRoot);

    restoreDynamicsTransforms();

    _actionTarget.reset();

    erase_if(_pickIgnores, [](const PickIgnore& ignore) {
        return ignore.remainingTime.has_value();
    });

    if (_cursorMarker) {
        _cursorMarker->hidden(true);
    }

    scene().physicsWorld()->gravity(GRAVITY_EARTH);

    runner().resetSimulation();

    if (runner().simulationPaused()) {
        runner().simulationPaused(false);
    }

    ++_simulationGeneration;
}

// [Private Static Non-Member Functions]

optional<App::PickResult> Pick(VisualWorld&               visualWorld,
                                    const vec2&                screenPosition,
                                    const vector<const Node*>& ignoredNodes,
                                    bool                       elementBoundsOnly) {

    const auto hits = visualWorld.hitTest(screenPosition, {.searchMode = HitTestSearchMode::Closest,
                                                           .elementBoundsOnly = elementBoundsOnly,
                                                           .ignoredNodes = ignoredNodes});

    for (const auto& hit : hits) {

        auto node = hit.node();

        if (!node) {
            continue;
        }

        return App::PickResult {.node = node,
                                     .hitPosition = hit.worldCoordinates(),
                                     .hitNormal = hit.worldNormal()};
    }

    return {};
}

pair<vec3, vec3> CalculateThrowTrajectory(const vec3& cameraPosition,
                                          const vec3& targetPosition,
                                          const vec3& gravity) {

    const float SPAWN_DISTANCE {0.5f};
    const float SPEED {15.0f};
    const float MIN_FLIGHT_TIME {0.25f};
    const float MAX_FLIGHT_TIME {1.5f};

    const vec3  cameraToTarget = targetPosition - cameraPosition;
    const float targetDistance = length(cameraToTarget);

    const vec3 aimDirection = cameraToTarget / targetDistance;

    const vec3 spawnPosition =
        cameraPosition + aimDirection * math::min(SPAWN_DISTANCE, targetDistance * 0.25f);

    const vec3 displacement = targetPosition - spawnPosition;

    const float flightTime = math::clamp(length(displacement) / SPEED, MIN_FLIGHT_TIME, MAX_FLIGHT_TIME);

    const vec3 velocity = displacement / flightTime - 0.5f * gravity * flightTime;

    return {spawnPosition, velocity};
}
