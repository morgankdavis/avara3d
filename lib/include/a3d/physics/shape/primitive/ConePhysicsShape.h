//
//  ConePhysicsShape.h
//  avara3d
//
//  Created by Morgan Davis on 11/19/23.
//  Copyright © 2024 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_PHYSICS_SHAPE_PRIMITIVE_CONEPHYSICSSHAPE_H
#define AVARA3D_PHYSICS_SHAPE_PRIMITIVE_CONEPHYSICSSHAPE_H

#include "a3d/physics/shape/PhysicsShape.h"

namespace a3d {

	class ConePhysicsShape : public PhysicsShape {

	public:
		/// Public Lifecycle Functions ///

		ConePhysicsShape(float radius, float height);

		/// Public Member Functions ///

		float 					radius() const;
		void					radius(float radius);

		float 					height() const;
		void					height(float height);

		/// PhysicsShape Public Member Functions ///

		PhysicsShape::Type		type() const override;
		void					type(PhysicsShape::Type type) override;

	private:
		/// Private Member Variables ///

		float 					_radius;
		float 					_height;
	};
}

#endif //AVARA3D_PHYSICS_SHAPE_PRIMITIVE_CONEPHYSICSSHAPE_H
