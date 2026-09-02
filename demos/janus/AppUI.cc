//
//  AppUI.cc
//  janus
//
//  Created by Morgan Davis on 8/30/26.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#include "App.h"
#include "Constants.h"

#include "a3d/a3d.h"

using namespace a3d;
using namespace a3d::math;
using namespace demo::janus;

// [App Private Member Functions]

bool App::drawPanel() {

    const float PANEL_WIDTH {180.0f};

    auto& runner = App::runner();
    auto& scene = App::scene();
    auto& visualWorld = *scene.visualWorld();
    auto& physicsWorld = *scene.physicsWorld();

    ui::Panel panel("controls", {.width = PANEL_WIDTH, .margin = 12.0f});

    if (panel.button("CAMERA")) {
        log::app::i()("CAMERA: {:P}", static_cast<void*>(&_cameraController));
    }

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

    float timeScale = runner.timeScale();
    if (panel.slider("time scale", timeScale, 0.1f, 2.0f, "%.1fx")) {
        runner.timeScale(timeScale);
    }

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

    panel.section("selected node");

    if (!_selection) {
        panel.text("click to select");
    }
    else if (auto node = _selection->node.lock()) {

        // node

        panel.value("name", node->name().value_or("(unnamed)"));

        // physics body

        if (auto body = node->physicsBody()) {

            static auto formatVec3 = [](const vec3& value) {
                return std::format("{:.1f}, {:.1f}, {:.1f}", value.x, value.y, value.z);
            };

            static auto bodyTypeName = [](PhysicsBody::Type type) -> std::string_view {
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

            static auto shapeTypeName = [](PhysicsShape::Type type) -> std::string_view {
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

        bool defaultLighting = visualWorld.defaultLightingEnabled();
        if (panel.toggle("default lighting", defaultLighting)) {
            visualWorld.defaultLightingEnabled(defaultLighting);
        }

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
    }

    return panel.hovered();
}
