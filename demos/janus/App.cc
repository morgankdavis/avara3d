//
//  App.cc
//  janus
//
//  Created by Morgan Davis on 8/4/26.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#include "App.h"

#include <algorithm>
#include <cmath>

#include "a3d/a3d.h"

#include "Constants.h"
#include "TransientsBuilder.h"

using namespace a3d;
using namespace a3d::math;
using namespace std;

namespace demo::janus {
namespace {

    // [Private Constants]

    const log::Level                  APP_LOG_LEVEL {log::Level::Debug};
    const uvec2                       WINDOW_SIZE {1280, 768};
    const RenderContext::Antialiasing ANTIALIASING {RenderContext::Antialiasing::Msaa2X};
    const double                      TIME_STEP {1.0 / 120.0};
    const std::uint32_t               MAX_CATCH_UP_STEPS {8};

    const u8vec3 ROCK_GRID_SIZE {3, 3, 3};
    const u8vec3 COIN_GRID_SIZE {3, 3, 3};
    const u8vec3 BALL_GRID_SIZE {3, 3, 3};

    // [Private Non-Member Prototypes]

    unique_ptr<VisualWorld>   CreateVisualWorld(RenderContext& context);
    vector<shared_ptr<Node>>  ConfigureEnvironmentNodes(const Node& root);
    void                      ConfigureDynamicsNodes(const Node& root);
    shared_ptr<Node>          CreateGroundNode();
    void                      ConfigureTransientsTracker(ext::TransientsTracker& tracker);
    shared_ptr<Node>          CreateCamera(ext::TurntableCameraController& controller);
    shared_ptr<Node>          CreateOrbWanderers(vector<ext::Wanderer>& wanderers);
    optional<App::PickResult> Pick(const VisualWorld&         visualWorld,
                                   const vec2&                screenPosition,
                                   const vector<const Node*>& ignoredNodes = {},
                                   bool                       elementBoundsOnly = true);
    pair<vec3, vec3>          CalculateThrowTrajectory(const vec3& cameraPosition,
                                                       const vec3& targetPosition,
                                                       const vec3& gravity);

} // namespace

// [Public Lifecycle Functions]

App::App(int argc, char* argv[]):
    Application(argc, argv, APP_LOG_LEVEL),
    _window {nullptr},
    _dynamicsRoot {nullptr},
    _transientsRoot {nullptr},
    _cameraNode {nullptr},
    _cameraController {},
    _hoveredNode {},
    _selection {},
    _action {Action::Drop},
    _dropAction {DropAction::Rocks},
    _throwAction {ThrowAction::Hammer},
    _pokiness {Pokiness::Soft},
    _transientsBuilder {},
    _transientsTracker {ext::TransientsTracker::SweepPolicy::EveryInterval(1.0)},
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

        // create and configure the visual world

        auto visualWorld = CreateVisualWorld(*_window);

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

        // configure the environment nodes

        auto environmentRoot = scene->rootNode()->childNamed("environment");
        for (const auto hoverIgnoreNode : ConfigureEnvironmentNodes(*environmentRoot)) {
            _pickIgnores.push_back({.node = hoverIgnoreNode, .purposes = PickPurpose::Hover});
        }

        // configure the permanent dynamic nodes

        _dynamicsRoot = scene->rootNode()->childNamed("dynamics");
        ConfigureDynamicsNodes(*_dynamicsRoot);
        saveDynamicsTransforms();

        // cache the transient object assets

        auto transientAssetsNode = scene->rootNode()->childNamed("transient_assets");
        _transientsBuilder.init(*transientAssetsNode);
        transientAssetsNode->removeFromParent();

        // create and configure the ground

        scene->rootNode()->addChild(CreateGroundNode());

        // setup ambient lighting

        scene->rootNode()->addChild(Node::LightNode(Light::Ambient(Color {0.075f})));

        // create a transient objects root

        _transientsRoot = Node::NamedNode("transients");
        scene->rootNode()->addChild(_transientsRoot);

