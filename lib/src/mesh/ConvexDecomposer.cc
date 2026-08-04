//
//  ConvexDecomposer.cc
//  avara3d
//
//  Created by Morgan Davis on 11/5/23.
//  Copyright © 2024 Morgan K Davis. All rights reserved.
//

#include "a3d/mesh/ConvexDecomposer.h"

#include <cstring>
#include <span>

#include <magic_enum/magic_enum.hpp>
#include <v-hacd/VHACD.h>

#include "a3d/mesh/IndexAccess.h"
#include "a3d/mesh/MeshElement.h"
#include "a3d/mesh/PrimitiveTopology.h"
#include "a3d/mesh/VertexAccess.h"
#include "a3d/mesh/VertexFormats.h"
#include "a3d/mesh/VertexLayout.h"
#include "a3d/log/Log.h"

using namespace a3d;
using namespace std;
using namespace VHACD;

/// Internal Lifecycle Functions ///

ConvexDecomposer::ConvexDecomposer(MeshElement& element, Options& options):
    _sourceElement {&element},
    _options {options} {}

/// Internal Member Functions ///

vector<unique_ptr<MeshElement>> ConvexDecomposer::decompose() {

    log::d()("Performing convex decomposition...");

    VHACD::IVHACD*            vhacd = CreateVHACD();

    int                       a3dFillModeUnderlying = magic_enum::enum_integer(_options.fillMode);
    VHACD::FillMode           vhacdFillMode = magic_enum::enum_value<VHACD::FillMode>(a3dFillModeUnderlying);

    VHACD::IVHACD::Parameters params = {nullptr,
                                        nullptr,
                                        nullptr,
                                        _options.maxConvexHulls,
                                        _options.resolution,
                                        _options.minVolumePercentErr,
                                        _options.maxRecursionDepth,
                                        _options.shrinkWrap,
                                        vhacdFillMode,
                                        _options.maxNumVerticesPerHull,
                                        false, // asyncACD
                                        _options.minEdgeLength,
                                        _options.findBestPlane};

    auto                      posOpt = VertexAccess::GetPositionStreamView(*_sourceElement);
    if (!posOpt) {
        vhacd->Release();
        return {};
    }
    const VertexStreamView pos = *posOpt;

    if (pos.count == 0 || !pos.base) {
        vhacd->Release();
        return {};
    }

    vector<float> verts;
    verts.resize(3u * pos.count);

    for (uint32_t i = 0; i < pos.count; ++i) {
        const std::byte* p = VertexBaseAt(pos, i) + pos.offset;

        float            xyz[3];
        memcpy(xyz, p, sizeof(xyz));

        verts[3u * i + 0] = xyz[0];
        verts[3u * i + 1] = xyz[1];
        verts[3u * i + 2] = xyz[2];
    }

    vector<uint32_t> indicesU32;

    // prefer real index buffer if present
    if (auto ivOpt = IndexAccess::GetIndexStreamView(*_sourceElement)) {
        const IndexStreamView iv = *ivOpt;

        // only triangles supported
        A3D_ASSERT(_sourceElement->topology() == PrimitiveTopology::Triangles);
        A3D_ASSERT((iv.count % 3u) == 0u);

        IndexAccess::ExpandToU32(iv, indicesU32);
    }
    else {
        // non-indexed fallback: treat vertices as already a triangle list
        A3D_ASSERT(_sourceElement->topology() == PrimitiveTopology::Triangles);
        A3D_ASSERT((pos.count % 3u) == 0u);

        indicesU32.resize(pos.count);
        for (uint32_t i = 0; i < pos.count; ++i) {
            indicesU32[i] = i;
        }
    }

    if (indicesU32.empty() || (indicesU32.size() % 3u) != 0u) {
        log::e()("Invalid index buffer. count: {}", (uint32_t) indicesU32.size());
        vhacd->Release();
        return {};
    }

#ifdef A3D_DEBUG
    for (uint32_t idx : indicesU32) {
        A3D_ASSERT(idx < pos.count);
    }
#endif

    const uint32_t numPoints = pos.count;
    const uint32_t numTris   = (uint32_t) (indicesU32.size() / 3u);

    vhacd->Compute(verts.data(), numPoints, indicesU32.data(), numTris, params);

    const int                       numHulls = (int) vhacd->GetNConvexHulls();

    vector<unique_ptr<MeshElement>> out;
    out.reserve(numHulls);

    for (int h = 0; h < numHulls; ++h) {
        VHACD::IVHACD::ConvexHull hull;
        vhacd->GetConvexHull(h, hull);

        // vertices
        vector<VertexPNT> decomposedVerts;
        decomposedVerts.reserve(hull.m_points.size());
        for (auto& v : hull.m_points) {
            decomposedVerts.push_back({{(float) v.mX, (float) v.mY, (float) v.mZ}, {}, {}});
        }

        // indices
        vector<uint32_t> decomposedIndices;
        decomposedIndices.reserve(hull.m_triangles.size() * 3u);
        for (auto& t : hull.m_triangles) {
            decomposedIndices.push_back((uint32_t) t.mI0);
            decomposedIndices.push_back((uint32_t) t.mI1);
            decomposedIndices.push_back((uint32_t) t.mI2);
        }

#ifdef A3D_DEBUG
        for (uint32_t idx : decomposedIndices) {
            A3D_ASSERT(idx < (uint32_t) decomposedVerts.size());
        }
#endif

        auto vbSpan  = std::span<const VertexPNT>(decomposedVerts.data(), decomposedVerts.size());
        auto vbBytes = std::as_bytes(vbSpan);

        auto ibSpan  = std::span<const uint32_t>(decomposedIndices.data(), decomposedIndices.size());
        auto ibBytes = std::as_bytes(ibSpan);

        out.push_back(std::make_unique<MeshElement>(VertexLayout::PNT, vbBytes,
                                                    (uint32_t) decomposedVerts.size(),
                                                    (uint16_t) sizeof(VertexPNT), PrimitiveTopology::Triangles,
                                                    IndexFormat::U32, ibBytes,
                                                    (uint32_t) decomposedIndices.size()));
    }

    log::d()("Decomposition done. numHulls: {}", numHulls);

    vhacd->Release();

    return out;
}
