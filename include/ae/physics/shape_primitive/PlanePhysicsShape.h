//
// Created by mkd on 11/19/23.
//

#ifndef AVARA_ENGINE_PLANEPHYSICSSHAPE_H
#define AVARA_ENGINE_PLANEPHYSICSSHAPE_H


#include "ae/physics/PhysicsShape.h"


namespace ae {

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


#endif //AVARA_ENGINE_PLANEPHYSICSSHAPE_H
