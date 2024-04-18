//
//  PhysicsContact.cc
//  avara3d
//
//  Created by Morgan Davis on 1/25/18.
//  Copyright © 2024 Morgan K Davis. All rights reserved.
//

#include "a3d/physics/PhysicsContact.h"


using namespace a3d;
using namespace glm;
using namespace std;


/*********************************************************************************************
	Public Lifecycle
 *********************************************************************************************/

PhysicsContact::PhysicsContact() { }

/*********************************************************************************************
	Public Members
 *********************************************************************************************/

weak_ptr<Node> PhysicsContact::nodeA() const {
	return _nodeA;
}

weak_ptr<Node> PhysicsContact::nodeB() const {
	return _nodeB;
}

const vec3& PhysicsContact::contactPoint() const {
	return _contactPoint;
}

const vec3& PhysicsContact::contactNormal() const {
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
