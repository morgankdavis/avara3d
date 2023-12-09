//
//  PhysicsSimulator.cc
//	avara-engine
//
//  Created by Morgan Davis on 5/1/18.
//  Copyright © 2018 Morgan K Davis. All rights reserved.
//

#include "physics/PhysicsSimulator.h"

#include "diagnostic/logging/Logger.h"
#include "scene/Node.h"
#include "physics/PhysicsBody.h"
#include "physics/PhysicsShape.h"
#include "scene/Scene.h"


using namespace ae;
using namespace glm;
using namespace std;


/*********************************************************************************************
	Lifescycle
 *********************************************************************************************/

PhysicsSimulator::PhysicsSimulator() {}/*:
		_gravity({0, -9.807, 0}),
		_speed(1.0),
		_timestep(1.0/60.0) { }*/

PhysicsSimulator::~PhysicsSimulator() {
	AE_LOG_D("Destroying PhysicsSimulator {:p}", (void*)this);
}

/*********************************************************************************************
	Internal
 *********************************************************************************************/

//void PhysicsSimulator::setTimestep(PhysicalWorld& world, float timestep) { }
//void PhysicsSimulator::setSpeed(PhysicalWorld& world, float speed) { }
void PhysicsSimulator::setGravity(PhysicalWorld& world, glm::vec3& gravity) { }

void PhysicsSimulator::create(PhysicsBody& body) { }
void PhysicsSimulator::remove(PhysicsBody& body) { }

void PhysicsSimulator::setType(PhysicsBody& body, PHYSICS_BODY_TYPE type) { }
void PhysicsSimulator::setShape(PhysicsBody& body, PhysicsShape& shape) { }
void PhysicsSimulator::setWorldTransform(PhysicsBody& body, const glm::mat4& transform) { }
void PhysicsSimulator::setMass(PhysicsBody& body, float mass) { }
void PhysicsSimulator::setMomentOfInertia(PhysicsBody& body, const glm::vec3& moment) { }
void PhysicsSimulator::setFriction(PhysicsBody& body, float friction) { }
void PhysicsSimulator::setRollingFriction(PhysicsBody& body, float friction) { }
void PhysicsSimulator::setRestitution(PhysicsBody& body, float restitution) { }
void PhysicsSimulator::setLinearVelocity(PhysicsBody& body, const glm::vec3& velocity) { }
void PhysicsSimulator::setAngularVelocity(PhysicsBody& body, const glm::vec3& velocity) { }
void PhysicsSimulator::setLinearFactor(PhysicsBody& body, const glm::vec3& factor) { }
void PhysicsSimulator::setAngularFactor(PhysicsBody& body, const glm::vec3& factor) { }
void PhysicsSimulator::setLinearDamping(PhysicsBody& body, float damping) { }
void PhysicsSimulator::setAngularDamping(PhysicsBody& body, float damping) { }
void PhysicsSimulator::setLinearSleepingThreshold(PhysicsBody& body, float threshold) { }
void PhysicsSimulator::setAngularSleepingThreshold(PhysicsBody& body, float threshold) { }
void PhysicsSimulator::setAffectedByGravity(PhysicsBody& body, bool flag) { }
void PhysicsSimulator::setAllowsResting(PhysicsBody& body, bool flag) { }

void PhysicsSimulator::create(PhysicsShape& shape) { }

void PhysicsSimulator::step(PhysicalWorld& world, float deltaT) { }

void PhysicsSimulator::sync(PhysicsBody& body, mat4& worldTransform) { }
