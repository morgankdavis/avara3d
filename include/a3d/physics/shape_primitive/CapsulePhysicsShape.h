//
// Created by mkd on 11/19/23.
//

#ifndef AVARA3D_CAPSULEPHYSICSSHAPE_H
#define AVARA3D_CAPSULEPHYSICSSHAPE_H


#include "a3d/physics/PhysicsShape.h"


namespace a3d {

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


#endif //AVARA3D_CAPSULEPHYSICSSHAPE_H
