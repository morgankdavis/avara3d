//
//  BulletPhysicsSimulator.cc
//	avara-engine
//
//  Created by Morgan Davis on 5/1/18.
//  Copyright © 2018 Morgan K Davis. All rights reserved.
//

#include "physics/bullet/BulletPhysicsSimulator.h"

#include <variant>

#include "btBulletCollisionCommon.h"
#include "btBulletDynamicsCommon.h"
#include "BulletCollision/Gimpact/btGImpactShape.h"
#include "BulletCollision/CollisionShapes/btShapeHull.h"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtx/matrix_decompose.hpp"
#include "LinearMath/btIDebugDraw.h"
#include "LinearMath/btScalar.h" // btGetVersion() !
#include "magic_enum.hpp"

#include "diagnostic/logging/Logger.h"
#include "geometry/Geometry.h"
#include "geometry/primitives/Box.h"
#include "geometry/primitives/Capsule.h"
#include "geometry/primitives/Cone.h"
#include "geometry/primitives/Cylinder.h"
#include "geometry/primitives/Plane.h"
#include "geometry/primitives/Sphere.h"
#include "physics/ConvexDecomposer.h"
#include "physics/PhysicsBody.h"
#include "physics/PhysicsBodyResources.h"
#include "physics/PhysicsShape.h"
#include "physics/PhysicsWorld.h"
#include "physics/bullet/BulletBodyResources.h"
#include "physics/bullet/BulletDebugDrawer.h"
#include "physics/bullet/BulletShapeResources.h"
#include "scene/Node.h"
#include "scene/Scene.h"
#include "utilities/Utilities.h"


using namespace ae;
using namespace ae::utils;
using namespace glm;
using namespace std;


constexpr unsigned MAX_SUBSTEPS = 20;


/*********************************************************************************************
	Static Prototypes
 *********************************************************************************************/

static shared_ptr<btCollisionShape>
BTShapeFromSourceGeometry(shared_ptr<Geometry> geometry,
						  PHYSICS_SHAPE_TYPE shapeType,
						  PHYSICS_BODY_TYPE bodyType,
						  vector<shared_ptr<btCollisionShape>>& btShapes,
						  vector<shared_ptr<btTriangleIndexVertexArray>>& btIndexVertexArrays);
static shared_ptr<btCollisionShape>
BTShapeFromSourceNode(shared_ptr<Node> node,
					  PHYSICS_SHAPE_TYPE shapeType,
					  PHYSICS_BODY_TYPE bodyType,
					  vector<shared_ptr<btCollisionShape>>& btShapes,
					  vector<shared_ptr<btTriangleIndexVertexArray>>& btIndexVertexArrays);
static shared_ptr<btCollisionShape>
BTShapeFromGeometryElement(shared_ptr<GeometryElement> element,
						   shared_ptr<Geometry> geometry,
						   PHYSICS_SHAPE_TYPE shapeType,
						   PHYSICS_BODY_TYPE bodyType,
						   shared_ptr<btTriangleIndexVertexArray>& btIndexVertexArray);
static shared_ptr<btCollisionShape>
BTShapeFromHACDGeometryElement(shared_ptr<GeometryElement> element,
							   shared_ptr<Geometry> geometry,
							   PHYSICS_SHAPE_TYPE shapeType,
							   PHYSICS_BODY_TYPE bodyType,
							   shared_ptr<btTriangleIndexVertexArray>& btIndexVertexArray);
static shared_ptr<btCompoundShape>
BTShapeFromGeometry(shared_ptr<Geometry> geometry,
					//shared_ptr<Node> node,
					PHYSICS_SHAPE_TYPE shapeType,
					PHYSICS_BODY_TYPE bodyType,
					vector<shared_ptr<btCollisionShape>>& btShapes,
					vector<shared_ptr<btTriangleIndexVertexArray>>& btIndexVertexArrays);
static void
AddBTShapeFromNodeRec(shared_ptr<Node> node,
					  PHYSICS_SHAPE_TYPE shapeType,
					  PHYSICS_BODY_TYPE bodyType,
					  shared_ptr<btCompoundShape> compoundShape,
					  vector<shared_ptr<btCollisionShape>>& btShapes,
					  vector<shared_ptr<btTriangleIndexVertexArray>>& btIndexVertexArrays);
static shared_ptr<btConvexHullShape>
BTConvexHullShapeFromGeometryElement(shared_ptr<GeometryElement> element);
static shared_ptr<btConvexHullShape>
BTConvexHullHACDShapeFromGeometryElement(shared_ptr<GeometryElement> element);
static shared_ptr<btGImpactMeshShape>
BTGImpactMeshShapeFromGeometryElement(shared_ptr<GeometryElement> element,
									  shared_ptr<btTriangleIndexVertexArray>& indexVertexArray);
static shared_ptr<btBvhTriangleMeshShape>
BTBvhTriangleMeshShapeFromGeometryElement(shared_ptr<GeometryElement> element,
										  shared_ptr<btTriangleIndexVertexArray>& indexVertexArray);
