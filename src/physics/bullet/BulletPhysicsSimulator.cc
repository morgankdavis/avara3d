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
#include "physics/PhysicalWorld.h"
#include "physics/bullet/BulletBodyResources.h"
#include "physics/bullet/BulletDebugDrawer.h"
#include "physics/bullet/BulletShapeResources.h"
#include "physics/bullet/BulletWorldResources.h"
#include "physics/shape_primitives/BoxPhysicsShape.h"
#include "physics/shape_primitives/CapsulePhysicsShape.h"
#include "physics/shape_primitives/ConePhysicsShape.h"
#include "physics/shape_primitives/CylinderPhysicsShape.h"
#include "physics/shape_primitives/PlanePhysicsShape.h"
#include "physics/shape_primitives/SpherePhysicsShape.h"
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
BTShapeFromSourceGeometry(Geometry* geometry,
						  PHYSICS_SHAPE_TYPE shapeType,
						  PHYSICS_BODY_TYPE bodyType,
						  vector<shared_ptr<btCollisionShape>>& btShapes,
						  vector<shared_ptr<btTriangleIndexVertexArray>>& btIndexVertexArrays);
static shared_ptr<btCollisionShape>
BTShapeFromSourceNode(Node* node,
					  PHYSICS_SHAPE_TYPE shapeType,
					  PHYSICS_BODY_TYPE bodyType,
					  vector<shared_ptr<btCollisionShape>>& btShapes,
					  vector<shared_ptr<btTriangleIndexVertexArray>>& btIndexVertexArrays);
static shared_ptr<btCollisionShape>
BTShapeFromPrimitiveShape(PhysicsShape& shape,
						  PHYSICS_BODY_TYPE bodyType);
static shared_ptr<btCollisionShape>
BTShapeFromGeometryElement(shared_ptr<GeometryElement> element,
						   Geometry* geometry,
						   PHYSICS_SHAPE_TYPE shapeType,
						   PHYSICS_BODY_TYPE bodyType,
						   vector<shared_ptr<btCollisionShape>>& btShapes,
						   shared_ptr<btTriangleIndexVertexArray>& btIndexVertexArray);

static shared_ptr<btCompoundShape>
BTShapeFromGeometry(Geometry* geometry,
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

static shared_ptr<btGImpactMeshShape>
BTGImpactMeshShapeFromGeometryElement(shared_ptr<GeometryElement> element,
									  shared_ptr<btTriangleIndexVertexArray>& indexVertexArray);

static shared_ptr<btBvhTriangleMeshShape>
BTBvhTriangleMeshShapeFromGeometryElement(shared_ptr<GeometryElement> element,
										  shared_ptr<btTriangleIndexVertexArray>& indexVertexArray);

static shared_ptr<btCompoundShape>
BTCompoundConvexHullHACDShapeFromGeometryElement(shared_ptr<GeometryElement> element,
												 vector<shared_ptr<btCollisionShape>>& btShapes);

static vector<shared_ptr<GeometryElement>>
HACDGeometryElementsFromGeometryElement(shared_ptr<GeometryElement> element);

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
		PhysicsSimulator() { }

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
//#ifdef OPENGL_CORE
//	auto btDebugModes = BTDebugDrawModesForAEDebugOptions(debugOptions);
//	_debugDrawer->setDebugMode(btDebugModes);
//	_debugDrawer->clear();
//	_btWorld->debugDrawWorld();
//	_debugDrawer->draw(renderer, viewMat, projectionMat);
//#endif
}

/*********************************************************************************************
	PhysicsSimulator
 *********************************************************************************************/

//void BulletPhysicsSimulator::setTimestep(PhysicalWorld& world, float timestep) {
//
//}
//
//void BulletPhysicsSimulator::setSpeed(PhysicalWorld& world, float speed) {
//
//}

void BulletPhysicsSimulator::setGravity(PhysicalWorld& world, glm::vec3& gravity) {

}

void BulletPhysicsSimulator::create(PhysicsBody& body) {
	AE_LOG_D("body: {:p}", (void*)&body);

	auto world = body.physicalWorld();
	auto worldResources = static_cast<BulletWorldResources*>(world->resources());
	auto btWorld = worldResources->world();
	auto node = body.node();
	auto shape = body.shape();
	auto dirtyMask = body.dirtyMask();

	auto bodyResources = static_cast<BulletBodyResources*>(body.resources());
	auto shapeResources = static_cast<BulletShapeResources*>(body.shape()->resources());
	// front is either the only btCollisionShape or a btCompound shape with child shapes at index 1+
	auto btShape = shapeResources->shapes().front();

	AE_LOG_D("Creating rigid body for physics body {:p}...", (void*)&body);

	bool wasScaled = false;
	auto transform = BTTransformFromGLMMat4(TransformByRemovingScale(node->worldTransform(),
																	 wasScaled));
	if (wasScaled) {
		// TODO: do something about this
		// can hold a burned transformed vertex data in the physics body/shape?
		AE_LOG_W("Ignorning scale for Node {:p} with PhysicsBody {:p}.",
				 (void*)&node, (void*)&body);
	}

	auto localInertia = BTVector3FromGLMVec3(body.momentOfInertia());
	auto mass = body.mass();
	if (body.type() == PHYSICS_BODY_TYPE::STATIC
		|| body.type() == PHYSICS_BODY_TYPE::KINEMATIC) {
		mass = 0;
	}
	else if (body.type() == PHYSICS_BODY_TYPE::DYNAMIC) {
		btShape->calculateLocalInertia(mass, localInertia);
	}

	auto newMotionState = make_shared<btDefaultMotionState>(transform);
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

	auto btBody = make_shared<btRigidBody>(rigidBodyInfo);

	switch (body.type()) {
		case PHYSICS_BODY_TYPE::STATIC:
			btBody->setCollisionFlags(btCollisionObject::CF_STATIC_OBJECT);
			break;
		case PHYSICS_BODY_TYPE::KINEMATIC:
			btBody->setCollisionFlags(btCollisionObject::CF_KINEMATIC_OBJECT);
			break;
		case PHYSICS_BODY_TYPE::DYNAMIC:
			btBody->setCollisionFlags(btCollisionObject::CF_DYNAMIC_OBJECT);
			break;
	}

	btWorld->addRigidBody(btBody.get());

	bodyResources->body(btBody);
	bodyResources->motionState(newMotionState);

	// I'm a cheap bastard
	setLinearFactor(body, body.linearFactor());
	setAngularFactor(body, body.angularFactor());
	setLinearDamping(body, body.linearDamping());
	setAngularDamping(body, body.angularDamping());
	setLinearSleepingThreshold(body, body.linearSleepingThreshold());
	setAngularSleepingThreshold(body, body.angularSleepingThreshold());
	setLinearVelocity(body, body.linearVelocity()); // ?
	setAngularVelocity(body, body.angularVelocity()); // ?
	setAffectedByGravity(body, body.affectedByGravity());
	setAllowsResting(body, body.allowsResting());
}

