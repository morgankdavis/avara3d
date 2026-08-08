//
//  BulletStats.h
//  avara3d
//
//  Created by Morgan Davis on 4/19/2024.
//  Copyright © 2024 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_PHYSICS_BACKEND_BULLET_BULLETSTATS_H
#define AVARA3D_PHYSICS_BACKEND_BULLET_BULLETSTATS_H

#include <memory>
#include <unordered_map>

namespace a3d {

    class PhysicsShape;

    struct BulletStats {

    public:
        /// Public Lifecycle Functions ///

        BulletStats();

        /// Internal Member Variables ///

        unsigned                                    numStaticBodies;
        unsigned                                    numDynamicBodies;
        unsigned                                    numKinematicBodies;
        std::unordered_map<PhysicsShape*, unsigned> primitiveShapes;
        std::unordered_map<PhysicsShape*, unsigned> boundingBoxShapes;
        std::unordered_map<PhysicsShape*, unsigned> convexHullShapes;
        std::unordered_map<PhysicsShape*, unsigned> concavePolyhedronShapes;
    };

}

#endif //AVARA3D_PHYSICS_BACKEND_BULLET_BULLETSTATS_H
