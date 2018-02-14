//
//  PhysicsWorld.cpp
//	avara-engine
//
//  Created by Morgan Davis on 1/26/18.
//  Copyright © 2018 Morgan K Davis. All rights reserved.
//

#include "PhysicsWorld.h"

#include <GLFW/glfw3.h>

#include "Logger.h"
#include "PhysicsDebugDrawer.h"
#include "Scene.h"
#include "Utilities.h"
#include "Window.h"


using namespace ae;
using namespace ae::utils;
using namespace glm;
using namespace std;


/***************************************************************************************
     MARK:   Static
 **************************************************************************************/

btIDebugDraw::DebugDrawModes BTDebugDrawModeForDebugOption(DebugOption option) {
	
	switch (option) {
		case DebugOption_ShowPhysicsBoundingBoxes:		return btIDebugDraw::DBG_DrawAabb;
		case DebugOption_ShowPhysicsWireframes:			return btIDebugDraw::DBG_DrawWireframe;
		case DebugOption_ShowPhysicsContactPoints: 		return btIDebugDraw::DBG_DrawContactPoints;
		case DebugOption_ShowPhysicsNormals: 			return btIDebugDraw::DBG_DrawNormals;
		case DebugOption_ShowPhysicsConstraints: 		return btIDebugDraw::DBG_DrawConstraints;
		case DebugOption_ShowPhysicsConstraintLimits:	return btIDebugDraw::DBG_DrawConstraintLimits;
		default:
			AE_LOG->warn("No corresponding BT debug draw mode for debug option: {}", option);
			return btIDebugDraw::DBG_NoDebug;
	}
}

/***************************************************************************************
     MARK:   Lifecycle
 **************************************************************************************/

PhysicsWorld::PhysicsWorld():
	m_scene(nullptr),
	m_debugDrawer(make_shared<PhysicsDebugDrawer>()),
	m_gravity({0, -9.807, 0}),
	m_speed(1.0),
	m_timestep(1.0/60.0) {
	
		m_btCollisionConfiguration = make_shared<btDefaultCollisionConfiguration>();
		m_btDispatcher = make_shared<btCollisionDispatcher>(m_btCollisionConfiguration.get());
		m_btBroadphase = make_shared<btDbvtBroadphase>();
		m_btSolver = make_shared<btSequentialImpulseConstraintSolver>();
		m_btWorld = make_shared<btDiscreteDynamicsWorld>(m_btDispatcher.get(),
														 m_btBroadphase.get(),
														 m_btSolver.get(),
														 m_btCollisionConfiguration.get());
		
		gravity(m_gravity);

		// **** TEMPORARY ****
//		m_debugDrawer->setDebugMode(btIDebugDraw::DBG_DrawAabb |
//									btIDebugDraw::DBG_DrawWireframe |
//									btIDebugDraw::DBG_DrawText |
//									btIDebugDraw::DBG_ProfileTimings |
//									btIDebugDraw::DBG_DrawContactPoints);
//		
		m_btWorld.get()->setDebugDrawer(m_debugDrawer.get());
}

/***************************************************************************************
     MARK:   Public
 **************************************************************************************/

vec3 PhysicsWorld::gravity() const {
	return m_gravity;
}

void PhysicsWorld::gravity(vec3 gravity) {
	m_gravity = gravity;
	m_btWorld->setGravity(BTVector3FromGLMVec3(gravity));
}

float PhysicsWorld::speed() const {
	return m_speed;
}

void PhysicsWorld::speed(float speed) {
	m_speed = speed;
}

float PhysicsWorld::timestep() const {
	return m_timestep;
}

void PhysicsWorld::timestep(float timestep) {
	m_timestep = timestep;
}

void PhysicsWorld::updateCollisionPairs() {
	m_btWorld->getCollisionWorld()->computeOverlappingPairs();
}

shared_ptr<PhysicsContact> PhysicsWorld::contactTest(shared_ptr<PhysicsBody> bodyA,
													 shared_ptr<PhysicsBody> bodyB) {
	
	// contactPairTest (btCollisionObject *colObjA, btCollisionObject *colObjB, ContactResultCallback &resultCallback)
	
	return nullptr;
}

