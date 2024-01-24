//
// Created by mkd on 11/13/23.
//

#ifndef AVARA_ENGINE_PHYSICSSHAPEMODELPROXY_H
#define AVARA_ENGINE_PHYSICSSHAPEMODELPROXY_H


#include "ae/Types.h"


namespace ae {


	class PhysicsShape;


	class PhysicsShapeModelProxy {

/*********************************************************************************************
	Lifecycle
 *********************************************************************************************/

	public:

		PhysicsShapeModelProxy(PhysicsShape* shape);
		virtual ~PhysicsShapeModelProxy() = 0;

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


#endif //AVARA_ENGINE_PHYSICSSHAPEMODELPROXY_H
