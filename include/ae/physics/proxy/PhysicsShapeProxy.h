//
// Created by mkd on 11/13/23.
//

#ifndef AVARA_ENGINE_PHYSICSSHAPEPROXY_H
#define AVARA_ENGINE_PHYSICSSHAPEPROXY_H


#include "ae/Types.h"


namespace ae {


	class PhysicsShape;


	class PhysicsShapeProxy {

/*********************************************************************************************
	Lifecycle
 *********************************************************************************************/

	public:

		PhysicsShapeProxy(PhysicsShape* shape);
		virtual ~PhysicsShapeProxy() = 0;

/*********************************************************************************************
	Internal
 *********************************************************************************************/

//		void			attachedToShape(PhysicsShape* shape);
//		void			detachedFromShape(PhysicsShape* shape);

/*********************************************************************************************
	Protected
 *********************************************************************************************/

	protected:

//		PhysicsShape*	_shape;
	};
}


#endif //AVARA_ENGINE_PHYSICSSHAPEPROXY_H
