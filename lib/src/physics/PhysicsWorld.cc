//
//  PhysicsWorld.cc
//  avara3d
//
//  Created by Morgan Davis on 1/26/18.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#include "a3d/physics/PhysicsWorld.h"

#include <cmath>
#include <stdexcept>

#include "a3d/log/Log.h"
#include "a3d/mesh/Line.h"
#include "a3d/physics/PhysicsBody.h"
#include "a3d/physics/PhysicsContact.h"
#include "a3d/physics/backend/bullet/BulletWorldProxy.h"
#include "a3d/physics/proxy/PhysicsWorldProxy.h"
#include "a3d/profile/Profile.h"
#include "a3d/scene/HitTestResult.h"
#include "a3d/scene/Node.h"
#include "a3d/scene/Scene.h"
#include "a3d/util/Flow.h"

using namespace a3d;
using namespace a3d::math;
using namespace std;

// [Public Lifecycle Functions]

PhysicsWorld::PhysicsWorld():
    _gravity {0, -9.807, 0},
    _scene {},
    _didBeginContactCallback {},
    _didContinueContactCallback {},
    _didEndContactCallback {} {

    _proxy = make_unique<BulletWorldProxy>(*this);
}

PhysicsWorld::~PhysicsWorld() {
    log::d()("Destroying PhysicsWorld {:p}", static_cast<void*>(this));
}

// [Public Member Functions]

const vec3& PhysicsWorld::gravity() const {
    return _gravity;
}

void PhysicsWorld::gravity(const vec3& gravity) {

    _gravity = gravity;

    if (_proxy) {
        _proxy->gravity(gravity);
    }
}

optional<PhysicsContact> PhysicsWorld::contactTest(const PhysicsBody& bodyA, const PhysicsBody& bodyB) const {
    return _proxy->contactTest(bodyA, bodyB);
}

vector<PhysicsContact> PhysicsWorld::contactTest(const PhysicsBody& body) const {
    return _proxy->contactTest(body);
}

vector<HitTestResult> PhysicsWorld::rayTest(const vec3& from, const vec3& to) const {
    return rayTest(from, to, RayTestOptions {});
}

vector<HitTestResult> PhysicsWorld::rayTest(const vec3&           from,
                                            const vec3&           to,
                                            const RayTestOptions& options) const {
    return _proxy->rayTest(from, to, options.searchMode);
}

vector<PhysicsContact> PhysicsWorld::convexSweepTest(const PhysicsShape&           shape,
                                                     const mat4&                   fromMat,
                                                     const mat4&                   toMat,
                                                     const ConvexSweepTestOptions& options) const {
    throw runtime_error("Not implemented.");
}

vector<PhysicsContact> PhysicsWorld::convexSweepTest(const PhysicsShape& shape,
                                                     const mat4&         fromMat,
                                                     const mat4&         toMat) const {
    throw runtime_error("Not implemented.");
}

Scene* PhysicsWorld::scene() const {
    return _scene;
}

PhysicsWorld::DidBeginContactCallback PhysicsWorld::didBeginContactCallback() const {
    return _didBeginContactCallback;
}

void PhysicsWorld::didBeginContactCallback(PhysicsWorld::DidBeginContactCallback function) {
    _didBeginContactCallback = function;
}

PhysicsWorld::DidContinueContactCallback PhysicsWorld::didContinueContactCallback() const {
    return _didContinueContactCallback;
}

void PhysicsWorld::didContinueContactCallback(PhysicsWorld::DidContinueContactCallback function) {
    _didContinueContactCallback = function;
}

PhysicsWorld::DidEndContactCallback PhysicsWorld::didEndContactCallback() const {
    return _didEndContactCallback;
}

void PhysicsWorld::didEndContactCallback(PhysicsWorld::DidEndContactCallback function) {
    _didEndContactCallback = function;
}

// [Internal Member Functions]

void PhysicsWorld::attachedToScene(Scene& scene) {
    log::t()("scene: {:p}", static_cast<void*>(&scene));

    _scene = &scene;
}

void PhysicsWorld::detachedFromScene(Scene& scene) {
    log::t()("scene: {:p}", static_cast<void*>(&scene));

    // removing bodies handled in PhysicsBody::physicsWorldUnreachable()

    _scene = nullptr;
}

void PhysicsWorld::add(PhysicsBody& body) {
    log::d()("body: {}", static_cast<void*>(&body));

    if (_proxy) {
        body.syncTransformFromNode();

        _proxy->add(body);
        body.addedToWorld(*this);
    }
    else {
        log::w()("_model is null.");
    }
}

void PhysicsWorld::remove(PhysicsBody& body) {
    log::d()("body: {}", static_cast<void*>(&body));

    if (_proxy) {
        _proxy->remove(body);
        body.removedFromWorld(*this);
    }
    else {
        log::w()("_model is null.");
    }
}

void PhysicsWorld::step(double deltaTime, Profiler& profiler) {

    if (!util::flow::edge_guard(_proxy, [&] {
            log::e()("No PhysicsWorldProxy attached to PhysicsWorld {:p}.", static_cast<void*>(this));
        })) {
        return;
    }

    const auto& contactEvents = _proxy->step(deltaTime, profiler);

    if (contactEvents.empty()
        || (!_didBeginContactCallback && !_didContinueContactCallback && !_didEndContactCallback)) {
        return;
    }

    prof::profile(profiler, Profiler::Tag::Application, [&] {
        for (const auto& event : contactEvents) {
            switch (event.type) {
                case PhysicsWorldProxy::ContactEventType::Begin:
                    if (_didBeginContactCallback) {
                        _didBeginContactCallback(*this, event.contact);
                    }
                    break;
                case PhysicsWorldProxy::ContactEventType::Continue:
                    if (_didContinueContactCallback) {
                        _didContinueContactCallback(*this, event.contact);
                    }
                    break;
                case PhysicsWorldProxy::ContactEventType::End:
                    if (_didEndContactCallback) {
                        _didEndContactCallback(*this, event.contact);
                    }
                    break;
            }
        }
    });
}

PhysicsWorld::Inventory PhysicsWorld::inventory() const {
    if (_proxy) {
        return _proxy->inventory();
    }
    return {};
}

void PhysicsWorld::appendDebugLines(vector<Line>& out) const {
    if (_proxy) {
        _proxy->appendDebugLines(out, _scene->debugOptions()); // HEH
    }
}

PhysicsWorldProxy* PhysicsWorld::proxy() const {
    return _proxy.get();
}