void BulletPhysicsSimulator::remove(PhysicsBody& body) {
	// *** DO IT ***
}

void BulletPhysicsSimulator::setType(PhysicsBody& body, PHYSICS_BODY_TYPE type) {
	// setCollisionFlags() ??
}

void BulletPhysicsSimulator::setShape(PhysicsBody& body, PhysicsShape& shape) {
	// *** re-create body ***
}

void BulletPhysicsSimulator::setWorldTransform(PhysicsBody& body, const glm::mat4& transform) {

	auto btBody = static_cast<BulletBodyResources*>(body.resources())->body();
	auto toTransform = BTTransformFromGLMMat4(transform);
	//	btBody->proceedToTransform(toTransform); // this appears to affect dynamic bodies
	auto motionState = btBody->getMotionState();
	motionState->setWorldTransform(toTransform); // and this kinematic...
	btBody->setMotionState(motionState);
	btBody->setActivationState(ACTIVE_TAG);
}

void BulletPhysicsSimulator::setMass(PhysicsBody& body, float mass) {
	// *** re-create body ***
	// **************** TEST THIS *************************
	// I think it's right...
	auto btBody = static_cast<BulletBodyResources*>(body.resources())->body();
	btBody->setMassProps(mass, BTVector3FromGLMVec3(body.momentOfInertia()));
}

void BulletPhysicsSimulator::setMomentOfInertia(PhysicsBody& body, const glm::vec3& moment) {
	// *** re-create body ***
}

void BulletPhysicsSimulator::setFriction(PhysicsBody& body, float friction) {
	auto btBody = static_cast<BulletBodyResources*>(body.resources())->body();
	btBody->setFriction(friction);
}

void BulletPhysicsSimulator::setRollingFriction(PhysicsBody& body, float friction) {
	auto btBody = static_cast<BulletBodyResources*>(body.resources())->body();
	btBody->setRollingFriction(friction);
}

void BulletPhysicsSimulator::setRestitution(PhysicsBody& body, float restitution) {
	auto btBody = static_cast<BulletBodyResources*>(body.resources())->body();
	btBody->setRestitution(restitution);
}

void BulletPhysicsSimulator::setLinearVelocity(PhysicsBody& body, const glm::vec3& velocity) {
	auto btBody = static_cast<BulletBodyResources*>(body.resources())->body();
	btBody->setLinearVelocity(BTVector3FromGLMVec3(velocity));
}

void BulletPhysicsSimulator::setAngularVelocity(PhysicsBody& body, const glm::vec3& velocity) {
	auto btBody = static_cast<BulletBodyResources*>(body.resources())->body();
	btBody->setAngularVelocity(BTVector3FromGLMVec3(velocity));
}

void BulletPhysicsSimulator::setLinearFactor(PhysicsBody& body, const glm::vec3& factor) {
	auto btBody = static_cast<BulletBodyResources*>(body.resources())->body();
	btBody->setLinearFactor(BTVector3FromGLMVec3(body.linearFactor()));
}

void BulletPhysicsSimulator::setAngularFactor(PhysicsBody& body, const glm::vec3& factor) {
	auto btBody = static_cast<BulletBodyResources*>(body.resources())->body();
	btBody->setAngularFactor(BTVector3FromGLMVec3(body.angularFactor()));
}

void BulletPhysicsSimulator::setLinearDamping(PhysicsBody& body, float damping) {
	auto btBody = static_cast<BulletBodyResources*>(body.resources())->body();
	btBody->setDamping(damping, btBody->getAngularDamping());
}

void BulletPhysicsSimulator::setAngularDamping(PhysicsBody& body, float damping) {
	auto btBody = static_cast<BulletBodyResources*>(body.resources())->body();
	btBody->setDamping(btBody->getLinearDamping(), damping);
}

void BulletPhysicsSimulator::setLinearSleepingThreshold(PhysicsBody& body, float threshold) {
	auto btBody = static_cast<BulletBodyResources*>(body.resources())->body();
	btBody->setSleepingThresholds(threshold, btBody->getAngularSleepingThreshold());
}

void BulletPhysicsSimulator::setAngularSleepingThreshold(PhysicsBody& body, float threshold) {
	auto btBody = static_cast<BulletBodyResources*>(body.resources())->body();
	btBody->setSleepingThresholds(btBody->getLinearSleepingThreshold(), threshold);
}

void BulletPhysicsSimulator::setAffectedByGravity(PhysicsBody& body, bool flag) {
	auto btBody = static_cast<BulletBodyResources*>(body.resources())->body();
	auto btWorld = static_cast<BulletWorldResources*>(body.physicalWorld()->resources())->world();
	btBody->setGravity(flag
					   ? btWorld->getGravity()
					   : btVector3{0, 0, 0});
}

void BulletPhysicsSimulator::setAllowsResting(PhysicsBody& body, bool flag) {
	auto btBody = static_cast<BulletBodyResources*>(body.resources())->body();
	btBody->setActivationState(body.allowsResting()
							   ? ACTIVE_TAG
							   : DISABLE_DEACTIVATION);
}

