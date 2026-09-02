//
//  Scene.cc
//  avara3d
//
//  Created by Morgan Davis on 10/21/16.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#include "a3d/scene/Scene.h"

#include <filesystem>
#include <stdexcept>
#include <utility>

#include "a3d/Buffer.h"
#include "a3d/Color.h"
#include "a3d/input/InputContext.h"
#include "a3d/log/Log.h"
#include "a3d/physics/PhysicsWorld.h"
#include "a3d/profile/Profile.h"
#include "a3d/profile/Profiler.h"
#include "a3d/render/context/RenderContext.h"
#include "a3d/scene/Node.h"
#include "a3d/scene/importer/GlTFImporter.h"
#include "a3d/util/Chrono.h"
#include "a3d/util/Timer.h"
#include "a3d/visual/VisualWorld.h"

using namespace a3d::math;
using namespace std;
using namespace std::filesystem;

namespace a3d {

// [Public Static Member Functions]

unique_ptr<Scene> Scene::FromFile(const filesystem::path& path, ImportOptions options) {
    GlTFImporter importer {path, options};
    auto         scene = importer.scene();
    return scene;
}

// [Public Lifecycle Functions]

Scene::Scene():
    _name {},
    _rootNode {make_shared<Node>("root node")},
    _visualWorld {},
    _physicsWorld {},
    _inputContext {},
    _debugOptions {DebugOptions::None},
    _willStepCallback {},
    _didStepCallback {} {

    _rootNode->attachedToScene(*this);
}

Scene::Scene(const string& name):
    Scene() {

    _name = name;
}

Scene::Scene(unique_ptr<VisualWorld>  visualWorld,
             unique_ptr<PhysicsWorld> physicsWorld,
             unique_ptr<InputContext> inputContext):
    Scene() {

    _visualWorld = std::move(visualWorld);
    _physicsWorld = std::move(physicsWorld);
    _inputContext = std::move(inputContext);

    if (_visualWorld) {
        _visualWorld->attachedToScene(*this);
    }
    if (_physicsWorld) {
        _physicsWorld->attachedToScene(*this);
    }
    if (_inputContext) {
        _inputContext->attachedToScene(*this);
    }
}

Scene::Scene(const string&            name,
             unique_ptr<VisualWorld>  visualWorld,
             unique_ptr<PhysicsWorld> physicsWorld,
             unique_ptr<InputContext> inputContext):
    Scene(std::move(visualWorld), std::move(physicsWorld), std::move(inputContext)) {

    _name = name;
}

Scene::~Scene() {

    if (_name != nullopt) {
        log::d()("Destroying Scene '{}' ({:p})", *_name, static_cast<void*>(this));
    }
    else {
        log::d()("Destroying Scene {:p}", static_cast<void*>(this));
    }

    if (_rootNode) {
        _rootNode->detachedFromScene(*this);
    }
    if (_visualWorld) {
        _visualWorld->detachedFromScene(*this);
    }
    if (_physicsWorld) {
        _physicsWorld->detachedFromScene(*this);
    }
    //	if (_inputContext) _inputContext->detachedFromScene(*this);
}

// [Public Member Functions]

const optional<string>& Scene::name() const {
    return _name;
}

void Scene::name(const string& name) {
    _name = name;
}

const shared_ptr<Node>& Scene::rootNode() const {
    return _rootNode;
}

void Scene::rootNode(const shared_ptr<Node>& node) {

    if (!node) {
        throw std::invalid_argument("Scene::rootNode() requires a non-null Node.");
    }

    if (_rootNode == node) {
        return;
    }

    _rootNode->detachedFromScene(*this);
    _rootNode = node;
    _rootNode->attachedToScene(*this);
}

VisualWorld* Scene::visualWorld() const {
    return _visualWorld.get();
}

void Scene::visualWorld(unique_ptr<VisualWorld> world) {

    if (_visualWorld) {

        _visualWorld->detachedFromScene(*this);

        if (_rootNode) {
            _rootNode->visualWorldDetachedFromScene(*_visualWorld, *this);
        }
    }

    _visualWorld = std::move(world);

    if (_visualWorld) {

        _visualWorld->attachedToScene(*this);

        if (_rootNode) {
            _rootNode->visualWorldAttachedToScene(*_visualWorld, *this);
        }

        if (_inputContext) {
            _inputContext->visualWorldAttachedToScene(*this);
        }
    }
}

PhysicsWorld* Scene::physicsWorld() const {
    return _physicsWorld.get();
}

void Scene::physicsWorld(unique_ptr<PhysicsWorld> world) {

    if (_physicsWorld) {

        _physicsWorld->detachedFromScene(*this);

        if (_rootNode) {
            _rootNode->physicsWorldDetachedFromScene(*_physicsWorld, *this);
        }
    }

    _physicsWorld = std::move(world);

    if (_physicsWorld) {

        _physicsWorld->attachedToScene(*this);

        if (_rootNode) {
            _rootNode->physicsWorldAttachedToScene(*_physicsWorld, *this);
        }
    }
}

InputContext* Scene::inputContext() const {
    return _inputContext.get();
}

void Scene::inputContext(unique_ptr<InputContext> inputContext) {

    if (_inputContext) {
        //		_inputContext->detachedFromScene(*this);
    }

    _inputContext = std::move(inputContext);

    if (_inputContext) {
        _inputContext->attachedToScene(*this);
    }
}

AABB Scene::aabb(bool vertfit) const {

    return rootNode()->aabb(vertfit);
}

vec3 Scene::extent(bool vertfit) const {

    const auto aabb = Scene::aabb(vertfit);

    if (!aabb.valid()) {
        return {0.0f, 0.0f, 0.0f};
    }

    return aabb.max - aabb.min;
}

Scene::DebugOptions Scene::debugOptions() const {
    return _debugOptions;
}

void Scene::debugOptions(DebugOptions options) {

#ifdef A3D_GL_ES
    if (DEBUG_OPTIONS_CONTAINS(options, DEBUG_OPTIONS::SHOW_WIREFRAMES)) {
        throw Exception("DEBUG_OPTIONS::SHOW_WIREFRAMES not supported on this platform.");
    }
    if (DEBUG_OPTIONS_CONTAINS(options, DEBUG_OPTIONS::SHOW_BOUNDING_BOXES)) {
        throw Exception("DEBUG_OPTIONS::SHOW_BOUNDING_BOXES not supported on this platform.");
    }
#endif

    _debugOptions = options;
}

Scene::WillStepCallback Scene::willStepCallback() const {
    return _willStepCallback;
}

void Scene::willStepCallback(WillStepCallback callback) {
    _willStepCallback = callback;
}

Scene::DidStepCallback Scene::didStepCallback() const {
    return _didStepCallback;
}

void Scene::didStepCallback(DidStepCallback callback) {
    _didStepCallback = callback;
}

// [Internal Member Functions]

void Scene::pollEvents(Profiler& profiler) {

    if (_visualWorld) {
        if (auto renderContext = _visualWorld->renderContext()) {
            prof::profile(profiler, Profiler::Tag::EngineCpu, [&] {
                renderContext->pollEvents();
            });
        }
    }
}

void Scene::updateInput(const InputContext::UpdateInfo& info, Profiler& profiler) {

    if (!_inputContext) {
        return;
    }

    prof::profile(profiler, Profiler::Tag::EngineCpu, [&] {
        _inputContext->update(info);
    });

    if (auto callback = _inputContext->didUpdateCallback()) {
        prof::profile(profiler, Profiler::Tag::Application, [&] {
            callback(*_inputContext, info);
        });
    }
}

void Scene::stepSimulation(const StepInfo& info, Profiler& profiler) {

    if (auto callback = willStepCallback()) {
        prof::profile(profiler, Profiler::Tag::Application, [&] {
            callback(*this, info);
        });
    }

    if (_physicsWorld) {
        _physicsWorld->step(info.deltaTime, profiler);
    }

    if (auto callback = didStepCallback()) {
        prof::profile(profiler, Profiler::Tag::Application, [&] {
            callback(*this, info);
        });
    }
}

} // namespace a3d
