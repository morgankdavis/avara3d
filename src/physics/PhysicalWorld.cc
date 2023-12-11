//
//  PhysicalWorld.cc
//	avara-engine
//
//  Created by Morgan Davis on 1/26/18.
//  Copyright © 2018 Morgan K Davis. All rights reserved.
//

#include "physics/PhysicalWorld.h"

#include "diagnostic/logging/Logger.h"
#include "physics/bullet/BulletPhysicsSimulator.h"
#include "physics/bullet/BulletWorldResources.h"
#include "scene/Node.h"
#include "scene/Scene.h"


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

PhysicalWorld::PhysicalWorld(PHYSICS_SIMULATION_ENGINE engine):
		_gravity({0, -9.807, 0}),
		_speed(1.0),
		_timestep(1.0/60.0),
		_resources(make_unique<BulletWorldResources>()),
		//_resources(nullptr),
		_simulator(make_unique<BulletPhysicsSimulator>()),
		_scene(nullptr),
		_dirtyMask(PHYSICS_WORLD_DIRTY_MASK::ALL),
		_didSimulate(nullptr),
		_beginContact(nullptr),
		_continueContact(nullptr),
		_endContact(nullptr) {

//#warning move?

//	_simulator->setTimestep(_timestep);
//	_simulator->setGravity(_gravity);
}

PhysicalWorld::~PhysicalWorld() {
	AE_LOG_D("Destroying PhysicalWorld {:p}", (void*)this);
}

/*********************************************************************************************
	Public
 *********************************************************************************************/

vec3 PhysicalWorld::gravity() const {
	return _gravity;
}

void PhysicalWorld::gravity(vec3 gravity) {
	_gravity = gravity;

	_dirtyMask = PHYSICS_WORLD_DIRTY_MASK_ADD(_dirtyMask, PHYSICS_WORLD_DIRTY_MASK::GRAVITY);
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

	_dirtyMask = PHYSICS_WORLD_DIRTY_MASK_ADD(_dirtyMask, PHYSICS_WORLD_DIRTY_MASK::TIMESTEP);
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
#warning FIX
	//_btWorld->getCollisionWorld()->computeOverlappingPairs();
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
	_scene = scene;
}

void PhysicalWorld::simulate(const Scene& scene,
							 double runT,
							 double deltaRunT,
							 Stats& stats) {

	if (_simulator) {

		auto startTime = scene.time();

		_simulator->update(*this, stats);
		_simulator->step(*this, deltaRunT);
		_simulator->sync(*this);

		UpdateTimeStats(stats, startTime, scene.time());

		if (auto didSimulate = PhysicalWorld::didSimulate()) {
			didSimulate(*this, runT);
		}
	}
	else {
		AE_LOG_E("No PhysicsSimulator attached to PhysicsWorld {:p}", (void*)this);
	}
}

//void PhysicalWorld::simulate(const Scene& scene,
//							 float runT,
//							 float deltaRunT,
//							 Stats& stats) {
//
//	if (_simulator) {
//
//		auto rootNode = scene.rootNode();
//
//		_simulator->beginUpdate(scene);
//		_simulator->update(scene);
//		rootNode->update(*_simulator,
//						  stats);
//		_simulator->step(deltaRunT * _speed);
//		_simulator->sync(scene);
//		rootNode->sync(*_simulator,
//						stats);
//		_simulator->endUpdate(scene);
//
//		if (didSimulate()) {
//			(didSimulate())(*this, runT);
//		}
//	}
//	else {
//		AE_LOG_E("No PhysicsSimulator attached to PhysicsWorld {:p}", (void*)this);
//	}
//}

PhysicalWorldResources* PhysicalWorld::resources() const {
	return  _resources.get();
}

PhysicsSimulator* PhysicalWorld::simulator() const {
	return _simulator.get();
}

PHYSICS_WORLD_DIRTY_MASK PhysicalWorld::dirtyMask() const {
	return _dirtyMask;
}

void PhysicalWorld::dirtyMask(PHYSICS_WORLD_DIRTY_MASK mask) {
	_dirtyMask = mask;
}

/*********************************************************************************************
	Static
 *********************************************************************************************/

void UpdateTimeStats(Stats& stats, double startTime, double endTime) {

	// current
	auto ms = endTime - startTime;
	stats.currentPhysicstime = ms * 1000.0f;

	static const double FRAMETIME_AVERAGING_INTERVAL = .25; // TEMPORARY

	// average
	static double msAvg = 0.0;
	static double sampleStartTime = startTime;
	static unsigned stepsSinceSampleStart = 0;
	double elapsedTimeSinceSampleStart = endTime - sampleStartTime;
	if (elapsedTimeSinceSampleStart >= FRAMETIME_AVERAGING_INTERVAL) {

		msAvg = (elapsedTimeSinceSampleStart * 1000.0f) / stepsSinceSampleStart;

		sampleStartTime = startTime;
		stepsSinceSampleStart = 0;
	}
	else {
		++stepsSinceSampleStart;
	}

	stats.averagePhysicstime = msAvg;
//	stats.averagingInterval = FRAMETIME_AVERAGING_INTERVAL;
}