static btIDebugDraw::DebugDrawModes 	BTDebugDrawModesForAEDebugOptions(const DEBUG_OPTIONS& options);
static vec3 							GLMVec3FromBTVector3(const btVector3& from);
static vec4 							GLMVec4FromBTVector4(const btVector4& from);
static mat4 							GLMMat4FromBTTransform(const btTransform& from);
static btVector3 						BTVector3FromGLMVec3(const vec3& from);
static btVector4 						BTVector4FromGLMVec4(const vec4& from);
static btQuaternion 					BTQuaternionFromGLMQuat(const quat& from);
static btTransform 						BTTransformFromGLMMat4(const mat4& from);
static mat4 							TransformByRemovingScale(const mat4& m, bool& scaled);
static btTransform&						BTIdentityTransform();

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
													  _btCollisionConfiguration.get())) {
#ifdef OPENGL_CORE
	_debugDrawer = make_shared<BulletDebugDrawer>();
#endif

	AE_LOG_I("Bullet Physics version: {}",  btGetVersion());

#ifdef OPENGL_CORE
	_btWorld.get()->setDebugDrawer(_debugDrawer.get());
#endif
}

BulletPhysicsSimulator::~BulletPhysicsSimulator() {
	AE_LOG_D("Destroying BulletPhysicsSimulator {:p}", (void*)this);
}

/*********************************************************************************************
	Internal
 *********************************************************************************************/

