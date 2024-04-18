//
//  ConePhysicsShape.h
//  avara3d
//
//  Created by Morgan Davis on 11/19/23.
//  Copyright © 2024 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_CONEPHYSICSSHAPE_H
#define AVARA3D_CONEPHYSICSSHAPE_H


#include "a3d/physics/PhysicsShape.h"


namespace a3d {

	class ConePhysicsShape : public PhysicsShape {

/*********************************************************************************************
	Public Lifecycle
 *********************************************************************************************/

	public:

		ConePhysicsShape(float radius, float height);
		~ConePhysicsShape();

/*********************************************************************************************
	Public Members
 *********************************************************************************************/

		float 					radius() const;
		void					radius(float radius);

		float 					height() const;
		void					height(float height);

/*********************************************************************************************
	PhysicsShape Public Members
 *********************************************************************************************/

		PhysicsShapeType		type() const override;
		void					type(PhysicsShapeType type) override;

/*********************************************************************************************
	Private IVars
 *********************************************************************************************/

	private:

		float 					_radius;
		float 					_height;
	};
}


#endif //AVARA3D_CONEPHYSICSSHAPE_H
