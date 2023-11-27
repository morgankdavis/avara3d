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


using namespace ae;
using namespace glm;
using namespace std;


/*********************************************************************************************
	Lifecycle
 *********************************************************************************************/

PhysicalWorld::PhysicalWorld(PHYSICS_SIMULATION_ENGINE engine):
		_gravity({0, -9.807, 0}),
		_timestep(1.0/60.0),
//	_scene({}),
	_dirtyMask(PHYSICS_WORLD_DIRTY_MASK::ALL),
	_simulator(make_shared<BulletPhysicsSimulator>()) { }

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

float PhysicalWorld::timestep() const {
	return _timestep;
}

void PhysicalWorld::timestep(float timestep) {
	_timestep = timestep;

	_dirtyMask = PHYSICS_WORLD_DIRTY_MASK_ADD(_dirtyMask, PHYSICS_WORLD_DIRTY_MASK::TIMESTEP);
}

void PhysicalWorld::updateCollisionPairs() {
	#warning FIX
	//_btWorld->getCollisionWorld()->computeOverlappingPairs();
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

Scene* PhysicalWorld::scene() const {
	return _scene;
}

/*********************************************************************************************
	Internal
 *********************************************************************************************/

PHYSICS_WORLD_DIRTY_MASK PhysicalWorld::dirtyMask() const {
	return _dirtyMask;
}

void PhysicalWorld::dirtyMask(PHYSICS_WORLD_DIRTY_MASK mask) {
	_dirtyMask = mask;
}

shared_ptr<PhysicsSimulator> PhysicalWorld::simulator() const {
	return _simulator;
}

void PhysicalWorld::simulator(shared_ptr<PhysicsSimulator> simulator) {
	_simulator = simulator;
}

void PhysicalWorld::attachedToScene(Scene* scene) {
	_scene = scene;
}
