//
//  App.cc
//  janus
//
//  Created by Morgan Davis on 8/4/26.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#include "App.h"
#include "App.h"
#include "App.h"

#include <algorithm>
#include <cmath>

#include "a3d/a3d.h"

using namespace a3d;
using namespace a3d::math;
using namespace demo::janus;
using namespace std;

/// Private Constants ///

const Log::Level                  APP_LOG_LEVEL {Log::Level::Debug};
const uvec2                       WINDOW_SIZE {1280, 768};
const RenderContext::Antialiasing ANTIALIASING {RenderContext::Antialiasing::Msaa4X};
const float                       TIME_STEP {1.0 / 120.0};
const std::uint32_t               MAX_CATCH_UP_STEPS {8};
const float                       BACKGROUND_ROTATION_SPEED {radians(0.5f)};
const vec3                        BACKGROUND_ROTATION_AXIS {0.5f, 1.0f, 1.0f};
const vec3                        GRAVITY_EARTH {0.0f, -9.807f, 0.0f};
const vec3                        GRAVITY_MOON {0.0f, -1.62f, 0.0f};
const vec3                        GRAVITY_ZERO {0.0f, 0.0f, 0.0f};
const float                       CURSOR_MARKER_RADIUS {0.1f};
const float                       DROP_HEIGHT {5.0f};
const vec3                        DROP_BOX_SIZE {0.25f, 0.25f, 0.25f};
const u8vec3                      DROP_STACK_SIZE {3, 3, 3};
const float                       DROP_PADDING {0.025f};
const float                       THROW_SPAWN_DISTANCE {0.5f};
const float                       THROW_SPEED {15.0f};
const float                       THROW_MIN_FLIGHT_TIME {0.25f};
const float                       THROW_MAX_FLIGHT_TIME {1.5f};
constexpr float                   POKE_IMPULSE = 10.0f;
const double                      PROJECTILE_PICK_IGNORE_DURATION {0.5};

/// Private Static Non-Member Prototypes ///

static shared_ptr<Node>          MakeSimulationRoot();
static optional<App::PickResult> Pick(VisualWorld&               visualWorld,
                                      const vec2&                screenPosition,
                                      const vector<const Node*>& ignoredNodes = {});
static optional<App::PickResult> FindActionTarget(Scene&                     scene,
                                                  const vec2&                screenPosition,
                                                  const vector<const Node*>& ignoredNodes = {});
static shared_ptr<Node>          ThrowRing(Node& parent, const vec3& location, const vec3& velocity);
static shared_ptr<Node>          ThrowDuck(Node& parent, const vec3& location, const vec3& velocity);
static vector<shared_ptr<Node>>  DropBoxs(Node&             parent,
                                          const vec3&       location,
                                          const vec3&       boxSize,
                                          const u8vec3&     stackSize,
                                          float             padding,
                                          shared_ptr<Color> color);
static bool IsIgnored(const shared_ptr<Node>& node, const vector<const Node*>& ignoredNodes);

/// Public Lifecycle Functions ///

App::App(int argc, char* argv[]):
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
    _dropAction {DropAction::Blocks},
    _throwAction {ThrowAction::Ring},
    _pokiness {Pokiness::Hard},
    _transients {ext::TransientNodeRegistry::SweepPolicy::EveryInterval(1.0)},
    _backgroundRotationTime {0.0},
    _orbWanders {},
    _pickIgnores {},
    _pendingReset {false} {}

App::~App() = default;

/// Application Protected Member Functions ///

