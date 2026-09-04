//
//  CapsulePhysicsShape.h
//  avara3d
//
//  Created by Morgan Davis on 11/19/23.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_PHYSICS_SHAPE_PRIMITIVE_CAPSULEPHYSICSSHAPE_H
#define AVARA3D_PHYSICS_SHAPE_PRIMITIVE_CAPSULEPHYSICSSHAPE_H

#include "a3d/physics/shape/PhysicsShape.h"

namespace a3d {

/**
 * @brief Capsule collision shape centered at the local origin and aligned along Y.
 *
 * Height is the distance between the centers of the hemispherical caps, so the
 * total tip-to-tip extent along Y is height + 2 * radius.
 */
class CapsulePhysicsShape : public PhysicsShape {

public:
    // [Public Lifecycle Functions]

    /** @brief Creates a capsule collision shape with the supplied radius and cylindrical-section height. */
    CapsulePhysicsShape(float radius, float height);

    // [Public Member Functions]

    /** @brief Returns the configured capsule radius. */
    float radius() const;

    /** @brief Returns the configured distance between the centers of the hemispherical caps. */
    float height() const;

    // [Public PhysicsShape Member Functions]

    /** @brief Returns PhysicsShape::Type::Primitive. */
    Type  type() const override;

    /**
     * @brief Rejects attempts to change the fixed primitive shape type.
     *
     * @throws std::logic_error always; CapsulePhysicsShape has a fixed type.
     */
    void  type(Type type) override;

    // [PhysicsShape Internal Member Functions]

    bool  supportsMargin() const override;

private:
    // [Private Member Variables]

    float _radius;
    float _height;
};

} // namespace a3d

#endif // AVARA3D_PHYSICS_SHAPE_PRIMITIVE_CAPSULEPHYSICSSHAPE_H
