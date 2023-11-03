//
//  BulletPhysicsSimulator.cc
//	avara-engine
//
//  Created by Morgan Davis on 5/1/18.
//  Copyright © 2018 Morgan K Davis. All rights reserved.
//

#include "BulletPhysicsSimulator.h"

#include <variant>

#include <btBulletCollisionCommon.h>
#include <btBulletDynamicsCommon.h>
#include <BulletCollision/Gimpact/btGImpactShape.h>
#include <BulletCollision/CollisionShapes/btShapeHull.h>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <LinearMath/btIDebugDraw.h>
#include <LinearMath/btScalar.h> // btGetVersion() !
#include <magic_enum.hpp>

#include "Box.h"
#include "BulletBodyResources.h"
#include "BulletDebugDrawer.h"
#include "BulletShapeResources.h"
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
#include "Plane.h"
#include "Scene.h"
#include "Sphere.h"
#include "Utilities.h"


using namespace ae;
using namespace ae::utils;
using namespace glm;
using namespace std;


constexpr unsigned MAX_SUBSTEPS = 20;


/*********************************************************************************************
	Static Prototypes
 *********************************************************************************************/

void 									GetPhysicsBodyBTModels(shared_ptr<Node> node,
															   shared_ptr<PhysicsBody> body,
															   shared_ptr<btRigidBody>* btBody,
															   shared_ptr<btDefaultMotionState>* btMotionState,
															   shared_ptr<btCollisionShape>* btShape,
															   vector<shared_ptr<btCollisionShape>>& btChildShapes,
															   btDiscreteDynamicsWorld& btWorld,
															   BulletPhysicsSimulator::PhysicsBodyBTMapping& bodyBTMapping,
															   BulletPhysicsSimulator::PhysicsShapeBTMapping& shapeBTMapping);
void 									GetPhysicsShapeBTModels(shared_ptr<PhysicsShape> shape,
																PHYSICS_BODY_TYPE bodyType,
																shared_ptr<btCollisionShape>* btShape,
																vector<shared_ptr<btCollisionShape>>& btChildShapes,
																btDiscreteDynamicsWorld& btWorld,
																BulletPhysicsSimulator::PhysicsShapeBTMapping& btShapeMapping,
																bool& newlyCreated);
static void 							CleanupPhysicsBodyResources(unordered_set<shared_ptr<PhysicsBody>>& active,
																   btDiscreteDynamicsWorld& btWorld,
																   BulletPhysicsSimulator::PhysicsBodyBTMapping& btBodyMapping);
static void 							CleanupPhysicsShapeResources(unordered_set<shared_ptr<PhysicsShape>>& active,
																	BulletPhysicsSimulator::PhysicsShapeBTMapping& btShapeMapping);
static void 							DeletePhysicsBodyBTResources(shared_ptr<PhysicsBody> body,
																	btDiscreteDynamicsWorld& btWorld,
																	BulletPhysicsSimulator::PhysicsBodyBTMapping& btBodyMapping);
static void 							DeletePhysicsShapeBTResources(shared_ptr<PhysicsShape> shape,
																	 BulletPhysicsSimulator::PhysicsShapeBTMapping& btShapeMapping);
static shared_ptr<btCollisionShape> 	BTCollisionShapeFromGeometry(shared_ptr<Geometry> geometry,
																	PHYSICS_SHAPE_TYPE shapeType,
																	PHYSICS_BODY_TYPE bodyType,
																	shared_ptr<btTriangleIndexVertexArray>& indexVertexArray);
static shared_ptr<btCompoundShape> 		BTCompoundShapeFromNode(shared_ptr<Node> node,
																  PHYSICS_SHAPE_TYPE shapeType,
																  PHYSICS_BODY_TYPE bodyType,
																  vector<shared_ptr<btCollisionShape>>& childShapes,
																  vector<shared_ptr<btTriangleIndexVertexArray>>& childIndexVertexArrays);
static btIDebugDraw::DebugDrawModes 	BTDebugDrawModesForAEDebugOptions(const DEBUG_OPTIONS& options);
static vec3 							GLMVec3FromBTVector3(const btVector3& from);
static vec4 							GLMVec4FromBTVector4(const btVector4& from);
static mat4 							GLMMat4FromBTTransform(const btTransform& from);
static btVector3 						BTVector3FromGLMVec3(const vec3& from);
static btVector4 						BTVector4FromGLMVec4(const vec4& from);
static btQuaternion 					BTQuaternionFromGLMQuat(const quat& from);
static btTransform 						BTTransformFromGLMMat4(const mat4& from);
static mat4 							TransformByRemovingScale(const mat4& m, bool& scaled);
//glm::mat4 							BulletToGlm(const btTransform& t);

/*********************************************************************************************
	Lifescycle
 *********************************************************************************************/

BulletPhysicsSimulator::BulletPhysicsSimulator():
	PhysicsSimulator(),
	_btCollisionConfiguration(make_shared<btDefaultCollisionConfiguration>()),
	_btDispatcher(make_shared<btCollisionDispatcher>(_btCollisionConfiguration.get())),
	_btBroadphase(make_shared<btDbvtBroadphase>()),
	_btSolver(make_shared<btSequentialImpulseConstraintSolver>()),
	_btWorld(make_shared<btDiscreteDynamicsWorld>(_btDispatcher.get(),
												   _btBroadphase.get(),
												   _btSolver.get(),
												   _btCollisionConfiguration.get())),
