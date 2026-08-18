//
//  PlanePhysicsShape.h
//  avara3d
//
//  Created by Morgan Davis on 11/19/23.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_PHYSICS_SHAPE_PRIMITIVE_PLANEPHYSICSSHAPE_H
#define AVARA3D_PHYSICS_SHAPE_PRIMITIVE_PLANEPHYSICSSHAPE_H

#include "a3d/physics/shape/PhysicsShape.h"

namespace a3d {

    class PlanePhysicsShape : public PhysicsShape {

    public:
        /// Public Lifecycle Functions ///

        PlanePhysicsShape(float width, float height);

        /// Public Member Functions ///

        float width() const;
        void  width(float width);

        float height() const;
        void  height(float height);

        /// PhysicsShape Public Member Functions ///

        Type  type() const override;
        void  type(Type type) override;

    private:
        /// Private Member Variables ///

        float _width;
        float _height;
    };

}

#endif //AVARA3D_PHYSICS_SHAPE_PRIMITIVE_PLANEPHYSICSSHAPE_H
