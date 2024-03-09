//
// Created by mkd on 11/19/23.
//

#ifndef AVARA3D_PLANEPHYSICSSHAPE_H
#define AVARA3D_PLANEPHYSICSSHAPE_H


#include "a3d/physics/PhysicsShape.h"


namespace a3d {

	class PlanePhysicsShape : public PhysicsShape {

/*********************************************************************************************
	Lifecycle
 *********************************************************************************************/

	public:

		PlanePhysicsShape(float width, float height);
		~PlanePhysicsShape();

/*********************************************************************************************
	Public
 *********************************************************************************************/

		float 					width() const;
		void					width(float width);

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

		float 					_width;
		float 					_height;
	};
}


#endif //AVARA3D_PLANEPHYSICSSHAPE_H
