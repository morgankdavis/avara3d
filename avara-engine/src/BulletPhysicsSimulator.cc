//
//  BulletPhysicsSimulator.cc
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
#include <LinearMath/btIDebugDraw.h>
#include <LinearMath/btScalar.h> // btGetVersion() !

#warning experimental
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/matrix_decompose.hpp>


#include "Box.h"
#include "BulletDebugDrawer.h"
#include "Capsule.h"
#include "Cone.h"
#include "Cylinder.h"
#include "Geometry.h"
#include "GeometryElement.h"
#include "Logger.h"
#include "Node.h"
#include "PhysicsBody.h"
#include "PhysicsShape.h"
#include "PhysicsWorld.h"
#include "Scene.h"
#include "Sphere.h"
#include "Utilities.h"


using namespace ae;
using namespace ae::utils;
using namespace glm;
using namespace std;


/**************************************************************************************
     Static Prototypes
 **************************************************************************************/

void GetPhysicsBodyBTModels(shared_ptr<PhysicsBody> body,
							shared_ptr<btRigidBody>* btBody,
							shared_ptr<btDefaultMotionState>* btMotionState,
							shared_ptr<btCollisionShape>* btShape,
							vector<shared_ptr<btCollisionShape>>& btChildShapes,
							btDiscreteDynamicsWorld& btWorld,
							BulletPhysicsSimulator::PhysicsBodyBTMapping& bodyBTMapping,
							BulletPhysicsSimulator::PhysicsShapeBTMapping& shapeBTMapping);
void GetPhysicsShapeBTModels(shared_ptr<PhysicsShape> shape,
							 shared_ptr<btCollisionShape>* btShape,
							 vector<shared_ptr<btCollisionShape>>& btChildShapes,
							 btDiscreteDynamicsWorld& btWorld,
							 BulletPhysicsSimulator::PhysicsShapeBTMapping& btMapping,
							 bool& wasDirty);
static void CleanupPhysicsBodyResources(set<shared_ptr<PhysicsBody>>& active,
										btDiscreteDynamicsWorld& btWorld,
										BulletPhysicsSimulator::PhysicsBodyBTMapping& btMapping);
static void CleanupPhysicsShapeResources(set<shared_ptr<PhysicsShape>>& active,
										 BulletPhysicsSimulator::PhysicsShapeBTMapping& btMapping);
static void DeletePhysicsBodyBTResources(shared_ptr<PhysicsBody> body,
										 btDiscreteDynamicsWorld& btWorld,
										 BulletPhysicsSimulator::PhysicsBodyBTMapping& btMapping);
static void DeletePhysicsShapeBTResources(shared_ptr<PhysicsShape> shape,
										  BulletPhysicsSimulator::PhysicsShapeBTMapping& btMapping);
static shared_ptr<btCollisionShape> BTCollisionShapeFromGeometry(shared_ptr<Geometry> geometry,
																 PHYSICS_SHAPE_TYPE type);
static shared_ptr<btCompoundShape> BTCompoundShapeFromNode(shared_ptr<Node> node,
														   PHYSICS_SHAPE_TYPE type,
														   vector<shared_ptr<btCollisionShape>>& childShapes);
static btIDebugDraw::DebugDrawModes BTDebugDrawModesForAEDebugOptions(const DEBUG_OPTIONS& options);
static vec3 GLMVec3FromBTVector3(const btVector3& from);
static vec4 GLMVec4FromBTVector4(const btVector4& from);
static mat4 GLMMat4FromBTTransform(const btTransform& from);
static btVector3 BTVector3FromGLMVec3(const vec3& from);
static btVector4 BTVector4FromGLMVec4(const vec4& from);
static btTransform BTTransformFromGLMMat4(const mat4& from);

#warning experimental
static mat4 TransformByRemovingScale(const mat4& m, bool& scaled);

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
	m_debugDrawer(make_shared<BulletDebugDrawer>()),
	m_bodyBTMapping(PhysicsBodyBTMapping()),
	m_shapeBTMapping(PhysicsShapeBTMapping()),
	m_activeBodies(set<shared_ptr<PhysicsBody>>()),
	m_activeShapes(set<shared_ptr<PhysicsShape>>()) {

		AE_LOG->info("Bullet version: {}",  btGetVersion());
		
	m_btWorld.get()->setDebugDrawer(m_debugDrawer.get());
	//m_btWorld.get()->setDebugDrawer(static_pointer_cast<btIDebugDraw>(m_debugDrawer).get());
}

BulletPhysicsSimulator::~BulletPhysicsSimulator() {
	AE_LOG->debug("Destroying BulletPhysicsSimulator {:p}", (void*)this);
	
	m_activeBodies.clear();
	m_activeShapes.clear();
	
	CleanupPhysicsBodyResources(m_activeBodies, *m_btWorld, m_bodyBTMapping);
	CleanupPhysicsShapeResources(m_activeShapes, m_shapeBTMapping);
}

