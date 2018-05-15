//
//  PhysicsWorld.cpp
//	avara-engine
//
//  Created by Morgan Davis on 1/26/18.
//  Copyright © 2018 Morgan K Davis. All rights reserved.
//

#include "PhysicsWorld.h"

#include "Logger.h"
#include "PhysicsDebugDrawer.h"
#include "RenderContext.h"
#include "Scene.h"
#include "Utilities.h"
#include "Window.h"


using namespace ae;
using namespace ae::utils;
using namespace glm;
using namespace std;


/***************************************************************************************
     Lifecycle
 ***************************************************************************************/

PhysicsWorld::PhysicsWorld():
	m_gravity({0, -9.807, 0}),
	m_speed(1.0),
	m_timestep(1.0/60.0),
	m_scene(weak_ptr<Scene>()) {

}

/***************************************************************************************
     Public
 ***************************************************************************************/

vec3 PhysicsWorld::gravity() const {
	return m_gravity;
}

void PhysicsWorld::gravity(vec3 gravity) {
	m_gravity = gravity;
	/****** m_btWorld->setGravity(BTVector3FromGLMVec3(gravity)); ****/
}

float PhysicsWorld::timestep() const {
	return m_timestep;
}

void PhysicsWorld::timestep(float timestep) {
	m_timestep = timestep;
}

void PhysicsWorld::updateCollisionPairs() {
	//m_btWorld->getCollisionWorld()->computeOverlappingPairs();
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
     Internal
 ***************************************************************************************/

void PhysicsWorld::attachedToScene(shared_ptr<Scene> scene) {
	m_scene = scene;
//	if (auto window = scene->window().lock()) {
//		debugOptions(window->debugOptions());
//	}
#ifdef DESKTOP
	if (auto renderer = scene->renderContext().lock()) {
		debugOptions(renderer->debugOptions());
	}
#endif
}

void PhysicsWorld::debugOptions(DEBUG_OPTIONS options) {
	btIDebugDraw::DebugDrawModes btModes = btIDebugDraw::DBG_NoDebug;
	
#if 0

	//if (static_cast<int>(options) & static_cast<int>(DEBUG_OPTIONS::SHOW_PHYSICS_BOUNDING_BOXES)) {
	if (DEBUG_OPTIONS_CONTAINS(options, DEBUG_OPTIONS::SHOW_PHYSICS_BOUNDING_BOXES)) {
		btModes = (btIDebugDraw::DebugDrawModes)
		(btModes | BTDebugDrawModeForDebugOption(DEBUG_OPTIONS::SHOW_PHYSICS_BOUNDING_BOXES));
	}
	//if (static_cast<int>(options) & static_cast<int>(DEBUG_OPTIONS::SHOW_PHYSICS_WIREFRAMES)) {
	if (DEBUG_OPTIONS_CONTAINS(options, DEBUG_OPTIONS::SHOW_PHYSICS_WIREFRAMES)) {
		btModes = (btIDebugDraw::DebugDrawModes)
		(btModes | BTDebugDrawModeForDebugOption(DEBUG_OPTIONS::SHOW_PHYSICS_WIREFRAMES));
	}
	//if (static_cast<int>(options) & static_cast<int>(DEBUG_OPTIONS::SHOW_PHYSICS_CONTACT_POINTS)) {
	if (DEBUG_OPTIONS_CONTAINS(options, DEBUG_OPTIONS::SHOW_PHYSICS_CONTACT_POINTS)) {
		btModes = (btIDebugDraw::DebugDrawModes)
		(btModes | BTDebugDrawModeForDebugOption(DEBUG_OPTIONS::SHOW_PHYSICS_CONTACT_POINTS));
	}
	//if (static_cast<int>(options) & static_cast<int>(DEBUG_OPTIONS::SHOW_PHYSICS_NORMALS)) {
	if (DEBUG_OPTIONS_CONTAINS(options, DEBUG_OPTIONS::SHOW_PHYSICS_NORMALS)) {
		btModes = (btIDebugDraw::DebugDrawModes)
		(btModes | BTDebugDrawModeForDebugOption(DEBUG_OPTIONS::SHOW_PHYSICS_NORMALS));
	}
	//if (static_cast<int>(options) & static_cast<int>(DEBUG_OPTIONS::SHOW_PHYSICS_CONSTRAINTS)) {
	if (DEBUG_OPTIONS_CONTAINS(options, DEBUG_OPTIONS::SHOW_PHYSICS_CONSTRAINTS)) {
		btModes = (btIDebugDraw::DebugDrawModes)
		(btModes | BTDebugDrawModeForDebugOption(DEBUG_OPTIONS::SHOW_PHYSICS_CONSTRAINTS));
	}
	//if (static_cast<int>(options) & static_cast<int>(DEBUG_OPTIONS::SHOW_PHYSICS_CONSTRAINT_LIMITS)) {
	if (DEBUG_OPTIONS_CONTAINS(options, DEBUG_OPTIONS::SHOW_PHYSICS_CONSTRAINT_LIMITS)) {
		btModes = (btIDebugDraw::DebugDrawModes)
		(btModes | BTDebugDrawModeForDebugOption(DEBUG_OPTIONS::SHOW_PHYSICS_CONSTRAINT_LIMITS));
	}
	
#endif
	
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

//#ifdef DESKTOP
//	m_debugDrawer->setDebugMode(btModes);
//#endif
}
