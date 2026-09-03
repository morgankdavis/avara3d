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

namespace demo::janus {

// [App Private Member Functions]

bool App::drawPanel() {

    auto& input = static_cast<DesktopInputContext&>(*scene().inputContext());

    if (input.keyDown(DesktopInputContext::Key::D)) {
        return drawDevPanel();
    }

    return drawDemoPanel();
}

bool App::drawDemoPanel() {

    const float PANEL_WIDTH {180.0f};

    auto& runner = App::runner();
    auto& scene = App::scene();
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

    panel.segmentedRow(3);
    if (panel.option("Earth", isGravity(GRAVITY_EARTH))) {
        physicsWorld.gravity(GRAVITY_EARTH);
    }
    if (panel.option("Moon", isGravity(GRAVITY_MOON))) {
        physicsWorld.gravity(GRAVITY_MOON);
    }
    if (panel.option("Zero", isGravity(GRAVITY_ZERO))) {
        physicsWorld.gravity(GRAVITY_ZERO);
    }

    panel.section("action");

    panel.segmentedRow(3);
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
            panel.segmentedRow(3);
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
            panel.segmentedRow(3);
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
            panel.segmentedRow(3);
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

        panel.value("name", node->name().value_or("(unnamed)"));

        if (auto body = node->physicsBody()) {

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
            panel.value("shape", body->shape() ? shapeTypeName(body->shape()->type()) : "none");

            panel.spacer(6.0f);

            panel.value("mass", std::format("{:.1f}", body->mass()));
            panel.value("friction", std::format("{:.1f}", body->friction()));
            panel.value("restitution", std::format("{:.1f}", body->restitution()));
            panel.value("contacts", std::format("{}", physicsWorld.contactTest(*body).size()));
            panel.value("resting", body->resting() ? "yes" : "no");
        }
        else {
            panel.spacer(6.0f);
            panel.value("body", "none", {.top = 6.0f});
        }

        if (const auto& mesh = node->mesh()) {

            panel.spacer(6.0f);

            panel.value("mesh", mesh->name().value_or("(unnamed)"));
            uint64_t polygons = 0;
            for (const auto& e : mesh->elements()) {
                polygons += e->indexCount() / 3u;
            }
            panel.value("polygons", std::format("{:.1f}k", float(polygons) / 1000.0f));
            panel.value("materials", std::format("{}", mesh->materials().size()));
        }
    }
    else {
        // the selected node was removed from the scene
        select({});
        panel.text("click an object to inspect");
    }

    panel.section("debug");

    using DebugOptions = Scene::DebugOptions;

    auto debugOptions = scene.debugOptions();

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

    return panel.hovered();
}

bool App::drawDevPanel() {

    const float PANEL_WIDTH {220.0f};

    auto& scene = App::scene();
    auto& visualWorld = *scene.visualWorld();

    ui::Panel panel("dev", {.width = PANEL_WIDTH, .margin = 12.0f});

    panel.section("atmosphere", {.line = true}, {.top = 0.0f, .bottom = 4.0f});

    if (visualWorld.atmosphere()) {

        auto atmosphere = *visualWorld.atmosphere();
        bool changed = false;

        if (atmosphere.haze) {

            float alpha = atmosphere.haze->color.a();

            if (panel.slider("haze alpha", alpha, 0.0f, 1.0f, "%.2f")) {
                const auto rgb = atmosphere.haze->color.rgb();
                atmosphere.haze->color = Color {rgb.x, rgb.y, rgb.z, alpha};
                changed = true;
            }

            changed |= panel.slider("haze density", atmosphere.haze->density, 0.0f, 1.5f, "%.2f");
        }

        changed |= panel.slider("scale height", atmosphere.scaleHeight, 0.1f, 5.0f, "%.2f");

        if (atmosphere.limbGlow) {

            float alpha = atmosphere.limbGlow->color.a();

            if (panel.slider("limb alpha", alpha, 0.0f, 1.0f, "%.2f")) {
                const auto rgb = atmosphere.limbGlow->color.rgb();
                atmosphere.limbGlow->color = Color {rgb.x, rgb.y, rgb.z, alpha};
                changed = true;
            }

            changed |= panel.slider("limb intensity", atmosphere.limbGlow->intensity, 0.0f, 2.0f, "%.2f");
        }

        if (changed) {
            visualWorld.atmosphere(atmosphere);
        }
    }

    panel.section("ground");

    if (visualWorld.ground()) {

        auto ground = *visualWorld.ground();
        bool changed = false;

        if (ground.horizonHaze) {

            float alpha = ground.horizonHaze->color.a();

            if (panel.slider("horizon alpha", alpha, 0.0f, 1.0f, "%.2f")) {
                const auto rgb = ground.horizonHaze->color.rgb();
                ground.horizonHaze->color = Color {rgb.x, rgb.y, rgb.z, alpha};
                changed = true;
            }

            float angularWidth = degrees(ground.horizonHaze->angularWidth);

            if (panel.slider("horizon width", angularWidth, 0.1f, 15.0f, "%.1f deg")) {
                ground.horizonHaze->angularWidth = radians(angularWidth);
                changed = true;
            }
        }

        if (ground.radialFade) {
            auto& fade = *ground.radialFade;
            changed |= panel.slider("fade start", fade.startDistance, 0.0f, fade.endDistance - 0.1f, "%.1f");
            changed |= panel.slider("fade end", fade.endDistance, fade.startDistance + 0.1f, 250.0f, "%.1f");
        }

        if (changed) {
            visualWorld.ground(ground);
        }
    }

    panel.spacer(12.0f);

    if (panel.button("Print Camera")) {

        const auto& view = _cameraController.view();
        const auto& target = std::get<vec3>(view.target);

        log::app::d()("controller.view({{\n"
                      "    .target = vec3 {{{}f, {}f, {}f}},\n"
                      "    .yaw = radians({}f),\n"
                      "    .pitch = radians({}f),\n"
                      "    .distance = {}f\n"
                      "}});",
                      target.x, target.y, target.z, degrees(view.yaw), degrees(view.pitch), view.distance);
    }

    return panel.hovered();
}

} // namespace demo::janus
