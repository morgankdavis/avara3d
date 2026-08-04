//
//  FrameStats.h
//  avara3d
//
//  Created by Morgan Davis on 1/6/26.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_PROFILE_FRAMESTATS_H
#define AVARA3D_PROFILE_FRAMESTATS_H

#include <chrono>
#include <cstdint>

namespace a3d {

    struct FrameStats {

        bool                     isRenderGpuTimeAvailable = false;

        std::chrono::nanoseconds frameTime {};
        std::chrono::nanoseconds engineCpuTime {};
        std::chrono::nanoseconds renderCpuTime {};
        std::chrono::nanoseconds renderGpuTime {};
        std::chrono::nanoseconds physicsTime {};
        std::chrono::nanoseconds applicationTime {};

        // simulation steps completed during this Runner update
        std::uint64_t            simulationStepCount {0};

        // complete fixed-step simulation debt discarded after bounded catch-up
        double                   discardedSimulationTime {0.0};

        unsigned                 nodes                   = 0;
        unsigned                 meshes                  = 0;
        unsigned                 elements                = 0;
        unsigned                 polygons                = 0;
        unsigned                 lights                  = 0;
        unsigned                 staticBodies            = 0;
        unsigned                 dynamicBodies           = 0;
        unsigned                 kinematicBodies         = 0;
        unsigned                 primitiveShapes         = 0;
        unsigned                 boundingBoxShapes       = 0;
        unsigned                 convexHullShapes        = 0;
        unsigned                 concavePolyhedronShapes = 0;
    };

}

#endif //AVARA3D_PROFILE_FRAMESTATS_H
