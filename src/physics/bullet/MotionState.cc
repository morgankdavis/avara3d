//
// Created by mkd on 12/15/23.
//

#include "physics/bullet/MotionState.h"

//#include "diagnostic/Logger.h"
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
void MotionState::getWorldTransform(btTransform &transform) const {

//	if (_body->type() == PHYSICS_BODY_TYPE::KINEMATIC) {
		if (auto node = _body->node()) {
			transform = BTTransformFromGLMMat4(node->worldTransform());
		}
//	}
}

// synchronize world transform from physics to graphics
void MotionState::setWorldTransform(const btTransform& transform) {

//	if (_body) {
//		if (_body->type() == PHYSICS_BODY_TYPE::DYNAMIC) {
			if (auto node = _body->node()) {
				node->applyPhysicsTransform(GLMMat4FromBTTransform(transform));
			}
//		}
//	}
}

/*********************************************************************************************
	Internal
 *********************************************************************************************/

PhysicsBody* MotionState::body() const {
	return _body;
}
