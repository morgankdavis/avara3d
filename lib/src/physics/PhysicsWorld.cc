//
//  PhysicsWorld.cc
//  avara3d
//
//  Created by Morgan Davis on 1/26/18.
//  Copyright © 2024 Morgan K Davis. All rights reserved.
//

#include "a3d/physics/PhysicsWorld.h"

#include "a3d/log/Log.h"
#include "a3d/mesh/Line.h"
#include "a3d/physics/HitTestResult.h"
#include "a3d/physics/PhysicsBody.h"
#include "a3d/physics/PhysicsContact.h"
#include "a3d/physics/backend/bullet/BulletWorldProxy.h"
#include "a3d/profile/Profile.h"
#include "a3d/scene/Node.h"
#include "a3d/scene/Scene.h"
#include "a3d/util/flow.h"

using namespace a3d;
using namespace a3d::math;
using namespace std;

/// Public Lifecycle Functions ///

PhysicsWorld::PhysicsWorld():
		_gravity{0, -9.807, 0},
		_speed{1.0},
		_timestep{1.0/60.0},
		_scene{},
		_didSimulateCallback{},
		_beginContactCallback{},
		_continueContactCallback{},
		_endContactCallback{} {

	_proxy = make_unique<BulletWorldProxy>(*this);
}

PhysicsWorld::~PhysicsWorld() {
	log::d()("Destroying PhysicsWorld {:p}", static_cast<void*>(this));
}

/// Public Member Functions ///

const vec3& PhysicsWorld::gravity() const {
	return _gravity;
}

void PhysicsWorld::gravity(const vec3& gravity) {
	_gravity = gravity;
}

float PhysicsWorld::speed() const {
	return _speed;
}

void PhysicsWorld::speed(float speed) {
	_speed = speed;
}

float PhysicsWorld::timestep() const {
	return _timestep;
}

void PhysicsWorld::timestep(float timestep) {
	_timestep = timestep;
}

optional<PhysicsContact> PhysicsWorld::contactTest(const PhysicsBody& bodyA,
													const PhysicsBody& bodyB) {

	// contactPairTest (btCollisionObject *colObjA, btCollisionObject *colObjB, ContactResultCallback &resultCallback)
	
	return {};
}

optional<PhysicsContact> PhysicsWorld::contactTest(const PhysicsBody& body) {
	
	// contactTest (btCollisionObject *colObj, ContactResultCallback &resultCallback)
	
	return {};
}

optional<HitTestResult> PhysicsWorld::rayTest(const vec3& fromVec, const vec3& toVec) {

	//rayTest (const btVector3 &rayFromWorld, const btVector3 &rayToWorld, RayResultCallback &resultCallback) const

	return {};
}

optional<PhysicsContact> PhysicsWorld::convexSweepTest(const PhysicsContact& contact,
														const mat4& fromMat,
														const mat4& toMat) {

	// convexSweepTest (const btConvexShape *castShape, const btTransform &from, const btTransform &to, ConvexResultCallback &resultCallback, btScalar allowedCcdPenetration=btScalar(0.)) const 

	return {};
}

void PhysicsWorld::updateCollisionPairs() {
	_proxy->updateCollisionPairs();
}

Scene* PhysicsWorld::scene() const {
	return _scene;
}

PhysicsWorld::DidSimulateCallback PhysicsWorld::didSimulateCallback() const {
	return _didSimulateCallback;
}

void PhysicsWorld::didSimulateCallback(DidSimulateCallback function) {
	_didSimulateCallback = function;
}

PhysicsWorld::BeginContactCallback PhysicsWorld::beginContactCallback() const {
	return _beginContactCallback;
}

void PhysicsWorld::beginContactCallback(PhysicsWorld::BeginContactCallback function) {
	_beginContactCallback = function;
}

PhysicsWorld::ContinueContactCallback PhysicsWorld::continueContactCallback() const {
	return _continueContactCallback;
}

void PhysicsWorld::continueContactCallback(PhysicsWorld::ContinueContactCallback function) {
	_continueContactCallback = function;
}

PhysicsWorld::EndContactCallback PhysicsWorld::endContactCallback() const {
	return _endContactCallback;
}

void PhysicsWorld::endContactCallback(PhysicsWorld::EndContactCallback function) {
	_endContactCallback = function;
}

/// Internal Member Functions ///

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
//		body.addedToWorld(this);
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

void PhysicsWorld::step(const Scene& scene,
						 double runT,
						 double deltaRunT,
						 FrameStats& stats,
						 Profiler& profiler) {

	if (!util::flow::edge_guard(_proxy, [&] {
		log::e()("No PhysicsWorldProxy attached to PhysicsWorld {:p}.", static_cast<void*>(this));
	})) return;

	_proxy->step(deltaRunT, _speed, _timestep, stats, profiler);

	if (auto didSimulate = PhysicsWorld::didSimulateCallback()) {
		prof::profile(profiler, Profiler::Tag::Application, [&] {
			didSimulate(*this, runT, deltaRunT);
		});
	}
}

void PhysicsWorld::appendDebugLines(vector<Line>& out,
									 Scene::DebugOptions debugOptions) const {
	if (_proxy) {
		_proxy->appendDebugLines(out, debugOptions);
	}
}

PhysicsWorldProxy* PhysicsWorld::proxy() const {
	return  _proxy.get();
}
