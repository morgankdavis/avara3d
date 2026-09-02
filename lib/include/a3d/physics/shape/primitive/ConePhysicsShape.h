//
//  ConePhysicsShape.h
//  avara3d
//
//  Created by Morgan Davis on 11/19/23.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_PHYSICS_SHAPE_PRIMITIVE_CONEPHYSICSSHAPE_H
#define AVARA3D_PHYSICS_SHAPE_PRIMITIVE_CONEPHYSICSSHAPE_H

#include "a3d/physics/shape/PhysicsShape.h"

namespace a3d {
/** @brief Cone collision shape centered at the local origin and aligned along Y. */
class ConePhysicsShape : public PhysicsShape {

public:
    // [Public Lifecycle Functions]

    /** @brief Creates a cone collision shape with the supplied base radius and full height. */
    ConePhysicsShape(float radius, float height);

    // [Public Member Functions]

    /** @brief Returns the configured circular base radius. */
    float radius() const;

    /** @brief Returns the configured full height along Y. */
    float height() const;

    // [Public PhysicsShape Member Functions]

    /** @brief Returns PhysicsShape::Type::Primitive. */
    Type  type() const override;

    /**
     * @brief Rejects attempts to change the fixed primitive shape type.
     *
     * @throws std::logic_error always; ConePhysicsShape has a fixed type.
     */
    void  type(Type type) override;

private:
    // [Private Member Variables]

    float _radius;
    float _height;
};
} // namespace a3d

#endif // AVARA3D_PHYSICS_SHAPE_PRIMITIVE_CONEPHYSICSSHAPE_H
