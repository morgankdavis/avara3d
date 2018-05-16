//
//  BulletPhysicsSimulator.cpp
//	avara-engine
//
//  Created by Morgan Davis on 5/1/18.
//  Copyright © 2018 Morgan K Davis. All rights reserved.
//

#include "BulletPhysicsSimulator.h"

#include <btBulletCollisionCommon.h>
#include <btBulletDynamicsCommon.h>
#include <BulletCollision/CollisionShapes/btShapeHull.h>
#include <glm/gtc/type_ptr.hpp>
#include <LinearMath/btScalar.h> // btGetVersion() !

#include "Box.h"
#include "Capsule.h"
#include "Cone.h"
#include "Cylinder.h"
#include "Geometry.h"
#include "GeometryElement.h"
#include "Logger.h"
#include "Node.h"
#include "PhysicsDebugDrawer.h"
#include "PhysicsBody.h"
#include "PhysicsShape.h"
#include "PhysicsWorld.h"
#include "Scene.h"
#include "Sphere.h"


using namespace ae;
using namespace glm;
using namespace std;


/**************************************************************************************
     Static Prototypes
 **************************************************************************************/

void GetPhysicsBodyBTModels(PhysicsBody& body,
							btRigidBody** btBody,
							btDefaultMotionState** btMotionState,
							btCollisionShape** btShape,
							vector<btCollisionShape*>& btShapes,
							btDiscreteDynamicsWorld& btWorld,
							BulletPhysicsSimulator::PhysicsBodyIDMapping& bodyIDMapping,
							PHYSICS_BODY_ID& bodyIDCounter,
							BulletPhysicsSimulator::PhysicsShapeIDMapping& shapeIDMapping,
							PHYSICS_SHAPE_ID& shapeIDCounter);
void GetPhysicsShapeBTModels(PhysicsShape& shape,
							 btCollisionShape** btShape,
							 vector<btCollisionShape*>& btShapes,
							 btDiscreteDynamicsWorld& btWorld,
							 BulletPhysicsSimulator::PhysicsShapeIDMapping& idMapping,
							 PHYSICS_SHAPE_ID& idCounter,
							 bool& wasDirty);
shared_ptr<btCollisionShape> BTCollisionShapeFromGeometry(shared_ptr<Geometry> geometry,
														  PHYSICS_SHAPE_TYPE type);
shared_ptr<btCompoundShape> BTCompoundShapeFromNode(shared_ptr<Node> node,
													PHYSICS_SHAPE_TYPE type,
													vector<shared_ptr<btCollisionShape>>& childShapes);
static btIDebugDraw::DebugDrawModes BTDebugDrawModeForDebugOption(DEBUG_OPTIONS option);
static vec3 GLMVec3FromBTVector3(const btVector3& from);
static vec4 GLMVec4FromBTVector4(const btVector4& from);
static btVector3 BTVector3FromGLMVec3(const vec3& from);
static btVector4 BTVector4FromGLMVec4(const vec4& from);

/***************************************************************************************
     Lifescycle
 ***************************************************************************************/

BulletPhysicsSimulator::BulletPhysicsSimulator():
	PhysicsSimulator(),
	m_btCollisionConfiguration(make_shared<btDefaultCollisionConfiguration>()),
	m_btDispatcher(make_shared<btCollisionDispatcher>(m_btCollisionConfiguration.get())),
	m_btBroadphase(make_shared<btDbvtBroadphase>()),
	m_btSolver(make_shared<btSequentialImpulseConstraintSolver>()),
	m_btWorld(make_shared<btDiscreteDynamicsWorld>(m_btDispatcher.get(),
												   m_btBroadphase.get(),
												   m_btSolver.get(),
												   m_btCollisionConfiguration.get())),
	m_bodyIDMapping(PhysicsBodyIDMapping()),
	m_bodyIDCounter(0),
	m_shapeIDMapping(PhysicsShapeIDMapping()),
	m_shapeIDCounter(0) {

							   
		AE_LOG->info("Bullet version: {}",  btGetVersion());
		
		m_btCollisionConfiguration = make_shared<btDefaultCollisionConfiguration>();
		m_btDispatcher = make_shared<btCollisionDispatcher>(m_btCollisionConfiguration.get());
		m_btBroadphase = make_shared<btDbvtBroadphase>();
		m_btSolver = make_shared<btSequentialImpulseConstraintSolver>();
		m_btWorld = make_shared<btDiscreteDynamicsWorld>(m_btDispatcher.get(),
														 m_btBroadphase.get(),
														 m_btSolver.get(),
														 m_btCollisionConfiguration.get());
}

