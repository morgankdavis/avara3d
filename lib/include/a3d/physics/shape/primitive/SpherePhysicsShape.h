//
//  SpherePhysicsShape.h
//  avara3d
//
//  Created by Morgan Davis on 11/19/23.
//  Copyright © 2023-2026 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_PHYSICS_SHAPE_PRIMITIVE_SPHEREPHYSICSSHAPE_H
#define AVARA3D_PHYSICS_SHAPE_PRIMITIVE_SPHEREPHYSICSSHAPE_H

#include "a3d/physics/shape/PhysicsShape.h"

namespace a3d {

    class SpherePhysicsShape : public PhysicsShape {

    public:
        /// Public Lifecycle Functions ///

        explicit SpherePhysicsShape(float radius);

        /// Public Member Functions ///

        float radius() const;
        void  radius(float radius);

        /// PhysicsShape Public Member Functions ///

        Type  type() const override;
        void  type(Type type) override;

    private:
        /// Private Member Variables ///

        float _radius;
    };

}

#endif //AVARA3D_PHYSICS_SHAPE_PRIMITIVE_SPHEREPHYSICSSHAPE_H
