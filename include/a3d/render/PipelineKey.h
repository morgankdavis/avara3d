//
//  PipelineKey.h
//  avara3d
//
//  Created by Morgan Davis on 12/23/25.
//  Copyright © 2025 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_RENDER_PIPELINEKEY_H
#define AVARA3D_RENDER_PIPELINEKEY_H

#include <cstdint>
#include <cstddef>

#include "a3d/Types.h"

namespace a3d {

	using PipelineHandle = uint32_t;
	static constexpr PipelineHandle INVALID_PIPELINE_HANDLE = 0xFFFFFFFFu;

	enum class DepthFunc : uint8_t {
		Less,
		Lequal,
		Equal,
		Greater,
		Gequal,
		Notequal,
		Always,
		Never
	};

	enum class ShaderKind : uint8_t {
		Skybox,
		Default,
		Wireframe,
		Lines
	};

	enum class PassKind : uint8_t {
		Background,
		MainOpaque,
		MainMask,
		MainTransparent,
		Wireframe,
		Lines
	};

	struct PipelineKey {

		PassKind 		pass = 				PassKind::MainOpaque;
		ShaderKind 		shaderKind = 		ShaderKind::Default;
		uint32_t 		vertexLayoutKey = 	0;
		FillMode 		fillMode = 			FillMode::Fill;
		BlendFunction 	blendFunction = 	BlendFunction::Disabled;
		bool 			doubleSided = 		false;
		bool 			depthTest = 		true;
		bool 			depthWrite = 		true; // Main = true, Wire = false
		DepthFunc 		depthFunc = 		DepthFunc::Less;
		bool 			polygonOffset = 	false; // Main = false, Wire = true

		bool operator==(const PipelineKey &) const = default;
	};

	struct PipelineKeyHash { // for unordered_map
		size_t operator()(const PipelineKey &k) const noexcept;
	};

}

#endif //AVARA3D_RENDER_PIPELINEKEY_H