shared_ptr<PhysicsContact> PhysicsWorld::contactTest(shared_ptr<PhysicsBody> body) {
	
	// contactTest (btCollisionObject *colObj, ContactResultCallback &resultCallback)
	
	return nullptr;
}

shared_ptr<HitTestResult> PhysicsWorld::rayTest(vec3 fromVec, vec3 toVec) {
	
	//rayTest (const btVector3 &rayFromWorld, const btVector3 &rayToWorld, RayResultCallback &resultCallback) const
	
	return nullptr;
}

shared_ptr<PhysicsContact> PhysicsWorld::convexSweepTest(shared_ptr<PhysicsContact> contact,
														 const mat4& fromMat,
														 const mat4& toMat) {
	
	// convexSweepTest (const btConvexShape *castShape, const btTransform &from, const btTransform &to, ConvexResultCallback &resultCallback, btScalar allowedCcdPenetration=btScalar(0.)) const 

	return nullptr;
}

/***************************************************************************************
     MARK:   Internal
 **************************************************************************************/

void PhysicsWorld::attachedToScene(Scene& scene) {
	if (scene.window()) {
		debugOptions(scene.window()->debugOptions());
	}
}

void PhysicsWorld::debugOptions(DebugOption options) {
	btIDebugDraw::DebugDrawModes btModes = btIDebugDraw::DBG_NoDebug;

	if (options & DebugOption_ShowPhysicsBoundingBoxes) {
		btModes = (btIDebugDraw::DebugDrawModes)
		(btModes | BTDebugDrawModeForDebugOption(DebugOption_ShowPhysicsBoundingBoxes));
	}
	if (options & DebugOption_ShowPhysicsWireframes) {
		btModes = (btIDebugDraw::DebugDrawModes)
		(btModes | BTDebugDrawModeForDebugOption(DebugOption_ShowPhysicsWireframes));
	}
	if (options & DebugOption_ShowPhysicsContactPoints) {
		btModes = (btIDebugDraw::DebugDrawModes)
		(btModes | BTDebugDrawModeForDebugOption(DebugOption_ShowPhysicsContactPoints));
	}
	if (options & DebugOption_ShowPhysicsNormals) {
		btModes = (btIDebugDraw::DebugDrawModes)
		(btModes | BTDebugDrawModeForDebugOption(DebugOption_ShowPhysicsNormals));
	}
	if (options & DebugOption_ShowPhysicsConstraints) {
		btModes = (btIDebugDraw::DebugDrawModes)
		(btModes | BTDebugDrawModeForDebugOption(DebugOption_ShowPhysicsConstraints));
	}
	if (options & DebugOption_ShowPhysicsConstraintLimits) {
		btModes = (btIDebugDraw::DebugDrawModes)
		(btModes | BTDebugDrawModeForDebugOption(DebugOption_ShowPhysicsConstraintLimits));
	}
	
	/*
	 what do these do?
	
	btModes = (btIDebugDraw::DebugDrawModes)
	(btModes | btIDebugDraw::DBG_ProfileTimings);
	
	btModes = (btIDebugDraw::DebugDrawModes)
	(btModes | btIDebugDraw::DBG_DrawFeaturesText);
	
	btModes = (btIDebugDraw::DebugDrawModes)
	(btModes | btIDebugDraw::DBG_DrawFrames);
	
	btModes = (btIDebugDraw::DebugDrawModes)
	(btModes | btIDebugDraw::DBG_EnableCCD);
	 */
	
	AE_LOG->debug("Bullet debug modes: {}", btModes);
	
	m_debugDrawer->setDebugMode(btModes);
}

void PhysicsWorld::step() {
	AE_LOG->trace("step()");
	
	float time = glfwGetTime();
	static double previousSeconds = time;
	float deltaSeconds = time - previousSeconds;
	previousSeconds = time;
	
	unsigned maxSubSteps = lroundf(1.0/m_timestep);
	m_btWorld->stepSimulation(deltaSeconds, maxSubSteps, m_timestep);
	
	m_debugDrawer->clear();
	m_btWorld->debugDrawWorld();
}

shared_ptr<PhysicsDebugDrawer> PhysicsWorld::debugDrawer() const {
	return m_debugDrawer;
}

shared_ptr<btDiscreteDynamicsWorld> PhysicsWorld::btWorld() const {
	return m_btWorld;
}
