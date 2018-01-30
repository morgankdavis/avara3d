//
//  PhysicsShape.cpp
//	avara-engine
//
//  Created by Morgan Davis on 1/25/18.
//  Copyright © 2018 Morgan K Davis. All rights reserved.
//

#include "PhysicsShape.h"

#include <glm/glm.hpp>

#include "Geometry.h"
#include "GeometryElement.h"
#include "Logger.h"


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
	
	unsigned numVerticies = 0;
	for (auto element : m_sourceGeometry->elements()) {
		numVerticies += element->vertices().size();
	}
	
//	switch (type) {
//		case PhysicsBodyType_Static: {
//
//			m_btTriangleMesh = make_shared<btTriangleMesh>();
//
//			for (auto element : m_sourceGeometry->elements()) {
//				auto verticies = element->vertices();
//				for (auto face : element->faces()) {
//					auto vertA = verticies[face.a].position;
//					auto vertB = verticies[face.b].position;
//					auto vertC = verticies[face.c].position;
//					
//					m_btTriangleMesh->addTriangle(*((btVector3*)(&vertA)),
//												  *((btVector3*)(&vertB)),
//												  *((btVector3*)(&vertC)),
//												  false);
//				}
//			}
//
//			m_btShape = make_shared<btBvhTriangleMeshShape>(m_btTriangleMesh.get(), false);
//
//			break; }
//			
//		case PhysicsBodyType_Dynamic: {
			
			vector<Vertex> verticies;
			verticies.reserve(numVerticies);
			
			for (auto element : m_sourceGeometry->elements()) {
				auto elementVerts = element->vertices();
				verticies.insert(verticies.end(), &elementVerts[0], &elementVerts[0] + elementVerts.size());
			}
			
			m_btShape = make_shared<btConvexHullShape>((const btScalar*)&verticies[0],
													   numVerticies,
													   sizeof(Vertex));
			
//			break; }
//			
//		case PhysicsBodyType_Kinematic: {
//			break; }
//	}
}

shared_ptr<btCollisionShape> PhysicsShape::btShape() const {
	return m_btShape;
}
