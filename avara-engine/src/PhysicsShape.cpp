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
#include "Geometry.h"
#include "GeometryElement.h"
#include "Logger.h"
#include "Plane.h"
#include "Sphere.h"


using namespace ae;
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

void PhysicsShape::createBTShape() {
	AE_LOG->debug("Creating bullet shape...");
	
	// *** CHECK ***
//	PhysicsShapeType_BoundingBox =			0,
//	PhysicsShapeType_ConcavePolyhedron =	1,
//	PhysicsShapeType_ConvexHull =			2
	
	if (dynamic_cast<Box*>(m_sourceGeometry.get())) {
		auto box = dynamic_cast<Box*>(m_sourceGeometry.get());
		m_btShape = make_shared<btBoxShape>(btVector3(box->width()/2.0, box->height()/2.0, box->length()/2.0));
	}
	else if (dynamic_cast<Sphere*>(m_sourceGeometry.get())) {
		auto sphere = dynamic_cast<Sphere*>(m_sourceGeometry.get());
		m_btShape = make_shared<btSphereShape>(sphere->radius());
	}
	
	// cylinder
	// capsule
	// cone
	
//	else if (dynamic_cast<Plane*>(m_sourceGeometry.get())) {
//		auto plane = dynamic_cast<Plane*>(m_sourceGeometry.get());
//		float thickness = (plane->width()/2.0 + plane->height()/2.0) / 50.0;
//		m_btShape = make_shared<btBoxShape>(btVector3(plane->width()/2.0, plane->height()/2.0, thickness));
//	}
	else {
		// some other non-primitive shape
		
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
	
//	// *** TEMPORARY ***
//	m_btShape->setMargin(0);
//	AE_LOG->debug("m_btShape->getMargin(): {}", m_btShape->getMargin());
}

shared_ptr<btCollisionShape> PhysicsShape::btShape() const {
	return m_btShape;
}
