//
//  FinitePlanePhysicsShape.h
//  avara3d
//
//  Created by Morgan Davis on 11/19/23.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_PHYSICS_SHAPE_PRIMITIVE_FINITEPLANEPHYSICSSHAPE_H
#define AVARA3D_PHYSICS_SHAPE_PRIMITIVE_FINITEPLANEPHYSICSSHAPE_H

#include "a3d/physics/shape/PhysicsShape.h"

namespace a3d {

/** @brief Finite rectangular collision plane centered at the local origin in the XY plane. */
class FinitePlanePhysicsShape : public PhysicsShape {

public:
    // [Public Lifecycle Functions]

    /** @brief Creates a finite collision plane with full width along X and full height along Y. */
    FinitePlanePhysicsShape(float width, float height);

    // [Public Member Functions]

    /** @brief Returns the configured full width along X. */
    float width() const;

    /** @brief Returns the configured full height along Y. */
    float height() const;

    // [Public PhysicsShape Member Functions]

    /** @brief Returns PhysicsShape::Type::Primitive. */
    Type  type() const override;

    /**
     * @brief Rejects attempts to change the fixed primitive shape type.
     *
     * @throws std::logic_error always; FinitePlanePhysicsShape has a fixed type.
     */
    void  type(Type type) override;

private:
    // [Private Member Variables]

    float _width;
    float _height;
};

} // namespace a3d

#endif // AVARA3D_PHYSICS_SHAPE_PRIMITIVE_FINITEPLANEPHYSICSSHAPE_H