void BulletPhysicsSimulator::drawDebug(Renderer& renderer,
									   const mat4& viewMat,
									   const mat4& projectionMat,
									   const DEBUG_OPTIONS& debugOptions) {
#ifdef OPENGL_CORE
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

void BulletPhysicsSimulator::beginUpdate(const Scene& scene) {
	PhysicsSimulator::beginUpdate(scene);
}

void BulletPhysicsSimulator::endUpdate(const Scene& scene) {
	PhysicsSimulator::endUpdate(scene);
}

void BulletPhysicsSimulator::update(Scene& scene) {
	PhysicsSimulator::update(scene);

	auto world = scene.physicsWorld();

	if (PHYSICS_WORLD_DIRTY_MASK_CONTAINS(world->dirtyMask(), PHYSICS_WORLD_DIRTY_MASK::TIMESTEP)) {
		_timestep = world->timestep();

		world->dirtyMask(PHYSICS_WORLD_DIRTY_MASK_REMOVE(world->dirtyMask(),
														 PHYSICS_WORLD_DIRTY_MASK::TIMESTEP));
	}

#warning set this gravity for all physics objects, too...
	if (PHYSICS_WORLD_DIRTY_MASK_CONTAINS(world->dirtyMask(), PHYSICS_WORLD_DIRTY_MASK::GRAVITY)) {
		_btWorld->setGravity(BTVector3FromGLMVec3(world->gravity()));

		world->dirtyMask(PHYSICS_WORLD_DIRTY_MASK_REMOVE(world->dirtyMask(),
														 PHYSICS_WORLD_DIRTY_MASK::GRAVITY));
	}
}

void BulletPhysicsSimulator::sync(Scene& scene) {
	PhysicsSimulator::sync(scene);
}

void BulletPhysicsSimulator::update(PhysicsBody& body,
									Node& node) {
	PhysicsSimulator::update(body, node);

	auto shape = body.shape();
	auto dirtyMask = body.dirtyMask();

	auto bodyResources = static_pointer_cast<BulletBodyResources>(body.resources());

	auto shapeResources = static_pointer_cast<BulletShapeResources>(body.shape()->resources());
	// front is either the only btCollisionShape or a btCompound shape with child shapes at index 1+
	auto btShape = shapeResources->shapes().front();

	shared_ptr<btRigidBody> btBody = nullptr;

	// since the BT body depends on the BT shape, if the shape was dirty (and re-created)
	// we also re-create the body.
	// additionally, since some physical properties have to be passed via btRigidBodyConstructionInfo,
	// they require re-creating the rigid body.
	if (PHYSICS_BODY_DIRTY_MASK_CONTAINS(dirtyMask, PHYSICS_BODY_DIRTY_MASK::SHAPE)
		// these properties only appear to be set-able via btRigidBodyConstructionInfo
		|| PHYSICS_BODY_DIRTY_MASK_CONTAINS(dirtyMask, PHYSICS_BODY_DIRTY_MASK::TYPE)
		|| PHYSICS_BODY_DIRTY_MASK_CONTAINS(dirtyMask, PHYSICS_BODY_DIRTY_MASK::MOMENT_OF_INERTIA)
		|| PHYSICS_BODY_DIRTY_MASK_CONTAINS(dirtyMask, PHYSICS_BODY_DIRTY_MASK::FRICTION)
		|| PHYSICS_BODY_DIRTY_MASK_CONTAINS(dirtyMask, PHYSICS_BODY_DIRTY_MASK::ROLLING_FRICTION)
		|| PHYSICS_BODY_DIRTY_MASK_CONTAINS(dirtyMask, PHYSICS_BODY_DIRTY_MASK::RESTITUTION)) {

		AE_LOG_D("Creating rigid body for physics body {:p}...", (void*)&body);

//		#warning experimental
//		if (PHYSICS_SHAPE_DIRTY_MASK_CONTAINS(shape->dirtyMask(),
//											  PHYSICS_SHAPE_DIRTY_MASK::SCALE)) {
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
////			shape->dirtyMask(PHYSICS_SHAPE_DIRTY_MASK_REMOVE(shape->dirtyMask(),
////															 PHYSICS_SHAPE_DIRTY_MASK::SCALE));
//		}

		bool wasScaled = false;
		auto transform = BTTransformFromGLMMat4(TransformByRemovingScale(node.worldTransform(), wasScaled));
		if (wasScaled) {
			// TODO: should address this.
			// can hold a burned transformed vertex data in the physics body/shape?
			AE_LOG_W("Ignorning scale for Node {:p} with PhysicsBody {:p}.",
					 (void*)&node, (void*)&body);
		}

		auto newMotionState = make_shared<btDefaultMotionState>(transform);

		//auto collisionShape = dynamic_pointer_cast<btCollisionShape>(*btShape);

		auto localInertia = BTVector3FromGLMVec3(body.momentOfInertia());

		auto mass = body.mass();

		if (body.type() == PHYSICS_BODY_TYPE::STATIC
			|| body.type() == PHYSICS_BODY_TYPE::KINEMATIC) {
			mass = 0;
		}
		else if (body.type() == PHYSICS_BODY_TYPE::DYNAMIC) {
			btShape->calculateLocalInertia(mass, localInertia);
		}

		btRigidBody::btRigidBodyConstructionInfo rigidBodyInfo(mass,
															   newMotionState.get(),
															   btShape.get(),
															   localInertia);

		rigidBodyInfo.m_mass = mass;
		rigidBodyInfo.m_linearDamping = body.linearDamping();
		rigidBodyInfo.m_angularDamping = body.angularDamping();
		rigidBodyInfo.m_friction = body.friction();
		rigidBodyInfo.m_rollingFriction = body.rollingFriction();
		rigidBodyInfo.m_restitution = body.restitution();
		rigidBodyInfo.m_linearSleepingThreshold = body.linearSleepingThreshold();
		rigidBodyInfo.m_angularSleepingThreshold = body.angularSleepingThreshold();

		auto newBody = make_shared<btRigidBody>(rigidBodyInfo);

		if (body.type() == PHYSICS_BODY_TYPE::STATIC) {
			newBody->setCollisionFlags(btCollisionObject::CF_STATIC_OBJECT);
		}
		else if (body.type() == PHYSICS_BODY_TYPE::KINEMATIC) {
			newBody->setCollisionFlags(btCollisionObject::CF_KINEMATIC_OBJECT);
		}

		_btWorld->addRigidBody(newBody.get());

		btBody = newBody;
		bodyResources->body(btBody);
		bodyResources->motionState(newMotionState);

		body.dirtyMask(PHYSICS_BODY_DIRTY_MASK_REMOVE(body.dirtyMask(),
													  PHYSICS_BODY_DIRTY_MASK::SHAPE));
		body.dirtyMask(PHYSICS_BODY_DIRTY_MASK_REMOVE(body.dirtyMask(),
													  PHYSICS_BODY_DIRTY_MASK::TYPE));
		body.dirtyMask(PHYSICS_BODY_DIRTY_MASK_REMOVE(body.dirtyMask(),
													  PHYSICS_BODY_DIRTY_MASK::MOMENT_OF_INERTIA));
		body.dirtyMask(PHYSICS_BODY_DIRTY_MASK_REMOVE(body.dirtyMask(),
													  PHYSICS_BODY_DIRTY_MASK::FRICTION));
		body.dirtyMask(PHYSICS_BODY_DIRTY_MASK_REMOVE(body.dirtyMask(),
													  PHYSICS_BODY_DIRTY_MASK::ROLLING_FRICTION));
		body.dirtyMask(PHYSICS_BODY_DIRTY_MASK_REMOVE(body.dirtyMask(),
													  PHYSICS_BODY_DIRTY_MASK::RESTITUTION));
	}
	else {
		btBody = bodyResources->body();
	}

	// check and set the rest of the properties

	if (PHYSICS_BODY_DIRTY_MASK_CONTAINS(dirtyMask, PHYSICS_BODY_DIRTY_MASK::LINEAR_FACTOR)) {
		btBody->setLinearFactor(BTVector3FromGLMVec3(body.linearFactor()));

		body.dirtyMask(PHYSICS_BODY_DIRTY_MASK_REMOVE(body.dirtyMask(),
													  PHYSICS_BODY_DIRTY_MASK::LINEAR_FACTOR));
	}
	if (PHYSICS_BODY_DIRTY_MASK_CONTAINS(dirtyMask, PHYSICS_BODY_DIRTY_MASK::ANGULAR_FACTOR)) {
		btBody->setAngularFactor(BTVector3FromGLMVec3(body.angularFactor()));

		body.dirtyMask(PHYSICS_BODY_DIRTY_MASK_REMOVE(body.dirtyMask(),
													  PHYSICS_BODY_DIRTY_MASK::ANGULAR_FACTOR));
	}
	if (PHYSICS_BODY_DIRTY_MASK_CONTAINS(dirtyMask, PHYSICS_BODY_DIRTY_MASK::LINEAR_DAMPING)
		|| PHYSICS_BODY_DIRTY_MASK_CONTAINS(dirtyMask, PHYSICS_BODY_DIRTY_MASK::ANGULAR_DAMPING)) {
		btBody->setDamping(body.linearDamping(), body.angularDamping());

		body.dirtyMask(PHYSICS_BODY_DIRTY_MASK_REMOVE(body.dirtyMask(),
													  PHYSICS_BODY_DIRTY_MASK::LINEAR_DAMPING));
		body.dirtyMask(PHYSICS_BODY_DIRTY_MASK_REMOVE(body.dirtyMask(),
													  PHYSICS_BODY_DIRTY_MASK::ANGULAR_DAMPING));
	}
	if (PHYSICS_BODY_DIRTY_MASK_CONTAINS(dirtyMask, PHYSICS_BODY_DIRTY_MASK::LINEAR_SLEEPING_THRESHOLD)
		|| PHYSICS_BODY_DIRTY_MASK_CONTAINS(dirtyMask, PHYSICS_BODY_DIRTY_MASK::ANGULAR_SLEEPING_THRESHOLD)) {
		btBody->setSleepingThresholds(body.linearSleepingThreshold(), body.angularSleepingThreshold());

//		AE_LOG_D("linearSleepingThreshold: {}", (*btBody)->getLinearSleepingThreshold());
//		AE_LOG_D("angularSleepingThreshold: {}", (*btBody)->getAngularSleepingThreshold());

		body.dirtyMask(PHYSICS_BODY_DIRTY_MASK_REMOVE(body.dirtyMask(),
													  PHYSICS_BODY_DIRTY_MASK::LINEAR_SLEEPING_THRESHOLD));
		body.dirtyMask(PHYSICS_BODY_DIRTY_MASK_REMOVE(body.dirtyMask(),
													  PHYSICS_BODY_DIRTY_MASK::ANGULAR_SLEEPING_THRESHOLD));
	}
	if (PHYSICS_BODY_DIRTY_MASK_CONTAINS(dirtyMask, PHYSICS_BODY_DIRTY_MASK::AFFECTED_BY_GRAVITY)) {
		if (body.affectedByGravity()) {
			btBody->setGravity(_btWorld->getGravity());
		}
		else {
			// NOTE: setting world gravity resets this
			btBody->setGravity({0, 0, 0});
		}

		body.dirtyMask(PHYSICS_BODY_DIRTY_MASK_REMOVE(body.dirtyMask(),
													  PHYSICS_BODY_DIRTY_MASK::AFFECTED_BY_GRAVITY));
	}
	if (PHYSICS_BODY_DIRTY_MASK_CONTAINS(dirtyMask, PHYSICS_BODY_DIRTY_MASK::ALLOWS_RESTING)) {
		if (body.allowsResting()) {
			btBody->setActivationState(ACTIVE_TAG);
		}
		else {
			btBody->setActivationState(DISABLE_DEACTIVATION);
		}

		body.dirtyMask(PHYSICS_BODY_DIRTY_MASK_REMOVE(body.dirtyMask(),
													  PHYSICS_BODY_DIRTY_MASK::ALLOWS_RESTING));
	}
	if (PHYSICS_BODY_DIRTY_MASK_CONTAINS(dirtyMask, PHYSICS_BODY_DIRTY_MASK::FORCES)) {
#warning TODO

		body.dirtyMask(PHYSICS_BODY_DIRTY_MASK_REMOVE(body.dirtyMask(),
													  PHYSICS_BODY_DIRTY_MASK::FORCES));
	}
	if (PHYSICS_BODY_DIRTY_MASK_CONTAINS(dirtyMask, PHYSICS_BODY_DIRTY_MASK::TORQUES)) {
#warning TODO

		body.dirtyMask(PHYSICS_BODY_DIRTY_MASK_REMOVE(body.dirtyMask(),
													  PHYSICS_BODY_DIRTY_MASK::TORQUES));
	}
	if (PHYSICS_BODY_DIRTY_MASK_CONTAINS(dirtyMask, PHYSICS_BODY_DIRTY_MASK::LINEAR_VELOCITY)) {
		btBody->setLinearVelocity(BTVector3FromGLMVec3(body.linearVelocity()));

		body.dirtyMask(PHYSICS_BODY_DIRTY_MASK_REMOVE(body.dirtyMask(),
													  PHYSICS_BODY_DIRTY_MASK::LINEAR_VELOCITY));
	}
	if (PHYSICS_BODY_DIRTY_MASK_CONTAINS(dirtyMask, PHYSICS_BODY_DIRTY_MASK::ANGULAR_VELOCITY)) {
		btBody->setAngularVelocity(BTVector3FromGLMVec3(body.angularVelocity()));

		body.dirtyMask(PHYSICS_BODY_DIRTY_MASK_REMOVE(body.dirtyMask(),
													  PHYSICS_BODY_DIRTY_MASK::ANGULAR_VELOCITY));
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
//	if (PHYSICS_SHAPE_DIRTY_MASK_CONTAINS(shape->dirtyMask(),
//										  PHYSICS_SHAPE_DIRTY_MASK::SCALE)) {
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
//		shape->dirtyMask(PHYSICS_SHAPE_DIRTY_MASK_REMOVE(shape->dirtyMask(),
//														 PHYSICS_SHAPE_DIRTY_MASK::SCALE));
//	}

	// back-fill PhysicsBody properties

// *** causing a loop canceling out any manual dynamic movement ***

	body.linearVelocity(GLMVec3FromBTVector3(btBody->getLinearVelocity()), false);
	body.angularVelocity(GLMVec3FromBTVector3(btBody->getAngularVelocity()), false);
	body.resting(btBody->getActivationState() == (ISLAND_SLEEPING ? true : false));

// if kinematic, apply visual transform to bullet model

	if (body.type() == PHYSICS_BODY_TYPE::KINEMATIC) {

		auto toTransform = BTTransformFromGLMMat4(node.worldTransform());
		//	btBody->proceedToTransform(toTransform); // this appears to affect dynamic bodies
		auto motionState = btBody->getMotionState();
		motionState->setWorldTransform(toTransform); // and this kinematic...
		btBody->setMotionState(motionState);

		btBody->setActivationState(ACTIVE_TAG);
		//	btBody->forceActivationState(ACTIVE_TAG);
	}
}

void BulletPhysicsSimulator::sync(PhysicsBody& body,
								  Node& node,
								  mat4& worldTransform) {
	PhysicsSimulator::sync(body, node, worldTransform);

	auto bodyResources = static_pointer_cast<BulletBodyResources>(body.resources());
	auto btBody = bodyResources->body();

	// get body transforms and apply back to scene graph

	btTransform btWorldTransform;
	btWorldTransform.setIdentity();
	//btMotionState->getWorldTransform(btWorldTransform); // crash?
	btBody->getMotionState()->getWorldTransform(btWorldTransform);

	worldTransform = GLMMat4FromBTTransform(btWorldTransform);
}

void BulletPhysicsSimulator::update(PhysicsShape& shape,
									PHYSICS_BODY_TYPE bodyType,
									bool& updated) {
	PhysicsSimulator::update(shape, bodyType, updated);

	if (PHYSICS_SHAPE_DIRTY_MASK_CONTAINS(shape.dirtyMask(),
										  PHYSICS_SHAPE_DIRTY_MASK::MODEL)) {

		AE_LOG_D("Shape {:p} model dirty. Rebuilding.", (void*)&shape);

		shared_ptr<btCollisionShape> newShape = nullptr;

		auto btShapes = vector<shared_ptr<btCollisionShape>>();
		auto btIndexVertexArrays = vector<shared_ptr<btTriangleIndexVertexArray>>();

		auto sourceObject = shape.sourceObject();

		if (holds_alternative<weak_ptr<Geometry>>(sourceObject)) {
			if (auto sourceGeometry = get<weak_ptr<Geometry>>(sourceObject).lock()) {

				newShape = BTShapeFromSourceGeometry(sourceGeometry,
													 shape.type(),
													 bodyType,
													 btShapes,
													 btIndexVertexArrays);
			}
		}

		else if (holds_alternative<weak_ptr<Node>>(sourceObject)) {
			if (auto sourceNode = get<weak_ptr<Node>>(sourceObject).lock()) {

				newShape = BTShapeFromSourceNode(sourceNode,
												 shape.type(),
												 bodyType,
												 btShapes,
												 btIndexVertexArrays);
			}
		}

		if (newShape) {
			btShapes.insert(btShapes.begin(), newShape);

			auto shapeResources = static_pointer_cast<BulletShapeResources>(shape.resources());
			shapeResources->shapes(btShapes);
			shapeResources->indexVertexArrays(btIndexVertexArrays);
			updated = true;

			shape.dirtyMask(PHYSICS_SHAPE_DIRTY_MASK_REMOVE(shape.dirtyMask(),
															PHYSICS_SHAPE_DIRTY_MASK::MODEL));
		}
		else {
			updated = false;
			AE_LOG_E("PhysicsShape with no geometry or source node.");
		}
	}
	else {
		updated = false;
	}
}

void BulletPhysicsSimulator::sync(PhysicsShape& shape,
								  PHYSICS_BODY_TYPE bodyType) {
	PhysicsSimulator::sync(shape, bodyType);

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

static shared_ptr<btCollisionShape>
BTShapeFromSourceGeometry(shared_ptr<Geometry> geometry,
						  PHYSICS_SHAPE_TYPE shapeType,
						  PHYSICS_BODY_TYPE bodyType,
						  vector<shared_ptr<btCollisionShape>>& btShapes,
						  vector<shared_ptr<btTriangleIndexVertexArray>>& btIndexVertexArrays) {

	shared_ptr<btCollisionShape> newShape = nullptr;

	if (bodyType == PHYSICS_BODY_TYPE::DYNAMIC
		&& shapeType == PHYSICS_SHAPE_TYPE::CONCAVE_POLYHEDRON) {
		// HACD
	}
	else if (geometry->elements().size() == 1) {
		// make a single shape

		auto indexVertexArray = make_shared<btTriangleIndexVertexArray>();
		newShape = BTShapeFromGeometryElement(geometry->elements().front(),
											  geometry,
											  shapeType,
											  bodyType,
											  indexVertexArray);
		btShapes.push_back(newShape);
		btIndexVertexArrays.push_back(indexVertexArray);
	}
	else if (geometry->elements().size() > 1) {
		// make compound shape, loop BTShapeFromGeometryElement()

		newShape = BTShapeFromGeometry(geometry,
									   shapeType,
									   bodyType,
									   btShapes,
									   btIndexVertexArrays);
	}
	else {
		AE_LOG_E("Can't create physic shape for Geometry {:p}: has no elements.",
				 (void*)geometry.get());
	}

	return newShape;
}

static shared_ptr<btCollisionShape>
BTShapeFromSourceNode(shared_ptr<Node> node,
					  PHYSICS_SHAPE_TYPE shapeType,
					  PHYSICS_BODY_TYPE bodyType,
					  vector<shared_ptr<btCollisionShape>>& btShapes,
					  vector<shared_ptr<btTriangleIndexVertexArray>>& btIndexVertexArrays) {

	auto newShape = make_shared<btCompoundShape>(true);

	// add the root geometry
	if (node->geometry()) {

		auto indexVertexArray = make_shared<btTriangleIndexVertexArray>();
		auto rootNodeShape = BTShapeFromGeometry(node->geometry(),
												 shapeType,
												 bodyType,
												 btShapes,
												 btIndexVertexArrays);
		static_pointer_cast<btCompoundShape>(newShape)->addChildShape(BTIdentityTransform(),
																	  rootNodeShape.get());

		btShapes.push_back(rootNodeShape);
	}

	// add child geometries recursively
	auto children = node->children(false);
	for (auto& childNode : children) {
		AddBTShapeFromNodeRec(childNode,
							  shapeType,
							  bodyType,
							  static_pointer_cast<btCompoundShape>(newShape),
							  btShapes,
							  btIndexVertexArrays);
	}

	return newShape;
}

shared_ptr<btCollisionShape> BTShapeFromGeometryElement(shared_ptr<GeometryElement> element,
														shared_ptr<Geometry> geometry,
														PHYSICS_SHAPE_TYPE shapeType,
														PHYSICS_BODY_TYPE bodyType,
														shared_ptr<btTriangleIndexVertexArray>& indexVertexArray) {
	if (shapeType == PHYSICS_SHAPE_TYPE::BOUNDING_BOX) {
		AE_LOG_I("Creating box physics shape for GeometryElement {:p}...",
				 (void*)element.get());

		// TODO: this isn't right
		vec3 extent = element->extent();
		float width = extent.x;
		float height = extent.y;
		float length = extent.z;
		return make_shared<btBoxShape>(btVector3((btScalar)width/2.0f,
												 (btScalar)height/2.0f,
												 (btScalar)length/2.0f));
	}
	else if (auto box = dynamic_cast<Box*>(geometry.get())) {
		AE_LOG_I("Creating box physics shape for GeometryElement {:p}... (ignoring physics shape type '{}')",
				 (void*)element.get(), magic_enum::enum_name(shapeType));

		return make_shared<btBoxShape>(btVector3((btScalar)box->width()/2.0f,
												 (btScalar)box->height()/2.0f,
												 (btScalar)box->length()/2.0f));
	}
	else if (auto capsule = dynamic_cast<Capsule*>(geometry.get())) {
		AE_LOG_I("Creating capsule physics shape for GeometryElement {:p}... (ignoring physics shape type '{}')",
				 (void*)element.get(), magic_enum::enum_name(shapeType));

		return make_shared<btCapsuleShape>((btScalar)capsule->radius(),
										   (btScalar)capsule->height());
	}
	else if (auto cone  = dynamic_cast<Cone*>(geometry.get())) {
		AE_LOG_I("Creating cone physics shape for GeometryElement {:p}... (ignoring physics shape type '{}')",
				 (void*)element.get(), magic_enum::enum_name(shapeType));

		return make_shared<btConeShape>((btScalar)cone->radius(),
										(btScalar)cone->height());
	}
	else if (auto cylinder = dynamic_cast<Cylinder*>(geometry.get())) {
		AE_LOG_I("Creating cylinder physics shape for GeometryElement {:p}... (ignoring physics shape type '{}')",
				 (void*)element.get(), magic_enum::enum_name(shapeType));

		return make_shared<btCylinderShape>(btVector3((btScalar)cylinder->radius(),
													  (btScalar)cylinder->height()/2.0,
													  (btScalar)cylinder->radius()));
	}
	else if (auto plane = dynamic_cast<Plane*>(geometry.get())) {
		// ae::Plane is not a true plane, it has a length and width, so we need to use a btBoxShape
		return make_shared<btBoxShape>(btVector3((btScalar)plane->width()/2.0f,
												 (btScalar)plane->height()/2.0f,
												 (btScalar)0));
	}
	else if (auto sphere = dynamic_cast<Sphere*>(geometry.get())) {
		AE_LOG_I("Creating sphere physics shape for GeometryElement {:p}... (ignoring physics shape type '{}')",
				 (void*)element.get(), magic_enum::enum_name(shapeType));

		return make_shared<btSphereShape>((btScalar)sphere->radius());
	}
		// * no Bullet primitives for Torus or Tube *
	else if (shapeType == PHYSICS_SHAPE_TYPE::CONVEX_HULL) {

		return BTConvexHullShapeFromGeometryElement(element);
	}
	else if (bodyType == PHYSICS_BODY_TYPE::DYNAMIC) {

		return BTConvexHullHACDShapeFromGeometryElement(element);
	}
	else if (bodyType == PHYSICS_BODY_TYPE::KINEMATIC) {

		return BTGImpactMeshShapeFromGeometryElement(element, indexVertexArray);
	}
	else if (bodyType == PHYSICS_BODY_TYPE::STATIC) {

		return BTBvhTriangleMeshShapeFromGeometryElement(element, indexVertexArray);
	}

	return nullptr;
}

shared_ptr<btCollisionShape> BTShapeFromHACDGeometryElement(shared_ptr<GeometryElement> element,
															shared_ptr<Geometry> geometry,
															PHYSICS_SHAPE_TYPE shapeType,
															PHYSICS_BODY_TYPE bodyType,
															shared_ptr<btTriangleIndexVertexArray>& btIndexVertexArray) {
	// DO IT
}

shared_ptr<btCompoundShape> BTShapeFromGeometry(shared_ptr<Geometry> geometry,
//												 shared_ptr<Node> node,
												PHYSICS_SHAPE_TYPE shapeType,
												PHYSICS_BODY_TYPE bodyType,
												vector<shared_ptr<btCollisionShape>>& btShapes,
												vector<shared_ptr<btTriangleIndexVertexArray>>& btIndexVertexArrays) {

	auto compoundShape = make_shared<btCompoundShape>(true);

	for (auto& element : geometry->elements()) {
		auto indexVertexArray = make_shared<btTriangleIndexVertexArray>();
		auto componentShape = BTShapeFromGeometryElement(geometry->elements().front(),
														 geometry,
														 shapeType,
														 bodyType,
														 indexVertexArray);

		// the Geometry's transform is added to the btRigidBody's localInertia
		compoundShape->addChildShape(BTIdentityTransform(), componentShape.get());

		btShapes.push_back(componentShape);
		btIndexVertexArrays.push_back(indexVertexArray);
	}

	return compoundShape;
}

void AddBTShapeFromNodeRec(shared_ptr<Node> node,
						   PHYSICS_SHAPE_TYPE shapeType,
						   PHYSICS_BODY_TYPE bodyType,
						   shared_ptr<btCompoundShape> compoundShape,
						   vector<shared_ptr<btCollisionShape>>& btShapes,
						   vector<shared_ptr<btTriangleIndexVertexArray>>& btIndexVertexArrays) {

	//auto compoundShape = make_shared<btCompoundShape>(true);

	auto geometry = node->geometry();
	if (geometry) {
		for (auto& element : node->geometry()->elements()) {
			auto indexVertexArray = make_shared<btTriangleIndexVertexArray>();
			auto componentShape = BTShapeFromGeometryElement(element,
															 geometry,
															 shapeType,
															 bodyType,
															 indexVertexArray);

			//auto localTransform = BTTransformFromGLMMat4(node->transform() * node->parent().lock()->transform());
			//compoundShape->addChildShape(localTransform, componentShape.get());
			//compoundShape->addChildShape(BTIdentityTransform(), componentShape.get());
			//auto localTransform = BTTransformFromGLMMat4(node->transform() * node->parent().lock()->transform());
			auto localTransform = BTTransformFromGLMMat4(node->transform());
			//auto localTransform = BTIdentityTransform();
			compoundShape->addChildShape(localTransform, componentShape.get());

			btShapes.push_back(componentShape);
			btIndexVertexArrays.push_back(indexVertexArray);
		}
	}

	// add child geometries recursively
	auto children = node->children(false);
	for (auto& childNode : children) {
		AddBTShapeFromNodeRec(childNode,
							  shapeType,
							  bodyType,
							  compoundShape,
							  btShapes,
							  btIndexVertexArrays);
	}
}

shared_ptr<btCollisionShape> BTCompoundShapeFromGeometry(shared_ptr<Geometry> geometry,
														 PHYSICS_SHAPE_TYPE shapeType,
														 PHYSICS_BODY_TYPE bodyType,
														 vector<shared_ptr<btCollisionShape>>& childShapes,
														 vector<shared_ptr<btTriangleIndexVertexArray>>& childIndexVertexArrays) {

	auto& elements = geometry->elements();
	auto compoundShape = make_shared<btCompoundShape>(true, elements.size());
	for (auto& element : elements) {
		auto indexVertexArray = make_shared<btTriangleIndexVertexArray>();
		auto shape = BTShapeFromGeometryElement(element,
												geometry,
												shapeType,
												bodyType,
												indexVertexArray);

		// the Geometry's transform is added to the btRigidBody's localInertia
		static auto identityTransform = btTransform();
		identityTransform.setIdentity(); // meh
		compoundShape->addChildShape(identityTransform, shape.get());

		childShapes.push_back(shape);
		childIndexVertexArrays.push_back(indexVertexArray);
	}

	return compoundShape;
}

shared_ptr<btConvexHullShape> BTConvexHullShapeFromGeometryElement(shared_ptr<GeometryElement> element) {
	AE_LOG_I("Creating convex hull physics shape for GeometryElement {:p}...", (void*)element.get());

	// tips here: https://pybullet.org/Bullet/phpBB3/viewtopic.php?t=11385

	// https://pybullet.org/Bullet/BulletFull/classbtConvexHullShape.html#a069cf26ba277f9f5f141128fee345eaf
	auto originalShape = make_shared<btConvexHullShape>();
	for (auto& vertex : element->vertices()) {
		originalShape->addPoint(BTVector3FromGLMVec3(vertex.position), false);
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

shared_ptr<btConvexHullShape> BTConvexHullHACDShapeFromGeometryElement(shared_ptr<GeometryElement> element) {
	AE_LOG_I("Creating HACD concave polyhedron physics shape for GeometryElement {:p}...", (void*)element.get());

	ConvexDecomposer::Options options;
//	options.maxConvexHulls = options.maxConvexHulls / 4;
//	options.resolution = options.resolution / 2;
//	options.maxRecursionDepth = options.maxRecursionDepth / 2;
//	options.maxNumVerticesPerHull = options.maxNumVerticesPerHull / 4;
//	vector<shared_ptr<GeometryElement>> elements = geometry->elements();
	auto decomposer = ConvexDecomposer(element, options);
	auto decomponsedElements = decomposer.decompose();

//	auto hacdGeometry = make_shared<Geometry>(decomponsedElements, decomposedTeapotMaterials);

/* MKD bt_tree_rework */
	//return BTConvexHullShapeFromGeometryElement(hacdGeometry);
}

shared_ptr<btGImpactMeshShape> BTGImpactMeshShapeFromGeometryElement(shared_ptr<GeometryElement> element,
																	 shared_ptr<btTriangleIndexVertexArray>& indexVertexArray) {
	AE_LOG_I("Creating concave polyhedron physics shape for GeometryElement {:p}...", (void*)element.get());

	// https://pybullet.org/Bullet/phpBB3/viewtopic.php?t=7997
	// "You can use btGImpactMeshShape (or btCompoundShapes plus HACD) for concave dynamic rigidbodies"
	// doesn't seem to want to collide with static shapes.
	// -> https://pybullet.org/Bullet/phpBB3/viewtopic.php?p=43020#p43020
	// "- BvhTriangleMeshShapes work well as static concave or convex shapes. But since they are meant to be static, there is no algorithm to make them collide with each other.
	// - ConvexTriangleMeshShapes are efficient as dynamic convex shapes.
	// - GImpact shapes are well optimized for when you need dynamic concave shapes.
	// - Convex decomposition can be used to decompose concave shapes into convex shapes. The resulting convex shapes can then be combined into a CompoundShape, which is also an efficient way to model dynamic concave shapes."
	// More: https://stackoverflow.com/questions/32668218/concave-collision-detection-in-bullet

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

	auto gImpactMeshShape = make_shared<btGImpactMeshShape>(indexVertexArray.get());
	// https://pybullet.org/Bullet/BulletFull/classbtGImpactShapeInterface.html#a7d26525396fa957d10e36c099c58480f
	gImpactMeshShape->updateBound();

	return gImpactMeshShape;
}

shared_ptr<btBvhTriangleMeshShape> BTBvhTriangleMeshShapeFromGeometryElement(shared_ptr<GeometryElement> element,
																			 shared_ptr<btTriangleIndexVertexArray>& indexVertexArray) {
	AE_LOG_I("Creating concave polyhedron physics shape for GeometryElement {:p}...", (void*)element.get());

	// static objects ALWAYS use btBvhTriangleMeshShape
	// https://pybullet.org/Bullet/phpBB3/viewtopic.php?t=7997

	// WORKS
	auto vertsBase = element->vertices().data();
	auto facesBase = element->faces().data();

	// WORKS
//	auto vertsBase = &element->vertices()[0];
//	auto facesBase = &element->faces()[0];

	// DOES NOT WORK
//	auto verts = element->vertices();
//	auto faces = element->faces();
//	auto vertsBase = verts.data();
//	auto facesBase = faces.data();

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

	return make_shared<btBvhTriangleMeshShape>(indexVertexArray.get(), true);
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

btQuaternion BTQuaternionFromGLMQuat(const quat& from) {
	
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

btTransform& BTIdentityTransform() {
	// TODO: optimize
	static auto identityTransform = btTransform();
	identityTransform.setIdentity();
	return identityTransform;
}
