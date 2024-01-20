//
// Created by mkd on 11/19/23.
//

#ifndef AVARA_ENGINE_BOXPHYSICSSHAPE_H
#define AVARA_ENGINE_BOXPHYSICSSHAPE_H


#include "ae/physics/PhysicsShape.h"


namespace ae {

	class BoxPhysicsShape : public PhysicsShape {

/*********************************************************************************************
	Lifecycle
 *********************************************************************************************/

	public:

		BoxPhysicsShape(float width, float height, float length);
		~BoxPhysicsShape();

/*********************************************************************************************
	Public
 *********************************************************************************************/

		float 					width() const;
		void					width(float width);

		float 					height() const;
		void					height(float height);

		float 					length() const;
		void					length(float length);

/*********************************************************************************************
	PhysicsShape
 *********************************************************************************************/

		PHYSICS_SHAPE_TYPE		type() const override;
		void					type(PHYSICS_SHAPE_TYPE type) override;

/*********************************************************************************************
	Private
 *********************************************************************************************/

	private:

		float 					_width;
		float 					_height;
		float 					_length;
	};
}


#endif //AVARA_ENGINE_BOXPHYSICSSHAPE_H
