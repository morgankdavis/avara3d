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

    struct RenderMemoryStats {

        struct Usage {

            std::uint64_t textureBytes {0};
            std::uint64_t vertexBufferBytes {0};
            std::uint64_t indexBufferBytes {0};
            std::uint64_t uniformBufferBytes {0};
            std::uint64_t renderTargetBytes {0};
            std::uint64_t otherBytes {0};

            std::uint64_t totalBytes() const {

                return textureBytes + vertexBufferBytes + indexBufferBytes + uniformBufferBytes
                       + renderTargetBytes + otherBytes;
            }
        };

        Usage         a3d {};
        Usage         imgui {};
        std::uint64_t peakTotalBytes {0};

        Usage         totalUsage() const {

            return {
                .textureBytes       = a3d.textureBytes + imgui.textureBytes,
                .vertexBufferBytes  = a3d.vertexBufferBytes + imgui.vertexBufferBytes,
                .indexBufferBytes   = a3d.indexBufferBytes + imgui.indexBufferBytes,
                .uniformBufferBytes = a3d.uniformBufferBytes + imgui.uniformBufferBytes,
                .renderTargetBytes  = a3d.renderTargetBytes + imgui.renderTargetBytes,
                .otherBytes         = a3d.otherBytes + imgui.otherBytes,
            };
        }

        std::uint64_t totalBytes() const {

            return a3d.totalBytes() + imgui.totalBytes();
        }
    };

    struct FrameStats {

        std::chrono::nanoseconds frameTime {};
        std::chrono::nanoseconds engineCpuTime {};
        std::chrono::nanoseconds renderCpuTime {};
        std::chrono::nanoseconds renderGpuTime {};
        std::chrono::nanoseconds physicsTime {};
        std::chrono::nanoseconds applicationTime {};

        RenderMemoryStats        renderMemory {};

        // fixed simulation configuration
        double                   simulationTimeStep {0.0};
        std::uint32_t            maxCatchUpSteps {0};

        // cumulative completed simulation
        std::uint64_t            simulationStepCount {0};
        double                   simulationTime {0.0};

        // simulation steps completed during this Runner update
        std::uint32_t            simulationStepsThisUpdate {0};

        // complete fixed-step simulation debt discarded after bounded catch-up
        double                   discardedSimulationTime {0.0};
        double                   totalDiscardedSimulationTime {0.0};

        std::uint32_t            nodes {0};
        std::uint32_t            meshes {0};
        std::uint32_t            elements {0};
        std::uint32_t            polygons {0};
        std::uint32_t            lights {0};

        std::uint32_t            staticBodies {0};
        std::uint32_t            dynamicBodies {0};
        std::uint32_t            kinematicBodies {0};

        std::uint32_t            primitiveShapes {0};
        std::uint32_t            boundingBoxShapes {0};
        std::uint32_t            convexHullShapes {0};
        std::uint32_t            concavePolyhedronShapes {0};

        std::uint32_t            activeContacts {0};
    };

}

#endif // AVARA3D_PROFILE_FRAMESTATS_H
