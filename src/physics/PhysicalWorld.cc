//
//  PhysicalWorld.cc
//  avara3d
//
//  Created by Morgan Davis on 1/26/18.
//  Copyright © 2024 Morgan K Davis. All rights reserved.
//

#include "a3d/physics/PhysicalWorld.h"

#include "a3d/Configuration.h"
#include "a3d/diagnostic/logging/Logger.h"
#include "a3d/physics/HitTestResult.h"
#include "a3d/physics/PhysicsBody.h"
#include "a3d/physics/PhysicsContact.h"
#include "a3d/physics/bullet/BulletWorldProxy.h"
#include "a3d/scene/Node.h"
#include "a3d/scene/Scene.h"


using namespace a3d;
using namespace glm;
using namespace std;


/*********************************************************************************************
	Private Static Non-Member Prorotypes
 *********************************************************************************************/

static void UpdateTimeStats(Stats& stats, double startTime, double endTime);

/*********************************************************************************************
	Public Lifecycle
 *********************************************************************************************/

PhysicalWorld::PhysicalWorld():
		_gravity{0, -9.807, 0},
		_speed{1.0},
		_timestep{1.0/60.0},
		_scene{},
		_didSimulate{},
		_beginContact{},
		_continueContact{},
		_endContact{} {

	_proxy = make_unique<BulletWorldProxy>(*this);
}

PhysicalWorld::~PhysicalWorld() {
	A3D_LOG_D("Destroying PhysicalWorld {:p}", static_cast<void*>(this));
}

/*********************************************************************************************
	Public Members
 *********************************************************************************************/

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

PhysicalWorld::DidSimulateCallback PhysicalWorld::didSimulate() const {
	return _didSimulate;
}

void PhysicalWorld::didSimulate(DidSimulateCallback function) {
	_didSimulate = function;
}

PhysicalWorld::BeginContactCallback PhysicalWorld::beginContact() const {
	return _beginContact;
}

void PhysicalWorld::beginContact(PhysicalWorld::BeginContactCallback function) {
	_beginContact = function;
}

PhysicalWorld::ContinueContactCallback PhysicalWorld::continueContact() const {
	return _continueContact;
}

void PhysicalWorld::continueContact(PhysicalWorld::ContinueContactCallback function) {
	_continueContact = function;
}

PhysicalWorld::EndContactCallback PhysicalWorld::endContact() const {
	return _endContact;
}

void PhysicalWorld::endContact(PhysicalWorld::EndContactCallback function) {
	_endContact = function;
}

/*********************************************************************************************
	Internal Members
 *********************************************************************************************/

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
						 Stats& stats) {

	if (_proxy) {

		auto startTime = scene.time();

		_proxy->step(deltaRunT, _speed, _timestep, stats);

		UpdateTimeStats(stats, startTime, scene.time());

		if (auto didSimulate = PhysicalWorld::didSimulate()) {
			didSimulate(*this, runT);
		}
	}
	else {
		A3D_LOG_E("No PhysicalWorldModelProxy attached to PhysicalWorld {:p}.", static_cast<void*>(this));
	}
}

PhysicalWorldProxy* PhysicalWorld::proxy() const {
	return  _proxy.get();
}

/*********************************************************************************************
	Private Static Non-Members
 *********************************************************************************************/

void UpdateTimeStats(Stats& stats, double startTime, double endTime) {

	// current
	auto stepTime = endTime - startTime;
	stats.currentPhysicstime = stepTime * 1000.0f;

	// average
	static double avg = 0.0;
	static double sampleStartTime = startTime;
	static unsigned stepsSinceSampleStart = 0;
	static double accumulatedStepTimeSinceSampleStart = 0;
	double elapsedTimeSinceSampleStart = endTime - sampleStartTime;
	if (elapsedTimeSinceSampleStart >= FRAMETIME_AVERAGING_INTERVAL) {

		avg = (accumulatedStepTimeSinceSampleStart * 1000.0f) / stepsSinceSampleStart;

		sampleStartTime = startTime;
		stepsSinceSampleStart = 0;
		accumulatedStepTimeSinceSampleStart = 0;
	}
	else {
		++stepsSinceSampleStart;
		accumulatedStepTimeSinceSampleStart += stepTime;
	}

	stats.averagePhysicstime = avg;
//	stats.averagingInterval = FRAMETIME_AVERAGING_INTERVAL;
}
