//
//  BoxPhysicsShape.h
//  avara3d
//
//  Created by Morgan Davis on 11/19/23.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_PHYSICS_SHAPE_PRIMITIVE_BOXPHYSICSSHAPE_H
#define AVARA3D_PHYSICS_SHAPE_PRIMITIVE_BOXPHYSICSSHAPE_H

#include "a3d/physics/shape/PhysicsShape.h"

namespace a3d {

    /** @brief Box collision shape centered at the local origin, with width along X, height along Y, and length along Z. */
    class BoxPhysicsShape : public PhysicsShape {

    public:
        // [Public Lifecycle Functions]

        /** @brief Creates a box collision shape with the supplied full dimensions. */
        BoxPhysicsShape(float width, float height, float length);

        // [Public Member Functions]

        /** @brief Returns the configured full width along X. */
        float width() const;

        /** @brief Sets the configured full width along X. */
        void  width(float width);

        /** @brief Returns the configured full height along Y. */
        float height() const;

        /** @brief Sets the configured full height along Y. */
        void  height(float height);

        /** @brief Returns the configured full length along Z. */
        float length() const;

        /** @brief Sets the configured full length along Z. */
        void  length(float length);

        // [Public PhysicsShape Member Functions]

        /** @brief Returns PhysicsShape::Type::Primitive. */
        Type  type() const override;

        /**
         * @brief Rejects attempts to change the fixed primitive shape type.
         *
         * @throws std::logic_error always; BoxPhysicsShape has a fixed type.
         */
        void  type(Type type) override;

    private:
        // [Private Member Variables]

        float _width;
        float _height;
        float _length;
    };

}

#endif //AVARA3D_PHYSICS_SHAPE_PRIMITIVE_BOXPHYSICSSHAPE_H
