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
#include <LinearMath/btScalar.h> // btGetVersion() !


#include "Logger.h"
#include "Node.h"
#include "PhysicsDebugDrawer.h"
#include "PhysicsBody.h"
#include "PhysicsShape.h"
#include "PhysicsWorld.h"


using namespace ae;
using namespace glm;
using namespace std;


/**************************************************************************************
     Static Prototypes
 **************************************************************************************/

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
	m_bodyMotionStateIDMapping(PhysicsBodyMotionStateIDMapping()),
	m_bodyIDCounter(0),
	m_shapeIDMapping(PhysicsShapeIDMapping()),
	m_shapeIDCounter(0) {

							   
		AE_LOG->info("Bullet version: {}",  btGetVersion());
		
//		m_btCollisionConfiguration = make_shared<btDefaultCollisionConfiguration>();
//		m_btDispatcher = make_shared<btCollisionDispatcher>(m_btCollisionConfiguration.get());
//		m_btBroadphase = make_shared<btDbvtBroadphase>();
//		m_btSolver = make_shared<btSequentialImpulseConstraintSolver>();
//		m_btWorld = make_shared<btDiscreteDynamicsWorld>(m_btDispatcher.get(),
//														 m_btBroadphase.get(),
//														 m_btSolver.get(),
//														 m_btCollisionConfiguration.get());
}

BulletPhysicsSimulator::~BulletPhysicsSimulator() {
	
}

/**************************************************************************************
     Physics Simulator
 **************************************************************************************/

//void BulletPhysicsSimulator::initialize() {
//	AE_LOG->trace("initialize()");
//}

void BulletPhysicsSimulator::update(PhysicsWorld& physicsWorld,
									const DEBUG_OPTIONS& debugOptions) {

#warning check dirty bit
	m_btWorld->setGravity(BTVector3FromGLMVec3(physicsWorld.gravity()));
}

void BulletPhysicsSimulator::update(PhysicsBody& physicsBody,
									const DEBUG_OPTIONS& debugOptions) {
	
	auto node = physicsBody.node().lock();
	
	if (PHYSICS_BODY_DIRTY_BITS_CONTAINS(physicsBody.dirtyBits(),
										 PHYSICS_BODY_DIRTY_BITS::TYPE) ||
		PHYSICS_BODY_DIRTY_BITS_CONTAINS(physicsBody.dirtyBits(),
										 PHYSICS_BODY_DIRTY_BITS::SHAPE)) {
		
		// 'type' indicates that the type has changed, or that the
		// internal body hasn't been initialized yet

		if (!physicsBody.shape()) {
			if (node->geometry()) {
				physicsBody.shape(make_shared<PhysicsShape>(node->geometry(), PHYSICS_SHAPE_TYPE::CONVEX_HULL));
			}
			else {
				physicsBody.shape(make_shared<PhysicsShape>(node, PHYSICS_SHAPE_TYPE::CONVEX_HULL));
			}
			physicsBody.shape()->attachedToBody(node->physicsBody());
		}
			
			
		btTransform transform;
		transform.setFromOpenGLMatrix(value_ptr(node->worldTransform()));
		auto motionState = make_shared<btDefaultMotionState>(transform);
		
//
//		resetTransform();
//		
//
//		btCollisionShape* collisionShape = m_shape->btShape().get();
//		
//
//		btVector3 localInertia(0, 0, 0);
//		if (m_mass != 0) collisionShape->calculateLocalInertia(m_mass, localInertia);
//		
//		btRigidBody::btRigidBodyConstructionInfo rigidBodyInfo((m_type == PHYSICS_BODY_TYPE::STATIC ? 0 : m_mass),
//															   m_btMotionState.get(),
//															   collisionShape,
//															   localInertia);
//		
//		// √ velocity factor
//		// √ angular velocity factor
//		// afected by gravity
//		rigidBodyInfo.m_mass = m_mass;
//		// charge
//		rigidBodyInfo.m_friction = m_friction;
//		rigidBodyInfo.m_rollingFriction = m_rollingFriction;
//		rigidBodyInfo.m_restitution = m_restitution;
//		rigidBodyInfo.m_linearDamping = m_damping;
//		rigidBodyInfo.m_angularDamping = m_angularDamping;
//		// moment of inertia
//		// √ velocity
//		// √ angular velocity
//		// resting
//		// allows resting
//		
//		m_btRigidBody = make_shared<btRigidBody>(rigidBodyInfo);
//		
//#if 0
//		
//		m_btRigidBody->setLinearFactor(BTVector3FromGLMVec3(m_velocityFactor));
//		m_btRigidBody->setAngularFactor(BTVector3FromGLMVec3(m_angularVelocityFactor));
//		m_btRigidBody->setLinearVelocity(BTVector3FromGLMVec3(m_velocity));
//		m_btRigidBody->setAngularVelocity(BTVector3FromGLMVec3(m_angularVelocity));
//		//m_btRigidBody->setGravity()
//		
//#endif
//		
//		if (auto scene = node->scene().lock()) {
//			scene->physicsWorld()->btWorld()->addRigidBody(m_btRigidBody.get());
//		}
		
		
		physicsBody.dirtyBits(PHYSICS_BODY_DIRTY_BITS_REMOVE(physicsBody.dirtyBits(),
															 PHYSICS_BODY_DIRTY_BITS::TYPE));
	}
}

void BulletPhysicsSimulator::step(float time) {
	
}
						  
/**************************************************************************************
     Static
 **************************************************************************************/
	
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