std::unique_ptr<Scene> App::init() {
    try {
        // create and configure the window

        _window = make_unique<Window>(RenderContext::RenderingApi::OpenGL, *util::fs::ExecutableName(),
                                      WINDOW_SIZE, false, true, ANTIALIASING);

        // create and configure the visual world

        auto visualWorld = make_unique<VisualWorld>(*_window);

        visualWorld->background(Background {
            make_shared<Texture>(std::move(util::fs::CubeImageNamed("nebula", "png")))});

        //visualWorld->fog(Fog {.color = Color::Black(), .startDistance = 30.0f, .endDistance = 150.0f});

        // visualWorld->atmosphericHaze(AtmosphericHaze {
        //     .color = make_shared<Color>(vec4 {0.35f, 0.4f, 0.45f, 1.0f}),
        //     .baseHeight = 0.0f,
        //     .density = 0.08f,
        //     .heightFalloff = 0.25f,
        // });

        // visualWorld->atmosphericHaze(AtmosphericHaze {
        //     .color = make_shared<Color>(vec4 {0.35f, 0.4f, 0.45f, 1.0f}),
        //     .baseHeight = 0.0f,
        //     .density = 0.01f,
        //     .heightFalloff = 0.30,
        // });

        visualWorld->atmosphericHaze(AtmosphericHaze {
            .color = make_shared<Color>(vec4 {0.16f, 0.19f, 0.22f, 0.25f}),
            .baseHeight = 0.0f,
            .density = 0.018f,
            .heightFalloff = 0.30f,
        });

        visualWorld->infiniteGround(InfiniteGround {
            .color = Color::DarkGray(),
            // .color = make_shared<Color>(.2f),
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
            // .horizonHaze =
            //     InfiniteGround::HorizonHaze {
            //         .color = make_shared<Color>(vec4 {0.075f, 0.075f, 0.075f, 0.65f}),
            //         .angularWidthDegrees = 4.5f,
            //     },
            .specularIntensity = 0.05f,
            .specularExponent = 8.0f,
        });

        // create the physics world

        auto physicsWorld = make_unique<PhysicsWorld>();

        // create the scene

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

        _transients.groupPolicy("box",
                                {.maxCount = {100},
                                 /*.distanceLimit =
                                     ext::TransientNodeRegistry::DistanceLimit {.center = {0.0f, 0.0f, 0.0f},
                                                                                .radius = 25.0f}*/});

        _transients.groupPolicy("projectile", {
                                                  //.maxAge = 15.0,
                                                  .maxCount = {100},
                                                  .distanceLimit =
                                                      ext::TransientNodeRegistry::DistanceLimit {
                                                          .center = {0.0f, 0.0f, 0.0f},
                                                          .radius = 100.0f,
                                                      },
                                              });

        // create and configure the camera and camera controller

        auto camera = make_shared<PerspectiveCamera>(0.1f, 1000.0f, radians(45.0f));
        _cameraNode = Node::CameraNode(camera);
        _cameraNode->name("Turntable camera");
        scene->rootNode()->addChild(_cameraNode);
        scene->visualWorld()->pointOfView(_cameraNode);

        auto cameraConfig = _cameraController.config();
        cameraConfig.controls.orbitButton = DesktopInputContext::MouseButton::One;
        cameraConfig.controls.panButton = DesktopInputContext::MouseButton::Two;
        //cameraConfig.minPitch = math::radians(0.0f);
        cameraConfig.invertPitch = true;
        cameraConfig.minDistance = 1.0f;
        cameraConfig.maxDistance = 100.0f;
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

        // create the action target marker

        {
            auto material = Material::EmissionMaterial(Color::Yellow());
            material->depthTestEnabled(false);
            material->depthWriteEnabled(false);

            auto mesh = Sphere::Mesh(CURSOR_MARKER_RADIUS, 8, material);

            _cursorMarker = Node::MeshNode(mesh);
            _cursorMarker->name("Marker");
            _cursorMarker->renderOrder(100);
            _cursorMarker->hidden(true);

            scene->rootNode()->addChild(_cursorMarker);
        }

        // wandering lights

        {

            const vec3 ORB_GROUP_POSITION {-8.0f, 2.5f, -6.0f};

            const vec3 ORB_POSITION_MIN {-1.0f, -0.5f, -1.0f};
            const vec3 ORB_POSITION_MAX {1.0f, 0.5f, 1.0f};

            const vec3   ORB_WANDER_EXTENTS {1.5f, 0.75f, 1.5f};
            const size_t ORB_COUNT {4};

            auto orbGroup = Node::NamedNode("Orbs");
            orbGroup->position(ORB_GROUP_POSITION);
            scene->rootNode()->addChild(orbGroup);

            auto orbMaterial = make_shared<Material>();
            orbMaterial->emission(Color::White());

            auto orbMesh = Sphere::Mesh(0.1, 3, orbMaterial);

            _orbWanders.reserve(ORB_COUNT);

            for (size_t i = 0; i < ORB_COUNT; ++i) {
                auto light = make_shared<PointLight>(Color::White());
                // light->attenuation(Attenuation {
                //     .quadratic = 0.5f
                // });
                //light->attenuation(Attenuation::FromRange(3.0f, 0.02f));

                auto orb = Node::LightNode(light);
                orb->name(std::format("Orb {}", i + 1));
                orb->mesh(orbMesh);

                orb->position({math::uniform_linear(ORB_POSITION_MIN.x, ORB_POSITION_MAX.x),
                               math::uniform_linear(ORB_POSITION_MIN.y, ORB_POSITION_MAX.y),
                               math::uniform_linear(ORB_POSITION_MIN.z, ORB_POSITION_MAX.z)});

                orb->physicsBody(PhysicsBody::KinematicBody());
                // TODO: make sphere physics shape

                orbGroup->addChild(orb);

                _orbWanders.push_back(make_unique<
                                      ext::Wander>(orb,
                                                   ext::Wander::Config {.halfExtents = ORB_WANDER_EXTENTS,
                                                                        .segmentDuration =
                                                                            math::uniform_linear(5.0f, 7.0f),
                                                                        .seed =
                                                                            1000u + static_cast<uint32_t>(i)}));
            }
        }

        // {
        //     auto mesh = util::fs::MeshNamed("teapot/teapot", Mesh::ImportOptions::None);
        //
        //     // shared_ptr<Image> marbleImage = util::fs::ImageNamed("marble1", std::string{"jpg"});
        //     //
        //     //
        //     // if (marbleImage) {
        //     //     log::app::i()("marbleImage: {} x {}",
        //     //                   marbleImage->width(),
        //     //                   marbleImage->height());
        //     // }
        //     //
        //     // A3D_ASSERT(marbleImage);
        //     //
        //     // auto sampler = make_shared<Sampler>();
        //     // sampler->wrapS(Sampler::WrapMode::Repeat);
        //     // sampler->wrapT(Sampler::WrapMode::Repeat);
        //     //
        //     // auto marbleTexture = make_shared<Texture>(marbleImage, sampler);
        //     //
        //     // //auto marbleMaterial = make_shared<Material>(monostate {}, marbleTexture, Color::White());
        //     // auto marbleMaterial = Material::DiffuseMaterial(marbleTexture);
        //     //
        //     // marbleMaterial->specularExponent(64.0f);
        //     // marbleMaterial->uvScale(1.0f);
        //     // // mesh->addMaterial(marbleMaterial);
        //     // mesh->replaceMaterial(0, marbleMaterial);
        //     //
        //
        //     auto marbleMaterial2 = make_shared<Material>(monostate {}, Color::Red(), Color::White());
        //
        //     marbleMaterial2->specularExponent(64.0f);
        //
        //     mesh->replaceMaterial(0, marbleMaterial2);
        //
        //     auto teapotNode = Node::MeshNode(mesh);
        //     teapotNode->position({5.0f, 5.0f, 5.0f});
        //     scene->rootNode()->addChild(teapotNode);
        //
        //     auto& element = *mesh->elements().at(0);
        //
        //     auto bytes = element.vertexBytes();
        //     auto verts = reinterpret_cast<const VertexPNT*>(bytes.data());
        //
        //     float minU = std::numeric_limits<float>::max();
        //     float minV = std::numeric_limits<float>::max();
        //     float maxU = std::numeric_limits<float>::lowest();
        //     float maxV = std::numeric_limits<float>::lowest();
        //
        //     for (uint32_t i = 0; i < element.vertexCount(); ++i) {
        //         const auto& uv = verts[i].texCoord;
        //
        //         minU = std::min(minU, uv.x);
        //         minV = std::min(minV, uv.y);
        //         maxU = std::max(maxU, uv.x);
        //         maxV = std::max(maxV, uv.y);
        //     }
        //
        //     log::app::i()("teapot UV range: ({}, {}) - ({}, {})", minU, minV, maxU, maxV);
        // }

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

void App::hostUpdate(Runner& runner, Scene&, const Runner::UpdateInfo&) {

    if (_pendingReset) {
        _pendingReset = false;

        reset();

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

    // ! TEMPORARY !
    if (input.keyPressed(Key::Escape)) {
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

void App::sceneWillStep(Runner& runner, Scene& scene, const Scene::StepInfo& info) {

    for (const auto& wander : _orbWanders) {
        wander->update(info.deltaTime);
    }
}

void App::sceneDidStep(Runner& runner, Scene& scene, const Scene::StepInfo& info) {

    _transients.update(info);

    for (auto& ignore : _pickIgnores) {
        ignore.remainingTime -= info.deltaTime;
    }

    std::erase_if(_pickIgnores, [](const PickIgnore& ignore) {
        return ignore.remainingTime <= 0.0 || ignore.node.expired();
    });
}

void App::frameDidBegin(Runner&                        runner,
                        Scene&                         scene,
                        VisualWorld&                   visualWorld,
                        const VisualWorld::RenderInfo& info) {

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

    bool hovered = drawPanel();

    if (_cursorMarker) {

        if (_window->cursorCaptured() || hovered) {

            hover(nullptr);

            _actionTarget.reset();
            _cursorMarker->hidden(true);
        }
        else {

            auto& input = static_cast<DesktopInputContext&>(*scene.inputContext());

            vector<const Node*> ignoredNodes;
            ignoredNodes.reserve(_pickIgnores.size() + 1);
            ignoredNodes.push_back(_cursorMarker.get());
            for (const auto& entry : _pickIgnores) {
                if (auto node = entry.node.lock()) {
                    ignoredNodes.push_back(node.get());
                }
            }
            _actionTarget = FindActionTarget(scene, input.mousePosition(), ignoredNodes);

            if (_actionTarget) {

                hover(_actionTarget->node.lock());

                _cursorMarker->position(_actionTarget->worldHitPosition);
                _cursorMarker->hidden(false);
            }
            else {

                hover(nullptr);

                _cursorMarker->hidden(true);
            }
        }
    }

    _window->cursorHidden(!_cursorMarker->hidden());
}

/// Private Member Functions ///

bool App::drawPanel() {

    auto& runner = App::runner();
    auto& scene = App::scene();
    auto& visualWorld = *scene.visualWorld();
    auto& physicsWorld = *scene.physicsWorld();

    ui::Panel panel("controls", {
                                    .width = 180.0f,
                                    .margin = 12.0f,
                                });

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

    //panel.spacer(12.0f);
    panel.section("environment");

    // if (auto physicsWorld = scene.physicsWorld()) {
    const auto gravity = physicsWorld.gravity();

    //panel.value("gravity", std::format("{:.1f}, {:.1f}, {:.1f}", gravity.x, gravity.y, gravity.z));
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
    // }

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
            panel.row(2);
            if (panel.option("Blocks", _dropAction == DropAction::Blocks)) {
                _dropAction = DropAction::Blocks;
            }
            if (panel.option("Balls", _dropAction == DropAction::Balls)) {
                _dropAction = DropAction::Balls;
            }
            // if (panel.option("Mesh", _dropAction == DropAction::ChainMesh)) {
            //     _dropAction = DropAction::ChainMesh;
            // }
            break;
        case Action::Throw:
            panel.row(2);
            if (panel.option("Ring", _throwAction == ThrowAction::Ring)) {
                _throwAction = ThrowAction::Ring;
            }
            if (panel.option("Duck", _throwAction == ThrowAction::Duck)) {
                _throwAction = ThrowAction::Duck;
            }
            // if (panel.option("Fruit", _throwAction == ThrowAction::Fruit)) {
            //     _throwAction = ThrowAction::Fruit;
            // }
            break;
        case Action::Poke:
            panel.row(2);
            if (panel.option("Hard", _pokiness == Pokiness::Hard)) {
                _pokiness = Pokiness::Hard;
            }
            if (panel.option("Harder", _pokiness == Pokiness::Harder)) {
                _pokiness = Pokiness::Harder;
            }
            // if (panel.option("Ouch", _pokiness == Pokiness::Ouch)) {
            //     _pokiness = Pokiness::Ouch;
            // }
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

            if (auto physicsWorld = scene.physicsWorld()) {
                const auto contacts = physicsWorld->contactTest(*body);
                panel.value("contacts", std::format("{}", contacts.size()));
            }

            panel.value("mass", std::format("{:.1f}", body->mass()));

            panel.spacer(6.0f);

            panel.value("velocity", formatVec3(body->linearVelocity()));
            panel.value("angular", formatVec3(body->angularVelocity()));

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

    return panel.hovered();
}

void App::hover(shared_ptr<Node> node) {

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

void App::select(optional<PickResult> pickResult) {

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

void App::queueAction(const vec2& screenPosition) {

    auto& scene = App::scene();

    auto target = FindActionTarget(scene, screenPosition, {_cursorMarker.get()});

    _actionTarget = target;

    if (!target) {

        if (_cursorMarker) {
            _cursorMarker->hidden(true);
        }

        return;
    }

    if (_cursorMarker) {
        _cursorMarker->position(target->worldHitPosition);
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

    PendingAction pendingAction {
        .action = _action,
        .target = *target,
        .simulationRoot = _simulationRoot,
        .cameraPosition = _cameraNode->worldPosition(),
        .rayDirection = normalize(ray),
    };

    queueScenePreStepCommand([this, pendingAction = std::move(pendingAction)](Scene& scene) {
        performAction(pendingAction);
    });
}

void App::performAction(const PendingAction& action) {

    auto& scene = App::scene();

    auto simulationRoot = action.simulationRoot.lock();

    // The simulation may have been reset while this action was waiting
    // for a simulation-step boundary.
    if (!simulationRoot || simulationRoot != _simulationRoot) {
        return;
    }

    switch (action.action) {

        case Action::Drop: {

            const vec3 spawnLocation = action.target.worldHitPosition + vec3 {0.0f, DROP_HEIGHT, 0.0f};

            auto boxes = DropBoxs(*simulationRoot, spawnLocation, DROP_BOX_SIZE, DROP_STACK_SIZE, DROP_PADDING,
                                  Color::White());

            _transients.track(boxes, "box");

            break;
        }

        case Action::Throw: {

            auto physicsWorld = scene.physicsWorld();

            if (!physicsWorld) {
                return;
            }

            const vec3  cameraPosition = action.cameraPosition;
            const vec3  targetPosition = action.target.worldHitPosition;
            const vec3  cameraToTarget = targetPosition - cameraPosition;
            const float targetDistance = length(cameraToTarget);

            if (targetDistance <= F32_COMPARE_EPSILON) {
                return;
            }

            const vec3 aimDirection = cameraToTarget / targetDistance;
            const vec3 spawnPosition =
                cameraPosition + aimDirection * math::min(THROW_SPAWN_DISTANCE, targetDistance * 0.25f);
            const vec3  displacement = targetPosition - spawnPosition;
            const float flightTime =
                math::clamp(length(displacement) / THROW_SPEED, THROW_MIN_FLIGHT_TIME, THROW_MAX_FLIGHT_TIME);
            const vec3 gravity = physicsWorld->gravity();
            const vec3 velocity = displacement / flightTime - 0.5f * gravity * flightTime;

            // auto projectile = ThrowRing(*simulationRoot, spawnPosition, velocity);
            auto projectile = ThrowDuck(*simulationRoot, spawnPosition, velocity);
            _pickIgnores.push_back({
                .node = projectile,
                .remainingTime = PROJECTILE_PICK_IGNORE_DURATION,
            });
            _transients.track(projectile, "projectile");

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

            body->applyForce(action.rayDirection * POKE_IMPULSE, action.target.worldHitPosition, true);

            break;
        }
    }
}

void App::reset() {

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
}

/// Private Static Non-Member Functions ///

shared_ptr<Node> MakeSimulationRoot() {
    auto root = Node::NamedNode("Simulation root");

    // janus

    {
        static auto mesh = [] {
            constexpr float HEIGHT = 1.0f;
            auto            mesh = util::fs::MeshNamed("janus_lod/janus_lod");
            const float     scaleFactor = HEIGHT / mesh->localExtent().y;
            mesh->burnTransform(math::scale(mat4(1.0f), vec3(scaleFactor)), true);
            return mesh;
        }();

        auto node = Node::MeshNode(mesh);
        node->name("Janus");
        node->position({-1.5f, 0.0f, 0.0f});

        static auto shape = make_shared<PhysicsShape>(PhysicsShape::Type::ConvexHull, mesh);
        auto        body = make_unique<PhysicsBody>(PhysicsBody::Type::Dynamic, shape);
        body->mass(10.0f);
        body->friction(0.6f);
        body->restitution(0.15f);
        body->linearDamping(0.03f);
        body->angularDamping(0.05f);
        node->physicsBody(std::move(body));

        root->addChild(node);
    }

    // angel

    {
        static auto mesh = [] {
            constexpr float HEIGHT = 2.0f;
            auto            mesh = util::fs::MeshNamed("aniel_lod/aniel_lod");
            const float     scaleFactor = HEIGHT / mesh->localExtent().y;
            mesh->burnTransform(math::scale(mat4(1.0f), vec3(scaleFactor)), true);
            return mesh;
        }();

        auto node = Node::MeshNode(mesh);
        node->name("Angel");
        node->rotation({0.0f, 1.0f, 0.0f}, radians(180.0f));
        node->position({0.0, 0.0f, 0.0f});

        static auto shape = make_shared<PhysicsShape>(PhysicsShape::Type::ConcavePolyhedron, mesh);
        auto        body = make_unique<PhysicsBody>(PhysicsBody::Type::Static, shape);
        node->physicsBody(std::move(body));

        root->addChild(node);
    }

    // teapot

    {
        static auto mesh = [] {
            constexpr float HEIGHT = 0.35f;
            auto            mesh = util::fs::MeshNamed("teapot/teapot");
            const float     teapotScale = HEIGHT / mesh->localExtent().y;
            mesh->burnTransform(math::scale(mat4(1.0f), vec3(teapotScale)), true);
            mesh->replaceMaterial(0, Material::DiffuseMaterial(Color::DarkGray()));
            return mesh;
        }();

        auto node = Node::MeshNode(mesh);
        node->name("Teapot");
        node->position({1.5f, 2.5f - mesh->localAABB().min.y, 0.0f});

        static auto shape = make_shared<PhysicsShape>(PhysicsShape::Type::ConvexHull, mesh);
        auto        body = make_unique<PhysicsBody>(PhysicsBody::Type::Dynamic, shape);
        body->mass(1.5f);
        body->friction(0.6f);
        body->restitution(0.15f);
        body->linearDamping(0.03f);
        body->angularDamping(0.05f);
        node->physicsBody(std::move(body));

        root->addChild(node);
    }

    return root;
}

optional<App::PickResult> Pick(VisualWorld&               visualWorld,
                               const vec2&                screenPosition,
                               const vector<const Node*>& ignoredNodes) {

    const auto hits = visualWorld.hitTest(screenPosition, {.searchMode = HitTestSearchMode::All});

    for (const auto& hit : hits) {

        auto node = hit.node();

        if (!node) {
            continue;
        }

        if (IsIgnored(node, ignoredNodes)) {
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

optional<App::PickResult> FindActionTarget(Scene&                     scene,
                                           const vec2&                screenPosition,
                                           const vector<const Node*>& ignoredNodes) {

    auto visualWorld = scene.visualWorld();

    if (auto result = Pick(*visualWorld, screenPosition, ignoredNodes)) {
        return result;
    }

    const vec3 from = visualWorld->unprojectPoint({
        screenPosition.x,
        screenPosition.y,
        0.0f,
    });

    const vec3 to = visualWorld->unprojectPoint({
        screenPosition.x,
        screenPosition.y,
        1.0f,
    });

    const auto hits = scene.physicsWorld()->rayTest(from, to);

    for (const auto& hit : hits) {

        auto node = hit.node();

        if (IsIgnored(node, ignoredNodes)) {
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

shared_ptr<Node> ThrowRing(Node& parent, const vec3& location, const vec3& velocity) {

    static auto mesh = [] {
        // auto material = Material::DiffuseMaterial(olor::LightGray());
        // material->specular(Color::White());
        // material->specularExponent(64.0f);

        auto material = Material::EmissionMaterial(Color::White());

        return Torus::Mesh(0.45f, 0.5f, 12, 32, material);
    }();

    static const auto extent = mesh->localExtent();

    auto node = Node::MeshNode(mesh);
    node->name("Ring");
    node->position(location);

    const vec3 up {0.0f, 1.0f, 0.0f};
    vec3       forward {0.0f, 0.0f, -1.0f};

    const vec3 horizontalVelocity {velocity.x, 0.0f, velocity.z};
    if (length(horizontalVelocity) > F32_COMPARE_EPSILON) {
        forward = normalize(horizontalVelocity);
    }

    const vec3 right = normalize(cross(forward, up));

    const float tilt = radians(uniform_linear(10.0f, 20.0f));
    const float bank = radians(uniform_linear(-4.0f, 4.0f));

    const auto flatOrientation = quaternion({1.0f, 0.0f, 0.0f}, radians(-90.0f));
    const auto tiltOrientation = quaternion(right, tilt);
    const auto bankOrientation = quaternion(forward, bank);

    node->orientation(bankOrientation * tiltOrientation * flatOrientation);

    const vec3 spinAxis = normalize(bankOrientation * tiltOrientation * up);

    static auto physicsShape = make_shared<PhysicsShape>(PhysicsShape::Type::ConcavePolyhedron, mesh);
    auto        physicsBody = make_unique<PhysicsBody>(PhysicsBody::Type::Dynamic, physicsShape);

    physicsBody->mass(0.4f);
    physicsBody->restitution(0.25f);
    physicsBody->friction(1.0f);
    physicsBody->rollingFriction(0.05f);

    const float minExtent = math::min(extent);
    physicsBody->ccdMotionThreshold(minExtent * 0.25f);
    physicsBody->ccdSweptSphereRadius(minExtent * 0.20f);
    physicsBody->ccdEnabled(true);

    const float SPIN_RATE = radians(360.0f);

    physicsBody->angularVelocity(spinAxis * SPIN_RATE);
    physicsBody->linearVelocity(velocity);

    node->physicsBody(std::move(physicsBody));

    parent.addChild(node);

    return node;
}

shared_ptr<Node> ThrowDuck(Node& parent, const vec3& location, const vec3& velocity) {

    constexpr float DUCK_HEIGHT = 0.5f;

    static auto mesh = [] {
        auto mesh = util::fs::MeshNamed("rubber_duck/rubber_duck");

        const float scaleFactor = DUCK_HEIGHT / mesh->localExtent().y;
        mesh->burnTransform(math::scale(mat4(1.0f), vec3(scaleFactor)), true);

        return mesh;
    }();

    static const auto extent = mesh->localExtent();

    auto node = Node::MeshNode(mesh);
    node->name("Quack");
    node->position(location);

    auto physicsBody = make_unique<PhysicsBody>(PhysicsBody::Type::Dynamic);
    physicsBody->mass(0.1f);
    physicsBody->restitution(0.5f);
    physicsBody->friction(2.0f);
    physicsBody->rollingFriction(0.1f);

    const float minExtent = math::min(extent);
    physicsBody->ccdMotionThreshold(minExtent * 0.25f);
    physicsBody->ccdSweptSphereRadius(minExtent * 0.25f);
    physicsBody->ccdEnabled(true);

    node->eulerAngles({uniform_linear(0.0f, TWO_PI), uniform_linear(0.0f, TWO_PI),
                       uniform_linear(0.0f, TWO_PI)});

    static const float ANGULAR_VARIANCE = radians(360.0f);

    physicsBody->angularVelocity({
        uniform_linear(-ANGULAR_VARIANCE, ANGULAR_VARIANCE),
        uniform_linear(-ANGULAR_VARIANCE, ANGULAR_VARIANCE),
        uniform_linear(-ANGULAR_VARIANCE, ANGULAR_VARIANCE),
    });

    physicsBody->linearVelocity(velocity);

    node->physicsBody(std::move(physicsBody));

    parent.addChild(node);

    return node;
}

vector<shared_ptr<Node>> DropBoxs(Node&             parent,
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
        //auto material = Material::EmissionMaterial(color);
        auto material = make_shared<Material>(color, color, color);
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
                physicsBody->restitution(0.05f);
                physicsBody->friction(0.8f);
                physicsBody->shape(physicsShape);

                const float angularVariance = radians(30.0f);
                physicsBody->angularVelocity({
                    uniform_linear(-angularVariance, angularVariance),
                    uniform_linear(-angularVariance, angularVariance),
                    uniform_linear(-angularVariance, angularVariance),
                });

                node->physicsBody(std::move(physicsBody));

                auto light = Light::Point(boxColor);
                light->attenuation(Attenuation::FromRange(3.0f, 0.02f));
                node->light(light);

                added.push_back(node);
                parent.addChild(node);
            }
        }
    }
    return added;
}

bool IsIgnored(const shared_ptr<Node>& node, const vector<const Node*>& ignoredNodes) {
    return !node || find(ignoredNodes.begin(), ignoredNodes.end(), node.get()) != ignoredNodes.end();
}
