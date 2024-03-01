//
// Created by mkd on 11/19/23.
//

#ifndef AVARA3D_CONEPHYSICSSHAPE_H
#define AVARA3D_CONEPHYSICSSHAPE_H


#include "a3d/physics/PhysicsShape.h"


namespace a3d {

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

		PhysicsShapeType		type() const override;
		void					type(PhysicsShapeType type) override;

/*********************************************************************************************
	Private
 *********************************************************************************************/

	private:

		float 					_radius;
		float 					_height;
	};
}


#endif //AVARA3D_CONEPHYSICSSHAPE_H
