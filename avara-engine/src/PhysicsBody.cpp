//
//  PhysicsBody.cpp
//	avara-engine
//
//  Created by Morgan Davis on 1/26/18.
//  Copyright © 2018 Morgan K Davis. All rights reserved.
//

#include "PhysicsBody.h"


#include "Node.h"
#include "Scene.h"
#include "PhysicsWorld.h"


using namespace ae;
using namespace std;


/***************************************************************************************
     MARK:   Static
 **************************************************************************************/



/***************************************************************************************
     MARK:   Lifecycle
 **************************************************************************************/

PhysicsBody::PhysicsBody(PhysicsBodyType type):
	/* ... */
	m_type(type),
	m_node(nullptr) {
	
}

PhysicsBody::PhysicsBody(PhysicsBodyType type, shared_ptr<PhysicsShape> shape):
	/* ... */
	m_node(nullptr) {
	
		
}

shared_ptr<btDefaultMotionState> PhysicsBody::btMotionState() const {
	return m_btMotionState;
}

//void PhysicsBody::btMotionState(shared_ptr<btDefaultMotionState> motionState) {
//	
//}

/***************************************************************************************
     MARK:   Internal
 **************************************************************************************/

void PhysicsBody::addedToNode(Node& node) {
	m_node = &node;
	
	
	btBoxShape* boxShape = new btBoxShape(btVector3(1.0f, 1.0f, 1.0f));
	
	
	
	btTransform transform;
	transform.setFromOpenGLMatrix(value_ptr(m_node->worldTransform()));
	//transform.setIdentity();
	//transform.setOrigin(btVector3(0.0f, 0.0f, 0.0f));
	// t.getOpenGLMatrix(glm::value_ptr(WoodenCrateInstances.at(i).transform));
	

	// *** may not be right... ***
	// http://bulletphysics.org/Bullet/BulletFull/structbtDefaultMotionState.html
	m_btMotionState = make_shared<btDefaultMotionState>(transform);
	
	
	btRigidBody::btRigidBodyConstructionInfo rbInfo(1.0f, m_btMotionState.get(), boxShape);
	btRigidBody* rigidBody = new btRigidBody(rbInfo);
	//m_btRigidBody = make_shared<btRigidBody>(rbInfo);
	
	//m_world->addRigidBody(rigidBody);
	
	
	m_node->scene()->physicsWorld()->btWorld()->addRigidBody(rigidBody);
}

/***************************************************************************************
     MARK:   Private
 **************************************************************************************/

//Node* PhysicsBody::node() const {
//	return m_node;
//}
//
//void PhysicsBody::node(Node* node) {
//	m_node = node;
//}

