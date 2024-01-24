//
// Created by mkd on 12/15/23.
//

#include "physics/bullet/MotionState.h"

#include "physics/PhysicsBody.h"
#include "physics/bullet/Utilities.h"
#include "scene/Node.h"


#include "diagnostic/logging/Logger.h"


using namespace ae;


/*********************************************************************************************
	Lifecycle
 *********************************************************************************************/

MotionState::MotionState(PhysicsBody* body):
		btMotionState(),
		_body(body) { }

/*********************************************************************************************
	btMotionState
*********************************************************************************************/

// apply node transform to kinematic physics body
void MotionState::getWorldTransform(btTransform &transform) const {

	if (auto node = _body->node()) {
		transform = BTTransformFromGLMMat4(node->worldTransform());
	}
}

// apply dynamic physics body transform to node
void MotionState::setWorldTransform(const btTransform& transform) {

	if (auto node = _body->node()) {
		node->applyPhysicsTransform(GLMMat4FromBTTransform(transform));
	}
}

/*********************************************************************************************
	Internal
 *********************************************************************************************/

PhysicsBody* MotionState::body() const {
	return _body;
}