/**************************************************************************************
     Internal
 **************************************************************************************/

void BulletPhysicsSimulator::drawDebug(Renderer& renderer,
									   const glm::mat4& viewMat,
									   const glm::mat4& projectionMat,
									   const DEBUG_OPTIONS& debugOptions) {
#ifdef DESKTOP
	auto btDebugModes = BTDebugDrawModesForAEDebugOptions(debugOptions);
	m_debugDrawer->setDebugMode(btDebugModes);
	m_debugDrawer->clear();
	m_btWorld->debugDrawWorld();
	m_debugDrawer->draw(renderer, viewMat, projectionMat);
#endif
}

/**************************************************************************************
     PhysicsSimulator
 **************************************************************************************/

void BulletPhysicsSimulator::beginUpdate(PASS pass,
										 const Scene& scene) {
	PhysicsSimulator::beginUpdate(pass, scene);
	
	if (pass == PASS::UPDATE_MODEL) {
		m_activeBodies.clear();
		m_activeShapes.clear();
	}
}

void BulletPhysicsSimulator::endUpdate(PASS pass,
									   const Scene& scene) {
	PhysicsSimulator::endUpdate(pass, scene);
	
	// we want to make sure the bt rigidbody model is removed from the simulation before stepping the simulation
	if (pass == PASS::UPDATE_MODEL) {
		CleanupPhysicsBodyResources(m_activeBodies, *m_btWorld, m_bodyBTMapping);
		CleanupPhysicsShapeResources(m_activeShapes, m_shapeBTMapping);
	}
}

void BulletPhysicsSimulator::update(PASS pass,
									shared_ptr<Scene> scene,
									const DEBUG_OPTIONS& debugOptions) {

	if (pass == BulletPhysicsSimulator::PASS::UPDATE_MODEL) {
		auto world = scene->physicsWorld();
		m_btWorld->setGravity(BTVector3FromGLMVec3(world->gravity()));
		m_timestep = world->timestep();
	}
}

void BulletPhysicsSimulator::update(PASS pass, 
									shared_ptr<Node> node,
									const DEBUG_OPTIONS& debugOptions) {
	
	auto body = node->physicsBody();
	if (body) {
		
		// creates and updates bullet models as needed
		// for PASS::UPDATE_MODEL this checks everything gets ready for the simulation step
		// for PASS::SYNC_GRAPH, it simple gets the handles for the BT models we're driving our graph from

		if (body->shape()) {
			
			shared_ptr<btRigidBody> btBody = nullptr;
			shared_ptr<btDefaultMotionState> btMotionState = nullptr;
			shared_ptr<btCollisionShape> btShape = nullptr;
			auto btChildShapes = vector<shared_ptr<btCollisionShape>>();
			
			GetPhysicsBodyBTModels(body,
								   &btBody, &btMotionState, &btShape, btChildShapes,
								   *m_btWorld,
								   m_bodyBTMapping,
								   m_shapeBTMapping);
			
			// if the body isn't complete (doesn't have a source geometry or source node?)
			// we can't make a BT model for it
			
			if (btBody) {
				
				if (pass == PASS::UPDATE_MODEL) {
					
					// if dynamic or kinematic, put their scene graph transforms into bullet model
					
//					if (body->type() == PHYSICS_BODY_TYPE::DYNAMIC
//						|| body->type() == PHYSICS_BODY_TYPE::KINEMATIC) {
					
					if (body->type() == PHYSICS_BODY_TYPE::KINEMATIC) {

						btTransform toTransform = BTTransformFromGLMMat4(node->worldTransform());
//						btBody->proceedToTransform(toTransform); // this appears to affect dynamic bodies
						auto motionState = btBody->getMotionState();
						motionState->setWorldTransform(toTransform); // and this kinematic...
						btBody->setMotionState(motionState);
						
						btBody->setActivationState(ACTIVE_TAG);
//						btBody->forceActivationState(ACTIVE_TAG);
					}
				}
				else if (pass == PASS::SYNC_GRAPH) {
					
					// get body transforms and apply back to scene graph
					
					btTransform btWorldTransform;
					btMotionState->getWorldTransform(btWorldTransform);

					auto worldMat = GLMMat4FromBTTransform(btWorldTransform);
					node->unrollWorldTransform(worldMat);
				}
			}
			
			// save reference for housekeeping
			m_activeShapes.emplace(body->shape());
		}
		else {
			AE_LOG->warn("No PhysicsShape attached to PhysicsBody.");
			//throw Exception("No PhysicsShape attached to PhysicsBody.");
		}
		
		// save reference for housekeeping
		m_activeBodies.emplace(body);
	}
}

