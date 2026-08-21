//
//  SpherePhysicsShape.h
//  avara3d
//
//  Created by Morgan Davis on 11/19/23.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_PHYSICS_SHAPE_PRIMITIVE_SPHEREPHYSICSSHAPE_H
#define AVARA3D_PHYSICS_SHAPE_PRIMITIVE_SPHEREPHYSICSSHAPE_H

#include "a3d/physics/shape/PhysicsShape.h"

namespace a3d {

    /** @brief Spherical collision shape centered at the local origin. */
    class SpherePhysicsShape : public PhysicsShape {

    public:
        // [Public Lifecycle Functions]

        /** @brief Creates a spherical collision shape with @p radius. */
        explicit SpherePhysicsShape(float radius);

        // [Public Member Functions]

        /** @brief Returns the configured sphere radius. */
        float radius() const;

        // [Public PhysicsShape Member Functions]

        /** @brief Returns PhysicsShape::Type::Primitive. */
        Type  type() const override;

        /**
         * @brief Rejects attempts to change the fixed primitive shape type.
         *
         * @throws std::logic_error always; SpherePhysicsShape has a fixed type.
         */
        void  type(Type type) override;

    private:
        // [Private Member Variables]

        float _radius;
    };

}

#endif //AVARA3D_PHYSICS_SHAPE_PRIMITIVE_SPHEREPHYSICSSHAPE_H
