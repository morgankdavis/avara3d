//
//  InfinitePlanePhysicsShape.h
//  avara3d
//
//  Created by Morgan Davis on 8/8/26.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_PHYSICS_SHAPE_PRIMITIVE_INFINITEPLANEPHYSICSSHAPE_H
#define AVARA3D_PHYSICS_SHAPE_PRIMITIVE_INFINITEPLANEPHYSICSSHAPE_H

#include "a3d/physics/shape/PhysicsShape.h"

namespace a3d {

    class InfinitePlanePhysicsShape : public PhysicsShape {

    public:
        /// Public Lifecycle Functions ///

        InfinitePlanePhysicsShape();

        /// PhysicsShape Public Member Functions ///

        Type type() const override;
        void type(Type type) override;

        /// PhysicsShape Internal Member Functions ///

        bool supportsBodyType(PhysicsBody::Type type) const override;
    };

}

#endif //AVARA3D_PHYSICS_SHAPE_PRIMITIVE_INFINITEPLANEPHYSICSSHAPE_H
