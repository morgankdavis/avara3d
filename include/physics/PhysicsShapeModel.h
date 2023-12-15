//
// Created by mkd on 11/13/23.
//

#ifndef AVARA_ENGINE_PHYSICSSHAPEMODEL_H
#define AVARA_ENGINE_PHYSICSSHAPEMODEL_H


#include "Types.h"


namespace ae {


	class PhysicsShape;


	class PhysicsShapeModel {

/*********************************************************************************************
	Lifecycle
 *********************************************************************************************/

	public:

		PhysicsShapeModel(PhysicsShape* shape);
		virtual ~PhysicsShapeModel() = 0;

/*********************************************************************************************
	Protected
 *********************************************************************************************/

	protected:

		PhysicsShape*		_shape;
	};
}


#endif //AVARA_ENGINE_PHYSICSSHAPEMODEL_H