BulletPhysicsSimulator::~BulletPhysicsSimulator() {
	
}

/**************************************************************************************
     Physics Simulator
 **************************************************************************************/

void BulletPhysicsSimulator::update(PASS pass,
									PhysicsWorld& world,
									const DEBUG_OPTIONS& debugOptions) {

	if (pass == BulletPhysicsSimulator::PASS::UPDATE_MODEL) {
		m_btWorld->setGravity(BTVector3FromGLMVec3(world.gravity()));
		m_timestep = world.timestep();
	}
}

void BulletPhysicsSimulator::update(PASS pass, 
									PhysicsBody& body,
									const DEBUG_OPTIONS& debugOptions) {
	
	// creates and updates bullet models as needed
	// for PASS::UPDATE_MODEL this checks everything gets ready for the simulation step
	// for PASS::SYNC_GRAPH, it simple gets the handles for the BT models we're driving our graph from
	
#warning TEMPORARY
	// quick cheat to get this working
	
	if (!body.shape()) {
		
		auto geometry = body.node().lock()->geometry();
		if (geometry) {
			auto shape = make_shared<PhysicsShape>(geometry, PHYSICS_SHAPE_TYPE::CONVEX_HULL);
			body.shape(shape);
		}
	}

	
	
	if (body.shape()) {
		
		btRigidBody* btBody = nullptr;
		btDefaultMotionState* btMotionState = nullptr;
		btCollisionShape* btShape = nullptr;
		auto btShapes = vector<btCollisionShape*>();
		
		GetPhysicsBodyBTModels(body,
							   &btBody, &btMotionState, &btShape, btShapes,
							   *(m_btWorld.get()),
							   m_bodyIDMapping, m_bodyIDCounter,
							   m_shapeIDMapping, m_shapeIDCounter);
		
		// if the body isn't complete (doesn't have a source geometry or source node?)
		// we can't make a BT model for it
		if (btBody) {

			if (pass == BulletPhysicsSimulator::PASS::SYNC_GRAPH) {
				
				auto modelMat = mat4(1.0);
				btTransform transform;
				btMotionState->getWorldTransform(transform);

				transform.getOpenGLMatrix(value_ptr(modelMat));
				
#warning WILL DECOMPOSE IN FUTURE
				auto node = body.node().lock();
				// this is a specuial case for now
				// if a node has a physics body, its local transform (actually a physics world transform)
				// will be used as its worldMatrix
				node->transform(modelMat);
			}
		}
	}
}

void BulletPhysicsSimulator::step(float time) {
	AE_LOG->trace("step()");
	
	//float time = glfwGetTime();
	//float time = m_scene.lock()->renderContext().lock()->sceneTime();
	static double previousSeconds = time;
	float deltaSeconds = time - previousSeconds;
	previousSeconds = time;
	
	unsigned maxSubSteps = lroundf(1.0/m_timestep);
	m_btWorld->stepSimulation(deltaSeconds, maxSubSteps, m_timestep);
	
//#ifdef DESKTOP
//	m_debugDrawer->clear();
//#endif
//	m_btWorld->debugDrawWorld();
}
						  
/**************************************************************************************
     Static
 **************************************************************************************/

