//
//  BulletMotionState.h
//  avara3d
//
//  Created by Morgan Davis on 12/15/23.
//  Copyright © 2024 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_BULLETMOTIONSTATE_H
#define AVARA3D_BULLETMOTIONSTATE_H

#include "LinearMath/btMotionState.h"

namespace a3d {

	class PhysicsBody;

	class BulletMotionState : public btMotionState {

/*********************************************************************************************
	Internal Lifecycle Functions
 *********************************************************************************************/

	public:

		explicit BulletMotionState(PhysicsBody& body);

/*********************************************************************************************
	btMotionState Members
 *********************************************************************************************/

		void getWorldTransform(btTransform& transform) const override;
		void setWorldTransform(const btTransform& transform) override;

/*********************************************************************************************
	Internal Member Functions
 *********************************************************************************************/

		PhysicsBody*		body() const;

/*********************************************************************************************
	Private Member Variables
 *********************************************************************************************/

//		btTransform 		_visualWorldTransform;
		PhysicsBody* 		_body;
	};
}

#endif //AVARA3D_BULLETMOTIONSTATE_H
