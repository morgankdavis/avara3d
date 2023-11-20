//
// Created by mkd on 11/19/23.
//

#ifndef AVARA_ENGINE_SPHEREPHYSICSSHAPE_H
#define AVARA_ENGINE_SPHEREPHYSICSSHAPE_H


#include "physics/PhysicsShape.h"


namespace ae {

	class SpherePhysicsShape : public PhysicsShape {

/*********************************************************************************************
	Lifecycle
 *********************************************************************************************/

	public:

		SpherePhysicsShape(float radius);
		~SpherePhysicsShape();

/*********************************************************************************************
	Public
 *********************************************************************************************/

		float 					radius() const;
		void					radius(float radius);

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
	};
}


#endif //AVARA_ENGINE_SPHEREPHYSICSSHAPE_H
