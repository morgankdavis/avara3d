//
//  PhysicsShape.cpp
//	avara-engine
//
//  Created by Morgan Davis on 1/25/18.
//  Copyright © 2018 Morgan K Davis. All rights reserved.
//

#include "PhysicsShape.h"

#include <glm/glm.hpp>

#include "Box.h"
#include "Capsule.h"
#include "Cone.h"
#include "Cylinder.h"
#include "Geometry.h"
#include "GeometryElement.h"
#include "Logger.h"
#include "Plane.h"
#include "Sphere.h"
#include "Utilities.h"


using namespace ae;
using namespace ae::utils;
using namespace glm;
using namespace std;


/***************************************************************************************
     MARK:   Lifecycle
 **************************************************************************************/

PhysicsShape::PhysicsShape(shared_ptr<Geometry> geometry, PhysicsShapeType type):
	m_sourceGeometry(geometry),
	m_type(type),
	m_transforms(vector<mat4>()),
	m_btShape(nullptr),
	m_btTriangleMesh(nullptr) {
		
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
	createBTShape();
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

void PhysicsShape::createBTShape() {
	AE_LOG->debug("Creating bullet shape...");

	// - if 'type' is PhysicsShapeType_BoundingBox, use box shape
	// - if 'geometry' is a primitive, use matching primitive
	// - if arbitrary mesh, use whatever 'type' is
	
	if (m_type == PhysicsShapeType_BoundingBox) {
		vec3 extent = m_sourceGeometry->extent(false);
		float width = extent.x;
		float height = extent.y;
		float length = extent.z;
		m_btShape = make_shared<btBoxShape>(btVector3(width/2.0,
													  height/2.0,
													  length/2.0));
	}
	else if (dynamic_cast<Box*>(m_sourceGeometry.get())) {
		AE_LOG->info("Ignoring physics shape type {}. Using box.", m_type);
		
		auto box = dynamic_cast<Box*>(m_sourceGeometry.get());
		m_btShape = make_shared<btBoxShape>(btVector3(box->width()/2.0,
													  box->height()/2.0,
													  box->length()/2.0));
	}
	else if (dynamic_cast<Sphere*>(m_sourceGeometry.get())) {
		AE_LOG->info("Ignoring physics shape type {}. Using sphere.", m_type);
		
		auto sphere = dynamic_cast<Sphere*>(m_sourceGeometry.get());
		m_btShape = make_shared<btSphereShape>(sphere->radius());
	}
	else if (dynamic_cast<Capsule*>(m_sourceGeometry.get())) {
		//AE_LOG->critical("Capsule physics shapes not yet supported.");
		AE_LOG->info("Ignoring physics shape type {}. Using capsule.", m_type);
		
		auto capsule = dynamic_cast<Capsule*>(m_sourceGeometry.get());
		m_btShape = make_shared<btCapsuleShape>(capsule->radius(), capsule->height());
	}
	else if (dynamic_cast<Cone*>(m_sourceGeometry.get())) {
		AE_LOG->critical("Cone physics shapes not yet supported.");
	}
	else if (dynamic_cast<Cylinder*>(m_sourceGeometry.get())) {
		AE_LOG->critical("Cylinder physics shapes not yet supported.");
	}
	else {
		if (m_type == PhysicsShapeType_ConcavePolyhedron) {
			AE_LOG->critical("Concave polyhedron physics shapes not yet supported.");
		}
		else { // PhysicsShapeType_ConvexHull
			unsigned numVerticies = 0;
			for (auto element : m_sourceGeometry->elements()) {
				numVerticies += element->vertices().size();
			}
			vector<Vertex> verticies;
			verticies.reserve(numVerticies);
			
			for (auto element : m_sourceGeometry->elements()) {
				auto elementVerts = element->vertices();
				verticies.insert(verticies.end(), &elementVerts[0], &elementVerts[0] + elementVerts.size());
			}
			
			m_btShape = make_shared<btConvexHullShape>((const btScalar*)&verticies[0],
													   numVerticies,
													   sizeof(Vertex));	
		}
	}
	
	

//	btCapsuleShape
//		btConeShape 
//	btCylinderShape
//	
//	if (dynamic_cast<Box*>(m_sourceGeometry.get())) {
//		auto box = dynamic_cast<Box*>(m_sourceGeometry.get());
//		m_btShape = make_shared<btBoxShape>(btVector3(box->width()/2.0, box->height()/2.0, box->length()/2.0));
//	}
//	else if (dynamic_cast<Sphere*>(m_sourceGeometry.get())) {
//		auto sphere = dynamic_cast<Sphere*>(m_sourceGeometry.get());
//		m_btShape = make_shared<btSphereShape>(sphere->radius());
//	}
//	
//	// cylinder
//	// capsule
//	// cone
//	
//	//	else if (dynamic_cast<Plane*>(m_sourceGeometry.get())) {
//	//		auto plane = dynamic_cast<Plane*>(m_sourceGeometry.get());
//	//		float thickness = (plane->width()/2.0 + plane->height()/2.0) / 50.0;
//	//		m_btShape = make_shared<btBoxShape>(btVector3(plane->width()/2.0, plane->height()/2.0, thickness));
//	//	}
//	else {
//		// some other non-primitive shape
//		
//		unsigned numVerticies = 0;
//		for (auto element : m_sourceGeometry->elements()) {
//			numVerticies += element->vertices().size();
//		}
//		vector<Vertex> verticies;
//		verticies.reserve(numVerticies);
//		
//		for (auto element : m_sourceGeometry->elements()) {
//			auto elementVerts = element->vertices();
//			verticies.insert(verticies.end(), &elementVerts[0], &elementVerts[0] + elementVerts.size());
//		}
//		
//		m_btShape = make_shared<btConvexHullShape>((const btScalar*)&verticies[0],
//												   numVerticies,
//												   sizeof(Vertex));	
//	}
}