void BulletPhysicsSimulator::create(PhysicsShape& shape) {
	AE_LOG_D("shape: {:p}", (void*)&shape);

	PHYSICS_BODY_TYPE bodyType = (*shape.bodies().begin())->type();

	shared_ptr<btCollisionShape> newShape = nullptr;

	auto btShapes = vector<shared_ptr<btCollisionShape>>();
	auto btIndexVertexArrays = vector<shared_ptr<btTriangleIndexVertexArray>>();

	auto sourceObject = shape.sourceObject();

	// souce GEOMETRY
	if (holds_alternative<Geometry*>(sourceObject)) {
		if (auto sourceGeometry = get<Geometry*>(sourceObject)) {

			newShape = BTShapeFromSourceGeometry(sourceGeometry,
												 shape.type(),
												 bodyType,
												 btShapes,
												 btIndexVertexArrays);
		}
	}

	// source NODE
	else if (holds_alternative<Node*>(sourceObject)) {
		if (auto sourceNode = get<Node*>(sourceObject)) {

			newShape = BTShapeFromSourceNode(sourceNode,
											 shape.type(),
											 bodyType,
											 btShapes,
											 btIndexVertexArrays);
		}
	}

	// primitive subclass
	else if (holds_alternative<monostate>(sourceObject)) {

		newShape = BTShapeFromPrimitiveShape(shape, bodyType);
	}

	if (newShape) {
		btShapes.insert(btShapes.begin(), newShape);

		auto shapeResources = static_cast<BulletShapeResources*>(shape.resources());
		shapeResources->shapes(btShapes);
		shapeResources->indexVertexArrays(btIndexVertexArrays);
//		updated = true;

		shape.dirtyMask(PHYSICS_SHAPE_DIRTY_MASK_REMOVE(shape.dirtyMask(),
														PHYSICS_SHAPE_DIRTY_MASK::MODEL));
	}
	else {
//		updated = false;
		AE_LOG_E("PhysicsShape with no geometry or source node.");
	}
}

void BulletPhysicsSimulator::step(PhysicalWorld& world, float deltaT) {

	auto resources = static_cast<BulletWorldResources*>(world.resources());
	auto btWorld = resources->world();

	auto result = btWorld->stepSimulation(deltaT * world.speed(),
										 MAX_SUBSTEPS,
										 world.timestep());

	if (result == MAX_SUBSTEPS) {
		AE_LOG_W("Physics simulation max substeps reached: {}", result);
	}
}

void BulletPhysicsSimulator::sync(PhysicsBody& body, mat4& worldTransform) {

	auto bodyResources = static_cast<BulletBodyResources*>(body.resources());
	auto btBody = bodyResources->body();

	btTransform btWorldTransform;
	btWorldTransform.setIdentity();
	//btMotionState->getWorldTransform(btWorldTransform); // crash?
	btBody->getMotionState()->getWorldTransform(btWorldTransform);

	worldTransform = GLMMat4FromBTTransform(btWorldTransform);

	// SYNC OTHER PROPERTIES?
	//	velocities, etc
	//	others?
}

















