//
//  PacketSorter.cc
//  avara3d
//
//  Created by Morgan Davis on 1/4/26.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#include "a3d/render/PacketSorter.h"

#include <algorithm>
#include <type_traits>

#include "a3d/render/DrawPacket.h"
#include "a3d/render/PipelineDesc.h"

using namespace a3d;
using namespace std;

// [Private Non-Member Prototypes]

static uint32_t PtrHash32(const void* p);
static uint16_t PtrHash16(const void* p);
static uint32_t FoldHash32(size_t h);
static void     SortItems(vector<DrawItem>& items);

namespace a3d {

// [Internal Static Member Functions]

uint64_t PacketSorter::MakeBatchKey(const DrawItem& item) {

    const uint32_t descHash = FoldHash32(PipelineDescHash {}(item.desc));

    uint64_t batchKey = (uint64_t) descHash << 32;
    batchKey |= (uint64_t) PtrHash16(item.material) << 16;
    batchKey |= (uint64_t) PtrHash16(item.element);

    return batchKey;
}

void PacketSorter::SortPacket(DrawPacket& packet) {

    for (auto& item : packet.mainPassItems) {
        item.batchKey = MakeBatchKey(item);
    }
    for (auto& item : packet.wireframePassItems) {
        item.batchKey = MakeBatchKey(item);
    }

    SortItems(packet.mainPassItems);
    SortItems(packet.wireframePassItems);
}

} // namespace a3d

// [Private Non-Member Functions]

uint32_t PtrHash32(const void* p) {
    uintptr_t v = reinterpret_cast<uintptr_t>(p) >> 4; // drop alignment bits

    uint32_t lo = static_cast<uint32_t>(v);
    uint32_t hi = 0;

    if constexpr (sizeof(uintptr_t) > 4) {
        hi = static_cast<uint32_t>(v >> 32);
    }

    uint32_t h = lo ^ hi;
    h ^= (h >> 16);
    return h;
}

uint16_t PtrHash16(const void* p) {
    uint32_t h = PtrHash32(p);
    return (uint16_t) (h ^ (h >> 16));
}

uint32_t FoldHash32(size_t h) {
    uint32_t x = static_cast<uint32_t>(h);

    if constexpr (sizeof(size_t) > 4) {
        x ^= static_cast<uint32_t>(h >> 32);
    }

    // cheap mix (avalanche-ish)
    x ^= x >> 16;
    x *= 0x7feb352d;
    x ^= x >> 15;
    x *= 0x846ca68b;
    x ^= x >> 16;
    return x;
}

void SortItems(vector<DrawItem>& items) {

    std::sort(items.begin(), items.end(), [](const DrawItem& a, const DrawItem& b) {
        if (a.renderOrder != b.renderOrder) {
            return a.renderOrder < b.renderOrder;
        }

        const auto pa = static_cast<underlying_type_t<PassKind>>(a.pass);
        const auto pb = static_cast<underlying_type_t<PassKind>>(b.pass);

        if (pa != pb) {
            return pa < pb;
        }

        if (a.batchKey != b.batchKey) {
            return a.batchKey < b.batchKey;
        }

        return a.sequence < b.sequence;
    });
}