void BulletPhysicsSimulator::step(float time) {
	AE_LOG->trace("step()");
	
	static double previousSeconds = time;
	float deltaSeconds = time - previousSeconds;
	previousSeconds = time;
	
	unsigned maxSubSteps = lroundf(1.0/m_timestep);
	m_btWorld->stepSimulation(deltaSeconds, maxSubSteps, m_timestep);
}
						  
/**************************************************************************************
     Static
 **************************************************************************************/

void GetPhysicsBodyBTModels(shared_ptr<PhysicsBody> body,
							shared_ptr<btRigidBody>* btBody,
							shared_ptr<btDefaultMotionState>* btMotionState,
							shared_ptr<btCollisionShape>* btShape,
							vector<shared_ptr<btCollisionShape>>& btChildShapes,
							btDiscreteDynamicsWorld& btWorld,
							BulletPhysicsSimulator::PhysicsBodyBTMapping& bodyBTMapping,
							BulletPhysicsSimulator::PhysicsShapeBTMapping& shapeBTMapping) {
	
	bool shapeNewlyCreated = false;
	
	GetPhysicsShapeBTModels(body->shape(),
							btShape, btChildShapes,
							btWorld,
							shapeBTMapping,
							shapeNewlyCreated);

	auto node = body->node().lock();
	auto type = body->type();
	
	auto dirtyBits = body->dirtyBits();
	
	// since the BT body depends on the BT shape, if the shape was dirty (and re-created)
	// we also re-create the body
	// additionally, since some physical properties have to be passed via btRigidBodyConstructionInfo,
	// they require re-creating the rigid body
	
	
	
	auto shape = body->shape();
	
	
	
	if (shapeNewlyCreated
		|| PHYSICS_BODY_DIRTY_BITS_CONTAINS(dirtyBits, PHYSICS_BODY_DIRTY_BITS::TYPE)
		|| PHYSICS_BODY_DIRTY_BITS_CONTAINS(dirtyBits, PHYSICS_BODY_DIRTY_BITS::SHAPE)
		|| PHYSICS_BODY_DIRTY_BITS_CONTAINS(dirtyBits, PHYSICS_BODY_DIRTY_BITS::MOMENT_OF_INERTIA)
		|| PHYSICS_BODY_DIRTY_BITS_CONTAINS(dirtyBits, PHYSICS_BODY_DIRTY_BITS::FRICTION)
		|| PHYSICS_BODY_DIRTY_BITS_CONTAINS(dirtyBits, PHYSICS_BODY_DIRTY_BITS::ROLLING_FRICTION)
		|| PHYSICS_BODY_DIRTY_BITS_CONTAINS(dirtyBits, PHYSICS_BODY_DIRTY_BITS::RESTITUTION)) {
		
		AE_LOG->info("Creating rigid body for physics body {:p}...", (void*)body.get());
		
		
		
//		#warning experimental
//		if (PHYSICS_SHAPE_DIRTY_BITS_CONTAINS(shape->dirtyBits(),
//											  PHYSICS_SHAPE_DIRTY_BITS::SCALE)) {
//			
//			auto worldScale = shape->sourceNode().lock()->worldScale();
//			AE_LOG->debug("worldScale: {}", StringFromGLMVec3(worldScale));
//			auto btScale = BTVector3FromGLMVec3(worldScale);
//			(*btShape)->setLocalScaling(btScale);
//			
////			for (auto& c : btChildShapes) {
////				c->setLocalScaling(btScale);
////			}
//			
//			//btWorld.updateSingleAabb((*btBody).get());
//			
////			shape->dirtyBits(PHYSICS_SHAPE_DIRTY_BITS_REMOVE(shape->dirtyBits(),
////															 PHYSICS_SHAPE_DIRTY_BITS::SCALE));
//		}
		
		
		
		

#warning experimental
		//btTransform transform = BTTransformFromGLMMat4(node->worldTransform());
		bool wasScaled = false;
		btTransform transform = BTTransformFromGLMMat4(TransformByRemovingScale(node->worldTransform(), wasScaled));
		if (wasScaled) {
			AE_LOG->warn("Ignorning scale for Node {:p} with PhysicsBody {:p}.",
						 (void*)node.get(), (void*)body.get());
		}
		
		auto newMotionState = make_shared<btDefaultMotionState>(transform);
		
		auto collisionShape = dynamic_pointer_cast<btCollisionShape>(*btShape);
		
		btVector3 momentOfInertia = BTVector3FromGLMVec3(body->momentOfInertia());
		auto mass = body->mass();
		
		if (body->type() == PHYSICS_BODY_TYPE::STATIC
			|| body->type() == PHYSICS_BODY_TYPE::KINEMATIC) {
			mass = 0;
		}
		else if (body->type() == PHYSICS_BODY_TYPE::DYNAMIC) {
#warning this is GENERATING momentOfInertia
			collisionShape->calculateLocalInertia(mass, momentOfInertia);
		}
		
		btRigidBody::btRigidBodyConstructionInfo rigidBodyInfo((type == PHYSICS_BODY_TYPE::STATIC ? 0 : mass),
															   newMotionState.get(),
															   collisionShape.get(),
															   momentOfInertia);
		
		rigidBodyInfo.m_mass = body->mass();
		rigidBodyInfo.m_linearDamping = body->linearDamping();
		rigidBodyInfo.m_angularDamping = body->angularDamping();
		rigidBodyInfo.m_friction = body->friction();
		rigidBodyInfo.m_rollingFriction = body->rollingFriction();
		rigidBodyInfo.m_restitution = body->restitution();
		rigidBodyInfo.m_linearSleepingThreshold = body->linearSleepingThreshold();
		rigidBodyInfo.m_angularSleepingThreshold = body->angularSleepingThreshold();
		
		auto newBody = make_shared<btRigidBody>(rigidBodyInfo);
		
		if (body->type() == PHYSICS_BODY_TYPE::STATIC) {
			newBody->setCollisionFlags(btCollisionObject::CF_STATIC_OBJECT);
		}
		else if (body->type() == PHYSICS_BODY_TYPE::KINEMATIC) {
			newBody->setCollisionFlags(btCollisionObject::CF_KINEMATIC_OBJECT);
		}
		
		btWorld.addRigidBody(newBody.get());
		
		// out parameters
		*btBody = newBody;
		*btMotionState = newMotionState;
		
		bodyBTMapping[body] = make_pair(newBody, newMotionState);
		
		body->dirtyBits(PHYSICS_BODY_DIRTY_BITS_REMOVE(body->dirtyBits(),
													   PHYSICS_BODY_DIRTY_BITS::TYPE));
		body->dirtyBits(PHYSICS_BODY_DIRTY_BITS_REMOVE(body->dirtyBits(),
													   PHYSICS_BODY_DIRTY_BITS::SHAPE));
		body->dirtyBits(PHYSICS_BODY_DIRTY_BITS_REMOVE(body->dirtyBits(),
													   PHYSICS_BODY_DIRTY_BITS::MOMENT_OF_INERTIA));
		body->dirtyBits(PHYSICS_BODY_DIRTY_BITS_REMOVE(body->dirtyBits(),
													   PHYSICS_BODY_DIRTY_BITS::FRICTION));
		body->dirtyBits(PHYSICS_BODY_DIRTY_BITS_REMOVE(body->dirtyBits(),
													   PHYSICS_BODY_DIRTY_BITS::ROLLING_FRICTION));
		body->dirtyBits(PHYSICS_BODY_DIRTY_BITS_REMOVE(body->dirtyBits(),
													   PHYSICS_BODY_DIRTY_BITS::RESTITUTION));
	}
	else {
		auto mapping = bodyBTMapping[body];
		*btBody = get<0>(mapping);
		*btMotionState = get<1>(mapping);
	}
	
	// check and set the rest of the properties
	
	if (PHYSICS_BODY_DIRTY_BITS_CONTAINS(dirtyBits, PHYSICS_BODY_DIRTY_BITS::LINEAR_FACTOR)) {
		(*btBody)->setLinearFactor(BTVector3FromGLMVec3(body->linearFactor()));
		
		body->dirtyBits(PHYSICS_BODY_DIRTY_BITS_REMOVE(body->dirtyBits(),
													   PHYSICS_BODY_DIRTY_BITS::LINEAR_FACTOR));
	}
	if (PHYSICS_BODY_DIRTY_BITS_CONTAINS(dirtyBits, PHYSICS_BODY_DIRTY_BITS::ANGULAR_FACTOR)) {
		(*btBody)->setAngularFactor(BTVector3FromGLMVec3(body->angularFactor()));
		
		body->dirtyBits(PHYSICS_BODY_DIRTY_BITS_REMOVE(body->dirtyBits(),
													   PHYSICS_BODY_DIRTY_BITS::ANGULAR_FACTOR));
	}
	if (PHYSICS_BODY_DIRTY_BITS_CONTAINS(dirtyBits, PHYSICS_BODY_DIRTY_BITS::LINEAR_DAMPING)
		|| PHYSICS_BODY_DIRTY_BITS_CONTAINS(dirtyBits, PHYSICS_BODY_DIRTY_BITS::ANGULAR_DAMPING)) {
		(*btBody)->setDamping(body->linearDamping(), body->angularDamping());
		
		body->dirtyBits(PHYSICS_BODY_DIRTY_BITS_REMOVE(body->dirtyBits(),
													   PHYSICS_BODY_DIRTY_BITS::LINEAR_DAMPING));
		body->dirtyBits(PHYSICS_BODY_DIRTY_BITS_REMOVE(body->dirtyBits(),
													   PHYSICS_BODY_DIRTY_BITS::ANGULAR_DAMPING));
	}
	if (PHYSICS_BODY_DIRTY_BITS_CONTAINS(dirtyBits, PHYSICS_BODY_DIRTY_BITS::LINEAR_SLEEPING_THRESHOLD)
		|| PHYSICS_BODY_DIRTY_BITS_CONTAINS(dirtyBits, PHYSICS_BODY_DIRTY_BITS::ANGULAR_SLEEPING_THRESHOLD)) {
		(*btBody)->setSleepingThresholds(body->linearSleepingThreshold(), body->angularSleepingThreshold());
		
		body->dirtyBits(PHYSICS_BODY_DIRTY_BITS_REMOVE(body->dirtyBits(),
													   PHYSICS_BODY_DIRTY_BITS::LINEAR_SLEEPING_THRESHOLD));
		body->dirtyBits(PHYSICS_BODY_DIRTY_BITS_REMOVE(body->dirtyBits(),
													   PHYSICS_BODY_DIRTY_BITS::ANGULAR_SLEEPING_THRESHOLD));
	}
	if (PHYSICS_BODY_DIRTY_BITS_CONTAINS(dirtyBits, PHYSICS_BODY_DIRTY_BITS::ALLOWS_RESTING)) {
		if (body->allowsResting()) {
			(*btBody)->setActivationState(ACTIVE_TAG);
		}
		else {
			(*btBody)->setActivationState(DISABLE_DEACTIVATION);
		}
		
		body->dirtyBits(PHYSICS_BODY_DIRTY_BITS_REMOVE(body->dirtyBits(),
													   PHYSICS_BODY_DIRTY_BITS::ALLOWS_RESTING));
	}
	if (PHYSICS_BODY_DIRTY_BITS_CONTAINS(dirtyBits, PHYSICS_BODY_DIRTY_BITS::FORCES)) {
#warning TODO
		
		body->dirtyBits(PHYSICS_BODY_DIRTY_BITS_REMOVE(body->dirtyBits(),
													   PHYSICS_BODY_DIRTY_BITS::FORCES));
	}
	if (PHYSICS_BODY_DIRTY_BITS_CONTAINS(dirtyBits, PHYSICS_BODY_DIRTY_BITS::TORQUES)) {
#warning TODO
		
		body->dirtyBits(PHYSICS_BODY_DIRTY_BITS_REMOVE(body->dirtyBits(),
													   PHYSICS_BODY_DIRTY_BITS::TORQUES));
	}
	if (PHYSICS_BODY_DIRTY_BITS_CONTAINS(dirtyBits, PHYSICS_BODY_DIRTY_BITS::LINEAR_VELOCITY)) {
		(*btBody)->setLinearVelocity(BTVector3FromGLMVec3(body->linearVelocity()));
		
		body->dirtyBits(PHYSICS_BODY_DIRTY_BITS_REMOVE(body->dirtyBits(),
													   PHYSICS_BODY_DIRTY_BITS::LINEAR_VELOCITY));
	}
	if (PHYSICS_BODY_DIRTY_BITS_CONTAINS(dirtyBits, PHYSICS_BODY_DIRTY_BITS::ANGULAR_VELOCITY)) {
		(*btBody)->setAngularVelocity(BTVector3FromGLMVec3(body->angularVelocity()));
		
		body->dirtyBits(PHYSICS_BODY_DIRTY_BITS_REMOVE(body->dirtyBits(),
													   PHYSICS_BODY_DIRTY_BITS::ANGULAR_VELOCITY));
	}
	
	// update shape scale. done here instead of GetPhysicsShapeBTModels() because we need the rigidbody
	
	
//	#warning experimental
////	auto shape = body->shape();
//	if (PHYSICS_SHAPE_DIRTY_BITS_CONTAINS(shape->dirtyBits(),
//										  PHYSICS_SHAPE_DIRTY_BITS::SCALE)) {
//		
////		auto btScale = BTVector3FromGLMVec3(shape->sourceNode().lock()->worldScale());
////		(*btShape)->setLocalScaling(btScale);
////		
////		for (auto& c : btChildShapes) {
////			c->setLocalScaling(btScale);
////		}
//		
//		btWorld.updateSingleAabb((*btBody).get());
//		
//		shape->dirtyBits(PHYSICS_SHAPE_DIRTY_BITS_REMOVE(shape->dirtyBits(),
//														 PHYSICS_SHAPE_DIRTY_BITS::SCALE));
//	}
	
	// back-fill PhysicsBody properties
	
#warning this is causing a loop canceling out any dynamic movement
	
	body->linearVelocity(GLMVec3FromBTVector3((*btBody)->getLinearVelocity()), false);
	body->angularVelocity(GLMVec3FromBTVector3((*btBody)->getAngularVelocity()), false);
	body->resting((*btBody)->getActivationState() == (ISLAND_SLEEPING ? true : false));
}

