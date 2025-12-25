//
// Created by mkd on 12/23/25.
//

#ifndef AVARA3D_PIPELINEKEY_H
#define AVARA3D_PIPELINEKEY_H

#include <cstdint>

#include "a3d/Types.h"

namespace a3d {

	enum class ShaderKind : uint8_t {
		Default,
//		Skybox,
		Wireframe,
//		Lines
	};

//	struct DepthState { bool test = true; bool write = true; };
//	struct RasterState { bool doubleSided = false; FillMode fillMode = FillMode::Fill; };
//	struct BlendState { BlendFunction blend = BlendFunction::Disabled; };
//
//	struct PipelineDesc {
//		uint32_t   vertexLayoutKey = 0;
//		ShaderKind shaderKind = ShaderKind::Default;
//		DepthState depth;
//		RasterState raster;
//		BlendState blend;
//
//		bool operator==(const PipelineDesc&) const = default;
//	};

	enum class PassKind : uint8_t {
		Main,
		Wire
	};

	struct PipelineKey {
		ShaderKind 		shaderKind = 		ShaderKind::Default;
		uint32_t 		vertexLayoutKey = 	0;
		FillMode 		fillMode = 			FillMode::Fill;
		BlendFunction 	blendFunction = 	BlendFunction::Disabled;
		bool 			doubleSided = 		false;

		PassKind 		pass = 				PassKind::Main;

		// these two are hugely useful for wire overlay correctness
		bool 			depthWrite = 		true; // Main = tue, Wire = false
		bool 			polygonOffset = 	false; // Main = false, Wire = true

		bool operator==(const PipelineKey &) const = default;
	};

	struct PipelineKeyHash { // for unordered_map
		size_t operator()(const PipelineKey &k) const noexcept {
			auto h = size_t{1469598103934665603ull}; // Fowler–Noll–Vo-ish

			auto mix = [&](uint64_t v) {
				// "golden ratio" from Boost hash_combine
				h ^= (size_t) v + 0x9e3779b97f4a7c15ull + (h << 6) + (h >> 2);
			};

			mix(k.vertexLayoutKey);
			mix((uint32_t) k.shaderKind);
			mix((uint32_t) k.fillMode);
			mix((uint32_t) k.blendFunction);
			mix((uint32_t) k.doubleSided ? 1u : 0u);
			mix((uint32_t) k.pass);
			mix((uint32_t) k.depthWrite ? 1u : 0u);
			mix((uint32_t) k.polygonOffset ? 1u : 0u);

			return h;
		}
	};
}

//	struct PipelineKeyHash {
//		size_t operator()(const PipelineKey& k) const {
//			// hash combine (simple)
//			size_t h = k.vertexLayoutKey;
//			h = h * 1315423911u + (uint32_t)k.doubleSided;
//			h = h * 1315423911u + (uint32_t)k.fillMode;
//			h = h * 1315423911u + (uint32_t)k.blendFunction;
//			h = h * 1315423911u + (uint32_t)k.shaderKind;
//			return h;
//		}
//	};

#endif //AVARA3D_PIPELINEKEY_H