void GetPhysicsBodyBTModels(PhysicsBody& body,
							btRigidBody** btBody,
							btDefaultMotionState** btMotionState,
							btCollisionShape** btShape,
							vector<btCollisionShape*>& btShapes,
							btDiscreteDynamicsWorld& btWorld,
							BulletPhysicsSimulator::PhysicsBodyIDMapping& bodyIDMapping,
							PHYSICS_BODY_ID& bodyIDCounter,
							BulletPhysicsSimulator::PhysicsShapeIDMapping& shapeIDMapping,
							PHYSICS_SHAPE_ID& shapeIDCounter) {
	
	bool shapeWasDirty = false;
	
	GetPhysicsShapeBTModels(*body.shape(),
							btShape, btShapes,
							btWorld,
							shapeIDMapping, shapeIDCounter,
							shapeWasDirty);

	auto node = body.node().lock();
	auto type = body.type();
	
	// since the BT body depends on the BT shape, if the shape was dirty (and re-created)
	// we also re-create the body
	
	if (shapeWasDirty || PHYSICS_BODY_DIRTY_BITS_CONTAINS(body.dirtyBits(),
														  PHYSICS_BODY_DIRTY_BITS::MODEL)) {
		
		AE_LOG->info("Creating rigid body for physics body {:p}...", (void*)&body);
		

		btTransform transform;
		transform.setFromOpenGLMatrix(value_ptr(node->worldTransform()));
		auto motionState_shared = make_shared<btDefaultMotionState>(transform);
		
		
		
		btCollisionShape* collisionShape = *btShape;
		
		btVector3 localInertia(0, 0, 0);
		auto mass = body.mass();
		if (mass != 0) {
			collisionShape->calculateLocalInertia(mass, localInertia);
		}
		
		btRigidBody::btRigidBodyConstructionInfo rigidBodyInfo((type == PHYSICS_BODY_TYPE::STATIC ? 0 : mass),
															   motionState_shared.get(),
															   collisionShape,
															   localInertia);
		
		// √ velocity factor
		// √ angular velocity factor
		// afected by gravity
		rigidBodyInfo.m_mass = body.mass();
		// charge
		rigidBodyInfo.m_friction = body.friction();
		rigidBodyInfo.m_rollingFriction = body.rollingFriction();
		rigidBodyInfo.m_restitution = body.restitution();
		rigidBodyInfo.m_linearDamping = body.damping();
		rigidBodyInfo.m_angularDamping = body.angularDamping();
		// moment of inertia
		// √ velocity
		// √ angular velocity
		// resting
		// allows resting
		

		
		shared_ptr<btRigidBody> body_shared = make_shared<btRigidBody>(rigidBodyInfo);
		
		body_shared->setLinearFactor(BTVector3FromGLMVec3(body.velocityFactor()));
		body_shared->setAngularFactor(BTVector3FromGLMVec3(body.angularVelocityFactor()));
		body_shared->setLinearVelocity(BTVector3FromGLMVec3(body.velocity()));
		body_shared->setAngularVelocity(BTVector3FromGLMVec3(body.angularVelocity()));
		//body_shared->setGravity()
		
		btWorld.addRigidBody(body_shared.get());
		
		// out parameters
		*btBody = body_shared.get();
		*btMotionState = motionState_shared.get();
		
		bodyIDMapping[++bodyIDCounter] = make_pair(body_shared, motionState_shared);
		body.simulationID(bodyIDCounter);
		
		body.dirtyBits(PHYSICS_BODY_DIRTY_BITS_REMOVE(body.dirtyBits(),
													  PHYSICS_BODY_DIRTY_BITS::MODEL));
	}
	else {
		*btBody = (get<0>(bodyIDMapping[body.simulationID()])).get();
		*btMotionState = (get<1>(bodyIDMapping[body.simulationID()])).get();
	}
}