void GetPhysicsShapeBTModels(shared_ptr<PhysicsShape> shape,
							 shared_ptr<btCollisionShape>* btShape,
							 vector<shared_ptr<btCollisionShape>>& btChildShapes,
							 btDiscreteDynamicsWorld& btWorld,
							 BulletPhysicsSimulator::PhysicsShapeBTMapping& btMapping,
							 bool& created) {

	if (PHYSICS_SHAPE_DIRTY_BITS_CONTAINS(shape->dirtyBits(),
										  PHYSICS_SHAPE_DIRTY_BITS::MODEL)) {
		
		if (auto sourceGeometry = shape->sourceGeometry().lock()) {

			auto newShape = BTCollisionShapeFromGeometry(sourceGeometry, shape->type());
			
			// out parameters
			*btShape = newShape;

			btMapping[shape] = make_pair(newShape, vector<std::shared_ptr<btCollisionShape>>());
		}
		else if (auto sourceNode = shape->sourceNode().lock()) {

			auto newChildShapes = vector<shared_ptr<btCollisionShape>>();
			auto newShape = BTCompoundShapeFromNode(sourceNode, shape->type(), newChildShapes);
			
			// out parameters
			*btShape = newShape;
			for (auto& c : newChildShapes) {
				btChildShapes.emplace_back(c);
			}
			
			btMapping[shape] = make_pair(dynamic_pointer_cast<btCollisionShape>(newShape), newChildShapes);
		}
		else {
			// this might better be an assertation where nodes are checked before submitted to the PhysicsSimulator
			throw Exception("PhysicsBody with no geometry or source node.");
			btShape = nullptr;
		}
		
		shape->dirtyBits(PHYSICS_SHAPE_DIRTY_BITS_REMOVE(shape->dirtyBits(),
														 PHYSICS_SHAPE_DIRTY_BITS::MODEL));

		created = true;
	}
	else {
		auto mapping = btMapping[shape];
		*btShape = get<0>(mapping);
		auto childShapes = get<1>(mapping);
		for (auto& c : childShapes) {
			btChildShapes.emplace_back(c);
		}

		created = false;
	}
}

