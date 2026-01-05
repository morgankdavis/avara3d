//
//  PipelineKey.cc
//  avara3d
//
//  Created by Morgan Davis on 12/23/25.
//  Copyright © 2025 Morgan K Davis. All rights reserved.
//

#include "a3d/render/PipelineKey.h"

using namespace a3d;

/// Internal Functions ///

size_t PipelineKeyHash::operator()(const PipelineKey &k) const noexcept {

	auto h = size_t{1469598103934665603ull}; // Fowler–Noll–Vo-ish

	auto mix = [&](uint64_t v) {
		// "golden ratio" from Boost hash_combine
		h ^= (size_t) v + 0x9e3779b97f4a7c15ull + (h << 6) + (h >> 2);
	};

	mix((uint32_t) k.pass);
	mix((uint32_t) k.vertexLayoutKey);
	mix((uint32_t) k.shaderKind);
	mix((uint32_t) k.fillMode);
	mix((uint32_t) k.blendFunction);
	mix((uint32_t) (k.doubleSided ? 1u : 0u));
	mix((uint32_t) (k.depthTest ? 1u : 0u));
	mix((uint32_t) (k.depthWrite ? 1u : 0u));
	mix((uint32_t) k.depthFunc);
	mix((uint32_t) (k.polygonOffset ? 1u : 0u));

	return h;
}