#ifdef GL_FULL
	_debugDrawer(make_shared<BulletDebugDrawer>()),
#endif
	_bodyBTMapping(PhysicsBodyBTMapping()),
	_shapeBTMapping(PhysicsShapeBTMapping()),
	_activeBodies(unordered_set<std::shared_ptr<ae::PhysicsBody>>()),
	_activeShapes(unordered_set<std::shared_ptr<ae::PhysicsShape>>()) {

		AE_LOG_I("Bullet Physics version: {}",  btGetVersion());

#ifdef GL_FULL
		_btWorld.get()->setDebugDrawer(_debugDrawer.get());
#endif
}

BulletPhysicsSimulator::~BulletPhysicsSimulator() {
	AE_LOG_D("Destroying BulletPhysicsSimulator {:p}", (void*)this);
	
	_activeBodies.clear();
	_activeShapes.clear();
	
	CleanupPhysicsBodyResources(_activeBodies, *_btWorld, _bodyBTMapping);
	CleanupPhysicsShapeResources(_activeShapes, _shapeBTMapping);
}

/*********************************************************************************************
	Internal
 *********************************************************************************************/

void BulletPhysicsSimulator::drawDebug(Renderer& renderer,
									   const mat4& viewMat,
									   const mat4& projectionMat,
									   const DEBUG_OPTIONS& debugOptions) {
#ifdef GL_FULL
	auto btDebugModes = BTDebugDrawModesForAEDebugOptions(debugOptions);
	_debugDrawer->setDebugMode(btDebugModes);
	_debugDrawer->clear();
	_btWorld->debugDrawWorld();
	_debugDrawer->draw(renderer, viewMat, projectionMat);
#endif
}

/*********************************************************************************************
	PhysicsSimulator
 *********************************************************************************************/

void BulletPhysicsSimulator::beginUpdate(PASS pass,
										 const Scene& scene) {
	PhysicsSimulator::beginUpdate(pass, scene);
	
	if (pass == PASS::STEP) {
		_activeBodies.clear();
		_activeShapes.clear();
	}
}

void BulletPhysicsSimulator::endUpdate(PASS pass,
									   const Scene& scene) {
	PhysicsSimulator::endUpdate(pass, scene);
	
	// we want to make sure the bt rigidbody model is removed from the simulation before stepping the simulation
	if (pass == PASS::STEP) {
		CleanupPhysicsBodyResources(_activeBodies, *_btWorld, _bodyBTMapping);
		CleanupPhysicsShapeResources(_activeShapes, _shapeBTMapping);
	}
}

void BulletPhysicsSimulator::update(PASS pass,
									shared_ptr<Scene> scene,
									const DEBUG_OPTIONS& debugOptions) {

	if (pass == BulletPhysicsSimulator::PASS::STEP) {
		auto world = scene->physicsWorld();
		
		if (PHYSICS_WORLD_DIRTY_BITS_CONTAINS(world->dirtyBits(), PHYSICS_WORLD_DIRTY_BITS::TIMESTEP)) {
			_timestep = world->timestep();
			
			world->dirtyBits(PHYSICS_WORLD_DIRTY_BITS_REMOVE(world->dirtyBits(),
															 PHYSICS_WORLD_DIRTY_BITS::TIMESTEP));
		}

		#warning set this gravity for all physics objects, too...
		if (PHYSICS_WORLD_DIRTY_BITS_CONTAINS(world->dirtyBits(), PHYSICS_WORLD_DIRTY_BITS::GRAVITY)) {
			_btWorld->setGravity(BTVector3FromGLMVec3(world->gravity()));
			
			world->dirtyBits(PHYSICS_WORLD_DIRTY_BITS_REMOVE(world->dirtyBits(),
															 PHYSICS_WORLD_DIRTY_BITS::GRAVITY));
		}
	}
}

