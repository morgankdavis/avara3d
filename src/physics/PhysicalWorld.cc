//
//  PhysicalWorld.cc
//	avara-engine
//
//  Created by Morgan Davis on 1/26/18.
//  Copyright © 2018 Morgan K Davis. All rights reserved.
//

#include "ae/physics/PhysicalWorld.h"

#include "ae/diagnostic/logging/Logger.h"
#include "ae/physics/PhysicsBody.h"
#include "ae/physics/bullet/BulletWorldProxy.h"
#include "ae/scene/Node.h"
#include "ae/scene/Scene.h"


using namespace ae;
using namespace glm;
using namespace std;


/*********************************************************************************************
	Static Prorotypes
 *********************************************************************************************/

static void UpdateTimeStats(Stats& stats, double startTime, double endTime);

/*********************************************************************************************
	Lifecycle
 *********************************************************************************************/

PhysicalWorld::PhysicalWorld():
		_gravity({0, -9.807, 0}),
		_speed(1.0),
		_timestep(1.0/60.0),
		_scene(nullptr),
		_didSimulate(nullptr),
		_beginContact(nullptr),
		_continueContact(nullptr),
		_endContact(nullptr) {

	_proxy = make_unique<BulletWorldProxy>(this);
}

PhysicalWorld::~PhysicalWorld() {
	AE_LOG_D("Destroying PhysicalWorld {:p}", static_cast<void*>(this));
}

/*********************************************************************************************
	Public
 *********************************************************************************************/

vec3 PhysicalWorld::gravity() const {
	return _gravity;
}

void PhysicalWorld::gravity(vec3 gravity) {
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

shared_ptr<PhysicsContact> PhysicalWorld::contactTest(shared_ptr<PhysicsBody> bodyA,
													  shared_ptr<PhysicsBody> bodyB) {
	
	// contactPairTest (btCollisionObject *colObjA, btCollisionObject *colObjB, ContactResultCallback &resultCallback)
	
	return nullptr;
}

shared_ptr<PhysicsContact> PhysicalWorld::contactTest(shared_ptr<PhysicsBody> body) {
	
	// contactTest (btCollisionObject *colObj, ContactResultCallback &resultCallback)
	
	return nullptr;
}

shared_ptr<HitTestResult> PhysicalWorld::rayTest(vec3 fromVec, vec3 toVec) {
	
	//rayTest (const btVector3 &rayFromWorld, const btVector3 &rayToWorld, RayResultCallback &resultCallback) const
	
	return nullptr;
}

shared_ptr<PhysicsContact> PhysicalWorld::convexSweepTest(shared_ptr<PhysicsContact> contact,
														  const mat4& fromMat,
														  const mat4& toMat) {
	
	// convexSweepTest (const btConvexShape *castShape, const btTransform &from, const btTransform &to, ConvexResultCallback &resultCallback, btScalar allowedCcdPenetration=btScalar(0.)) const 

	return nullptr;
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
	Internal
 *********************************************************************************************/

void PhysicalWorld::attachedToScene(Scene* scene) {
	AE_LOG_T("scene: {:p}", static_cast<void*>(scene));

	_scene = scene;
}

void PhysicalWorld::detachedFromScene(Scene* scene) {
	AE_LOG_T("scene: {:p}", static_cast<void*>(scene));

	// removing bodies handled in PhysicalBody::physicalWorldUnreachable()

	_scene = nullptr;
}

void PhysicalWorld::add(PhysicsBody& body) {
	AE_LOG_D("body: {}", static_cast<void*>(&body));

	if (_proxy) {
//		body.addedToWorld(this);
		_proxy->add(body);
		body.addedToWorld(this);
	}
	else {
		AE_LOG_W("_model is null.");
	}
}

void PhysicalWorld::remove(PhysicsBody& body) {
	AE_LOG_D("body: {}", static_cast<void*>(&body));

	if (_proxy) {
		_proxy->remove(body);
		body.removedFromWorld(this);
	}
	else {
		AE_LOG_W("_model is null.");
	}
}

void PhysicalWorld::step(const Scene& scene,
						 double runT,
						 double deltaRunT,
						 Stats& stats) {

	if (_proxy) {

		auto startTime = scene.time();

		_proxy->step(deltaRunT, _speed, _timestep);

		UpdateTimeStats(stats, startTime, scene.time());

		if (auto didSimulate = PhysicalWorld::didSimulate()) {
			didSimulate(*this, runT);
		}
	}
	else {
		AE_LOG_E("No PhysicalWorldModelProxy attached to PhysicalWorld {:p}.", static_cast<void*>(this));
	}
}

PhysicalWorldProxy* PhysicalWorld::proxy() const {
	return  _proxy.get();
}

/*********************************************************************************************
	Static
 *********************************************************************************************/

void UpdateTimeStats(Stats& stats, double startTime, double endTime) {

	// current
	auto stepTime = endTime - startTime;
	stats.currentPhysicstime = stepTime * 1000.0f;

	static const double FRAMETIME_AVERAGING_INTERVAL = .25; // TEMPORARY

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
