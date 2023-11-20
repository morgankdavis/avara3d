//
// Created by mkd on 11/19/23.
//

#ifndef AVARA_ENGINE_CYLINDERPHYSICSSHAPE_H
#define AVARA_ENGINE_CYLINDERPHYSICSSHAPE_H


#include "physics/PhysicsShape.h"


namespace ae {

	class CylinderPhysicsShape : public PhysicsShape {

/*********************************************************************************************
	Lifecycle
 *********************************************************************************************/

	public:

		CylinderPhysicsShape(float radius, float height);
		~CylinderPhysicsShape();

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


#endif //AVARA_ENGINE_CYLINDERPHYSICSSHAPE_H
