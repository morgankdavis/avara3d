//
//  IndexAccess.cc
//  avara3d
//
//  Created by Morgan Davis on 1/6/2026.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#include "a3d/mesh/IndexAccess.h"

#include "a3d/mesh/MeshElement.h"

using namespace std;

namespace a3d {
optional<IndexStreamView> IndexAccess::GetIndexStreamView(const MeshElement& element) {
    const auto        ib = element.indexBytes();
    const uint32_t    count = element.indexCount();
    const IndexFormat fmt = element.indexFormat();

    if (count == 0 || ib.empty() || fmt == IndexFormat::None) {
        return std::nullopt;
    }

    return IndexStreamView {ib.data(), count, fmt};
}

optional<IndexStreamView> IndexAccess::GetIndexStreamView(const MeshElement& element,
                                                          IndexFormat        expectedFormat) {

    auto vOpt = GetIndexStreamView(element);
    if (!vOpt) {
        return std::nullopt;
    }
    if (vOpt->format != expectedFormat) {
        return std::nullopt;
    }

    return *vOpt;
}

uint32_t IndexAccess::ReadIndexU32(const IndexStreamView& v, uint32_t i) {

    if (!v.base) {
        return 0;
    }
    if (i >= v.count) {
        return 0;
    }

    // format/stride consistency checks
    if (v.format == IndexFormat::U16) {
        if (IndexStride(v.format) != 2) {
            return 0;
        }
    }
    else if (v.format == IndexFormat::U32) {
        if (IndexStride(v.format) != 4) {
            return 0;
        }
    }
    else {
        return 0;
    }

    const std::byte* p = v.base + size_t(i) * size_t(IndexStride(v.format));

    if (v.format == IndexFormat::U16) {
        uint16_t x = 0;
        memcpy(&x, p, sizeof(x));
        return (uint32_t) x;
    }

    uint32_t x = 0;
    memcpy(&x, p, sizeof(x));
    return x;
}

void IndexAccess::ExpandToU32(const IndexStreamView& v, std::vector<uint32_t>& out) {
    out.clear();

    if (!v.base || v.count == 0) {
        return;
    }

    const uint16_t expectedStride = IndexStride(v.format);
    if (expectedStride == 0) {
        return;
    }

    // keep the view honest (GetIndexStreamView should already guarantee this)
    A3D_ASSERT(IndexStride(v.format) == expectedStride);
    if (IndexStride(v.format) != expectedStride) {
        return;
    }

    out.reserve(v.count);

    const std::byte* p = v.base;

    if (v.format == IndexFormat::U16) {
        for (uint32_t i = 0; i < v.count; ++i) {
            uint16_t x = 0;
            memcpy(&x, p + size_t(i) * size_t(IndexStride(v.format)), sizeof(x));
            out.push_back(static_cast<uint32_t>(x));
        }
        return;
    }

    if (v.format == IndexFormat::U32) {
        for (uint32_t i = 0; i < v.count; ++i) {
            uint32_t x = 0;
            memcpy(&x, p + size_t(i) * size_t(IndexStride(v.format)), sizeof(x));
            out.push_back(x);
        }
        return;
    }

    // IndexFormat::None or unknown
}

void IndexAccess::GetTrianglesU32(const MeshElement& element, std::vector<uint32_t>& out) {
    out.clear();

    // this helper is triangles-only by definition.
    A3D_ASSERT(element.topology() == PrimitiveTopology::Triangles);
    if (element.topology() != PrimitiveTopology::Triangles) {
        return;
    }

    if (auto ivOpt = GetIndexStreamView(element)) {
        // indexed triangles
        const auto v = *ivOpt;

        A3D_ASSERT((v.count % 3u) == 0u);
        if ((v.count % 3u) != 0u) {
            return;
        }

        ExpandToU32(v, out);
        return;
    }

    // N\non-indexed fallback (drawArrays-style)
    const uint32_t vcount = element.vertexCount();

    A3D_ASSERT((vcount % 3u) == 0u);
    if ((vcount % 3u) != 0u) {
        return;
    }

    out.resize(vcount);
    for (uint32_t i = 0; i < vcount; ++i) {
        out[i] = i;
    }
}
} // namespace a3d
