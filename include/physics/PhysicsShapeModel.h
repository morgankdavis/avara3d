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
		~PhysicsShapeModel();
	};
}


#endif //AVARA_ENGINE_PHYSICSSHAPEMODEL_H
