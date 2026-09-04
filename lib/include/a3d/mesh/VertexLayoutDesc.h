//
//  VertexLayoutDesc.h
//  avara3d
//
//  Created by Morgan Davis on 1/4/26.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_MESH_VERTEXLAYOUTDESC_H
#define AVARA3D_MESH_VERTEXLAYOUTDESC_H

#include <cstdint>
#include <span>

#include "a3d/mesh/VertexLayout.h"

namespace a3d {

enum class VertexAttribFormat : uint8_t {
    F32x2,
    F32x3,
    F32x4,
    // UN8x4
    // I16x4N
};

enum class VertexSemantic : uint8_t {
    Position,
    Normal,
    TexCoord0,
    Color0,
};

struct VertexAttribDesc {
    VertexSemantic     semantic   = VertexSemantic::Position;
    uint8_t            location   = 0; // shader location
    VertexAttribFormat format     = VertexAttribFormat::F32x3;
    uint16_t           offset     = 0; // byte offset in vertex
    bool               normalized = false;
};

struct VertexLayoutDesc {
    uint16_t                          stride  = 0;
    std::span<const VertexAttribDesc> attribs = {};
};

const VertexLayoutDesc& GetVertexLayoutDesc(VertexLayout layout);

} // namespace a3d

#endif // AVARA3D_MESH_VERTEXLAYOUTDESC_H
