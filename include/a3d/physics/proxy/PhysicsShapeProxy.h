//
// Created by mkd on 11/13/23.
//

#ifndef AVARA3D_PHYSICSSHAPEPROXY_H
#define AVARA3D_PHYSICSSHAPEPROXY_H


#include "a3d/Types.h"


namespace a3d {


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


#endif //AVARA3D_PHYSICSSHAPEPROXY_H
