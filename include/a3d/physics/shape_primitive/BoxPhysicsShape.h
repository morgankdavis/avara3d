//
//  BoxPhysicsShape.h
//  avara3d
//
//  Created by Morgan Davis on 11/19/23.
//  Copyright © 2024 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_BOXPHYSICSSHAPE_H
#define AVARA3D_BOXPHYSICSSHAPE_H


#include "a3d/physics/PhysicsShape.h"


namespace a3d {

	class BoxPhysicsShape : public PhysicsShape {

	public:

/*********************************************************************************************
	Public
 *********************************************************************************************/

		BoxPhysicsShape(float width, float height, float length);
		~BoxPhysicsShape();

		float 					width() const;
		void					width(float width);

		float 					height() const;
		void					height(float height);

		float 					length() const;
		void					length(float length);

/*********************************************************************************************
	PhysicsShape Public
 *********************************************************************************************/

		PhysicsShapeType		type() const override;
		void					type(PhysicsShapeType type) override;

	private:

/*********************************************************************************************
	Private
 *********************************************************************************************/

		float 					_width;
		float 					_height;
		float 					_length;
	};
}


#endif //AVARA3D_BOXPHYSICSSHAPE_H