void CleanupPhysicsBodyResources(set<shared_ptr<PhysicsBody>>& active,
								 btDiscreteDynamicsWorld& btWorld,
								 BulletPhysicsSimulator::PhysicsBodyBTMapping& btMapping) {
	
	// gather sorted vector of bodies used this frame
	auto activeBodiesSorted = vector<shared_ptr<PhysicsBody>>();
	activeBodiesSorted.reserve(active.size());
	copy(active.begin(), active.end(), back_inserter(activeBodiesSorted));
	sort(activeBodiesSorted.begin(), activeBodiesSorted.end());
	
	// gather sorted vector of bodies in the mapping
	auto storedBodiesSorted = vector<shared_ptr<PhysicsBody>>();
	storedBodiesSorted.reserve(btMapping.size());
	for (auto it = btMapping.begin(); it != btMapping.end(); ++it) {
		storedBodiesSorted.emplace_back(it->first);
	}
	sort(storedBodiesSorted.begin(), storedBodiesSorted.end());
	
	// find unused bodies 
	auto unused = vector<shared_ptr<PhysicsBody>>(storedBodiesSorted.size());
	vector<shared_ptr<PhysicsBody>>::iterator it;
	it = set_difference(storedBodiesSorted.begin(), storedBodiesSorted.end(),
						activeBodiesSorted.begin(), activeBodiesSorted.end(),
						unused.begin());
	unused.resize(it - unused.begin());
	
	// deallocate unused bodies
	if (unused.size()) {
		AE_LOG->debug("Deallocating bullet body for {} physics bodies...", unused.size());
		
		for (it=unused.begin(); it!=unused.end(); ++it) {
			shared_ptr<PhysicsBody> body = *it;

			DeletePhysicsBodyBTResources(body, btWorld, btMapping);
			
			body->dirtyBits(PHYSICS_BODY_DIRTY_BITS_REMOVE(body->dirtyBits(),
														   PHYSICS_BODY_DIRTY_BITS::ALL));
		}
	}
}

