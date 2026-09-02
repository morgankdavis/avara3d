//
//  PipelineDesc.cc
//  avara3d
//
//  Created by Morgan Davis on 12/23/25.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#include "a3d/render/PipelineDesc.h"

namespace a3d {

// [Internal Functions]

size_t PipelineDescHash::operator()(const PipelineDesc& desc) const noexcept {

    uint64_t h = 1469598103934665603ull; // Fowler–Noll–Vo-ish

    auto mix = [&](uint64_t v) {
        // "golden ratio" from Boost hash_combine
        h ^= (uint64_t) v + 0x9e3779b97f4a7c15ull + (h << 6) + (h >> 2);
    };

    mix((uint32_t) desc.passKind);
    mix((uint32_t) desc.vertexLayoutKey);
    mix((uint32_t) desc.shaderKind);
    mix((uint32_t) desc.fillMode);
    mix((uint32_t) desc.blendFunction);
    mix((uint32_t) (desc.doubleSided ? 1u : 0u));
    mix((uint32_t) (desc.depthTest ? 1u : 0u));
    mix((uint32_t) (desc.depthWrite ? 1u : 0u));
    mix((uint32_t) desc.depthFunc);
    mix((uint32_t) (desc.polygonOffset ? 1u : 0u));

    return static_cast<size_t>(h);
}

} // namespace a3d