//
//void BulletPhysicsSimulator::update(PhysicsBody& body,
//									Node& node) {
//	PhysicsSimulator::update(body, node);
//
//	auto shape = body.shape();
//	auto dirtyMask = body.dirtyMask();
//
//	auto bodyResources = static_pointer_cast<BulletBodyResources>(body.resources());
//	auto shapeResources = static_pointer_cast<BulletShapeResources>(body.shape()->resources());
//	// front is either the only btCollisionShape or a btCompound shape with child shapes at index 1+
//	auto btShape = shapeResources->shapes().front();
//
//	shared_ptr<btRigidBody> btBody = nullptr;
//
//	// since the BT body depends on the BT shape, if the shape was dirty (and re-created)
//	// we also re-create the body.
//	// additionally, since some physical properties have to be passed via btRigidBodyConstructionInfo,
//	// they require re-creating the rigid body.
//	if (PHYSICS_BODY_DIRTY_MASK_CONTAINS(dirtyMask, PHYSICS_BODY_DIRTY_MASK::SHAPE)
//		// these properties only appear to be set-able via btRigidBodyConstructionInfo
//		|| PHYSICS_BODY_DIRTY_MASK_CONTAINS(dirtyMask, PHYSICS_BODY_DIRTY_MASK::TYPE)
//		|| PHYSICS_BODY_DIRTY_MASK_CONTAINS(dirtyMask, PHYSICS_BODY_DIRTY_MASK::MOMENT_OF_INERTIA)
//		|| PHYSICS_BODY_DIRTY_MASK_CONTAINS(dirtyMask, PHYSICS_BODY_DIRTY_MASK::FRICTION) // CAN SET LIVE
//		|| PHYSICS_BODY_DIRTY_MASK_CONTAINS(dirtyMask, PHYSICS_BODY_DIRTY_MASK::ROLLING_FRICTION) // CAN SET LIVE
//		|| PHYSICS_BODY_DIRTY_MASK_CONTAINS(dirtyMask, PHYSICS_BODY_DIRTY_MASK::RESTITUTION)) { // CAN SET LIVE
//
//		AE_LOG_D("Creating rigid body for physics body {:p}...", (void*)&body);
//
//		bool wasScaled = false;
//		auto transform = BTTransformFromGLMMat4(TransformByRemovingScale(node.worldTransform(),
//																		 wasScaled));
//		if (wasScaled) {
//			// TODO: should address this.
//			// can hold a burned transformed vertex data in the physics body/shape?
//			AE_LOG_W("Ignorning scale for Node {:p} with PhysicsBody {:p}.",
//					 (void*)&node, (void*)&body);
//		}
//
//		auto localInertia = BTVector3FromGLMVec3(body.momentOfInertia());
//		auto mass = body.mass();
//		if (body.type() == PHYSICS_BODY_TYPE::STATIC
//			|| body.type() == PHYSICS_BODY_TYPE::KINEMATIC) {
//			mass = 0;
//		}
//		else if (body.type() == PHYSICS_BODY_TYPE::DYNAMIC) {
//			btShape->calculateLocalInertia(mass, localInertia);
//		}
//
//		auto newMotionState = make_shared<btDefaultMotionState>(transform);
//		btRigidBody::btRigidBodyConstructionInfo rigidBodyInfo(mass,
//															   newMotionState.get(),
//															   btShape.get(),
//															   localInertia);
//		rigidBodyInfo.m_mass = mass;
//		rigidBodyInfo.m_linearDamping = body.linearDamping();
//		rigidBodyInfo.m_angularDamping = body.angularDamping();
//		rigidBodyInfo.m_friction = body.friction();
//		rigidBodyInfo.m_rollingFriction = body.rollingFriction();
//		rigidBodyInfo.m_restitution = body.restitution();
//		rigidBodyInfo.m_linearSleepingThreshold = body.linearSleepingThreshold();
//		rigidBodyInfo.m_angularSleepingThreshold = body.angularSleepingThreshold();
//
//		btBody = make_shared<btRigidBody>(rigidBodyInfo);
//
//		switch (body.type()) {
//			case PHYSICS_BODY_TYPE::STATIC:
//				btBody->setCollisionFlags(btCollisionObject::CF_STATIC_OBJECT);
//				break;
//			case PHYSICS_BODY_TYPE::KINEMATIC:
//				btBody->setCollisionFlags(btCollisionObject::CF_KINEMATIC_OBJECT);
//				break;
//			case PHYSICS_BODY_TYPE::DYNAMIC:
//				btBody->setCollisionFlags(btCollisionObject::CF_DYNAMIC_OBJECT);
//				break;
//		}
//
//		_btWorld->addRigidBody(btBody.get());
//
//		bodyResources->body(btBody);
//		bodyResources->motionState(newMotionState);
//
//		body.dirtyMask(PHYSICS_BODY_DIRTY_MASK_REMOVE(body.dirtyMask(),
//													  PHYSICS_BODY_DIRTY_MASK::SHAPE));
//		body.dirtyMask(PHYSICS_BODY_DIRTY_MASK_REMOVE(body.dirtyMask(),
//													  PHYSICS_BODY_DIRTY_MASK::TYPE));
//		body.dirtyMask(PHYSICS_BODY_DIRTY_MASK_REMOVE(body.dirtyMask(),
//													  PHYSICS_BODY_DIRTY_MASK::MOMENT_OF_INERTIA));
//		body.dirtyMask(PHYSICS_BODY_DIRTY_MASK_REMOVE(body.dirtyMask(),
//													  PHYSICS_BODY_DIRTY_MASK::FRICTION));
//		body.dirtyMask(PHYSICS_BODY_DIRTY_MASK_REMOVE(body.dirtyMask(),
//													  PHYSICS_BODY_DIRTY_MASK::ROLLING_FRICTION));
//		body.dirtyMask(PHYSICS_BODY_DIRTY_MASK_REMOVE(body.dirtyMask(),
//													  PHYSICS_BODY_DIRTY_MASK::RESTITUTION));
//	}
//	else {
//		btBody = bodyResources->body();
//	}
//
//	// check and set the rest of the properties
//
//	if (PHYSICS_BODY_DIRTY_MASK_CONTAINS(dirtyMask, PHYSICS_BODY_DIRTY_MASK::LINEAR_FACTOR)) {
//		btBody->setLinearFactor(BTVector3FromGLMVec3(body.linearFactor()));
//
//		body.dirtyMask(PHYSICS_BODY_DIRTY_MASK_REMOVE(body.dirtyMask(),
//													  PHYSICS_BODY_DIRTY_MASK::LINEAR_FACTOR));
//	}
//	if (PHYSICS_BODY_DIRTY_MASK_CONTAINS(dirtyMask, PHYSICS_BODY_DIRTY_MASK::ANGULAR_FACTOR)) {
//		btBody->setAngularFactor(BTVector3FromGLMVec3(body.angularFactor()));
//
//		body.dirtyMask(PHYSICS_BODY_DIRTY_MASK_REMOVE(body.dirtyMask(),
//													  PHYSICS_BODY_DIRTY_MASK::ANGULAR_FACTOR));
//	}
//	if (PHYSICS_BODY_DIRTY_MASK_CONTAINS(dirtyMask, PHYSICS_BODY_DIRTY_MASK::LINEAR_DAMPING)
//		|| PHYSICS_BODY_DIRTY_MASK_CONTAINS(dirtyMask, PHYSICS_BODY_DIRTY_MASK::ANGULAR_DAMPING)) {
//		btBody->setDamping(body.linearDamping(), body.angularDamping());
//
//		body.dirtyMask(PHYSICS_BODY_DIRTY_MASK_REMOVE(body.dirtyMask(),
//													  PHYSICS_BODY_DIRTY_MASK::LINEAR_DAMPING));
//		body.dirtyMask(PHYSICS_BODY_DIRTY_MASK_REMOVE(body.dirtyMask(),
//													  PHYSICS_BODY_DIRTY_MASK::ANGULAR_DAMPING));
//	}
//	if (PHYSICS_BODY_DIRTY_MASK_CONTAINS(dirtyMask,
//										 PHYSICS_BODY_DIRTY_MASK::LINEAR_SLEEPING_THRESHOLD)
//		|| PHYSICS_BODY_DIRTY_MASK_CONTAINS(dirtyMask,
//											PHYSICS_BODY_DIRTY_MASK::ANGULAR_SLEEPING_THRESHOLD)) {
//		btBody->setSleepingThresholds(body.linearSleepingThreshold(),
//									  body.angularSleepingThreshold());
//
////		AE_LOG_D("linearSleepingThreshold: {}", (*btBody)->getLinearSleepingThreshold());
////		AE_LOG_D("angularSleepingThreshold: {}", (*btBody)->getAngularSleepingThreshold());
//
//		body.dirtyMask(PHYSICS_BODY_DIRTY_MASK_REMOVE(body.dirtyMask(),
//													  PHYSICS_BODY_DIRTY_MASK::LINEAR_SLEEPING_THRESHOLD));
//		body.dirtyMask(PHYSICS_BODY_DIRTY_MASK_REMOVE(body.dirtyMask(),
//													  PHYSICS_BODY_DIRTY_MASK::ANGULAR_SLEEPING_THRESHOLD));
//	}
//	if (PHYSICS_BODY_DIRTY_MASK_CONTAINS(dirtyMask, PHYSICS_BODY_DIRTY_MASK::AFFECTED_BY_GRAVITY)) {
//		if (body.affectedByGravity()) {
//			btBody->setGravity(_btWorld->getGravity());
//		}
//		else {
//			// NOTE: setting world gravity resets this
//			btBody->setGravity({0, 0, 0});
//		}
//
//		body.dirtyMask(PHYSICS_BODY_DIRTY_MASK_REMOVE(body.dirtyMask(),
//													  PHYSICS_BODY_DIRTY_MASK::AFFECTED_BY_GRAVITY));
//	}
//	if (PHYSICS_BODY_DIRTY_MASK_CONTAINS(dirtyMask, PHYSICS_BODY_DIRTY_MASK::ALLOWS_RESTING)) {
//		if (body.allowsResting()) {
//			btBody->setActivationState(ACTIVE_TAG);
//		}
//		else {
//			btBody->setActivationState(DISABLE_DEACTIVATION);
//		}
//
//		body.dirtyMask(PHYSICS_BODY_DIRTY_MASK_REMOVE(body.dirtyMask(),
//													  PHYSICS_BODY_DIRTY_MASK::ALLOWS_RESTING));
//	}
//	if (PHYSICS_BODY_DIRTY_MASK_CONTAINS(dirtyMask, PHYSICS_BODY_DIRTY_MASK::FORCES)) {
//#warning TODO
//
//		body.dirtyMask(PHYSICS_BODY_DIRTY_MASK_REMOVE(body.dirtyMask(),
//													  PHYSICS_BODY_DIRTY_MASK::FORCES));
//	}
//	if (PHYSICS_BODY_DIRTY_MASK_CONTAINS(dirtyMask, PHYSICS_BODY_DIRTY_MASK::TORQUES)) {
//#warning TODO
//
//		body.dirtyMask(PHYSICS_BODY_DIRTY_MASK_REMOVE(body.dirtyMask(),
//													  PHYSICS_BODY_DIRTY_MASK::TORQUES));
//	}
//	if (PHYSICS_BODY_DIRTY_MASK_CONTAINS(dirtyMask, PHYSICS_BODY_DIRTY_MASK::LINEAR_VELOCITY)) {
//		btBody->setLinearVelocity(BTVector3FromGLMVec3(body.linearVelocity()));
//
//		body.dirtyMask(PHYSICS_BODY_DIRTY_MASK_REMOVE(body.dirtyMask(),
//													  PHYSICS_BODY_DIRTY_MASK::LINEAR_VELOCITY));
//	}
//	if (PHYSICS_BODY_DIRTY_MASK_CONTAINS(dirtyMask, PHYSICS_BODY_DIRTY_MASK::ANGULAR_VELOCITY)) {
//		btBody->setAngularVelocity(BTVector3FromGLMVec3(body.angularVelocity()));
//
//		body.dirtyMask(PHYSICS_BODY_DIRTY_MASK_REMOVE(body.dirtyMask(),
//													  PHYSICS_BODY_DIRTY_MASK::ANGULAR_VELOCITY));
//	}
//
//	// back-fill PhysicsBody properties
//
//	// *** causing a loop canceling out any manual dynamic movement ***
//
//	body.linearVelocity(GLMVec3FromBTVector3(btBody->getLinearVelocity()), false);
//	body.angularVelocity(GLMVec3FromBTVector3(btBody->getAngularVelocity()), false);
//	body.resting(btBody->getActivationState() == (ISLAND_SLEEPING ? true : false));
//
//	// if kinematic, apply visual transform to bullet model
//
//	if (body.type() == PHYSICS_BODY_TYPE::KINEMATIC) {
//
//		auto toTransform = BTTransformFromGLMMat4(node.worldTransform());
//		//	btBody->proceedToTransform(toTransform); // this appears to affect dynamic bodies
//		auto motionState = btBody->getMotionState();
//		motionState->setWorldTransform(toTransform); // and this kinematic...
//		btBody->setMotionState(motionState);
//
//		btBody->setActivationState(ACTIVE_TAG);
//		//	btBody->forceActivationState(ACTIVE_TAG);
//	}
//}

