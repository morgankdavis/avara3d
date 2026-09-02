//
//  ConvexDecomposer.h
//  avara3d
//
//  Created by Morgan Davis on 11/5/23.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_MESH_CONVEXDECOMPOSER_H
#define AVARA3D_MESH_CONVEXDECOMPOSER_H

#include <cstdint>
#include <functional>
#include <memory>
#include <vector>

class IVHACD;

namespace a3d {
class MeshElement;

class ConvexDecomposer {

public:
    // [Internal Types]

    enum class FILL_MODE {
        FLOOD_FILL,
        SURFACE_ONLY,
        RAYCAST_FILL
    };

    struct Options {
        uint32_t  maxConvexHulls {64};
        uint32_t  resolution {400000};
        double    minVolumePercentErr {1};
        uint32_t  maxRecursionDepth {10};
        bool      shrinkWrap {true};
        FILL_MODE fillMode {FILL_MODE::FLOOD_FILL};
        uint32_t  maxNumVerticesPerHull {64};
//			bool		asyncACD 				{true};
        uint32_t  minEdgeLength {2};
        bool      findBestPlane {false};
    };

    // [Internal Lifecycle Functions]

    ConvexDecomposer(MeshElement& element, Options& options);

    // [Internal Member Functions]

    std::vector<std::unique_ptr<MeshElement>> decompose();

private:
    // [Private Member Variables]

    Options      _options;
    MeshElement* _sourceElement;
};
} // namespace a3d

#endif // AVARA3D_MESH_CONVEXDECOMPOSER_H
