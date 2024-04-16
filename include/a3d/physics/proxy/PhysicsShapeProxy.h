//
//  PhysicsShapeProxy.h
//	avara3d
//
//  Created by Morgan Davis on 11/13/23.
//  Copyright © 2024 Morgan K Davis. All rights reserved.
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

		explicit PhysicsShapeProxy(PhysicsShape& shape);
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
