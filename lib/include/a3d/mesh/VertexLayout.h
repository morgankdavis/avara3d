//
//  VertexLayout.h
//  avara3d
//
//  Created by Morgan Davis on 1/4/26.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_MESH_VERTEXLAYOUT_H
#define AVARA3D_MESH_VERTEXLAYOUT_H

#include <cstdint>

namespace a3d {

// [Internal Types]

using VertexLayoutKey = uint32_t;

enum class VertexLayout : uint32_t {
    None = 0,
    PNT  = 1, // Position/Normal/UV0
    PC   = 2  // Position/Color
    // PNTT - Position/Normal/UV0/Tangent
    // PNTC - Position/Normal/UV0/Color
    // PNT2 - Position/Normal/UV0/UV1
    // SkinnedPNT - joints + weights
    // InstancedPNT - per-instance transform stream
    // MorphPNT - multiple position/normal deltas
};

}

#endif // AVARA3D_MESH_VERTEXLAYOUT_H
