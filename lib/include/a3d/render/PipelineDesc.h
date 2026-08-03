//
//  PipelineDesc.h
//  avara3d
//
//  Created by Morgan Davis on 12/23/25.
//  Copyright © 2025 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_RENDER_PIPELINEDESC_H
#define AVARA3D_RENDER_PIPELINEDESC_H

#include <cstdint>
#include <cstddef>

#include "a3d/mesh/VertexLayout.h"
#include "a3d/visual/material/Material.h"

namespace a3d {

	/// Internal Types ///

	using PipelineId = uint32_t;
	static constexpr PipelineId INVALID_PIPELINE_ID = 0xFFFFFFFFu;

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
		// note: this is RENDER ORDER
		Background = 		0,
		MainOpaque = 		1,
		MainMask =			2,
		MainTransparent =	3,
		Wireframe =			4,
		Lines =				5
	};

	struct PipelineDesc {
		// *** MUST update PipelineDescHash() when this struct changes ***
		PassKind 				passKind = 			PassKind::MainOpaque;
		ShaderKind 				shaderKind = 		ShaderKind::Default;
		VertexLayout			vertexLayoutKey = 	VertexLayout::None;
		Material::FillMode 		fillMode = 			Material::FillMode::Fill;
		Material::BlendFunction	blendFunction = 	Material::BlendFunction::Disabled;
		bool 					doubleSided = 		false;
		bool 					depthTest = 		true;
		bool 					depthWrite = 		true; // Main = true, Wire = false
		DepthFunc 				depthFunc = 		DepthFunc::Less;
		bool 					polygonOffset = 	false; // Main = false, Wire = true

		bool operator==(const PipelineDesc&) const = default;
	};

	/// Internal Functions ///

	struct PipelineDescHash { // for unordered_map
		size_t operator()(const PipelineDesc& desc) const noexcept;
	};
}

#endif //AVARA3D_RENDER_PIPELINEDESC_H