void CleanupPhysicsShapeResources(set<shared_ptr<PhysicsShape>>& active,
								  BulletPhysicsSimulator::PhysicsShapeBTMapping& btMapping) {
	
	// gather sorted vector of shapes used this frame
	auto activeShapesSorted = vector<shared_ptr<PhysicsShape>>();
	activeShapesSorted.reserve(active.size());
	copy(active.begin(), active.end(), back_inserter(activeShapesSorted));
	sort(activeShapesSorted.begin(), activeShapesSorted.end());
	
	// gather sorted vector of shapes in the mapping
	auto storedShapesSorted = vector<shared_ptr<PhysicsShape>>();
	storedShapesSorted.reserve(btMapping.size());
	for (auto it = btMapping.begin(); it != btMapping.end(); ++it) {
		storedShapesSorted.emplace_back(it->first);
	}
	sort(storedShapesSorted.begin(), storedShapesSorted.end());
	
	// find unused shapes
	auto unused = vector<shared_ptr<PhysicsShape>>(storedShapesSorted.size());
	vector<shared_ptr<PhysicsShape>>::iterator it;
	it = set_difference(storedShapesSorted.begin(), storedShapesSorted.end(),
						activeShapesSorted.begin(), activeShapesSorted.end(),
						unused.begin());
	unused.resize(it - unused.begin());
	
	// deallocate unused shapes
	if (unused.size()) {
		AE_LOG->debug("Deallocating bullet shape for {} physics shape...", unused.size());
		
		for (it=unused.begin(); it!=unused.end(); ++it) {
			shared_ptr<PhysicsShape> shape = *it;

			DeletePhysicsShapeBTResources(shape, btMapping);
			
			shape->dirtyBits(PHYSICS_SHAPE_DIRTY_BITS_REMOVE(shape->dirtyBits(),
															 PHYSICS_SHAPE_DIRTY_BITS::ALL));
		}
	}
}

