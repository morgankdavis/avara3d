//
// Created by mkd on 12/15/23.
//

#ifndef AVARA_ENGINE_BULLET_MOTIONSTATE_H
#define AVARA_ENGINE_BULLET_MOTIONSTATE_H


#import "LinearMath/btDefaultMotionState.h"


namespace ae {


	class PhysicsBody;


	class MotionState : public btMotionState {

/*********************************************************************************************
	Lifecycle
 *********************************************************************************************/

	public:

		MotionState(PhysicsBody* body/*,
					const btTransform& worldTransform = btTransform::getIdentity()*/);

/*********************************************************************************************
	btMotionState
 *********************************************************************************************/

		void getWorldTransform(btTransform& worldTrans) const override;
		void setWorldTransform(const btTransform& worldTrans) override;

/*********************************************************************************************
	Internal
 *********************************************************************************************/

		PhysicsBody*		body() const;

/*********************************************************************************************
	Private
 *********************************************************************************************/

//		btTransform 		_graphicsWorldTransform;
		PhysicsBody* 		_body;
	};
}

#endif //AVARA_ENGINE_BULLET_MOTIONSTATE_H
