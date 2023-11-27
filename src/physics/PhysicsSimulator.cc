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
		_timestep(1.0/60.0) { }

PhysicsSimulator::~PhysicsSimulator() {
	AE_LOG_D("Destroying PhysicsSimulator {:p}", (void*)this);
}

/*********************************************************************************************
	Internal
 *********************************************************************************************/

void PhysicsSimulator::beginUpdate(const Scene& scene) { }

void PhysicsSimulator::endUpdate(const Scene& scene) { }

void PhysicsSimulator::update(Scene& scene) { }

void PhysicsSimulator::sync(Scene& scene) { }

void PhysicsSimulator::update(PhysicsBody& body,
							  Node& node) { }

void PhysicsSimulator::sync(PhysicsBody& body,
							Node& node,
							mat4& worldTransform) { }

void PhysicsSimulator::update(PhysicsShape& shape,
							  PHYSICS_BODY_TYPE bodyType,
							  bool& updated) { }

void PhysicsSimulator::sync(PhysicsShape& shape,
							PHYSICS_BODY_TYPE bodyType) { }

void PhysicsSimulator::step(float time) { }