void DeletePhysicsBodyBTResources(shared_ptr<PhysicsBody> body,
								  btDiscreteDynamicsWorld& btWorld,
								  BulletPhysicsSimulator::PhysicsBodyBTMapping& btMapping) {
	
	auto btHandles = btMapping[body];
	
	shared_ptr<btRigidBody> btRigidBody = get<0>(btHandles);
	btWorld.removeRigidBody(btRigidBody.get());
	
	btMapping.erase(body);
}

void DeletePhysicsShapeBTResources(shared_ptr<PhysicsShape> shape,
								   BulletPhysicsSimulator::PhysicsShapeBTMapping& btMapping) {
	
	btMapping.erase(shape);
}

shared_ptr<btCollisionShape> BTCollisionShapeFromGeometry(shared_ptr<Geometry> geometry,
														  PHYSICS_SHAPE_TYPE type) {
	AE_LOG->trace("BTCollisionShapeFromGeometry()");
	
	// - if 'type' is PhysicsShapeType_BoundingBox, use box shape
	// - if 'geometry' is a primitive, use matching primitive
	// - if arbitrary mesh, use whatever 'type' is
	
	if (type == PHYSICS_SHAPE_TYPE::BOUNDING_BOX) {
		AE_LOG->info("Creating box physics shape for geometry {:p}...", (void*)geometry.get());
					 
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
					 (void*)geometry.get(), static_cast<underlying_type<PHYSICS_SHAPE_TYPE>::type>(type));
		
		auto box = dynamic_cast<Box*>(geometry.get());
		return make_shared<btBoxShape>(btVector3((btScalar)box->width()/2.0,
												 (btScalar)box->height()/2.0,
												 (btScalar)box->length()/2.0));
	}
	else if (dynamic_cast<Sphere*>(geometry.get())) {
		AE_LOG->info("Creating sphere physics shape for geometry {:p}... (ignoring physics shape type '{}')",
					 (void*)geometry.get(), static_cast<underlying_type<PHYSICS_SHAPE_TYPE>::type>(type));
		
		auto sphere = dynamic_cast<Sphere*>(geometry.get());
		return make_shared<btSphereShape>((btScalar)sphere->radius());
	}
	else if (dynamic_cast<Capsule*>(geometry.get())) {
		AE_LOG->info("Creating capsule physics shape for geometry {:p}... (ignoring physics shape type '{}')",
					 (void*)geometry.get(), static_cast<underlying_type<PHYSICS_SHAPE_TYPE>::type>(type));
		
		auto capsule = dynamic_cast<Capsule*>(geometry.get());
		return make_shared<btCapsuleShape>((btScalar)capsule->radius(),
										   (btScalar)capsule->height());
	}
	else if (dynamic_cast<Cone*>(geometry.get())) {
		AE_LOG->info("Creating cone physics shape for geometry {:p}... (ignoring physics shape type '{}')",
					 (void*)geometry.get(), static_cast<underlying_type<PHYSICS_SHAPE_TYPE>::type>(type));
		
		auto cone = dynamic_cast<Cone*>(geometry.get());
		return make_shared<btConeShape>((btScalar)cone->radius(),
										(btScalar)cone->height());
	}
	else if (dynamic_cast<Cylinder*>(geometry.get())) {
		AE_LOG->info("Creating cylinder physics shape for geometry {:p}... (ignoring physics shape type '{}')",
					 (void*)geometry.get(), static_cast<underlying_type<PHYSICS_SHAPE_TYPE>::type>(type));
		
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
			
			AE_LOG->info("Creating convex hull physics shape for geometry {:p}...", (void*)geometry.get());
			
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
			
			// for debug drawing
			if (!reducedShape->initializePolyhedralFeatures()) {
				AE_LOG->warn("Could not initialize polyhedral features for reduced btConvexHullShape.");
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

			btTransform localTransform = BTTransformFromGLMMat4(n->transform());
			compoundShape->addChildShape(localTransform, collisionShape.get());
		}
	}
	
	return compoundShape;
}

