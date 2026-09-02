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

/**
 * @brief Infinite collision plane through the local origin with normal along +Z.
 *
 * InfinitePlanePhysicsShape is compatible only with static PhysicsBody objects.
 */
class InfinitePlanePhysicsShape : public PhysicsShape {

public:
    // [Public Lifecycle Functions]

    /** @brief Creates an infinite collision plane through the local XY plane. */
    InfinitePlanePhysicsShape();

    // [PhysicsShape Public Member Functions]

    /** @brief Returns PhysicsShape::Type::Primitive. */
    Type type() const override;

    /**
     * @brief Rejects attempts to change the fixed primitive shape type.
     *
     * @throws std::logic_error always; InfinitePlanePhysicsShape has a fixed type.
     */
    void type(Type type) override;

    // [PhysicsShape Internal Member Functions]

    bool supportsBodyType(PhysicsBody::Type type) const override;
    bool supportsMargin() const override;
};

} // namespace a3d

#endif // AVARA3D_PHYSICS_SHAPE_PRIMITIVE_INFINITEPLANEPHYSICSSHAPE_H
