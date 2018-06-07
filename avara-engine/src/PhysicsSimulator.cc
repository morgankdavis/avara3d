//
//  PhysicsSimulator.cc
//	avara-engine
//
//  Created by Morgan Davis on 5/1/18.
//  Copyright © 2018 Morgan K Davis. All rights reserved.
//

#include "PhysicsSimulator.h"

#include "Logger.h"


using namespace ae;
using namespace glm;
using namespace std;


/***************************************************************************************
     Lifescycle
 ***************************************************************************************/

PhysicsSimulator::PhysicsSimulator():
	m_gravity({0, -9.807, 0}),
	m_speed(1.0),
	m_timestep(1.0/60.0) {
		
}

PhysicsSimulator::~PhysicsSimulator() {
	AE_LOG->debug("Destroying PhysicsSimulator {:p}", (void*)this);
}

/**************************************************************************************
     Internal
 **************************************************************************************/

void PhysicsSimulator::beginUpdate(PASS pass,
								   const Scene& scene) {
	
}

void PhysicsSimulator::endUpdate(PASS pass,
								 const Scene& scene) {
	
}

void PhysicsSimulator::update(PASS pass,
							  shared_ptr<Scene> scene,
							  const DEBUG_OPTIONS& debugOptions) {
	
}

void PhysicsSimulator::update(PASS pass,
							  shared_ptr<Node> node,
							  const DEBUG_OPTIONS& debugOptions) {
	
}

void PhysicsSimulator::step(float time) {
	
}

