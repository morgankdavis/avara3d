//
//  PhysicsWorld.cc
//	avara-engine
//
//  Created by Morgan Davis on 1/26/18.
//  Copyright © 2018 Morgan K Davis. All rights reserved.
//

#include "physics/PhysicsWorld.h"

#include "diagnostic/logging/Logger.h"


using namespace ae;
using namespace glm;
using namespace std;


/*********************************************************************************************
	Lifecycle
 *********************************************************************************************/

PhysicsWorld::PhysicsWorld():
	_gravity({0, -9.807, 0}),
	_timestep(1.0/60.0),
//	_scene({}),
	_dirtyBits(PHYSICS_WORLD_DIRTY_BITS::ALL) {

}

PhysicsWorld::~PhysicsWorld() {
	AE_LOG_D("Destroying PhysicsWorld {:p}", (void*)this);
}

/*********************************************************************************************
	Public
 *********************************************************************************************/

vec3 PhysicsWorld::gravity() const {
	return _gravity;
}

void PhysicsWorld::gravity(vec3 gravity) {
	_gravity = gravity;
	
	_dirtyBits = PHYSICS_WORLD_DIRTY_BITS_ADD(_dirtyBits, PHYSICS_WORLD_DIRTY_BITS::GRAVITY);
}

float PhysicsWorld::timestep() const {
	return _timestep;
}

void PhysicsWorld::timestep(float timestep) {
	_timestep = timestep;
	
	_dirtyBits = PHYSICS_WORLD_DIRTY_BITS_ADD(_dirtyBits, PHYSICS_WORLD_DIRTY_BITS::TIMESTEP);
}

void PhysicsWorld::updateCollisionPairs() {
	#warning FIX
	//_btWorld->getCollisionWorld()->computeOverlappingPairs();
}

shared_ptr<PhysicsContact> PhysicsWorld::contactTest(shared_ptr<PhysicsBody> bodyA,
													 shared_ptr<PhysicsBody> bodyB) {
	
	// contactPairTest (btCollisionObject *colObjA, btCollisionObject *colObjB, ContactResultCallback &resultCallback)
	
	return nullptr;
}

shared_ptr<PhysicsContact> PhysicsWorld::contactTest(shared_ptr<PhysicsBody> body) {
	
	// contactTest (btCollisionObject *colObj, ContactResultCallback &resultCallback)
	
	return nullptr;
}

shared_ptr<HitTestResult> PhysicsWorld::rayTest(vec3 fromVec, vec3 toVec) {
	
	//rayTest (const btVector3 &rayFromWorld, const btVector3 &rayToWorld, RayResultCallback &resultCallback) const
	
	return nullptr;
}

shared_ptr<PhysicsContact> PhysicsWorld::convexSweepTest(shared_ptr<PhysicsContact> contact,
														 const mat4& fromMat,
														 const mat4& toMat) {
	
	// convexSweepTest (const btConvexShape *castShape, const btTransform &from, const btTransform &to, ConvexResultCallback &resultCallback, btScalar allowedCcdPenetration=btScalar(0.)) const 

	return nullptr;
}

/*********************************************************************************************
	Internal
 *********************************************************************************************/

//void PhysicsWorld::attachedToScene(shared_ptr<Scene> scene) {
//	_scene = scene;
////	if (auto window = scene->window().lock()) {
////		debugOptions(window->debugOptions());
////	}
////#ifdef DESKTOP
////	if (auto renderer = scene->renderContext().lock()) {
////		debugOptions(renderer->debugOptions());
////	}
////#endif
//}

PHYSICS_WORLD_DIRTY_BITS PhysicsWorld::dirtyBits() const {
	return _dirtyBits;
}

void PhysicsWorld::dirtyBits(PHYSICS_WORLD_DIRTY_BITS bits) {
	_dirtyBits = bits;
}