void BulletPhysicsSimulator::update(PASS pass, 
									shared_ptr<Node> node,
									const DEBUG_OPTIONS& debugOptions) {

//	if ((node->name() != nullopt) && (*node->name() == "g duck")) {
//		AE_LOG_I("QUACK");
//	}
	
	auto body = node->physicsBody();
	if (body) {
		
		// creates and updates bullet models as needed
		// for PASS::STEP this checks everything gets ready for the simulation step
		// for PASS::SYNC, it simply gets the handles for the BT models we're driving our graph from

		if (body->shape()) {
			
			shared_ptr<btRigidBody> btBody = nullptr;
			shared_ptr<btDefaultMotionState> btMotionState = nullptr;
			shared_ptr<btCollisionShape> btShape = nullptr;
			auto btChildShapes = vector<shared_ptr<btCollisionShape>>();
			
			GetPhysicsBodyBTModels(node,
								   body,
								   &btBody, &btMotionState, &btShape, btChildShapes,
								   *_btWorld,
								   _bodyBTMapping,
								   _shapeBTMapping);
			
			// if the body isn't complete (doesn't have a source geometry or source node?)
			// we can't make a BT model for it
			
			if (btBody) {
				
				if (pass == PASS::STEP) {
					
					// if dynamic or kinematic, put their scene graph transforms into bullet model
					
//					if (body->type() == PHYSICS_BODY_TYPE::DYNAMIC
//						|| body->type() == PHYSICS_BODY_TYPE::KINEMATIC) {
					
					if (body->type() == PHYSICS_BODY_TYPE::KINEMATIC) {

						auto toTransform = BTTransformFromGLMMat4(node->worldTransform());
//						btBody->proceedToTransform(toTransform); // this appears to affect dynamic bodies
						auto motionState = btBody->getMotionState();
						motionState->setWorldTransform(toTransform); // and this kinematic...
						btBody->setMotionState(motionState);
						
						btBody->setActivationState(ACTIVE_TAG);
//						btBody->forceActivationState(ACTIVE_TAG);
					}
				}
				else if (pass == PASS::SYNC) {
					
					// get body transforms and apply back to scene graph
					
					btTransform btWorldTransform;
					btWorldTransform.setIdentity();
					btMotionState->getWorldTransform(btWorldTransform);

					auto worldMat = GLMMat4FromBTTransform(btWorldTransform);
					node->unrollWorldTransform(worldMat);
				}
			}
			
			// save reference for housekeeping
			_activeShapes.emplace(body->shape());
		}
		else {
			AE_LOG_W("No PhysicsShape attached to PhysicsBody.");
			//throw Exception("No PhysicsShape attached to PhysicsBody.");
		}
		
		// save reference for housekeeping
		_activeBodies.emplace(body);
	}
}

void BulletPhysicsSimulator::step(float time) {
	AE_LOG_T("");
	
	static float previousSeconds = time;
	float deltaSeconds = time - previousSeconds;
	previousSeconds = time;
	
	int result = _btWorld->stepSimulation(deltaSeconds, MAX_SUBSTEPS, _timestep);
	
	if (result == MAX_SUBSTEPS) {
		AE_LOG_W("Physics simulation max substeps reached: {}", result);
	}
}
						  
/*********************************************************************************************
	Static
 *********************************************************************************************/

