//
//  PhysicsShapeProxy.h
//  avara3d
//
//  Created by Morgan Davis on 11/13/23.
//  Copyright © 2024 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_PHYSICSSHAPEPROXY_H
#define AVARA3D_PHYSICSSHAPEPROXY_H

namespace a3d {

	class PhysicsShape;

	class PhysicsShapeProxy {

	public:
		/// Internal Lifecycle Functions ///

		explicit PhysicsShapeProxy(PhysicsShape& shape);
		virtual ~PhysicsShapeProxy() = 0;
	};
}

#endif //AVARA3D_PHYSICSSHAPEPROXY_H
