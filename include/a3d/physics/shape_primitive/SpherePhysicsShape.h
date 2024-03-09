//
// Created by mkd on 11/19/23.
//

#ifndef AVARA3D_SPHEREPHYSICSSHAPE_H
#define AVARA3D_SPHEREPHYSICSSHAPE_H


#include "a3d/physics/PhysicsShape.h"


namespace a3d {

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


#endif //AVARA3D_SPHEREPHYSICSSHAPE_H