void GetPhysicsShapeBTModels(PhysicsShape& shape,
							 btCollisionShape** btShape,
							 vector<btCollisionShape*>& btShapes,
							 btDiscreteDynamicsWorld& btWorld,
							 BulletPhysicsSimulator::PhysicsShapeIDMapping& idMapping,
							 PHYSICS_SHAPE_ID& idCounter,
							 bool& wasDirty) {

	if (PHYSICS_SHAPE_DIRTY_BITS_CONTAINS(shape.dirtyBits(),
										  PHYSICS_SHAPE_DIRTY_BITS::MODEL)) {
		
		if (shape.sourceGeometry()) {

			auto shape_shared = BTCollisionShapeFromGeometry(shape.sourceGeometry(), shape.type());
			
			
			*btShape = shape_shared.get();

			idMapping[++idCounter] = make_pair(shape_shared, vector<std::shared_ptr<btCollisionShape>>());
			shape.simulationID(idCounter);
		}
		else if (auto sourceNode = shape.sourceNode().lock()) {

			auto childShapes = vector<shared_ptr<btCollisionShape>>();
			
			
//			shared_ptr<btCompoundShape> BTCompoundShapeFromNode(shared_ptr<Node> node,
//																PHYSICS_SHAPE_TYPE type,
//																vector<shared_ptr<btCollisionShape>>& childShapes) {
			
			
			auto shape_shared = BTCompoundShapeFromNode(sourceNode, shape.type(), childShapes);
			
			// out parameters
			*btShape = shape_shared.get();
			for (auto& s : childShapes) {
				btShapes.emplace_back(s.get());
			}
			
			idMapping[++idCounter] = make_pair(dynamic_pointer_cast<btCollisionShape>(shape_shared), childShapes);
			shape.simulationID(idCounter);
		}
		else {
			// this might better be an assertation where nodes are checked before submitted to the PhysicsSimulator
			//throw Exception("PhysicsBody with no geometry or source node.");
			btShape = nullptr;
		}
		
		shape.dirtyBits(PHYSICS_SHAPE_DIRTY_BITS_REMOVE(shape.dirtyBits(),
														PHYSICS_SHAPE_DIRTY_BITS::MODEL));
		
		wasDirty = true;
	}
	else {
		*btShape = (get<0>(idMapping[shape.simulationID()])).get();
		auto shapes_shared = get<1>(idMapping[shape.simulationID()]);
		for (auto& s : shapes_shared) {
			btShapes.emplace_back(s.get());
		}
		
		wasDirty = false;
	}
}
	
shared_ptr<btCollisionShape> BTCollisionShapeFromGeometry(shared_ptr<Geometry> geometry,
														  PHYSICS_SHAPE_TYPE type) {
	AE_LOG->trace("BTCollisionShapeFromGeometry()");
	
	// - if 'type' is PhysicsShapeType_BoundingBox, use box shape
	// - if 'geometry' is a primitive, use matching primitive
	// - if arbitrary mesh, use whatever 'type' is
	
	if (type == PHYSICS_SHAPE_TYPE::BOUNDING_BOX) {
		AE_LOG->info("Creating box physics shape for geometry {:p}...", (void*)&geometry);
					 
		vec3 extent = geometry->extent(false);
		float width = extent.x;
		float height = extent.y;
		float length = extent.z;
		return make_shared<btBoxShape>(btVector3((btScalar)width/2.0,
												 (btScalar)height/2.0,
												 (btScalar)length/2.0));
	}
	else if (dynamic_cast<Box*>(geometry.get())) {
		AE_LOG->info("Creating box physics shape for geometry {:p}... (ignoring physics shape type '{}')",
					 (void*)&geometry, PHYSICS_SHAPE_TYPE_TO_RAW(type));
		
		auto box = dynamic_cast<Box*>(geometry.get());
		return make_shared<btBoxShape>(btVector3((btScalar)box->width()/2.0,
												 (btScalar)box->height()/2.0,
												 (btScalar)box->length()/2.0));
	}
	else if (dynamic_cast<Sphere*>(geometry.get())) {
		AE_LOG->info("Creating sphere physics shape for geometry {:p}... (ignoring physics shape type '{}')",
					 (void*)&geometry, PHYSICS_SHAPE_TYPE_TO_RAW(type));
		
		auto sphere = dynamic_cast<Sphere*>(geometry.get());
		return make_shared<btSphereShape>((btScalar)sphere->radius());
	}
	else if (dynamic_cast<Capsule*>(geometry.get())) {
		AE_LOG->info("Creating capsule physics shape for geometry {:p}... (ignoring physics shape type '{}')",
					 (void*)&geometry, PHYSICS_SHAPE_TYPE_TO_RAW(type));
		
		auto capsule = dynamic_cast<Capsule*>(geometry.get());
		return make_shared<btCapsuleShape>((btScalar)capsule->radius(),
										   (btScalar)capsule->height());
	}
	else if (dynamic_cast<Cone*>(geometry.get())) {
		AE_LOG->info("Creating cone physics shape for geometry {:p}... (ignoring physics shape type '{}')",
					 (void*)&geometry, PHYSICS_SHAPE_TYPE_TO_RAW(type));
		
		auto cone = dynamic_cast<Cone*>(geometry.get());
		return make_shared<btConeShape>((btScalar)cone->radius(),
										(btScalar)cone->height());
	}
	else if (dynamic_cast<Cylinder*>(geometry.get())) {
		AE_LOG->info("Creating cylinder physics shape for geometry {:p}... (ignoring physics shape type '{}')",
					 (void*)&geometry, PHYSICS_SHAPE_TYPE_TO_RAW(type));
		
		auto cylinder = dynamic_cast<Cylinder*>(geometry.get());
		return make_shared<btCylinderShape>(btVector3((btScalar)cylinder->radius(),
													  (btScalar)cylinder->height()/2.0,
													  (btScalar)cylinder->radius()));
	}
	else {
		
		if (type == PHYSICS_SHAPE_TYPE::CONCAVE_POLYHEDRON) {
			AE_LOG->critical("Concave polyhedron physics shapes not supported.");
		}
		else { // PhysicsShapeType_ConvexHull
			
			AE_LOG->info("Creating convex hull physics shape for geometry {:p}...", (void*)&geometry);
			
			// tips here: https://pybullet.org/Bullet/phpBB3/viewtopic.php?t=11385
			
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
			
			auto reducedShape = make_shared<btConvexHullShape>((btScalar*)hull->getVertexPointer(),
															   hull->numVertices(),
															   sizeof(btVector3));
			
			reducedShape->optimizeConvexHull();
			
			if (!reducedShape->initializePolyhedralFeatures()) {
				AE_LOG->warn("Could not initialize polyhedral features for reduced ConvexHullShape.");
			}
			
			return reducedShape;
		}
	}
	
	return nullptr;
}

