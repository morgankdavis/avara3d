//
//  PhysicsShape.cpp
//	avara-engine
//
//  Created by Morgan Davis on 1/25/18.
//  Copyright © 2018 Morgan K Davis. All rights reserved.
//

#include "PhysicsShape.h"

//#include <BulletCollision/CollisionShapes/btShapeHull.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Box.h"
#include "Capsule.h"
#include "Cone.h"
#include "Cylinder.h"
#include "Geometry.h"
#include "GeometryElement.h"
#include "Logger.h"
#include "Node.h"
#include "Plane.h"
#include "Sphere.h"
#include "Utilities.h"


using namespace ae;
using namespace ae::utils;
using namespace glm;
using namespace std;


/***************************************************************************************
     Lifecycle
 ***************************************************************************************/

PhysicsShape::PhysicsShape(shared_ptr<Geometry> geometry, PHYSICS_SHAPE_TYPE type):
	m_sourceGeometry({}),
	m_sourceNode({}),
	m_type(PHYSICS_SHAPE_TYPE::CONVEX_HULL),
	m_transforms(vector<mat4>()),
	m_physicsBody({}),
	m_simulationID(0),
	m_dirtyBits(PHYSICS_SHAPE_DIRTY_BITS::ALL) {
		
		m_sourceGeometry = geometry;
		m_type = type;
}

// construct a compound shape based on geometries under this node
PhysicsShape::PhysicsShape(shared_ptr<Node> node, PHYSICS_SHAPE_TYPE type):
	m_sourceGeometry({}),
	m_sourceNode({}),
	m_type(PHYSICS_SHAPE_TYPE::CONVEX_HULL),
	m_transforms(vector<mat4>()),
	m_physicsBody({}),
	m_simulationID(0),
	m_dirtyBits(PHYSICS_SHAPE_DIRTY_BITS::ALL) {
		
		m_sourceNode = node;
		m_type = type;
}

/***************************************************************************************
     Public
 ***************************************************************************************/

weak_ptr<Geometry> PhysicsShape::sourceGeometry() const {
	return m_sourceGeometry;
}

weak_ptr<Node> PhysicsShape::sourceNode() const {
	return m_sourceNode;
}

PHYSICS_SHAPE_TYPE PhysicsShape::type() const {
	return m_type;
}

/* ? */ vector<glm::mat4> PhysicsShape::transforms() const {
	return m_transforms;
}

/***************************************************************************************
     Internal
 ***************************************************************************************/

void PhysicsShape::attachedToBody(shared_ptr<PhysicsBody> body) {
	m_physicsBody = body;
}

//vector<shared_ptr<btCollisionShape>>& PhysicsShape::childShapes() {
//	return m_childShapes;
//}

weak_ptr<PhysicsBody> PhysicsShape::physicsBody() const {
	return m_physicsBody;
}

void PhysicsShape::physicsBody(shared_ptr<PhysicsBody> body) {
	m_physicsBody = body;
	attachedToBody(body);
}

PHYSICS_SHAPE_ID PhysicsShape::simulationID() const {
	return m_simulationID;
}

void PhysicsShape::simulationID(PHYSICS_SHAPE_ID simID) {
	m_simulationID = simID;
}

PHYSICS_SHAPE_DIRTY_BITS PhysicsShape::dirtyBits() const {
	return m_dirtyBits;
}

void PhysicsShape::dirtyBits(PHYSICS_SHAPE_DIRTY_BITS bits) {
	m_dirtyBits = bits;
}
