//
// Created by mkd on 12/15/23.
//

#include "a3d/physics/bullet/MotionState.h"

#include "a3d/physics/PhysicsBody.h"
#include "a3d/physics/bullet/Utilities.h"
#include "a3d/scene/Node.h"


#include "a3d/diagnostic/logging/Logger.h"


using namespace a3d;


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
