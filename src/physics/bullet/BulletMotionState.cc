//
//  BulletMotionState.cc
//  avara3d
//
//  Created by Morgan Davis on 12/15/23.
//  Copyright © 2024 Morgan K Davis. All rights reserved.
//

#include "a3d/physics/bullet/BulletMotionState.h"

#include "a3d/physics/PhysicsBody.h"
#include "a3d/physics/bullet/BulletUtilities.h"
#include "a3d/scene/Node.h"


#include "a3d/diagnostic/logging/Logger.h"


using namespace a3d;


/*********************************************************************************************
	Internal Lifecycle
 *********************************************************************************************/

BulletMotionState::BulletMotionState(PhysicsBody& body):
		btMotionState{},
		_body{&body} { }

/*********************************************************************************************
	btMotionState Members
*********************************************************************************************/

// apply node transform to kinematic physics body
void BulletMotionState::getWorldTransform(btTransform &transform) const {

	if (auto node = _body->node().lock()) {
		transform = BTTransformFromGLMMat4(node->worldTransform());
	}
	// causes warning to be printed every time a new PhysicsBody is created,
	// since when the body is created, it's not yet attahed to a Node.
	// see note at BulletWorldProxy::add()
//	else {
//		A3D_LOG_W("node is null.");
//	}
}

// apply dynamic physics body transform to node
void BulletMotionState::setWorldTransform(const btTransform& transform) {

	if (auto node = _body->node().lock()) {
		node->applyPhysicsTransform(GLMMat4FromBTTransform(transform));
	}
//	else {
//		A3D_LOG_W("node is null.");
//	}
}

/*********************************************************************************************
	Internal Members
 *********************************************************************************************/

PhysicsBody* BulletMotionState::body() const {
	return _body;
}