//void BulletPhysicsSimulator::sync(PhysicsBody& body,
//								  mat4& worldTransform) {
//	PhysicsSimulator::sync(body, worldTransform);
//
//	auto bodyResources = static_cast<BulletBodyResources*>(body.resources());
//	auto btBody = bodyResources->body();
//
//	// get body transforms and apply back to scene graph
//
//	btTransform btWorldTransform;
//	btWorldTransform.setIdentity();
//	//btMotionState->getWorldTransform(btWorldTransform); // crash?
//	btBody->getMotionState()->getWorldTransform(btWorldTransform);
//
//	worldTransform = GLMMat4FromBTTransform(btWorldTransform);
//
//	// GET OTHER PROPERTIES
//	//	velocities, etc
//	//	other properties?
//}
//
//void BulletPhysicsSimulator::update(PhysicsShape& shape,
//									PHYSICS_BODY_TYPE bodyType,
//									bool& updated) {
//	PhysicsSimulator::update(shape, bodyType, updated);
//
//	if (PHYSICS_SHAPE_DIRTY_MASK_CONTAINS(shape.dirtyMask(),
//										  PHYSICS_SHAPE_DIRTY_MASK::MODEL)) {
//
//		AE_LOG_D("Shape {:p} model dirty. Rebuilding.", (void*)&shape);
//
//		shared_ptr<btCollisionShape> newShape = nullptr;
//
//		auto btShapes = vector<shared_ptr<btCollisionShape>>();
//		auto btIndexVertexArrays = vector<shared_ptr<btTriangleIndexVertexArray>>();
//
//		auto sourceObject = shape.sourceObject();
//
//		// souce GEOMETRY
//		if (holds_alternative<Geometry*>(sourceObject)) {
//			if (auto sourceGeometry = get<Geometry*>(sourceObject)) {
//
//				newShape = BTShapeFromSourceGeometry(sourceGeometry,
//													 shape.type(),
//													 bodyType,
//													 btShapes,
//													 btIndexVertexArrays);
//			}
//		}
//
//		// source NODE
//		else if (holds_alternative<Node*>(sourceObject)) {
//			if (auto sourceNode = get<Node*>(sourceObject)) {
//
//				newShape = BTShapeFromSourceNode(sourceNode,
//												 shape.type(),
//												 bodyType,
//												 btShapes,
//												 btIndexVertexArrays);
//			}
//		}
//
//		// primitive subclass
//		else if (holds_alternative<monostate>(sourceObject)) {
//
//			newShape = BTShapeFromPrimitiveShape(shape, bodyType);
//		}
//
//		if (newShape) {
//			btShapes.insert(btShapes.begin(), newShape);
//
//			auto shapeResources = static_pointer_cast<BulletShapeResources>(shape.resources());
//			shapeResources->shapes(btShapes);
//			shapeResources->indexVertexArrays(btIndexVertexArrays);
//			updated = true;
//
//			shape.dirtyMask(PHYSICS_SHAPE_DIRTY_MASK_REMOVE(shape.dirtyMask(),
//															PHYSICS_SHAPE_DIRTY_MASK::MODEL));
//		}
//		else {
//			updated = false;
//			AE_LOG_E("PhysicsShape with no geometry or source node.");
//		}
//	}
//	else {
//		updated = false;
//	}
//}
//
//void BulletPhysicsSimulator::sync(PhysicsShape& shape,
//								  PHYSICS_BODY_TYPE bodyType) {
//	PhysicsSimulator::sync(shape, bodyType);
//
//}



