        // setup transient tracker groups

        ConfigureTransientsTracker(_transientsTracker);

        // create and configure the camera and camera controller

        _cameraNode = CreateCamera(_cameraController);
        scene->rootNode()->addChild(_cameraNode);
        scene->visualWorld()->pointOfView(_cameraNode);

        // create the wandering orbs

        auto wanderGroupNode = CreateOrbWanderers(_orbWanderers);
        scene->rootNode()->childNamed("environment")->addChild(wanderGroupNode);

        // open the window

        _window->center();
        _window->open();

        return scene;
    }
    catch (const std::exception& e) {
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
        queueAction(result.orbitButtonClick->position);
    }

    if (result.panButtonClick) {
        select(*scene.visualWorld(), result.panButtonClick->position);
    }
}

void App::sceneWillStep(Runner& runner, Scene& scene, const Scene::StepInfo& info) {

    for (auto& wander : _orbWanderers) {
        wander.update(info.deltaTime);
    }
}

void App::sceneDidStep(Runner& runner, Scene& scene, const Scene::StepInfo& info) {

    _transientsTracker.update(info);

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

    const float BACKGROUND_START_ANGLE {radians(210.0f)};
    const float BACKGROUND_ROTATION_SPEED {radians(0.25f)};
    const vec3  BACKGROUND_ROTATION_AXIS {0.5f, 1.0f, 1.0f};

    const float angle =
        BACKGROUND_START_ANGLE + static_cast<float>(_backgroundRotationTime) * BACKGROUND_ROTATION_SPEED;

    if (const auto& current = visualWorld.background()) {
        auto background = *current;
        background.orientation = quaternion(BACKGROUND_ROTATION_AXIS, angle);
        visualWorld.background(background);
    }

    if (_cameraNode) {
        _cameraController.apply(*_cameraNode);
    }

    const bool hovered = drawPanel();

    if (_window->cursorCaptured() || hovered) {
        hover(nullptr);
    }
    else {
        auto& input = static_cast<DesktopInputContext&>(*scene.inputContext());
        hover(visualWorld, input.mousePosition());
    }
}

// [Private Member Functions]

void App::hover(const VisualWorld& visualWorld, const vec2& screenPosition) {

    auto result = Pick(visualWorld, screenPosition, pickIgnoredNodes(PickPurpose::Hover));

    if (result) {
        hover(result->node.lock());
    }
    else {
        hover(nullptr);
    }
}

void App::hover(const shared_ptr<Node>& node) {

    using DebugOptions = Node::DebugOptions;

    if (auto previous = _hoveredNode.lock()) {
        previous->debugOptions(util::bitmask::remove(previous->debugOptions(), DebugOptions::ShowHighlightBox));
    }

    _hoveredNode = node;

    if (node) {
        node->debugOptions(util::bitmask::add(node->debugOptions(), DebugOptions::ShowHighlightBox));
    }
}

void App::select(const VisualWorld& visualWorld, const vec2& screenPosition) {

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

optional<App::PickResult> App::target(const Scene& scene, const vec2& screenPosition) const {

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

        return PickResult {.node = node, .hitPosition = hit.worldCoordinates()};
    }

    return {};
}

