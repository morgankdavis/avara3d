//
//  BoxPhysicsShape.h
//  avara3d
//
//  Created by Morgan Davis on 11/19/23.
//  Copyright © 2023-2026 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_PHYSICS_SHAPE_PRIMITIVE_BOXPHYSICSSHAPE_H
#define AVARA3D_PHYSICS_SHAPE_PRIMITIVE_BOXPHYSICSSHAPE_H

#include "a3d/physics/shape/PhysicsShape.h"

namespace a3d {

    class BoxPhysicsShape : public PhysicsShape {

    public:
        /// Public Lifecycle Functions ///

        BoxPhysicsShape(float width, float height, float length);

        /// Public Member Functions ///

        float width() const;
        void  width(float width);

        float height() const;
        void  height(float height);

        float length() const;
        void  length(float length);

        /// PhysicsShape Public Member Functions ///

        Type  type() const override;
        void  type(Type type) override;

    private:
        /// Private Member Variables ///

        float _width;
        float _height;
        float _length;
    };

}

#endif //AVARA3D_PHYSICS_SHAPE_PRIMITIVE_BOXPHYSICSSHAPE_H
