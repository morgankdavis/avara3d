//
// Created by mkd on 12/23/25.
//

#ifndef AVARA3D_PIPELINEKEY_H
#define AVARA3D_PIPELINEKEY_H

#include <cstdint>
#include <cstddef> // size_t

#include "a3d/Types.h"

namespace a3d {

	using PipelineHandle = uint32_t;
	static constexpr PipelineHandle INVALID_PIPELINE = 0xFFFFFFFFu;

	enum class DepthFunc : uint8_t {
		Less, Lequal, Equal, Greater, Gequal, Notequal, Always, Never
	};

	enum class ShaderKind : uint8_t {
		Default,
//		Skybox,
		Wireframe,
		Lines
	};

	enum class PassKind : uint8_t {
		MainOpaque,
		MainMask,
		MainTransparent,
		Wireframe,
		Lines
	};

	struct PipelineKey {
		ShaderKind 		shaderKind = 		ShaderKind::Default;
		uint32_t 		vertexLayoutKey = 	0;
		FillMode 		fillMode = 			FillMode::Fill;
		BlendFunction 	blendFunction = 	BlendFunction::Disabled;
		bool 			doubleSided = 		false;

		PassKind 		pass = 				PassKind::MainOpaque;

		// these two are hugely useful for wire overlay correctness
		bool 			depthTest = 		true;
		bool 			depthWrite = 		true; // Main = true, Wire = false
		DepthFunc 		depthFunc = 		DepthFunc::Less;
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
			mix((uint32_t) (k.doubleSided ? 1u : 0u));

			mix((uint32_t) k.pass);

			// ✅ these were missing before:
			mix((uint32_t) (k.depthTest ? 1u : 0u));
			mix((uint32_t) (k.depthWrite ? 1u : 0u));
			mix((uint32_t) k.depthFunc);

			mix((uint32_t) (k.polygonOffset ? 1u : 0u));

			return h;
		}
	};

} // namespace a3d

#endif //AVARA3D_PIPELINEKEY_H