btIDebugDraw::DebugDrawModes BTDebugDrawModesForAEDebugOptions(const DEBUG_OPTIONS& options) {
	btIDebugDraw::DebugDrawModes btModes = btIDebugDraw::DBG_NoDebug;

	if (DEBUG_OPTIONS_CONTAINS(options, DEBUG_OPTIONS::SHOW_PHYSICS_BOUNDING_BOXES)) {
		btModes = (btIDebugDraw::DebugDrawModes)(btModes | btIDebugDraw::DBG_DrawAabb);
	}
	if (DEBUG_OPTIONS_CONTAINS(options, DEBUG_OPTIONS::SHOW_PHYSICS_WIREFRAMES)) {
		btModes = (btIDebugDraw::DebugDrawModes)(btModes | btIDebugDraw::DBG_DrawWireframe);
	}
	if (DEBUG_OPTIONS_CONTAINS(options, DEBUG_OPTIONS::SHOW_PHYSICS_CONTACT_POINTS)) {
		btModes = (btIDebugDraw::DebugDrawModes)(btModes | btIDebugDraw::DBG_DrawContactPoints);
	}
	if (DEBUG_OPTIONS_CONTAINS(options, DEBUG_OPTIONS::SHOW_PHYSICS_NORMALS)) {
		btModes = (btIDebugDraw::DebugDrawModes)(btModes | btIDebugDraw::DBG_DrawNormals);
	}
	if (DEBUG_OPTIONS_CONTAINS(options, DEBUG_OPTIONS::SHOW_PHYSICS_CONSTRAINTS)) {
		btModes = (btIDebugDraw::DebugDrawModes)(btModes | btIDebugDraw::DBG_DrawConstraints);
	}
	if (DEBUG_OPTIONS_CONTAINS(options, DEBUG_OPTIONS::SHOW_PHYSICS_CONSTRAINT_LIMITS)) {
		btModes = (btIDebugDraw::DebugDrawModes)(btModes | btIDebugDraw::DBG_DrawConstraintLimits);
	}

	/* what do these do?
	 
	 btModes = (btIDebugDraw::DebugDrawModes)
	 (btModes | btIDebugDraw::DBG_ProfileTimings);
	 
	 btModes = (btIDebugDraw::DebugDrawModes)
	 (btModes | btIDebugDraw::DBG_DrawFeaturesText);
	 
	 btModes = (btIDebugDraw::DebugDrawModes)
	 (btModes | btIDebugDraw::DBG_DrawFrames);
	 
	 btModes = (btIDebugDraw::DebugDrawModes)
	 (btModes | btIDebugDraw::DBG_EnableCCD); */
	
	static btIDebugDraw::DebugDrawModes previousModes = btIDebugDraw::DBG_NoDebug;
	if (btModes != previousModes) {
		AE_LOG->debug("Bullet debug modes: {}", btModes);
	}
	previousModes = btModes;
	
	return btModes;
}

vec3 GLMVec3FromBTVector3(const btVector3& from) {
	return vec3(from.x(), from.y(), from.z());
}

vec4 GLMVec4FromBTVector4(const btVector4& from) {
	return vec4(from.x(), from.y(), from.z(), from.w());
}

mat4 GLMMat4FromBTTransform(const btTransform& from) {
	mat4 gldMat;
	from.getOpenGLMatrix(value_ptr(gldMat));
	return gldMat;
}

btVector3 BTVector3FromGLMVec3(const vec3& from) {
	return btVector3(from.x, from.y, from.z);
}

btVector4 BTVector4FromGLMVec4(const vec4& from) {
	return btVector4(from.x, from.y, from.z, from.w);
}

btTransform BTTransformFromGLMMat4(const mat4& from) {
	btTransform bulletTransform;
	bulletTransform.setFromOpenGLMatrix(value_ptr(from));
	return bulletTransform;
}

mat4 TransformByRemovingScale(const mat4& m, bool& scaled) {
	vec3 scale;
	quat orientation;
	vec3 translation;
	vec3 skew;
	vec4 perspective;
	
	decompose(m,
			  scale,
			  orientation,
			  translation,
			  skew,
			  perspective);
	
	if (Equal(scale, {1, 1, 1})) {
		scaled = false;
	}
	else {
		scaled = true;
	}
	
	return translate(mat4(1.0), translation) * mat4_cast(orientation) * mat4(1.0);
}
