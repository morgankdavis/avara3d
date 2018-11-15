//
//  PhysicsContact.cc
//	avara-engine
//
//  Created by Morgan Davis on 1/25/18.
//  Copyright © 2018 Morgan K Davis. All rights reserved.
//

#include "PhysicsContact.h"


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
	return m_nodeA;
}

shared_ptr<Node> PhysicsContact::nodeB() const {
	return m_nodeB;
}

vec3 PhysicsContact::contactPoint() const {
	return m_contactPoint;
}

vec3 PhysicsContact::contactNormal() const {
	return m_contactNormal;
}

float PhysicsContact::collisionImpulse() const {
	return m_collisionImpulse;
}

float PhysicsContact::penetrationDistance() const {
	return m_penetrationDistance;
}

float PhysicsContact::sweepTestFraction() const {
	return m_sweepTestFraction;
}