void App::queueAction(const vec2& screenPosition) {

    if (runner().simulationPaused()) {
        return;
    }

    auto& scene = App::scene();

    auto actionTarget = target(scene, screenPosition);

    if (!actionTarget) {
        return;
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

    const float POKE_DELTA_SPEED_SOFT {1.0f};
    const float POKE_DELTA_SPEED_HARD {7.5f};
    const float FLIP_DELTA_SPEED {2.5f};
    const float FLIP_DELTA_ANGULAR_SPEED {radians(330.0f)};

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
                    auto rockNodes = _transientsBuilder.rocks(spawnLocation, ROCK_GRID_SIZE, ROCK_GAP);
                    _transientsRoot->addChildren(rockNodes);
                    _transientsTracker.track(rockNodes, "rocks");
                    break;
                }
                case DropAction::Coins: {
                    auto coinNodes = _transientsBuilder.coins(spawnLocation, COIN_GRID_SIZE, COIN_GAP);
                    _transientsRoot->addChildren(coinNodes);
                    _transientsTracker.track(coinNodes, "coins");
                    break;
                }
                case DropAction::Balls: {
                    auto ballNodes = _transientsBuilder.balls(spawnLocation, BALL_GRID_SIZE, BALL_GAP);
                    _transientsRoot->addChildren(ballNodes);
                    _transientsTracker.track(ballNodes, "balls");
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
                    auto hammerNode = _transientsBuilder.hammer(spawnPosition, velocity);
                    _transientsRoot->addChild(hammerNode);
                    _pickIgnores.push_back({.node = hammerNode,
                                            .remainingTime = PROJECTILE_PICK_IGNORE_DURATION});
                    _transientsTracker.track(hammerNode, "hammers");
                    break;
                }
                case ThrowAction::Hula: {
                    auto hulaNode = _transientsBuilder.hula(spawnPosition, velocity);
                    _pickIgnores.push_back({.node = hulaNode,
                                            .remainingTime = PROJECTILE_PICK_IGNORE_DURATION});
                    _transientsRoot->addChild(hulaNode);
                    _transientsTracker.track(hulaNode, "hulas");
                    break;
                }
                case ThrowAction::Duck: {
                    auto duckNode = _transientsBuilder.duck(spawnPosition, velocity);
                    _transientsRoot->addChild(duckNode);
                    _pickIgnores.push_back({.node = duckNode,
                                            .remainingTime = PROJECTILE_PICK_IGNORE_DURATION});
                    _transientsTracker.track(duckNode, "ducks");
                    break;
                }
            }

            break;
        }

        case Action::Poke: {

            auto node = action.target.node.lock();
            if (!node) {
                return;
            }

            auto body = node->physicsBody();

            if (!body || body->type() != PhysicsBody::Type::Dynamic) {
                return;
            }

            switch (_pokiness) {

                case Pokiness::Soft: {
                    const float impulse = body->mass() * POKE_DELTA_SPEED_SOFT;
                    body->applyForce(action.rayDirection * impulse, action.target.hitPosition, true);
                    break;
                }

                case Pokiness::Hard: {
                    const float impulse = body->mass() * POKE_DELTA_SPEED_HARD;
                    body->applyForce(action.rayDirection * impulse, action.target.hitPosition, true);
                    break;
                }

                case Pokiness::Flip: {

                    body->applyForce(vec3 {0.0f, body->mass() * FLIP_DELTA_SPEED, 0.0f}, true);

                    const vec3 worldUp {0.0f, 1.0f, 0.0f};
                    const vec3 worldCOM = vec3 {node->worldTransform() * vec4 {body->centerOfMass(), 1.0f}};

                    const vec3 lever = action.target.hitPosition - worldCOM;
                    vec3 flipAxis = cross(lever, worldUp);
                    if (length(flipAxis) <= F32_COMPARE_EPSILON) {
                        flipAxis = cross(action.rayDirection, worldUp);
                    }
                    flipAxis = normalize(flipAxis);

                    const quat orientation = node->worldOrientation();
                    const vec3 deltaOmegaWorld = flipAxis * FLIP_DELTA_ANGULAR_SPEED;
                    const vec3 deltaOmegaLocal = inverse(orientation) * deltaOmegaWorld;
                    const vec3 angularImpulseLocal = body->momentOfInertia() * deltaOmegaLocal;
                    const vec3 angularImpulseWorld = orientation * angularImpulseLocal;

                    body->applyTorque(angularImpulseWorld, true);

                    break;
                }
            }

            break;
        }
    }
}

