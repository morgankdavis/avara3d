//
// Created by mkd on 11/19/23.
//

#ifndef AVARA_ENGINE_CAPSULEPHYSICSSHAPE_H
#define AVARA_ENGINE_CAPSULEPHYSICSSHAPE_H


#include "ae/physics/PhysicsShape.h"


namespace ae {

	class CapsulePhysicsShape : public PhysicsShape {

/*********************************************************************************************
	Lifecycle
 *********************************************************************************************/

	public:

		CapsulePhysicsShape(float radius, float height);
		~CapsulePhysicsShape();

/*********************************************************************************************
	Public
 *********************************************************************************************/

		float 					radius() const;
		void					radius(float radius);

		float 					height() const;
		void					height(float height);

/*********************************************************************************************
	PhysicsShape
 *********************************************************************************************/

		PHYSICS_SHAPE_TYPE		type() const override;
		void					type(PHYSICS_SHAPE_TYPE type) override;

/*********************************************************************************************
	Private
 *********************************************************************************************/

	private:

		float 					_radius;
		float 					_height;
	};
}


#endif //AVARA_ENGINE_CAPSULEPHYSICSSHAPE_H
