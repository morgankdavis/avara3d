//
//  SpherePhysicsShape.h
//  avara3d
//
//  Created by Morgan Davis on 11/19/23.
//  Copyright © 2024 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_SPHEREPHYSICSSHAPE_H
#define AVARA3D_SPHEREPHYSICSSHAPE_H

#include "a3d/physics/PhysicsShape.h"

namespace a3d {

	class SpherePhysicsShape : public PhysicsShape {

/*********************************************************************************************
	Public Lifecycle Functions
 *********************************************************************************************/

	public:

		explicit SpherePhysicsShape(float radius);
		~SpherePhysicsShape();

/*********************************************************************************************
	Public Member Functions
 *********************************************************************************************/

		float 					radius() const;
		void					radius(float radius);

/*********************************************************************************************
	PhysicsShape Public Member Functions
 *********************************************************************************************/

		PhysicsShapeType		type() const override;
		void					type(PhysicsShapeType type) override;

/*********************************************************************************************
	Private Member Variables
 *********************************************************************************************/

	private:

		float 					_radius;
	};
}

#endif //AVARA3D_SPHEREPHYSICSSHAPE_H