shared_ptr<btCompoundShape> BTCompoundShapeFromNode(shared_ptr<Node> node,
													PHYSICS_SHAPE_TYPE type,
													vector<shared_ptr<btCollisionShape>>& childShapes) {
	AE_LOG->trace("BTCompoundShapeFromNode()");
	
	auto compoundShape = make_shared<btCompoundShape>(true);
	
	auto allNodes = node->children(true);
	for (auto n : allNodes) {
		auto geometry = n->geometry();
		if (geometry) {
			auto collisionShape = BTCollisionShapeFromGeometry(geometry, type);
			
			childShapes.emplace_back(collisionShape);
			
			btTransform localTransform;
			localTransform.setFromOpenGLMatrix(value_ptr(node->worldTransform()));
			compoundShape->addChildShape(localTransform, collisionShape.get());
		}
	}
	
	return compoundShape;
}

btIDebugDraw::DebugDrawModes BTDebugDrawModeForDebugOption(DEBUG_OPTIONS option) {
	
	switch (option) {
		case DEBUG_OPTIONS::SHOW_PHYSICS_BOUNDING_BOXES:	return btIDebugDraw::DBG_DrawAabb;
		case DEBUG_OPTIONS::SHOW_PHYSICS_WIREFRAMES:		return btIDebugDraw::DBG_DrawWireframe;
		case DEBUG_OPTIONS::SHOW_PHYSICS_CONTACT_POINTS:	return btIDebugDraw::DBG_DrawContactPoints;
		case DEBUG_OPTIONS::SHOW_PHYSICS_NORMALS: 			return btIDebugDraw::DBG_DrawNormals;
		case DEBUG_OPTIONS::SHOW_PHYSICS_CONSTRAINTS: 		return btIDebugDraw::DBG_DrawConstraints;
		case DEBUG_OPTIONS::SHOW_PHYSICS_CONSTRAINT_LIMITS:	return btIDebugDraw::DBG_DrawConstraintLimits;
		default:
			AE_LOG->warn("No corresponding BT debug draw mode for debug option: {}", option);
			return btIDebugDraw::DBG_NoDebug;
	}
}

vec3 GLMVec3FromBTVector3(const btVector3& from) {
	return vec3(from.x(), from.y(), from.z());
}

vec4 GLMVec4FromBTVector4(const btVector4& from) {
	return vec4(from.x(), from.y(), from.z(), from.w());
}

btVector3 BTVector3FromGLMVec3(const vec3& from) {
	return btVector3(from.x, from.y, from.z);
}

btVector4 BTVector4FromGLMVec4(const vec4& from) {
	return btVector4(from.x, from.y, from.z, from.w);
}
