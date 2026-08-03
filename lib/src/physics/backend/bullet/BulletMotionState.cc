//
//  BulletMotionState.cc
//  avara3d
//
//  Created by Morgan Davis on 12/15/23.
//  Copyright © 2024 Morgan K Davis. All rights reserved.
//

#include "a3d/physics/backend/bullet/BulletMotionState.h"

#include "a3d/physics/PhysicsBody.h"
#include "a3d/physics/backend/bullet/BulletUtilities.h"
#include "a3d/scene/Node.h"

#include "a3d/log/Log.h"

using namespace a3d;

/// Internal Lifecycle Functions ///

BulletMotionState::BulletMotionState(PhysicsBody& body):
		btMotionState{},
		_body{&body} { }

/// btMotionState Members ///

// apply node transform to kinematic physics body (only called for kinematic bodies)
void BulletMotionState::getWorldTransform(btTransform& transform) const {

	if (auto node = _body->node().lock()) {
		transform = BTTransformFromA3DMat4(node->worldTransform());
	}
	// causes warning to be printed every time a new PhysicsBody is created,
	// since when the body is created, it's not yet attahed to a Node.
	// see note at BulletWorldProxy::add()
//	else {
//		log::w()("node is null.");
//	}
}

// apply dynamic physics body transform to node (only called for dynamic bodies)
void BulletMotionState::setWorldTransform(const btTransform& transform) {

	if (auto node = _body->node().lock()) {
		node->applyPhysicsTransform(A3DMat4FromBTTransform(transform));
	}
//	else {
//		log::w()("node is null.");
//	}
}

/// Internal Member Functions ///

PhysicsBody* BulletMotionState::body() const {
	return _body;
}
