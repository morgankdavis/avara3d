//
//  PrimitiveTopology.h
//  avara3d
//
//  Created by Morgan Davis on 1/7/2026.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_MESH_PRIMITIVETOPOLOGY_H
#define AVARA3D_MESH_PRIMITIVETOPOLOGY_H

#include <cstdint>

namespace a3d {
enum class PrimitiveTopology : uint8_t {
    Triangles,
    Lines,
    Points,
};
} // namespace a3d

#endif // AVARA3D_MESH_PRIMITIVETOPOLOGY_H
