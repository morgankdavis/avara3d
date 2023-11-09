//
//  PhysicsContact.cc
//	avara-engine
//
//  Created by Morgan Davis on 1/25/18.
//  Copyright © 2018 Morgan K Davis. All rights reserved.
//

#include "physics/PhysicsContact.h"


using namespace ae;
using namespace glm;
using namespace std;


/*********************************************************************************************
	Lifecycle
 *********************************************************************************************/

PhysicsContact::PhysicsContact() {
	
}

/*********************************************************************************************
	Public
 *********************************************************************************************/

shared_ptr<Node> PhysicsContact::nodeA() const {
	return _nodeA;
}

shared_ptr<Node> PhysicsContact::nodeB() const {
	return _nodeB;
}

vec3 PhysicsContact::contactPoint() const {
	return _contactPoint;
}

vec3 PhysicsContact::contactNormal() const {
	return _contactNormal;
}

float PhysicsContact::collisionImpulse() const {
	return _collisionImpulse;
}

float PhysicsContact::penetrationDistance() const {
	return _penetrationDistance;
}

float PhysicsContact::sweepTestFraction() const {
	return _sweepTestFraction;
}