vector<const Node*> App::pickIgnoredNodes(PickPurpose purpose) const {

    vector<const Node*> nodes;
    nodes.reserve(_pickIgnores.size());

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

void App::restoreDynamics() const {

    for (const auto& state : _dynamicsTransforms) {

        state.node->transform(state.transform);

        auto body = state.node->physicsBody();
        if (body && body->type() == PhysicsBody::Type::Dynamic) {
            body->linearVelocity(vec3 {0.0f});
            body->angularVelocity(vec3 {0.0f});
            body->clearForces();
        }
    }
}

void App::reset() {

    select({});

    _transientsTracker.clear();

    _transientsRoot->removeFromParent();
    _transientsRoot = Node::NamedNode("transients");
    scene().rootNode()->addChild(_transientsRoot);

    restoreDynamics();

    erase_if(_pickIgnores, [](const PickIgnore& ignore) {
        return ignore.remainingTime.has_value();
    });

    scene().physicsWorld()->gravity(GRAVITY_EARTH);

    runner().resetSimulation();

    if (runner().simulationPaused()) {
        runner().simulationPaused(false);
    }

    ++_simulationGeneration;
}

namespace {

    // [Private Non-Member Functions]

    unique_ptr<VisualWorld> CreateVisualWorld(RenderContext& context) {

        auto world = make_unique<VisualWorld>(context);

        world->background(Background {make_shared<Texture>(std::move(util::fs::CubeImageAt("nebula.webp")))});

        world->surface(SphereSurface {.center = {0.0f, -5000.0f, 0.0f}, .radius = 5000.0f});

        auto gridMinor = Ground::Procedural::GridComponent {.color = {0.5f, 0.5f, 0.5f, 0.25f},
                                                            .spacing = 1.0f,
                                                            .lineWidthPixels = 1.0f,
                                                            .reliefStrength = -0.125f};

        auto gridMajor = Ground::Procedural::GridComponent {.color = {0.75f, 0.75f, 0.75f, 0.25f},
                                                            .spacing = 10.0f,
                                                            .lineWidthPixels = 1.0f,
                                                            .reliefStrength = -0.125f};

        auto grid = Ground::Procedural::Grid {.color = Color::DarkGray(),
                                              .minor = gridMinor,
                                              .major = gridMajor,
                                              .specularIntensity = 0.05f,
                                              .specularExponent = 8.0f};

        auto radialFade = Ground::RadialFade {.color = {0.005f, 0.005f, 0.005f, 1.0f},
                                              .center = {0.0f, 0.0f},
                                              .startDistance = 10.0f,
                                              .endDistance = 100.0f};

        auto horizonHaze =
            Ground::HorizonHaze {.color = {0.2f, 0.2f, 0.2f, 0.35f}, .angularWidth = math::radians(4.0f)};

        world->ground(Ground {.fill = Ground::Procedural {.content = grid},
                              .radialFade = radialFade,
                              .horizonHaze = horizonHaze});

        auto atmosphericHaze = Atmosphere::Haze {.color = {0.10f, 0.11f, 0.12f, 0.3f}, .density = 0.5f};

        auto limbGlow = Atmosphere::LimbGlow {.color = {0.30f, 0.38f, 0.48f, 0.5f}, .intensity = 0.25f};

        world->atmosphere(Atmosphere {.scaleHeight = 0.9f, .haze = atmosphericHaze, .limbGlow = limbGlow});

        return world;
    }

    vector<shared_ptr<Node>> ConfigureEnvironmentNodes(const Node& root) {

        root.childNamed("environment.phys")->hidden(true);

        vector<shared_ptr<Node>> hoverIgnores;

        if (auto node = root.childNamed("evora")) {

            auto physNode = root.childNamed("evora.phys", true);
            auto shape = PhysicsShape::ConcavePolyhedronShape(physNode->mesh());
            auto body = PhysicsBody::StaticBody(shape);

            body->friction(STONE_FRICTION);
            body->restitution(STONE_RESTITUTION);

            node->physicsBody(std::move(body));

            hoverIgnores.push_back(node);
        }

        if (auto node = root.childNamed("lion")) {

            auto physNode = root.childNamed("lion.phys", true);
            auto shape = PhysicsShape::ConcavePolyhedronShape(physNode->mesh());
            auto body = PhysicsBody::StaticBody(shape);

            body->friction(STONE_FRICTION);
            body->restitution(STONE_RESTITUTION);

            node->physicsBody(std::move(body));

            hoverIgnores.push_back(node);
        }

        return hoverIgnores;
    }

    void ConfigureDynamicsNodes(const Node& root) {

        root.childNamed("dynamics.phys")->hidden(true);

        if (auto node = root.childNamed("janus")) {

            auto body = PhysicsBody::DynamicBody();

            body->mass(20.0f);
            body->friction(STONE_FRICTION);
            body->restitution(STONE_RESTITUTION);

            node->physicsBody(std::move(body));
        }

        if (auto node = root.childNamed("plinth_janus")) {

            auto body = PhysicsBody::DynamicBody();

            body->mass(40.0f);
            body->friction(STONE_FRICTION);
            body->restitution(STONE_RESTITUTION);

            node->physicsBody(std::move(body));
        }

        if (auto node = root.childNamed("diana")) {

            auto physNode = root.childNamed("diana.phys", true);
            auto shape = PhysicsShape::ConcavePolyhedronShape(physNode->mesh());
            auto body = PhysicsBody::DynamicBody(shape);

            body->mass(50.0f);
            body->friction(STONE_FRICTION);
            body->restitution(STONE_RESTITUTION);
            body->angularSleepingThreshold(0.25f);

            auto extent = node->mesh()->localExtent();
            body->centerOfMass(body->centerOfMass() + extent * vec3 {0.0f, 0.0f, -0.025f});

            node->physicsBody(std::move(body));
        }

        if (auto node = root.childNamed("augustus")) {

            auto physNode = root.childNamed("augustus.phys", true);
            auto shape = PhysicsShape::ConcavePolyhedronShape(physNode->mesh());
            auto body = PhysicsBody::DynamicBody(shape);

            body->mass(55.0f);
            body->friction(STONE_FRICTION);
            body->restitution(STONE_RESTITUTION);
            body->angularSleepingThreshold(0.25f);

            auto extent = node->mesh()->localExtent();
            body->centerOfMass(body->centerOfMass() + extent * vec3 {0.1f, 0.05f, -0.15f});

            node->physicsBody(std::move(body));
        }

        if (auto node = root.childNamed("teapot")) {

            auto physNode = root.childNamed("teapot.phys", true);
            auto shape = PhysicsShape::ConcavePolyhedronShape(physNode->mesh());
            auto body = PhysicsBody::DynamicBody(shape);

            body->mass(1.0f);
            body->friction(0.5f);
            body->restitution(0.2f);
            body->rollingFriction(0.05f);
            body->spinningFriction(0.05f);
            body->angularSleepingThreshold(0.25f);

            node->physicsBody(std::move(body));
        }

        if (auto node = root.childNamed("plinth_teapot")) {

            auto body = PhysicsBody::DynamicBody();

            body->mass(30.0f);
            body->friction(STONE_FRICTION);
            body->restitution(STONE_RESTITUTION);

            body->angularSleepingThreshold(0.25f);

            node->physicsBody(std::move(body));
        }
    }

    shared_ptr<Node> CreateGroundNode() {

        auto node = Node::NamedNode("ground");

        node->orientation(quaternion({1.0f, 0.0f, 0.0f}, radians(-90.0f)));

        auto shape = make_shared<InfinitePlanePhysicsShape>();
        auto body = PhysicsBody::StaticBody(shape);

        body->friction(STONE_FRICTION);
        body->restitution(0.25f);

        node->physicsBody(std::move(body));

        return node;
    }

    void ConfigureTransientsTracker(ext::TransientsTracker& tracker) {

        tracker.groupPolicy("rocks",
                            {.maxCount = (ROCK_GRID_SIZE.x * ROCK_GRID_SIZE.y * ROCK_GRID_SIZE.z) * 1.5});
        tracker.groupPolicy("coins",
                            {.maxCount = (COIN_GRID_SIZE.x * COIN_GRID_SIZE.y * COIN_GRID_SIZE.z) * 2.5});
        tracker.groupPolicy("balls",
                            {.maxCount = (BALL_GRID_SIZE.x * BALL_GRID_SIZE.y * BALL_GRID_SIZE.z) * 2.5});
        tracker.groupPolicy("hammers",
                            {.maxCount = 10,
                             .distanceLimit = ext::TransientsTracker::DistanceLimit {.radius = 100.0f}});
        tracker.groupPolicy("hulas",
                            {.maxCount = 10,
                             .distanceLimit = ext::TransientsTracker::DistanceLimit {.radius = 100.0f}});
        tracker.groupPolicy("ducks",
                            {.maxCount = 10,
                             .distanceLimit = ext::TransientsTracker::DistanceLimit {.radius = 100.0f}});
    }

    shared_ptr<Node> CreateCamera(ext::TurntableCameraController& controller) {

        auto camera = make_shared<PerspectiveCamera>(0.1f, 1000.0f, radians(45.0f));
        auto cameraNode = Node::CameraNode(camera);
        cameraNode->name("Camera");

        auto cameraConfig = controller.config();
        cameraConfig.controls.orbitButton = DesktopInputContext::MouseButton::One;
        cameraConfig.controls.panButton = DesktopInputContext::MouseButton::Two;
        cameraConfig.invertPitch = true;
        cameraConfig.minDistance = 0.5f;
        cameraConfig.maxDistance = 100.0f;
        controller.config(cameraConfig);

        controller.view({.target = vec3 {-0.17735787f, 4.7262487f, 0.43490386f},
                         .yaw = radians(-12.607612f),
                         .pitch = radians(-4.3214903f),
                         .distance = 17.4275f});

        return cameraNode;
    }

    shared_ptr<Node> CreateOrbWanderers(vector<ext::Wanderer>& wanderers) {

        const vec3 ORB_GROUP_POSITION {-4.5f, 10.0f, -2.5f};

        const vec3 ORB_POSITION_MIN {-1.0f, -0.5f, -1.0f};
        const vec3 ORB_POSITION_MAX {1.0f, 0.5f, 1.0f};

        const vec3   ORB_WANDER_EXTENTS {1.5f, 0.75f, 1.5f};
        const size_t ORB_COUNT {4};

        auto orbGroup = Node::NamedNode("orbs");
        orbGroup->position(ORB_GROUP_POSITION);

        auto orbMaterial = Material::EmissionMaterial(Color::White());
        auto orbMesh = Sphere::Mesh(0.1f, 3, orbMaterial);

        wanderers.reserve(ORB_COUNT);

        for (size_t i = 0; i < ORB_COUNT; ++i) {
            auto light = make_shared<PointLight>(Color::White());
            light->attenuation(Attenuation::FromRange(3.0f, 0.02f));

            auto orb = Node::LightNode(light);
            orb->name(std::format("orb {}", i + 1));
            orb->mesh(orbMesh);

            orb->position(math::uniform_linear(ORB_POSITION_MIN, ORB_POSITION_MAX));

            orb->physicsBody(PhysicsBody::KinematicBody());

            orbGroup->addChild(orb);

            ext::Wanderer::Config config {.halfExtents = ORB_WANDER_EXTENTS,
                                          .segmentDuration = math::uniform_linear(5.0f, 7.0f),
                                          .seed = 1000u + static_cast<uint32_t>(i)};
            wanderers.push_back(ext::Wanderer(orb, config));
        }

        return orbGroup;
    }

    optional<App::PickResult> Pick(const VisualWorld&         visualWorld,
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

            return App::PickResult {.node = node, .hitPosition = hit.worldCoordinates()};
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

} // namespace
} // namespace demo::janus
