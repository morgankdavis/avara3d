//
// Created by mkd on 11/19/23.
//

#ifndef AVARA_ENGINE_SPHEREPHYSICSSHAPE_H
#define AVARA_ENGINE_SPHEREPHYSICSSHAPE_H


#include "ae/physics/PhysicsShape.h"


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

		PhysicsShapeType		type() const override;
		void					type(PhysicsShapeType type) override;

/*********************************************************************************************
	Private
 *********************************************************************************************/

	private:

		float 					_radius;
	};
}


#endif //AVARA_ENGINE_SPHEREPHYSICSSHAPE_H
