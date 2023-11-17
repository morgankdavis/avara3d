//
//  PhysicsSimulator.cc
//	avara-engine
//
//  Created by Morgan Davis on 5/1/18.
//  Copyright © 2018 Morgan K Davis. All rights reserved.
//

#include "physics/PhysicsSimulator.h"

#include "diagnostic/logging/Logger.h"
#include "scene/Node.h"
#include "physics/PhysicsBody.h"
#include "physics/PhysicsShape.h"
#include "scene/Scene.h"


using namespace ae;
using namespace glm;
using namespace std;


/*********************************************************************************************
	Lifescycle
 *********************************************************************************************/

PhysicsSimulator::PhysicsSimulator():
	_gravity({0, -9.807, 0}),
	_speed(1.0),
	_timestep(1.0/60.0) {
		
}

PhysicsSimulator::~PhysicsSimulator() {
	AE_LOG_D("Destroying PhysicsSimulator {:p}", (void*)this);
}

/*********************************************************************************************
	Internal
 *********************************************************************************************/

//void PhysicsSimulator::beginUpdate(PASS pass,
//								   const Scene& scene) {
//
//}
//
//void PhysicsSimulator::endUpdate(PASS pass,
//								 const Scene& scene) {
//
//}
void PhysicsSimulator::beginUpdate(const Scene& scene) {

}

void PhysicsSimulator::endUpdate(const Scene& scene) {

}





void PhysicsSimulator::update(Scene& scene) {

}

void PhysicsSimulator::sync(Scene& scene,
							FrameStats& stats) {

}

void PhysicsSimulator::update(PhysicsBody& body,
							  Node& node) {

}

void PhysicsSimulator::sync(PhysicsBody& body,
							Node& node,
							mat4 localTransform,
							FrameStats& stats) {

	switch (body.type()) {
		case (PHYSICS_BODY_TYPE::DYNAMIC): ++stats.dynamicBodies; break;
		case (PHYSICS_BODY_TYPE::KINEMATIC): ++stats.kinematicBodies; break;
		case (PHYSICS_BODY_TYPE::STATIC): ++stats.staticBodies; break;
	}
}

void PhysicsSimulator::update(PhysicsShape& shape,
							  PHYSICS_BODY_TYPE bodyType,
							  bool& updated) {

}

void PhysicsSimulator::sync(PhysicsShape& shape,
							PHYSICS_BODY_TYPE bodyType,
							FrameStats& stats) {

	switch (shape.type()) {
		case (PHYSICS_SHAPE_TYPE::BOUNDING_BOX): ++stats.boundingBoxShapes; break;
		case (PHYSICS_SHAPE_TYPE::CONVEX_HULL): ++stats.convexHullShapes; break;
		case (PHYSICS_SHAPE_TYPE::CONCAVE_POLYHEDRON): ++stats.concavePolyhedronShapes; break;
	}
}





void PhysicsSimulator::update(PASS pass,
							  Scene& scene,
							  const DEBUG_OPTIONS& debugOptions) {
	
}

void PhysicsSimulator::update(PASS pass,
							  shared_ptr<Node> node,
							  const DEBUG_OPTIONS& debugOptions) {
	
}

void PhysicsSimulator::step(float time) {
	
}

