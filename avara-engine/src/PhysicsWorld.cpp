//
//  PhysicsWorld.cpp
//	avara-engine
//
//  Created by Morgan Davis on 1/26/18.
//  Copyright © 2018 Morgan K Davis. All rights reserved.
//

#include "PhysicsWorld.h"

#include <GLFW/glfw3.h>

#include "Logger.h"
#include "Utilities.h"


using namespace ae;
using namespace ae::utils;
using namespace glm;
using namespace std;


/***************************************************************************************
     MARK:   Lifecycle
 **************************************************************************************/

PhysicsWorld::PhysicsWorld():
	m_gravity({0, -9.807, 0}),
	m_speed(1.0),
	m_timestep(1.0/60.0) {
	
		m_btCollisionConfiguration = make_shared<btDefaultCollisionConfiguration>();
		m_btDispatcher = make_shared<btCollisionDispatcher>(m_btCollisionConfiguration.get());
		m_btBroadphase = make_shared<btDbvtBroadphase>();
		m_btSolver = make_shared<btSequentialImpulseConstraintSolver>();
		m_btWorld = make_shared<btDiscreteDynamicsWorld>(m_btDispatcher.get(),
														 m_btBroadphase.get(),
														 m_btSolver.get(),
														 m_btCollisionConfiguration.get());
		
		gravity(m_gravity);
}

/***************************************************************************************
     MARK:   Public
 **************************************************************************************/

vec3 PhysicsWorld::gravity() const {
	return m_gravity;
}

void PhysicsWorld::gravity(vec3 gravity) {
	m_gravity = gravity;
	m_btWorld->setGravity(BTVector3FromGLMVec3(gravity));
}

float PhysicsWorld::speed() const {
	return m_speed;
}

void PhysicsWorld::speed(float speed) {
	m_speed = speed;
}

float PhysicsWorld::timestep() const {
	return m_timestep;
}

void PhysicsWorld::timestep(float timestep) {
	m_timestep = timestep;
}

void PhysicsWorld::updateCollisionPairs() {
	
}

shared_ptr<PhysicsContact> PhysicsWorld::contactTest(shared_ptr<PhysicsBody> bodyA,
													 shared_ptr<PhysicsBody> bodyB) {
	return nullptr;
}

shared_ptr<PhysicsContact> PhysicsWorld::contactTest(shared_ptr<PhysicsBody> body) {
	return nullptr;
}

shared_ptr<HitTestResult> PhysicsWorld::rayTest(vec3 fromVec, vec3 toVec) {
	return nullptr;
}

shared_ptr<PhysicsContact> PhysicsWorld::convexSweepTest(shared_ptr<PhysicsContact> contact,
														 const mat4& fromMat,
														 const mat4& toMat) {
	return nullptr;
}

/***************************************************************************************
     MARK:   Internal
 **************************************************************************************/

//void PhysicsWorld::step(float deltaTime) {
void PhysicsWorld::step() {
	AE_LOG->trace("step()");
	
	float time = glfwGetTime();
	static double previousSeconds = time;
	float deltaSeconds = time - previousSeconds;
	previousSeconds = time;
	
	//m_btWorld->stepSimulation(deltaSeconds);
	m_btWorld->stepSimulation(deltaSeconds, 5, m_timestep);
	
// http://bulletphysics.org/mediawiki-1.5.8/index.php/Stepping_The_World
//	btDynamicsWorld::stepSimulation(
//									btScalar timeStep,
//									int maxSubSteps=1,
//									btScalar fixedTimeStep=btScalar(1.)/btScalar(60.));
}

shared_ptr<btDiscreteDynamicsWorld> PhysicsWorld::btWorld() const {
	return m_btWorld;
}
