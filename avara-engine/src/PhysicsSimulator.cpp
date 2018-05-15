//
//  PhysicsSimulator.cpp
//	avara-engine
//
//  Created by Morgan Davis on 5/1/18.
//  Copyright © 2018 Morgan K Davis. All rights reserved.
//

#include "PhysicsSimulator.h"

#include "Geometry.h"
#include "Scene.h"


using namespace ae;


/***************************************************************************************
     Lifescycle
 ***************************************************************************************/

PhysicsSimulator::PhysicsSimulator()
//	m_gravity({0, -9.807, 0}),
//	m_speed(1.0),
//	m_timestep(1.0/60.0) {
	{
		
}

PhysicsSimulator::~PhysicsSimulator() {
	
}

/**************************************************************************************
     Internal
 **************************************************************************************/

//void PhysicsSimulator::initialize() {
//	
//}

void PhysicsSimulator::update(PASS pass,
							  PhysicsWorld& physicsWorld,
							  const DEBUG_OPTIONS& debugOptions) {
	
}

void PhysicsSimulator::update(PASS pass,
							  PhysicsBody& physicsBody,
							  const DEBUG_OPTIONS& debugOptions) {
	
}

void PhysicsSimulator::step(float time) {
	
}

