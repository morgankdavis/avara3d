//
//  PhysicsShape.cpp
//	avara-engine
//
//  Created by Morgan Davis on 1/25/18.
//  Copyright © 2018 Morgan K Davis. All rights reserved.
//

#include "PhysicsShape.h"

#include <BulletCollision/CollisionShapes/btShapeHull.h>
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


#define SIMPLIFY_CONVEX_HULLS		true


using namespace ae;
using namespace ae::utils;
using namespace glm;
using namespace std;


/***************************************************************************************
     MARK:   Static
 **************************************************************************************/

shared_ptr<btCollisionShape> BTCollisionShapeFromGeometry(shared_ptr<Geometry> geometry,
														  PhysicsShapeType type) {
	AE_LOG->trace("BTCollisionShapeFromGeometry()");
	
	// - if 'type' is PhysicsShapeType_BoundingBox, use box shape
	// - if 'geometry' is a primitive, use matching primitive
	// - if arbitrary mesh, use whatever 'type' is
	
	shared_ptr<btCollisionShape> btShape = nullptr;
	
	if (type == PhysicsShapeType_BoundingBox) {
		vec3 extent = geometry->extent(false);
		float width = extent.x;
		float height = extent.y;
		float length = extent.z;
		btShape = make_shared<btBoxShape>(btVector3((btScalar)width/2.0,
										   (btScalar)height/2.0,
										   (btScalar)length/2.0));
	}
	else if (dynamic_cast<Box*>(geometry.get())) {
		AE_LOG->info("Ignoring physics shape type {}. Using box.", type);
		
		auto box = dynamic_cast<Box*>(geometry.get());
		btShape = make_shared<btBoxShape>(btVector3((btScalar)box->width()/2.0,
													  (btScalar)box->height()/2.0,
													  (btScalar)box->length()/2.0));
	}
	else if (dynamic_cast<Sphere*>(geometry.get())) {
		AE_LOG->info("Ignoring physics shape type {}. Using sphere.", type);
		
		auto sphere = dynamic_cast<Sphere*>(geometry.get());
		btShape = make_shared<btSphereShape>((btScalar)sphere->radius());
	}
	else if (dynamic_cast<Capsule*>(geometry.get())) {
		AE_LOG->info("Ignoring physics shape type {}. Using capsule.", type);
		
		auto capsule = dynamic_cast<Capsule*>(geometry.get());
		btShape = make_shared<btCapsuleShape>((btScalar)capsule->radius(),
												(btScalar)capsule->height());
	}
	else if (dynamic_cast<Cone*>(geometry.get())) {
		AE_LOG->info("Ignoring physics shape type {}. Using cone.", type);
		
		auto cone = dynamic_cast<Cone*>(geometry.get());
		btShape = make_shared<btConeShape>((btScalar)cone->radius(),
											 (btScalar)cone->height());
	}
	else if (dynamic_cast<Cylinder*>(geometry.get())) {
		AE_LOG->info("Ignoring physics shape type {}. Using cylinder.", type);
		
		auto cylinder = dynamic_cast<Cylinder*>(geometry.get());
		btShape = make_shared<btCylinderShape>(btVector3((btScalar)cylinder->radius(),
														   (btScalar)cylinder->height()/2.0,
														   (btScalar)cylinder->radius()));
	}
	else {
		
		if (type == PhysicsShapeType_ConcavePolyhedron) {
			AE_LOG->critical("Concave polyhedron physics shapes not yet supported.");
		}
		else { // PhysicsShapeType_ConvexHull

			unsigned numVerticies = 0;
			for (auto element : geometry->elements()) {
				numVerticies += element->vertices().size();
			}
			vector<Vertex> verticies;
			verticies.reserve(numVerticies);
			
			for (auto element : geometry->elements()) {
				auto elementVerts = element->vertices();
				verticies.insert(verticies.end(), &elementVerts[0], &elementVerts[0] + elementVerts.size());
			}
			
			auto originalShape = make_shared<btConvexHullShape>((const btScalar*)&verticies[0],
																numVerticies,
																sizeof(Vertex));
			
			// reduce number of verticies
			// http://www.bulletphysics.org/mediawiki-1.5.8/index.php/BtShapeHull_vertex_reduction_utility
			
			auto hull = make_shared<btShapeHull>(originalShape.get());
			btScalar margin = originalShape->getMargin();
			hull->buildHull((btScalar)margin);
			
			btShape = make_shared<btConvexHullShape>((btScalar*)hull->getVertexPointer(),
													 hull->numVertices(),
													 sizeof(btVector3));
		}
	}
	
	return btShape;
}

shared_ptr<btCompoundShape> BTCompoundShapeFromNode(shared_ptr<Node> node,
													PhysicsShapeType type) {
	AE_LOG->trace("BTCollisionShapeFromGeometry()");
	
	auto compoundShape = shared_ptr<btCompoundShape>();
	
	auto allNodes = node->childNodes(true);
	for (auto n : allNodes) {
		auto geometry = n->geometry();
		if (geometry) {
			auto collisionShape = BTCollisionShapeFromGeometry(geometry, type);
			btTransform localTransform;
			localTransform.setFromOpenGLMatrix(value_ptr(node->worldTransform()));
			compoundShape->addChildShape((const btTransform)localTransform, collisionShape.get());
		}
	}
	
	return compoundShape;
}

/***************************************************************************************
     MARK:   Lifecycle
 **************************************************************************************/

PhysicsShape::PhysicsShape(shared_ptr<Geometry> geometry, PhysicsShapeType type):
	m_sourceGeometry(geometry),
	m_sourceNode(nullptr),
	m_type(type),
	m_transforms(vector<mat4>()),
	m_btShape(nullptr) {
		
}

// will construct a compound shape based on geometries under this node
PhysicsShape::PhysicsShape(shared_ptr<Node> node, PhysicsShapeType type):
	m_sourceGeometry(nullptr),
	m_sourceNode(node),
	m_type(type),
	m_transforms(vector<mat4>()),
	m_btShape(nullptr) {
		
}

/***************************************************************************************
     MARK:   Public
 **************************************************************************************/

shared_ptr<Geometry> PhysicsShape::sourceGeometry() const {
	return m_sourceGeometry;
}

PhysicsShapeType PhysicsShape::type() const {
	return m_type;
}

/* ? */ vector<glm::mat4> PhysicsShape::transforms() const {
	return m_transforms;
}

/***************************************************************************************
     MARK:   Internal
 **************************************************************************************/

void PhysicsShape::attachedToBody(PhysicsBody& body) {
	m_physicsBody = &body;
	//createBTShape();
	m_btShape = BTCollisionShapeFromGeometry(m_sourceGeometry, m_type);
}

PhysicsBody* PhysicsShape::physicsBody() const {
	return m_physicsBody;
}

void PhysicsShape::physicsBody(PhysicsBody* body) {
	m_physicsBody = body;
}

shared_ptr<btCollisionShape> PhysicsShape::btShape() const {
	return m_btShape;
}

/***************************************************************************************
     MARK:   Private
 **************************************************************************************/

//void PhysicsShape::createBTShape() {
//	AE_LOG->debug("Creating bullet shape...");
//
//	m_btShape = BTCollisionShapeFromGeometry(m_sourceGeometry, m_type);
//}