void GetPhysicsBodyBTModels(shared_ptr<Node> node,
							shared_ptr<PhysicsBody> body,
							shared_ptr<btRigidBody>* btBody,
							shared_ptr<btDefaultMotionState>* btMotionState,
							shared_ptr<btCollisionShape>* btShape,
							vector<shared_ptr<btCollisionShape>>& btChildShapes,
							btDiscreteDynamicsWorld& btWorld,
							BulletPhysicsSimulator::PhysicsBodyBTMapping& bodyBTMapping,
							BulletPhysicsSimulator::PhysicsShapeBTMapping& shapeBTMapping) {

	bool shapeNewlyCreated = false;

	// g duck has no source geometry
	GetPhysicsShapeBTModels(body->shape(),
							body->type(),
							btShape, btChildShapes,
							btWorld,
							shapeBTMapping,
							shapeNewlyCreated);

	//auto node = body->node().lock();
	auto type = body->type();
	
	auto dirtyBits = body->dirtyBits();
	
	// since the BT body depends on the BT shape, if the shape was dirty (and re-created)
	// we also re-create the body.
	// additionally, since some physical properties have to be passed via btRigidBodyConstructionInfo,
	// they require re-creating the rigid body.

	auto shape = body->shape();

	if (shapeNewlyCreated
		// these properties only appear to be set-able via btRigidBodyConstructionInfo
		|| PHYSICS_BODY_DIRTY_BITS_CONTAINS(dirtyBits, PHYSICS_BODY_DIRTY_BITS::TYPE)
		|| PHYSICS_BODY_DIRTY_BITS_CONTAINS(dirtyBits, PHYSICS_BODY_DIRTY_BITS::SHAPE)
		|| PHYSICS_BODY_DIRTY_BITS_CONTAINS(dirtyBits, PHYSICS_BODY_DIRTY_BITS::MOMENT_OF_INERTIA)
		|| PHYSICS_BODY_DIRTY_BITS_CONTAINS(dirtyBits, PHYSICS_BODY_DIRTY_BITS::FRICTION)
		|| PHYSICS_BODY_DIRTY_BITS_CONTAINS(dirtyBits, PHYSICS_BODY_DIRTY_BITS::ROLLING_FRICTION)
		|| PHYSICS_BODY_DIRTY_BITS_CONTAINS(dirtyBits, PHYSICS_BODY_DIRTY_BITS::RESTITUTION)) {
		
		AE_LOG_D("Creating rigid body for physics body {:p}...", (void*)body.get());

//		#warning experimental
//		if (PHYSICS_SHAPE_DIRTY_BITS_CONTAINS(shape->dirtyBits(),
//											  PHYSICS_SHAPE_DIRTY_BITS::SCALE)) {
//			
//			auto worldScale = shape->sourceNode().lock()->worldScale();
//			AE_LOG_D("worldScale: {}", StringFromGLMVec3(worldScale));
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

//#warning experimental
		//btTransform transform = BTTransformFromGLMMat4(node->worldTransform());
		bool wasScaled = false;
		auto transform = BTTransformFromGLMMat4(TransformByRemovingScale(node->worldTransform(), wasScaled));
		if (wasScaled) {
			// TODO: should address this.
			// can hold a burned transformed vertex data in the physics body/shape?
			AE_LOG_W("Ignorning scale for Node {:p} with PhysicsBody {:p}.",
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
//#warning this is GENERATING momentOfInertia
			collisionShape->calculateLocalInertia(mass, momentOfInertia);
		}
		
		btRigidBody::btRigidBodyConstructionInfo rigidBodyInfo(mass,
															   newMotionState.get(),
															   collisionShape.get(),
															   momentOfInertia);
		
		rigidBodyInfo.m_mass = mass;
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

		//bodyBTMapping[body] = make_pair(newBody, newMotionState);
		bodyBTMapping[body] = make_shared<BulletBodyResources>(newBody, newMotionState);
		
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
		auto resources = bodyBTMapping[body];
//		*btBody = get<0>(resources);
//		*btMotionState = get<1>(resources);
		*btBody = resources->body();
		*btMotionState = resources->motionState();
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
		
//		AE_LOG_D("linearSleepingThreshold: {}", (*btBody)->getLinearSleepingThreshold());
//		AE_LOG_D("angularSleepingThreshold: {}", (*btBody)->getAngularSleepingThreshold());
		
		body->dirtyBits(PHYSICS_BODY_DIRTY_BITS_REMOVE(body->dirtyBits(),
													   PHYSICS_BODY_DIRTY_BITS::LINEAR_SLEEPING_THRESHOLD));
		body->dirtyBits(PHYSICS_BODY_DIRTY_BITS_REMOVE(body->dirtyBits(),
													   PHYSICS_BODY_DIRTY_BITS::ANGULAR_SLEEPING_THRESHOLD));
	}
	if (PHYSICS_BODY_DIRTY_BITS_CONTAINS(dirtyBits, PHYSICS_BODY_DIRTY_BITS::AFFECTED_BY_GRAVITY)) {
		if (body->affectedByGravity()) {
			(*btBody)->setGravity(btWorld.getGravity());
		}
		else {
			// NOTE: setting world gravity resets this
			(*btBody)->setGravity({0, 0, 0});
		}
		
		body->dirtyBits(PHYSICS_BODY_DIRTY_BITS_REMOVE(body->dirtyBits(),
													   PHYSICS_BODY_DIRTY_BITS::AFFECTED_BY_GRAVITY));
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
	
	
//	if (body->type() == PHYSICS_BODY_TYPE::STATIC) {
//		AE_LOG_D("STATIC");
//	}
//	else if (body->type() == PHYSICS_BODY_TYPE::DYNAMIC) {
//		AE_LOG_D("DYNAMIC");
//		(*btBody)->setGravity((btVector3){0, 0, 0});
//	}
//	else if (body->type() == PHYSICS_BODY_TYPE::KINEMATIC) {
//		AE_LOG_D("KINEMATIC");
//	}
//	AE_LOG_D("Gravity: {}", StringFromGLMVec3(GLMVec3FromBTVector3((*btBody)->getGravity())));
	
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
	
	
	
	
// *** causing a loop canceling out any manual dynamic movement ***
	
	body->linearVelocity(GLMVec3FromBTVector3((*btBody)->getLinearVelocity()), false);
	body->angularVelocity(GLMVec3FromBTVector3((*btBody)->getAngularVelocity()), false);
	body->resting((*btBody)->getActivationState() == (ISLAND_SLEEPING ? true : false));
}

void GetPhysicsShapeBTModels(shared_ptr<PhysicsShape> shape,
							 PHYSICS_BODY_TYPE bodyType,
							 shared_ptr<btCollisionShape>* btShape,
							 vector<shared_ptr<btCollisionShape>>& btChildShapes,
							 btDiscreteDynamicsWorld& btWorld,
							 BulletPhysicsSimulator::PhysicsShapeBTMapping& btShapeMapping,
							 bool& newlyCreated) {

	if (PHYSICS_SHAPE_DIRTY_BITS_CONTAINS(shape->dirtyBits(),
										  PHYSICS_SHAPE_DIRTY_BITS::MODEL)) {

		AE_LOG_D("Shape {:p} model dirty. Rebuilding.", (void*)&shape);

		shared_ptr<btCollisionShape> newShape = nullptr;
		auto indexVertexArray = make_shared<btTriangleIndexVertexArray>();
		auto childShapes = vector<shared_ptr<btCollisionShape>>();
		auto childTriangleMeshes = std::vector<shared_ptr<btIndexedMesh>>();
		auto childIndexVertexArrays = vector<shared_ptr<btTriangleIndexVertexArray>>();

		// if the shape was created with a Node, construct a compound shape from that node's geometry
		// (if any) and the geometry of all child nodes
//		if (auto sourceObject = shape->sourceObject().lock()) {

		bool hasSource = false;
		auto sourceObject = shape->sourceObject();
		if (std::holds_alternative<weak_ptr<Node>>(sourceObject)) {
			auto sourceNodeWeak = std::get<weak_ptr<Node>>(sourceObject);
			if (auto sourceNode = sourceNodeWeak.lock()) {
				hasSource = true;
				newShape = BTCompoundShapeFromNode(sourceNode,
												   shape->type(),
												   bodyType,
												   childShapes,
												   childIndexVertexArrays);
			}
		}
		else if (std::holds_alternative<weak_ptr<Geometry>>(sourceObject)) {
			auto sourceGeometryWeak = std::get<weak_ptr<Geometry>>(sourceObject);
			if (auto sourceGeometry = sourceGeometryWeak.lock()) {
				hasSource = true;
				newShape = BTCollisionShapeFromGeometry(sourceGeometry,
														shape->type(),
														bodyType,
														indexVertexArray);
			}
		}

		if (!hasSource) {
			AE_LOG_E("No shape source.");
			btShape = nullptr;
		}


//		// if the shape was created with a Node, construct a compound shape from that node's geometry
//		// (if any) and the geometry of all child nodes
//		if (auto sourceNode = shape->sourceNode().lock()) {
//			newShape = BTCompoundShapeFromNode(sourceNode,
//											   shape->type(),
//											   bodyType,
//											   childShapes,
//											   childIndexVertexArrays);
//		}
//		// if the shape was created with a geometry, construct a shape with it.
//		else if (auto sourceGeometry = shape->sourceGeometry().lock()) {
//			newShape = BTCollisionShapeFromGeometry(sourceGeometry,
//													shape->type(),
//													bodyType,
//													indexVertexArray);
//		}
//		else {
//			AE_LOG_E("No shape source.");
//			btShape = nullptr;
//		}

//		if (auto sourceGeometry = shape->sourceGeometry().lock()) {
//
//			newShape = BTCollisionShapeFromGeometry(sourceGeometry,
//													shape->type(),
//													bodyType,
//													indexVertexArray);
//		}
//		// if there is no geometry on this node, construct a compound shape encompassing
//		// all child node geometries.
//		else if (auto sourceNode = shape->sourceNode().lock()) {
//
//			newShape = BTCompoundShapeFromNode(sourceNode,
//											   shape->type(),
//											   bodyType,
//											   childShapes,
//											   childIndexVertexArrays);
//		}
//		else {
//			// this might better be an assertation where nodes are checked before submitted to the PhysicsSimulator
//			throw Exception("PhysicsBody with no geometry or source node.");
//			btShape = nullptr;
//		}

		// out parameters
		*btShape = newShape;
		btChildShapes.insert(btChildShapes.end(), childShapes.begin(), childShapes.end());
		btShapeMapping[shape] = make_shared<BulletShapeResources>(newShape,
																  indexVertexArray,
																  childShapes,
																  childIndexVertexArrays);
		newlyCreated = true;
		
		shape->dirtyBits(PHYSICS_SHAPE_DIRTY_BITS_REMOVE(shape->dirtyBits(),
														 PHYSICS_SHAPE_DIRTY_BITS::MODEL));
	}
	else {
		auto resources = btShapeMapping[shape];
		*btShape = resources->shape();
		auto childShapes = resources->childShapes();
		btChildShapes.insert(btChildShapes.end(), childShapes.begin(), childShapes.end());
		newlyCreated = false;
	}
}

void CleanupPhysicsBodyResources(unordered_set<shared_ptr<PhysicsBody>>& active,
								 btDiscreteDynamicsWorld& btWorld,
								 BulletPhysicsSimulator::PhysicsBodyBTMapping& btBodyMapping) {
	
	// gather sorted vector of bodies used this frame
	auto activeBodiesSorted = vector<std::shared_ptr<ae::PhysicsBody>>();
	activeBodiesSorted.reserve(active.size());
	copy(active.begin(), active.end(), back_inserter(activeBodiesSorted));
	sort(activeBodiesSorted.begin(), activeBodiesSorted.end());
	
	// gather sorted vector of bodies in the mapping
	auto storedBodiesSorted = vector<std::shared_ptr<ae::PhysicsBody>>();
	storedBodiesSorted.reserve(btBodyMapping.size());
	for (auto it = btBodyMapping.begin(); it != btBodyMapping.end(); ++it) {
		storedBodiesSorted.emplace_back(it->first);
	}
	sort(storedBodiesSorted.begin(), storedBodiesSorted.end());
	
	// find unused bodies
	auto unused = vector<std::shared_ptr<ae::PhysicsBody>>(storedBodiesSorted.size());
	vector<std::shared_ptr<ae::PhysicsBody>>::iterator it;
	it = set_difference(storedBodiesSorted.begin(), storedBodiesSorted.end(),
						activeBodiesSorted.begin(), activeBodiesSorted.end(),
						unused.begin());
	unused.resize(it - unused.begin());
	
	// deallocate unused bodies
	if (unused.size()) {
		AE_LOG_D("Deallocating bullet body for {} physics bodies...", unused.size());
		
		for (it=unused.begin(); it!=unused.end(); ++it) {
			std::shared_ptr<ae::PhysicsBody> body = *it;

			DeletePhysicsBodyBTResources(body, btWorld, btBodyMapping);
			
			body->dirtyBits(PHYSICS_BODY_DIRTY_BITS_REMOVE(body->dirtyBits(),
														   PHYSICS_BODY_DIRTY_BITS::ALL));
		}
	}
}

void CleanupPhysicsShapeResources(unordered_set<shared_ptr<PhysicsShape>>& active,
								  BulletPhysicsSimulator::PhysicsShapeBTMapping& btShapeMapping) {
	
	// gather sorted vector of shapes used this frame
	auto activeShapesSorted = vector<std::shared_ptr<ae::PhysicsShape>>();
	activeShapesSorted.reserve(active.size());
	copy(active.begin(), active.end(), back_inserter(activeShapesSorted));
	sort(activeShapesSorted.begin(), activeShapesSorted.end());
	
	// gather sorted vector of shapes in the mapping
	auto storedShapesSorted = vector<std::shared_ptr<ae::PhysicsShape>>();
	storedShapesSorted.reserve(btShapeMapping.size());
	for (auto it = btShapeMapping.begin(); it != btShapeMapping.end(); ++it) {
		storedShapesSorted.emplace_back(it->first);
	}
	sort(storedShapesSorted.begin(), storedShapesSorted.end());
	
	// find unused shapes
	auto unused = vector<std::shared_ptr<ae::PhysicsShape>>(storedShapesSorted.size());
	vector<std::shared_ptr<ae::PhysicsShape>>::iterator it;
	it = set_difference(storedShapesSorted.begin(), storedShapesSorted.end(),
						activeShapesSorted.begin(), activeShapesSorted.end(),
						unused.begin());
	unused.resize(it - unused.begin());
	
	// deallocate unused shapes
	if (unused.size()) {
		AE_LOG_D("Deallocating bullet shape for {} physics shape...", unused.size());
		
		for (it=unused.begin(); it!=unused.end(); ++it) {
			std::shared_ptr<ae::PhysicsShape> shape = *it;

			DeletePhysicsShapeBTResources(shape, btShapeMapping);
			
			shape->dirtyBits(PHYSICS_SHAPE_DIRTY_BITS_REMOVE(shape->dirtyBits(),
															 PHYSICS_SHAPE_DIRTY_BITS::ALL));
		}
	}
}

void DeletePhysicsBodyBTResources(shared_ptr<PhysicsBody> body,
								  btDiscreteDynamicsWorld& btWorld,
								  BulletPhysicsSimulator::PhysicsBodyBTMapping& btBodyMapping) {
	
	auto resources = btBodyMapping[body];
	
	shared_ptr<btRigidBody> btRigidBody = resources->body();
	btWorld.removeRigidBody(btRigidBody.get());
	
	btBodyMapping.erase(body);
}

void DeletePhysicsShapeBTResources(shared_ptr<PhysicsShape> shape,
								   BulletPhysicsSimulator::PhysicsShapeBTMapping& btShapeMapping) {
	
	btShapeMapping.erase(shape);
}

shared_ptr<btCollisionShape> BTCollisionShapeFromGeometry(shared_ptr<Geometry> geometry,
														  PHYSICS_SHAPE_TYPE shapeType,
														  PHYSICS_BODY_TYPE bodyType,
														  shared_ptr<btTriangleIndexVertexArray>& indexVertexArray) {
	AE_LOG_T("");

	if (bodyType == PHYSICS_BODY_TYPE::STATIC) {
		AE_LOG_I("Creating static concave polyhedron physics shape for geometry {:p}...", (void*)geometry.get());

		// static objects ALWAYS use btBvhTriangleMeshShape
		// https://pybullet.org/Bullet/phpBB3/viewtopic.php?t=7997

		for (auto& element : geometry->elements()) {

			// WORKS
			auto vertsBase = element->vertices().data();
			auto facesBase = element->faces().data();

			// WORKS
//			auto vertsBase = &element->vertices()[0];
//			auto facesBase = &element->faces()[0];

			// DOES NOT WORK
//			auto verts = element->vertices();
//			auto faces = element->faces();
//			auto vertsBase = verts.data();
//			auto facesBase = faces.data();

			// ^^ asked about on Bullet forum:
			// https://pybullet.org/Bullet/phpBB3/viewtopic.php?p=44462#p44462

			auto indexedMesh = make_shared<btIndexedMesh>();

			indexedMesh->m_numTriangles = (int)element->faces().size();
			indexedMesh->m_triangleIndexBase = (const unsigned char *)facesBase;
			indexedMesh->m_triangleIndexStride = sizeof(Face);
			indexedMesh->m_numVertices = (int)element->vertices().size();
			indexedMesh->m_vertexBase = (const unsigned char *)vertsBase;
			indexedMesh->m_vertexStride = sizeof(Vertex);
			indexedMesh->m_vertexType = PHY_FLOAT;

			indexVertexArray->addIndexedMesh(*indexedMesh, PHY_INTEGER);
		}

		return make_shared<btBvhTriangleMeshShape>(indexVertexArray.get(), true);
	}
	else { // PHYSICS_BODY_TYPE::DYNAMIC or PHYSICS_BODY_TYPE::KINEMATIC

		if (shapeType == PHYSICS_SHAPE_TYPE::BOUNDING_BOX) {
			AE_LOG_I("Creating box physics shape for geometry {:p}...", (void*)geometry.get());

			vec3 extent = geometry->extent();
			float width = extent.x;
			float height = extent.y;
			float length = extent.z;
			return make_shared<btBoxShape>(btVector3((btScalar)width/2.0f,
													 (btScalar)height/2.0f,
													 (btScalar)length/2.0f));
		}
		else if (dynamic_cast<Box*>(geometry.get())) {
			AE_LOG_I("Creating box physics shape for geometry {:p}... (ignoring physics shape type '{}')",
					 (void*)geometry.get(), magic_enum::enum_name(shapeType));

			auto box = dynamic_cast<Box*>(geometry.get());
			return make_shared<btBoxShape>(btVector3((btScalar)box->width()/2.0f,
													 (btScalar)box->height()/2.0f,
													 (btScalar)box->length()/2.0f));
		}
		else if (dynamic_cast<Sphere*>(geometry.get())) {
			AE_LOG_I("Creating sphere physics shape for geometry {:p}... (ignoring physics shape type '{}')",
					 (void*)geometry.get(), magic_enum::enum_name(shapeType));

			auto sphere = dynamic_cast<Sphere*>(geometry.get());
			return make_shared<btSphereShape>((btScalar)sphere->radius());
		}
		else if (dynamic_cast<Capsule*>(geometry.get())) {
			AE_LOG_I("Creating capsule physics shape for geometry {:p}... (ignoring physics shape type '{}')",
					 (void*)geometry.get(), magic_enum::enum_name(shapeType));

			auto capsule = dynamic_cast<Capsule*>(geometry.get());
			return make_shared<btCapsuleShape>((btScalar)capsule->radius(),
											   (btScalar)capsule->height());
		}
		else if (dynamic_cast<Cone*>(geometry.get())) {
			AE_LOG_I("Creating cone physics shape for geometry {:p}... (ignoring physics shape type '{}')",
					 (void*)geometry.get(), magic_enum::enum_name(shapeType));

			auto cone = dynamic_cast<Cone*>(geometry.get());
			return make_shared<btConeShape>((btScalar)cone->radius(),
											(btScalar)cone->height());
		}
		else if (dynamic_cast<Cylinder*>(geometry.get())) {
			AE_LOG_I("Creating cylinder physics shape for geometry {:p}... (ignoring physics shape type '{}')",
					 (void*)geometry.get(), magic_enum::enum_name(shapeType));

			auto cylinder = dynamic_cast<Cylinder*>(geometry.get());
			return make_shared<btCylinderShape>(btVector3((btScalar)cylinder->radius(),
														  (btScalar)cylinder->height()/2.0,
														  (btScalar)cylinder->radius()));
		}
		else {

			if (shapeType == PHYSICS_SHAPE_TYPE::CONCAVE_POLYHEDRON) {
				AE_LOG_I("Creating concave polyhedron physics shape for geometry {:p}...", (void*)geometry.get());

				// https://pybullet.org/Bullet/phpBB3/viewtopic.php?t=7997
				// "You can use btGImpactMeshShape (or btCompoundShapes plus HACD) for concave dynamic rigidbodies"
				// doesn't seem to want to collide with static shapes.
				// -> https://pybullet.org/Bullet/phpBB3/viewtopic.php?p=43020#p43020
				// "- BvhTriangleMeshShapes work well as static concave or convex shapes. But since they are meant to be static, there is no algorithm to make them collide with each other.
				// - ConvexTriangleMeshShapes are efficient as dynamic convex shapes.
				// - GImpact shapes are well optimized for when you need dynamic concave shapes.
				// - Convex decomposition can be used to decompose concave shapes into convex shapes. The resulting convex shapes can then be combined into a CompoundShape, which is also an efficient way to model dynamic concave shapes."
				// More: https://stackoverflow.com/questions/32668218/concave-collision-detection-in-bullet

				for (auto& element : geometry->elements()) {

					// see notes above under PHYSICS_BODY_TYPE::STATIC
					auto vertsBase = element->vertices().data();
					auto facesBase = element->faces().data();

					auto indexedMesh = make_shared<btIndexedMesh>();

					indexedMesh->m_numTriangles = (int)element->faces().size();
					indexedMesh->m_triangleIndexBase = (const unsigned char *)facesBase;
					indexedMesh->m_triangleIndexStride = sizeof(Face);
					indexedMesh->m_numVertices = (int)element->vertices().size();
					indexedMesh->m_vertexBase = (const unsigned char *)vertsBase;
					indexedMesh->m_vertexStride = sizeof(Vertex);
					indexedMesh->m_vertexType = PHY_FLOAT;

					indexVertexArray->addIndexedMesh(*indexedMesh, PHY_INTEGER);
				}

				auto gImpactMeshShape = make_shared<btGImpactMeshShape>(indexVertexArray.get());
				// https://pybullet.org/Bullet/BulletFull/classbtGImpactShapeInterface.html#a7d26525396fa957d10e36c099c58480f
				gImpactMeshShape->updateBound();

				return gImpactMeshShape;
			}
			else { // PHYSICS_SHAPE_TYPE::CONVEX_HULL

				AE_LOG_I("Creating convex hull physics shape for geometry {:p}...", (void*)geometry.get());

				// tips here: https://pybullet.org/Bullet/phpBB3/viewtopic.php?t=11385

				// https://pybullet.org/Bullet/BulletFull/classbtConvexHullShape.html#a069cf26ba277f9f5f141128fee345eaf
				auto originalShape = make_shared<btConvexHullShape>();
				for (auto& element : geometry->elements()) {
					for (auto& vertex : element->vertices()) {
						originalShape->addPoint(BTVector3FromGLMVec3(vertex.position), false);
					}
				}
				originalShape->recalcLocalAabb();

				// reduce number of verticies
				// http://www.bulletphysics.org/mediawiki-1.5.8/index.php/BtShapeHull_vertex_reduction_utility
				auto hull = btShapeHull(originalShape.get());
				btScalar margin = originalShape->getMargin();
				hull.buildHull((btScalar)margin);

				auto reducedShape = make_shared<btConvexHullShape>((btScalar*)hull.getVertexPointer(),
																   hull.numVertices(),
																   sizeof(btVector3));

				reducedShape->optimizeConvexHull();

				// for debug drawing
				if (!reducedShape->initializePolyhedralFeatures()) {
					AE_LOG_W("Could not initialize polyhedral features for reduced btConvexHullShape.");
				}

				return reducedShape;
			}
		}
	}

	return nullptr;
}

shared_ptr<btCompoundShape> BTCompoundShapeFromNode(shared_ptr<Node> node,
													PHYSICS_SHAPE_TYPE shapeType,
													PHYSICS_BODY_TYPE bodyType,
													vector<shared_ptr<btCollisionShape>>& childShapes,
													vector<shared_ptr<btTriangleIndexVertexArray>>& childIndexVertexArrays) {
	AE_LOG_T("");

	auto compoundShape = make_shared<btCompoundShape>(true); // for compound shapes only

	// TODO: if node also has a geometry, include that
	for (auto& n : node->children(true)) {
		if (auto geometry = n->geometry()) {

			auto childTriangleMeshes = std::vector<shared_ptr<btIndexedMesh>>(); // TEMP? ^ if used pass in
			auto childIndexVertexArray = make_shared<btTriangleIndexVertexArray>();
			auto childCollisionShape = BTCollisionShapeFromGeometry(geometry,
																	shapeType,
																	bodyType,
																	childIndexVertexArray);
			
			childShapes.push_back(childCollisionShape);
			childIndexVertexArrays.push_back(childIndexVertexArray);

			bool wasScaled = false;
			auto localTransform = BTTransformFromGLMMat4(TransformByRemovingScale(n->transform(), wasScaled));
			if (wasScaled) {
				AE_LOG_W("Ignorning (child) scale for Node {:p}.", (void*)node.get());
			}
			compoundShape->addChildShape(localTransform, childCollisionShape.get());
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
		AE_LOG_D("Bullet debug modes: {}", btModes);
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
	mat4 glmMat;
	from.getOpenGLMatrix(value_ptr(glmMat));
	return glmMat;
}

btVector3 BTVector3FromGLMVec3(const vec3& from) {
	return btVector3(from.x, from.y, from.z);
}

btVector4 BTVector4FromGLMVec4(const vec4& from) {
	return btVector4(from.x, from.y, from.z, from.w);
}

static btQuaternion BTQuaternionFromGLMQuat(const quat& from) {
	
	return btQuaternion(from.x, from.y, from.z, from.w);
	
}

btTransform BTTransformFromGLMMat4(const mat4& from) {
	
	// this version (probably) does not strip scale & sheer
	
	btTransform bulletTransform;
	bulletTransform.setIdentity();
	bulletTransform.setFromOpenGLMatrix(value_ptr(from));
	return bulletTransform;
	
	
//	// THIS VERSION STRIPS (hopefully!) scale & sheer
//	
//	btTransform bulletTransform;
//	bulletTransform.setIdentity();
//	
//	vec3 scale;
//	quat orientation;
//	vec3 translation;
//	vec3 skew;
//	vec4 perspective;
//
//	decompose(from,
//			  scale,
//			  orientation,
//			  translation,
//			  skew,
//			  perspective);
//	
//	bulletTransform.setOrigin(BTVector3FromGLMVec3(translation));
//	bulletTransform.setRotation(BTQuaternionFromGLMQuat(orientation));
//	
//	return bulletTransform;
}

mat4 TransformByRemovingScale(const mat4& m, bool& scaled) {
	// TODO: optimize

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

	scaled = !Equal(scale, {1, 1, 1});
	if (scaled) return translate(mat4(1.0), translation) * mat4_cast(orientation) * mat4(1.0);
	else return m;
}

//glm::mat4 BulletToGlm(const btTransform& t)
//{
//	// from here: https://pybullet.org/Bullet/phpBB3/viewtopic.php?f=9&t=12161&p=41084#p41084
//	
//	glm::mat4 m(0);
//	const btMatrix3x3& basis = t.getBasis();
//	// rotation
//	for (int r = 0; r < 3; r++)
//	{
//		for (int c = 0; c < 3; c++)
//		{
//			m[c][r] = basis[r][c];
//		}
//	}
//	// traslation
//	btVector3 origin = t.getOrigin();
//	m[3][0] = origin.getX();
//	m[3][1] = origin.getY();
//	m[3][2] = origin.getZ();
//	// unit scale
//	m[0][3] = 0.0f;
//	m[1][3] = 0.0f;
//	m[2][3] = 0.0f;
//	m[3][3] = 1.0f;
//	return m;
//}
