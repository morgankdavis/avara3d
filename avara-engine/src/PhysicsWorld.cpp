//
//  PhysicsWorld.cpp
//	avara-engine
//
//  Created by Morgan Davis on 1/26/18.
//  Copyright © 2018 Morgan K Davis. All rights reserved.
//

#include "PhysicsWorld.h"

//#include "btBulletDynamicsCommon.h"
#include <GLFW/glfw3.h>

#include "Logger.h"


using namespace ae;
using namespace std;


/***************************************************************************************
     MARK:   Lifecycle
 **************************************************************************************/

PhysicsWorld::PhysicsWorld() {

	m_btCollisionConfiguration = make_shared<btDefaultCollisionConfiguration>();
	m_btDispatcher = make_shared<btCollisionDispatcher>(m_btCollisionConfiguration.get());
	m_btBroadphase = make_shared<btDbvtBroadphase>();
	m_btSolver = make_shared<btSequentialImpulseConstraintSolver>();
	m_btWorld = make_shared<btDiscreteDynamicsWorld>(m_btDispatcher.get(),
													 m_btBroadphase.get(),
													 m_btSolver.get(),
													 m_btCollisionConfiguration.get());
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
	m_btWorld->stepSimulation(deltaSeconds, 5);
	
// http://bulletphysics.org/mediawiki-1.5.8/index.php/Stepping_The_World
//	btDynamicsWorld::stepSimulation(
//									btScalar timeStep,
//									int maxSubSteps=1,
//									btScalar fixedTimeStep=btScalar(1.)/btScalar(60.));
}

shared_ptr<btDiscreteDynamicsWorld> PhysicsWorld::btWorld() const {
	return m_btWorld;
}
