//
//  PlanePhysicsShape.h
//  avara3d
//
//  Created by Morgan Davis on 11/19/23.
//  Copyright © 2024 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_PLANEPHYSICSSHAPE_H
#define AVARA3D_PLANEPHYSICSSHAPE_H

#include "a3d/physics/PhysicsShape.h"

namespace a3d {

	class PlanePhysicsShape : public PhysicsShape {

	public:
		/// Public Lifecycle Functions ///

		PlanePhysicsShape(float width, float height);
		~PlanePhysicsShape();

		/// Public Member Functions ///

		float 					width() const;
		void					width(float width);

		float 					height() const;
		void					height(float height);

		/// PhysicsShape Public Member Functions ///

		PhysicsShapeType		type() const override;
		void					type(PhysicsShapeType type) override;

	private:
		/// Private Member Variables ///

		float 					_width;
		float 					_height;
	};
}

#endif //AVARA3D_PLANEPHYSICSSHAPE_H
