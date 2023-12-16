//
// Created by mkd on 12/15/23.
//

#include "physics/bullet/MotionState.h"

#include "physics/PhysicsBody.h"
#include "physics/bullet/Utilities.h"
#include "scene/Node.h"


using namespace ae;


/*********************************************************************************************
	Lifecycle
 *********************************************************************************************/

MotionState::MotionState(PhysicsBody* body/*,
						 const btTransform& worldTransform*/):
		btMotionState(),
		_body(body)
//		_graphicsWorldTransform()
		{ }

/*********************************************************************************************
	btMotionState
*********************************************************************************************/

// synchronize world transform from graphics to physics
void MotionState::getWorldTransform(btTransform & worldTrans) const {

	if (_body) {
		if (_body->type() == PHYSICS_BODY_TYPE::KINEMATIC) {
			if (auto node = _body->node()) {
				worldTrans = BTTransformFromGLMMat4(node->worldTransform());
			}
		}
	}
}

// synchronize world transform from physics to graphics
void MotionState::setWorldTransform(const btTransform& worldTrans) {

	if (_body) {
		if (_body->type() == PHYSICS_BODY_TYPE::DYNAMIC) {
			if (auto node = _body->node()) {
				node->applyPhysicsTransform(GLMMat4FromBTTransform(worldTrans));
			}
		}
	}
}

/*********************************************************************************************
	Internal
 *********************************************************************************************/

PhysicsBody* MotionState::body() const {
	return _body;
}