/*********************************************************************************************
	Static
 *********************************************************************************************/

static shared_ptr<btCollisionShape>
BTShapeFromSourceGeometry(Geometry* geometry,
						  PHYSICS_SHAPE_TYPE shapeType,
						  PHYSICS_BODY_TYPE bodyType,
						  vector<shared_ptr<btCollisionShape>>& btShapes,
						  vector<shared_ptr<btTriangleIndexVertexArray>>& btIndexVertexArrays) {

	shared_ptr<btCollisionShape> newShape = nullptr;

	if (geometry->elements().size() == 1) {
		// make a single shape

		auto indexVertexArray = make_shared<btTriangleIndexVertexArray>();
		newShape = BTShapeFromGeometryElement(geometry->elements().front(),
											  geometry,
											  shapeType,
											  bodyType,
											  btShapes,
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
				 (void*)geometry);
	}

	return newShape;
}

static shared_ptr<btCollisionShape>
BTShapeFromSourceNode(Node* node,
					  PHYSICS_SHAPE_TYPE shapeType,
					  PHYSICS_BODY_TYPE bodyType,
					  vector<shared_ptr<btCollisionShape>>& btShapes,
					  vector<shared_ptr<btTriangleIndexVertexArray>>& btIndexVertexArrays) {

	// *** won't work for most static, kinematic? ***
	// "adding the following shapes to a btCompoundShape is not supported: btTriangleShape,
	// btBvhTriangleMeshShape, btGImpact*Shape, btStaticPlaneShape and a bunch more."
	// https://pybullet.org/Bullet/phpBB3/viewtopic.php?p=17718#p17718

	auto rootShape = make_shared<btCompoundShape>(true); // added to btShapes by caller

	// add the root geometry
	auto geometry = node->geometry().get();
	if (geometry) {

		auto nodeGeoShape = BTShapeFromGeometry(geometry,
												shapeType,
												bodyType,
												btShapes,
												btIndexVertexArrays);
		rootShape->addChildShape(BTIdentityTransform(),
								 nodeGeoShape.get());
		btShapes.push_back(nodeGeoShape);
	}

	// add child geometries recursively
	for (auto& childNode : node->children(false)) {
		AddBTShapeFromNodeRec(childNode,
							  shapeType,
							  bodyType,
							  rootShape,
							  btShapes,
							  btIndexVertexArrays);
	}

	return rootShape;
}

static shared_ptr<btCollisionShape>
BTShapeFromPrimitiveShape(PhysicsShape& shape,
						  PHYSICS_BODY_TYPE bodyType) {

	if (auto boxShape = dynamic_cast<BoxPhysicsShape*>(&shape)) {
		AE_LOG_I("BoxPhysicsShape");

		return make_shared<btBoxShape>(btVector3((btScalar)boxShape->width()/2.0f,
												 (btScalar)boxShape->height()/2.0f,
												 (btScalar)boxShape->length()/2.0f));
	}
	else if (auto capsuleShape = dynamic_cast<CapsulePhysicsShape*>(&shape)) {
		AE_LOG_I("CapsulePhysicsShape");

		return make_shared<btCapsuleShape>((btScalar)capsuleShape->radius(),
										   (btScalar)capsuleShape->height());
	}
	else if (auto coneShape = dynamic_cast<ConePhysicsShape*>(&shape)) {
		AE_LOG_I("ConePhysicsShape");

		return make_shared<btConeShape>((btScalar)coneShape->radius(),
										(btScalar)coneShape->height());
	}
	else if (auto cylinderShape = dynamic_cast<CylinderPhysicsShape*>(&shape)) {
		AE_LOG_I("CylinderPhysicsShape");

		return make_shared<btCylinderShape>(btVector3((btScalar)cylinderShape->radius(),
													  (btScalar)cylinderShape->height()/2.0,
													  (btScalar)cylinderShape->radius()));
	}
	else if (auto planeShape = dynamic_cast<PlanePhysicsShape*>(&shape)) {
		AE_LOG_I("PlanePhysicsShape");

		return make_shared<btBoxShape>(btVector3((btScalar)planeShape->width()/2.0f,
												 (btScalar)planeShape->height()/2.0f,
												 (btScalar)0));
	}
	else if (auto sphereShape = dynamic_cast<SpherePhysicsShape*>(&shape)) {
		AE_LOG_I("SpherePhysicsShape");

		return make_shared<btSphereShape>((btScalar)sphereShape->radius());
	}
	else {
		AE_LOG_E("PhysicsShape {:p} is not a valid subclass.",
				 (void*)&shape);
	}
}

