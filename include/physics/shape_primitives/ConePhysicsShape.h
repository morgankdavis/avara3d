//
// Created by mkd on 11/19/23.
//

#ifndef AVARA_ENGINE_CONEPHYSICSSHAPE_H
#define AVARA_ENGINE_CONEPHYSICSSHAPE_H


#include "physics/PhysicsShape.h"


namespace ae {

	class ConePhysicsShape : public PhysicsShape {

/*********************************************************************************************
	Lifecycle
 *********************************************************************************************/

	public:

		ConePhysicsShape(float radius, float height);
		~ConePhysicsShape();

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


#endif //AVARA_ENGINE_CONEPHYSICSSHAPE_H
