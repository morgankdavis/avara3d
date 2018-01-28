//
//  PhysicsWorld.cpp
//	avara-engine
//
//  Created by Morgan Davis on 1/26/18.
//  Copyright © 2018 Morgan K Davis. All rights reserved.
//

#include "PhysicsWorld.h"

//#include "btBulletDynamicsCommon.h"


using namespace ae;
using namespace std;


/***************************************************************************************
     MARK:   Lifecycle
 **************************************************************************************/

PhysicsWorld::PhysicsWorld() {
	
	m_btCollisionConfiguration = make_shared<btDefaultCollisionConfiguration>();
	m_btDispatcher = make_shared<btCollisionDispatcher>(&(*m_btCollisionConfiguration));
	m_btBroadphase = make_shared<btDbvtBroadphase>();
	m_btSolver = make_shared<btSequentialImpulseConstraintSolver>();
	m_btWorld = make_shared<btDiscreteDynamicsWorld>(&(*m_btDispatcher),
													 &(*m_btBroadphase),
													 &(*m_btSolver),
													 &(*m_btCollisionConfiguration));
}

/***************************************************************************************
     MARK:   Internal
 **************************************************************************************/

void PhysicsWorld::step(float deltaTime) {
	
}
