//
// Created by mkd on 12/15/23.
//

#ifndef AVARA3D_BULLET_MOTIONSTATE_H
#define AVARA3D_BULLET_MOTIONSTATE_H


#include "LinearMath/btMotionState.h"


namespace a3d {


	class PhysicsBody;


	class MotionState : public btMotionState {

/*********************************************************************************************
	Lifecycle
 *********************************************************************************************/

	public:

		MotionState(PhysicsBody* body);

/*********************************************************************************************
	btMotionState
 *********************************************************************************************/

		void getWorldTransform(btTransform& transform) const override;
		void setWorldTransform(const btTransform& transform) override;

/*********************************************************************************************
	Internal
 *********************************************************************************************/

		PhysicsBody*		body() const;

/*********************************************************************************************
	Private
 *********************************************************************************************/

		btTransform 		_graphicsWorldTransform;
		PhysicsBody* 		_body;
	};
}

#endif //AVARA3D_BULLET_MOTIONSTATE_H
