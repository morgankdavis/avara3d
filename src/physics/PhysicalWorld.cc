//
//  PhysicalWorld.cc
//  avara3d
//
//  Created by Morgan Davis on 1/26/18.
//  Copyright © 2024 Morgan K Davis. All rights reserved.
//

#include "a3d/physics/PhysicalWorld.h"

#include "a3d/Utilities.h"
#include "a3d/diagnostic/log/Log.h"
#include "a3d/physics/HitTestResult.h"
#include "a3d/physics/PhysicsBody.h"
#include "a3d/physics/PhysicsContact.h"
#include "a3d/physics/bullet/BulletWorldProxy.h"
#include "a3d/profiling/Profiling.h"
#include "a3d/scene/Node.h"
#include "a3d/scene/Scene.h"

using namespace a3d;
using namespace a3d::math;
using namespace std;

/// Public Lifecycle Functions ///

PhysicalWorld::PhysicalWorld():
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

PhysicalWorld::~PhysicalWorld() {
	A3D_LOG_D("Destroying PhysicalWorld {:p}", static_cast<void*>(this));
}

/// Public Member Functions ///

const vec3& PhysicalWorld::gravity() const {
	return _gravity;
}

void PhysicalWorld::gravity(const vec3& gravity) {
	_gravity = gravity;
}

float PhysicalWorld::speed() const {
	return _speed;
}

void PhysicalWorld::speed(float speed) {
	_speed = speed;
}

float PhysicalWorld::timestep() const {
	return _timestep;
}

void PhysicalWorld::timestep(float timestep) {
	_timestep = timestep;
}

optional<PhysicsContact> PhysicalWorld::contactTest(const PhysicsBody& bodyA,
													const PhysicsBody& bodyB) {

	// contactPairTest (btCollisionObject *colObjA, btCollisionObject *colObjB, ContactResultCallback &resultCallback)
	
	return {};
}

optional<PhysicsContact> PhysicalWorld::contactTest(const PhysicsBody& body) {
	
	// contactTest (btCollisionObject *colObj, ContactResultCallback &resultCallback)
	
	return {};
}

optional<HitTestResult> PhysicalWorld::rayTest(const vec3& fromVec, const vec3& toVec) {

	//rayTest (const btVector3 &rayFromWorld, const btVector3 &rayToWorld, RayResultCallback &resultCallback) const

	return {};
}

optional<PhysicsContact> PhysicalWorld::convexSweepTest(const PhysicsContact& contact,
														const mat4& fromMat,
														const mat4& toMat) {

	// convexSweepTest (const btConvexShape *castShape, const btTransform &from, const btTransform &to, ConvexResultCallback &resultCallback, btScalar allowedCcdPenetration=btScalar(0.)) const 

	return {};
}

void PhysicalWorld::updateCollisionPairs() {
	_proxy->updateCollisionPairs();
}

Scene* PhysicalWorld::scene() const {
	return _scene;
}

PhysicalWorld::DidSimulateCallback PhysicalWorld::didSimulateCallback() const {
	return _didSimulateCallback;
}

void PhysicalWorld::didSimulateCallback(DidSimulateCallback function) {
	_didSimulateCallback = function;
}

PhysicalWorld::BeginContactCallback PhysicalWorld::beginContactCallback() const {
	return _beginContactCallback;
}

void PhysicalWorld::beginContactCallback(PhysicalWorld::BeginContactCallback function) {
	_beginContactCallback = function;
}

PhysicalWorld::ContinueContactCallback PhysicalWorld::continueContactCallback() const {
	return _continueContactCallback;
}

void PhysicalWorld::continueContactCallback(PhysicalWorld::ContinueContactCallback function) {
	_continueContactCallback = function;
}

PhysicalWorld::EndContactCallback PhysicalWorld::endContactCallback() const {
	return _endContactCallback;
}

void PhysicalWorld::endContactCallback(PhysicalWorld::EndContactCallback function) {
	_endContactCallback = function;
}

/// Internal Member Functions ///

void PhysicalWorld::attachedToScene(Scene& scene) {
	A3D_LOG_T("scene: {:p}", static_cast<void*>(&scene));

	_scene = &scene;
}

void PhysicalWorld::detachedFromScene(Scene& scene) {
	A3D_LOG_T("scene: {:p}", static_cast<void*>(&scene));

	// removing bodies handled in PhysicalBody::physicalWorldUnreachable()

	_scene = nullptr;
}

void PhysicalWorld::add(PhysicsBody& body) {
	A3D_LOG_D("body: {}", static_cast<void*>(&body));

	if (_proxy) {
//		body.addedToWorld(this);
		_proxy->add(body);
		body.addedToWorld(*this);
	}
	else {
		A3D_LOG_W("_model is null.");
	}
}

void PhysicalWorld::remove(PhysicsBody& body) {
	A3D_LOG_D("body: {}", static_cast<void*>(&body));

	if (_proxy) {
		_proxy->remove(body);
		body.removedFromWorld(*this);
	}
	else {
		A3D_LOG_W("_model is null.");
	}
}

void PhysicalWorld::step(const Scene& scene,
						 double runT,
						 double deltaRunT,
						 FrameStats& stats,
						 Profiler& profiler) {

	A3D_EDGE_GUARD(!_proxy, return;, [&] {
		A3D_LOG_E("No PhysicalWorldProxy attached to PhysicalWorld {:p}.", static_cast<void*>(this));
	});

	_proxy->step(deltaRunT, _speed, _timestep, stats, profiler);

	if (auto didSimulate = PhysicalWorld::didSimulateCallback()) {
		A3D_PROFILE(profiler, Profiler::Tag::Application, [&] {
			didSimulate(*this, runT, deltaRunT);
		});
	}
}

PhysicalWorldProxy* PhysicalWorld::proxy() const {
	return  _proxy.get();
}
