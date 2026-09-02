//
//  CylinderPhysicsShape.h
//  avara3d
//
//  Created by Morgan Davis on 11/19/23.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_PHYSICS_SHAPE_PRIMITIVE_CYLINDERPHYSICSSHAPE_H
#define AVARA3D_PHYSICS_SHAPE_PRIMITIVE_CYLINDERPHYSICSSHAPE_H

#include "a3d/physics/shape/PhysicsShape.h"

namespace a3d {

    /** @brief Cylinder collision shape centered at the local origin and aligned along Y. */
class CylinderPhysicsShape : public PhysicsShape {

public:
    // [Public Lifecycle Functions]

        /** @brief Creates a cylinder collision shape with the supplied radius and full height. */
    CylinderPhysicsShape(float radius, float height);

    // [Public Member Functions]

        /** @brief Returns the configured cylinder radius. */
    float radius() const;

        /** @brief Returns the configured full height along Y. */
    float height() const;

    // [Public PhysicsShape Member Functions]

        /** @brief Returns PhysicsShape::Type::Primitive. */
    Type  type() const override;

        /**
     * @brief Rejects attempts to change the fixed primitive shape type.
     *
     * @throws std::logic_error always; CylinderPhysicsShape has a fixed type.
     */
    void  type(Type type) override;

private:
    // [Private Member Variables]

    float _radius;
    float _height;
};

}

#endif // AVARA3D_PHYSICS_SHAPE_PRIMITIVE_CYLINDERPHYSICSSHAPE_H
