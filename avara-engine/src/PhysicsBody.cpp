//
//  PhysicsBody.cpp
//	avara-engine
//
//  Created by Morgan Davis on 1/26/18.
//  Copyright © 2018 Morgan K Davis. All rights reserved.
//

#include "PhysicsBody.h"


#include "Logger.h"
#include "Node.h"
#include "Scene.h"
#include "PhysicsShape.h"
#include "PhysicsWorld.h"
#include "Utilities.h"


using namespace ae;
using namespace std;
using namespace ae::utils;


/***************************************************************************************
     MARK:   Static
 **************************************************************************************/



/***************************************************************************************
     MARK:   Lifecycle
 **************************************************************************************/

PhysicsBody::PhysicsBody(PhysicsBodyType type):
	/* ... */
	m_mass(1.0f),
	m_type(type),
	m_shape(nullptr),
	m_node(nullptr) {
		
		//auto boxPhysicsShape = make_shared<PhysicsShape>(boxNode->geometry(), PhysicsShapeType_ConvexHull);
	
}

PhysicsBody::PhysicsBody(PhysicsBodyType type, shared_ptr<PhysicsShape> shape):
	/* ... */
	m_mass(1.0f),
	m_type(type),
	m_shape(shape),
	m_node(nullptr) {
	
		
}

/***************************************************************************************
     MARK:   Public
 **************************************************************************************/

float PhysicsBody::mass() const {
	return m_mass;
}

void PhysicsBody::mass(float mass) {
	m_mass = mass;
}

float PhysicsBody::restitution() const {
	return m_restitution;
}

void PhysicsBody::restitution(float restitution) {
	m_restitution = restitution;
}

/***************************************************************************************
     MARK:   Internal
 **************************************************************************************/

void PhysicsBody::addedToNode(Node& node) {
	m_node = &node;
	
//	if (!m_shape) {
//		m_shape = make_shared<PhysicsShape>(m_node->geometry(), PhysicsShapeType_ConvexHull);
//	}
	
	if (m_shape->btShape() == nullptr) {
		m_shape->createBTShape();
	}
	
	btTransform transform;
	transform.setFromOpenGLMatrix(value_ptr(m_node->worldTransform()));
	m_btMotionState = make_shared<btDefaultMotionState>(transform);
	
	
	
	btCollisionShape* collisionShape = static_cast<btCollisionShape*>(m_shape->btShape().get());

	btVector3 localInertia(0, 0, 0);
	if (m_mass != 0) collisionShape->calculateLocalInertia(m_mass, localInertia);
	
	btRigidBody::btRigidBodyConstructionInfo rigidBodyInfo((m_type == PhysicsBodyType_Static ? 0 : m_mass),
														   m_btMotionState.get(),
														   collisionShape,
														   localInertia);
	
	rigidBodyInfo.m_mass = m_mass;
	rigidBodyInfo.m_restitution = m_restitution;
	// TODO: hard-coded4
//	rigidBodyInfo.m_angularDamping = 0.0;
	rigidBodyInfo.m_friction = .25f;
	rigidBodyInfo.m_rollingFriction = .25f;
	

	
	// ionInfo BoxRBCI1(mass, mBoxMotionState1, mBoxShape, boxInertia);
	m_btRigidBody = make_shared<btRigidBody>(rigidBodyInfo);
	
	m_node->scene()->physicsWorld()->btWorld()->addRigidBody(m_btRigidBody.get());
}

shared_ptr<btDefaultMotionState> PhysicsBody::btMotionState() const {
	return m_btMotionState;
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