shared_ptr<btCollisionShape>
BTShapeFromGeometryElement(shared_ptr<GeometryElement> element,
						   Geometry* geometry,
						   PHYSICS_SHAPE_TYPE shapeType,
						   PHYSICS_BODY_TYPE bodyType,
						   vector<shared_ptr<btCollisionShape>>& btShapes,
						   shared_ptr<btTriangleIndexVertexArray>& indexVertexArray) {

	if (shapeType == PHYSICS_SHAPE_TYPE::BOUNDING_BOX) {
		AE_LOG_I("Creating box physics shape for GeometryElement {:p}...",
				 (void*)element.get());

		auto extent = element->extent();
		return make_shared<btBoxShape>(btVector3((btScalar)extent.x/2.0f,
												 (btScalar)extent.y/2.0f,
												 (btScalar)extent.z/2.0f));
	}
	else if (auto box = dynamic_cast<Box*>(geometry)) {
		AE_LOG_I("Creating box physics shape for GeometryElement {:p}... "
		"(ignoring physics shape type '{}')",
				 (void*)element.get(), magic_enum::enum_name(shapeType));

		return make_shared<btBoxShape>(btVector3((btScalar)box->length()/2.0f,
												 (btScalar)box->width()/2.0f,
												 (btScalar)box->height()/2.0f));
	}
	else if (auto capsule = dynamic_cast<Capsule*>(geometry)) {
		AE_LOG_I("Creating capsule physics shape for GeometryElement {:p}... " \
		"(ignoring physics shape type '{}')",
				 (void*)element.get(), magic_enum::enum_name(shapeType));

		return make_shared<btCapsuleShape>((btScalar)capsule->radius(),
										   (btScalar)capsule->height());
	}
	else if (auto cone  = dynamic_cast<Cone*>(geometry)) {
		AE_LOG_I("Creating cone physics shape for GeometryElement {:p}... " \
		"(ignoring physics shape type '{}')",
				 (void*)element.get(), magic_enum::enum_name(shapeType));

		return make_shared<btConeShape>((btScalar)cone->radius(),
										(btScalar)cone->height());
	}
	else if (auto cylinder = dynamic_cast<Cylinder*>(geometry)) {
		AE_LOG_I("Creating cylinder physics shape for GeometryElement {:p}... " \
		"(ignoring physics shape type '{}')",
				 (void*)element.get(), magic_enum::enum_name(shapeType));

		return make_shared<btCylinderShape>(btVector3((btScalar)cylinder->radius(),
													  (btScalar)cylinder->height()/2.0,
													  (btScalar)cylinder->radius()));
	}
	else if (auto plane = dynamic_cast<Plane*>(geometry)) {
		// ae::Plane is not a true plane, it has a length and width, so we need to use a btBoxShape
		return make_shared<btBoxShape>(btVector3((btScalar)plane->width()/2.0f,
												 (btScalar)plane->height()/2.0f,
												 (btScalar)0));
	}
	else if (auto sphere = dynamic_cast<Sphere*>(geometry)) {
		AE_LOG_I("Creating sphere physics shape for GeometryElement {:p}... " \
		"(ignoring physics shape type '{}')",
				 (void*)element.get(), magic_enum::enum_name(shapeType));

		return make_shared<btSphereShape>((btScalar)sphere->radius());
	}
	// * no Bullet primitives for Torus or Tube *
	else if (shapeType == PHYSICS_SHAPE_TYPE::CONVEX_HULL) {

		return BTConvexHullShapeFromGeometryElement(element);
	}
	else if (bodyType == PHYSICS_BODY_TYPE::DYNAMIC) {

		return BTCompoundConvexHullHACDShapeFromGeometryElement(element, btShapes);
	}
	else if (bodyType == PHYSICS_BODY_TYPE::KINEMATIC) {

		return BTGImpactMeshShapeFromGeometryElement(element, indexVertexArray);
	}
	else if (bodyType == PHYSICS_BODY_TYPE::STATIC) {

		return BTBvhTriangleMeshShapeFromGeometryElement(element, indexVertexArray);
	}

	return nullptr;
}

shared_ptr<btCompoundShape>
BTShapeFromGeometry(Geometry* geometry,
					PHYSICS_SHAPE_TYPE shapeType,
					PHYSICS_BODY_TYPE bodyType,
					vector<shared_ptr<btCollisionShape>>& btShapes,
					vector<shared_ptr<btTriangleIndexVertexArray>>& btIndexVertexArrays) {

	auto newShape = make_shared<btCompoundShape>(true); // added to btShapes bt caller

	for (auto& element : geometry->elements()) {

		auto indexVertexArray = make_shared<btTriangleIndexVertexArray>();
		auto childShape = BTShapeFromGeometryElement(element,
													 geometry,
													 shapeType,
													 bodyType,
													 btShapes,
													 indexVertexArray);

		// the Geometry's transform is added to the btRigidBody's localInertia
		newShape->addChildShape(BTIdentityTransform(),
								childShape.get());

		btShapes.push_back(childShape);
		btIndexVertexArrays.push_back(indexVertexArray);
	}

	return newShape;
}

