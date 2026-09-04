//
//  IndexAccess.h
//  avara3d
//
//  Created by Morgan Davis on 1/6/2026.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_MESH_INDEXACCESS_H
#define AVARA3D_MESH_INDEXACCESS_H

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <optional>
#include <span>
#include <type_traits>
#include <vector>

#include "a3d/Assert.h"
#include "a3d/mesh/IndexFormats.h"
#include "a3d/mesh/MeshElement.h"
#include "a3d/mesh/PrimitiveTopology.h"

namespace a3d {

struct IndexStreamView {
    const std::byte* base   = nullptr;
    uint32_t         count  = 0; // number of indices
    IndexFormat      format = IndexFormat::None;
};

struct IndexAccess {

    static std::optional<IndexStreamView> GetIndexStreamView(const MeshElement& element);
    static std::optional<IndexStreamView> GetIndexStreamView(const MeshElement& element,
                                                             IndexFormat        expectedFormat);

    // read as u32
    static uint32_t                       ReadIndexU32(const IndexStreamView& v, uint32_t i);

    // convenience: expand to U32 vector (useful for libs like VHACD, meshopt, etc.)
    static void                           ExpandToU32(const IndexStreamView& v, std::vector<uint32_t>& out);

    // expand indices to U32. if mesh is non-indexed, generate 0..vertexCount-1
    // assumes triangles - vertexCount must be multiple of 3 for non-indexed
    static void GetTrianglesU32(const MeshElement& element, std::vector<uint32_t>& out);

    // triangle iteration - assumes triangles, 3 indices per face
    template<class F>
    static void ForEachTriangle(const MeshElement& element, F&& fn) {

        A3D_ASSERT(element.topology() == PrimitiveTopology::Triangles);
        if (element.topology() != PrimitiveTopology::Triangles) {
            return;
        }

        auto vOpt = GetIndexStreamView(element);
        if (vOpt) {
            const auto v = *vOpt;

            A3D_ASSERT((v.count % 3u) == 0u);
            if ((v.count % 3u) != 0u) {
                return; // release safety
            }

            for (uint32_t i = 0; (i + 2u) < v.count; i += 3u) {
                const uint32_t a = ReadIndexU32(v, i + 0u);
                const uint32_t b = ReadIndexU32(v, i + 1u);
                const uint32_t c = ReadIndexU32(v, i + 2u);
                fn(a, b, c);
            }
            return;
        }

        // non-indexed fallback: triangles are implicit (0,1,2), (3,4,5), ...
        const uint32_t vcount = element.vertexCount();

        A3D_ASSERT((vcount % 3u) == 0u);
        if ((vcount % 3u) != 0u) {
            return;
        }

        for (uint32_t i = 0; (i + 2u) < vcount; i += 3u) {
            fn(i + 0u, i + 1u, i + 2u);
        }
    }
};

} // namespace a3d

#endif // AVARA3D_MESH_INDEXACCESS_H
