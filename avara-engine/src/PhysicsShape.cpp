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

shared_ptr<btCollisionShape> BTConvexHullShapeFromGeometry(Geometry& geometry) {
	
	auto elements = geometry.elements();
	
	if (elements.size() > 0) {
		
		vector<btConvexHullShape> convexHullShapes;
		convexHullShapes.resize(elements.size());
		
		for (auto element : elements) {
//			vector<Vertex> verticies;
//			auto elementVerts = element->vertices();
//			//verticies.insert(verticies.end(), &elementVerts[0], &elementVerts[0] + elementVerts.size());
//			verticies.insert(verticies.end(), elementVerts.begin(), elementVerts.end());
//			
//			auto fullShape = btConvexHullShape((const btScalar*)&verticies[0],
//											   element->vertices().size(),
//											   sizeof(Vertex));
			
//			vector<Vertex> verticies;
//			auto elementVerts = element->vertices();
//			//verticies.insert(verticies.end(), &elementVerts[0], &elementVerts[0] + elementVerts.size());
//			verticies.insert(verticies.end(), elementVerts.begin(), elementVerts.end());
			
			auto fullShape = btConvexHullShape((const btScalar*)&element->vertices()[0],
											   element->vertices().size(),
											   sizeof(Vertex));
			
			if (SIMPLIFY_CONVEX_HULLS) {
				// reduce number of verticies
				// http://www.bulletphysics.org/mediawiki-1.5.8/index.php/BtShapeHull_vertex_reduction_utility
				
				auto hull = make_shared<btShapeHull>(&fullShape);
				btScalar margin = fullShape.getMargin();
				hull->buildHull((btScalar)margin);
				
				convexHullShapes.emplace_back((btScalar*)hull->getVertexPointer(),
											  hull->numVertices(),
											  sizeof(btVector3));
				
				//			return make_shared<btConvexHullShape>((btScalar*)hull->getVertexPointer(),
				//												  hull->numVertices(),
				//												  sizeof(btVector3));
			}
			else {
				//			return make_shared<btConvexHullShape>(fullShape);
				
				convexHullShapes.emplace_back(fullShape);
			}
		}
		
		if (elements.size() > 1) { // compound shape
			btCompoundShape compoundShape;
			
			for (auto hullShape : convexHullShapes) {
				btTransform localTransform;
				localTransform.setFromOpenGLMatrix(value_ptr(mat4(1.0)));
				compoundShape.addChildShape((const btTransform)localTransform, (btCollisionShape *)&hullShape);
			}
			
			return make_shared<btCompoundShape>(&compoundShape);
		}
		else {
			return make_shared<btConvexHullShape>(convexHullShapes[0]);
		}
	}
	else {
		return nullptr;
	}
	
	
	
//	if (elements.size() > 1) { // compound shape
//		
//	}
//	else {
//		
//	}
//	
//	//			unsigned numVerticies = 0;
//	//			for (auto element : m_sourceGeometry->elements()) {
//	//				numVerticies += element->vertices().size();
//	//			}
//	//			vector<Vertex> verticies;
//	//			verticies.reserve(numVerticies);
//	//			
//	//			for (auto element : m_sourceGeometry->elements()) {
//	//				auto elementVerts = element->vertices();
//	//				verticies.insert(verticies.end(), &elementVerts[0], &elementVerts[0] + elementVerts.size());
//	//			}
//	//			
//	//			m_btShape = make_shared<btConvexHullShape>((const btScalar*)&verticies[0],
//	//													   numVerticies,
//	//													   sizeof(Vertex));
//	
//	
//	unsigned numVerticies = 0;
//	for (auto element : elements) {
//		numVerticies += element->vertices().size();
//	}
//	vector<Vertex> verticies;
//	verticies.reserve(numVerticies);
//	
//	for (auto element : elements) {
//		auto elementVerts = element->vertices();
//		verticies.insert(verticies.end(), &elementVerts[0], &elementVerts[0] + elementVerts.size());
//	}
//	
//	auto originalShape = make_shared<btConvexHullShape>((const btScalar*)&verticies[0],
//														numVerticies,
//														sizeof(Vertex));
//	
//	// reduce number of verticies
//	// http://www.bulletphysics.org/mediawiki-1.5.8/index.php/BtShapeHull_vertex_reduction_utility
//	
//	auto hull = make_shared<btShapeHull>(originalShape.get());
//	btScalar margin = originalShape->getMargin();
//	hull->buildHull((btScalar)margin);
//	
//	return make_shared<btConvexHullShape>((btScalar*)hull->getVertexPointer(),
//										  hull->numVertices(),
//										  sizeof(btVector3));
}

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
		m_btShape = make_shared<btBoxShape>(btVector3((btScalar)width/2.0,
													  (btScalar)height/2.0,
													  (btScalar)length/2.0));
	}
	else if (dynamic_cast<Box*>(m_sourceGeometry.get())) {
		AE_LOG->info("Ignoring physics shape type {}. Using box.", m_type);
		
		auto box = dynamic_cast<Box*>(m_sourceGeometry.get());
		m_btShape = make_shared<btBoxShape>(btVector3((btScalar)box->width()/2.0,
													  (btScalar)box->height()/2.0,
													  (btScalar)box->length()/2.0));
	}
	else if (dynamic_cast<Sphere*>(m_sourceGeometry.get())) {
		AE_LOG->info("Ignoring physics shape type {}. Using sphere.", m_type);
		
		auto sphere = dynamic_cast<Sphere*>(m_sourceGeometry.get());
		m_btShape = make_shared<btSphereShape>((btScalar)sphere->radius());
	}
	else if (dynamic_cast<Capsule*>(m_sourceGeometry.get())) {
		AE_LOG->info("Ignoring physics shape type {}. Using capsule.", m_type);
		
		auto capsule = dynamic_cast<Capsule*>(m_sourceGeometry.get());
		m_btShape = make_shared<btCapsuleShape>((btScalar)capsule->radius(),
												(btScalar)capsule->height());
	}
	else if (dynamic_cast<Cone*>(m_sourceGeometry.get())) {
		//AE_LOG->critical("Cone physics shapes not yet supported.");
		AE_LOG->info("Ignoring physics shape type {}. Using cone.", m_type);
		
		auto cone = dynamic_cast<Cone*>(m_sourceGeometry.get());
		m_btShape = make_shared<btConeShape>((btScalar)cone->radius(),
											 (btScalar)cone->height());
	}
	else if (dynamic_cast<Cylinder*>(m_sourceGeometry.get())) {
		//AE_LOG->critical("Cylinder physics shapes not yet supported.");
		AE_LOG->info("Ignoring physics shape type {}. Using cylinder.", m_type);
		
		auto cylinder = dynamic_cast<Cylinder*>(m_sourceGeometry.get());
		m_btShape = make_shared<btCylinderShape>(btVector3((btScalar)cylinder->radius(),
														   (btScalar)cylinder->height()/2.0,
														   (btScalar)cylinder->radius()));
	}
	else {
		
		if (m_type == PhysicsShapeType_ConcavePolyhedron) {
			AE_LOG->critical("Concave polyhedron physics shapes not yet supported.");
		}
		else { // PhysicsShapeType_ConvexHull
			
			//			unsigned numVerticies = 0;
			//			for (auto element : m_sourceGeometry->elements()) {
			//				numVerticies += element->vertices().size();
			//			}
			//			vector<Vertex> verticies;
			//			verticies.reserve(numVerticies);
			//			
			//			for (auto element : m_sourceGeometry->elements()) {
			//				auto elementVerts = element->vertices();
			//				verticies.insert(verticies.end(), &elementVerts[0], &elementVerts[0] + elementVerts.size());
			//			}
			//			
			//			m_btShape = make_shared<btConvexHullShape>((const btScalar*)&verticies[0],
			//													   numVerticies,
			//													   sizeof(Vertex));
			
			
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
			
			auto originalShape = make_shared<btConvexHullShape>((const btScalar*)&verticies[0],
																numVerticies,
																sizeof(Vertex));
			
			// reduce number of verticies
			// http://www.bulletphysics.org/mediawiki-1.5.8/index.php/BtShapeHull_vertex_reduction_utility
			
			auto hull = make_shared<btShapeHull>(originalShape.get());
			btScalar margin = originalShape->getMargin();
			hull->buildHull((btScalar)margin);
			
			m_btShape = make_shared<btConvexHullShape>((btScalar*)hull->getVertexPointer(),
													   hull->numVertices(),
													   sizeof(btVector3));
		}
		
//		if (m_type == PhysicsShapeType_ConcavePolyhedron) {
//			AE_LOG->critical("Concave polyhedron physics shapes not yet supported.");
//		}
//		else { // PhysicsShapeType_ConvexHull
//			
//			m_btShape = BTConvexHullShapeFromGeometry(*m_sourceGeometry);
//		}
	}
	
	//m_btShape->setMargin(0);
}