void AddBTShapeFromNodeRec(shared_ptr<Node> node,
						   PHYSICS_SHAPE_TYPE shapeType,
						   PHYSICS_BODY_TYPE bodyType,
						   shared_ptr<btCompoundShape> btParentShape,
						   vector<shared_ptr<btCollisionShape>>& btShapes,
						   vector<shared_ptr<btTriangleIndexVertexArray>>& btIndexVertexArrays) {

	auto newShape = make_shared<btCompoundShape>(true);

	if (node->name() != nullopt) {
		AE_LOG_I("name: {}", *node->name());
	}

	auto geometry = node->geometry().get();
	if (geometry) {
		auto nodeGeoShape = BTShapeFromGeometry(geometry,
												shapeType,
												bodyType,
												btShapes,
												btIndexVertexArrays);
		newShape->addChildShape(BTIdentityTransform(),
								nodeGeoShape.get());
		btShapes.push_back(nodeGeoShape);
	}

	btParentShape->addChildShape(BTTransformFromGLMMat4(node->transform()),
								 newShape.get());
	btShapes.push_back(newShape);

	// add child geometries recursively
	for (auto& childNode : node->children(false)) {
		AddBTShapeFromNodeRec(childNode,
							  shapeType,
							  bodyType,
							  newShape,
							  btShapes,
							  btIndexVertexArrays);
	}
}

shared_ptr<btConvexHullShape>
BTConvexHullShapeFromGeometryElement(shared_ptr<GeometryElement> element) {
	AE_LOG_I("Creating convex hull physics shape for GeometryElement {:p}...", (void*)element.get());

	// tips here: https://pybullet.org/Bullet/phpBB3/viewtopic.php?t=11385

	// https://pybullet.org/Bullet/BulletFull/classbtConvexHullShape.html#a069cf26ba277f9f5f141128fee345eaf
	auto originalShape = make_shared<btConvexHullShape>();
	for (const auto& vertex : element->vertices()) {
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

shared_ptr<btGImpactMeshShape>
BTGImpactMeshShapeFromGeometryElement(shared_ptr<GeometryElement> element,
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

	const auto& verts = element->vertices();
	const auto& faces = element->faces();

	auto indexedMesh = make_shared<btIndexedMesh>();

	indexedMesh->m_numTriangles = (int)faces.size();
	indexedMesh->m_triangleIndexBase = (const unsigned char *)faces.data();
	indexedMesh->m_triangleIndexStride = sizeof(Face);
	indexedMesh->m_numVertices = (int)verts.size();
	indexedMesh->m_vertexBase = (const unsigned char *)verts.data();
	indexedMesh->m_vertexStride = sizeof(Vertex);
	indexedMesh->m_vertexType = PHY_FLOAT;

	indexVertexArray->addIndexedMesh(*indexedMesh, PHY_INTEGER);

	auto gImpactMeshShape = make_shared<btGImpactMeshShape>(indexVertexArray.get());
	// https://pybullet.org/Bullet/BulletFull/classbtGImpactShapeInterface.html#a7d26525396fa957d10e36c099c58480f
	gImpactMeshShape->updateBound();

	return gImpactMeshShape;
}

shared_ptr<btBvhTriangleMeshShape>
BTBvhTriangleMeshShapeFromGeometryElement(shared_ptr<GeometryElement> element,
										  shared_ptr<btTriangleIndexVertexArray>& indexVertexArray) {
	AE_LOG_I("Creating concave polyhedron physics shape for GeometryElement {:p}...", (void*)element.get());

	// static objects ALWAYS use btBvhTriangleMeshShape
	// https://pybullet.org/Bullet/phpBB3/viewtopic.php?t=7997

	const auto& verts = element->vertices();
	const auto& faces = element->faces();

	// ^^ asked about on Bullet forum:
	// https://pybullet.org/Bullet/phpBB3/viewtopic.php?p=44462#p44462

	auto indexedMesh = make_shared<btIndexedMesh>();

	indexedMesh->m_numTriangles = (int)faces.size();
	indexedMesh->m_triangleIndexBase = (const unsigned char *)faces.data();
	indexedMesh->m_triangleIndexStride = sizeof(Face);
	indexedMesh->m_numVertices = (int)verts.size();
	indexedMesh->m_vertexBase = (const unsigned char *)verts.data();
	indexedMesh->m_vertexStride = sizeof(Vertex);
	indexedMesh->m_vertexType = PHY_FLOAT;

	indexVertexArray->addIndexedMesh(*indexedMesh, PHY_INTEGER);

	return make_shared<btBvhTriangleMeshShape>(indexVertexArray.get(), true);
}

shared_ptr<btCompoundShape>
BTCompoundConvexHullHACDShapeFromGeometryElement(shared_ptr<GeometryElement> element,
												 vector<shared_ptr<btCollisionShape>>& btShapes) {
	AE_LOG_I("Creating convex hull compound physics shape for HACD GeometryElement {:p}...",
			 (void*)element.get());

	auto compoundShape = make_shared<btCompoundShape>(true);

	auto hacdElements = HACDGeometryElementsFromGeometryElement(element);
	for (auto& hacdElement : hacdElements) {
		auto convextHullShape = BTConvexHullShapeFromGeometryElement(hacdElement);
		compoundShape->addChildShape(BTIdentityTransform(), convextHullShape.get());
		btShapes.push_back(convextHullShape);
	}

	return compoundShape;
}

vector<shared_ptr<GeometryElement>>
HACDGeometryElementsFromGeometryElement(shared_ptr<GeometryElement> element) {
	AE_LOG_I("Creating HACD GeometryElements for GeometryElement {:p}...", (void*)element.get());

	ConvexDecomposer::Options options;
	options.maxConvexHulls = options.maxConvexHulls / 8;
	options.resolution = options.resolution / 8;
	options.maxRecursionDepth = options.maxRecursionDepth / 4;
	options.maxNumVerticesPerHull = options.maxNumVerticesPerHull / 2;

	auto decomposer = ConvexDecomposer(element, options);
	return decomposer.decompose();
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
