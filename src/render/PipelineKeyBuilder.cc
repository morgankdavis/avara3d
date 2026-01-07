//
//  PipelineKeyBuilder.cc
//  avara3d
//
//  Created by Morgan Davis on 1/4/26.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#include "a3d/render/PipelineKeyBuilder.h"

#include "a3d/render/GatherOutput.h"
#include "a3d/visual/material/Material.h"

using namespace a3d;
using namespace std;

/// Private Static Non-Member Prototypes ///

static PipelineKey 	MakeBaseKey(const Material& material, VertexLayout layoutKey);
static void 		ApplyDepthPolicy(PipelineKey& key);

/// Internal Static Member Functions ///

PipelineKey PipelineKeyBuilder::MakeBackgroundKey() {
	PipelineKey k{};
	k.pass = 			PassKind::Background;
	k.shaderKind =		ShaderKind::Skybox;
	k.vertexLayoutKey = VertexLayout::PNT;
	k.fillMode =		Material::FillMode::Fill;
	k.doubleSided =		true; // change winding order instead??
	k.depthTest =		true;
	k.depthWrite =		false;
	k.depthFunc =		DepthFunc::Lequal;
	k.blendFunction =	Material::BlendFunction::Disabled;
	k.polygonOffset = 	false;
	return k;
}

PipelineKey PipelineKeyBuilder::MakeOpaqueKey(const Material& material, VertexLayout layoutKey) {
	PipelineKey k = MakeBaseKey(material, layoutKey);
	k.pass = 			PassKind::MainOpaque;
	k.depthWrite = 		true;
	k.polygonOffset =	false;
	ApplyDepthPolicy(k);
	return k;
}

PipelineKey PipelineKeyBuilder::MakeMaskKey(const Material& material, VertexLayout layoutKey) {
	PipelineKey k = MakeBaseKey(material, layoutKey);
	k.pass = 			PassKind::MainMask;
	k.depthWrite = 		true;
	k.polygonOffset = 	false;
	ApplyDepthPolicy(k);
	return k;
}

PipelineKey PipelineKeyBuilder::MakeTransparentKey(const Material& material, VertexLayout layoutKey) {
	PipelineKey k = MakeBaseKey(material, layoutKey);
	k.pass = 			PassKind::MainTransparent;
	k.depthWrite = 		false; // critical for blending correctness
	k.polygonOffset = 	false;
	ApplyDepthPolicy(k);
	return k;
}

PipelineKey PipelineKeyBuilder::MakeWireframeKey(VertexLayout layoutKey) {
	PipelineKey k{};
	k.pass = 			PassKind::Wireframe;
	k.shaderKind = 		ShaderKind::Wireframe;
	k.vertexLayoutKey = layoutKey;
	k.fillMode = 		Material::FillMode::Lines;
	k.doubleSided = 	true;
	k.blendFunction = 	Material::BlendFunction::Disabled;
	k.polygonOffset = 	true;
	ApplyDepthPolicy(k);
	return k;
}

PipelineKey PipelineKeyBuilder::MakeLinesKey() {
	PipelineKey k{};
	k.pass = 			PassKind::Lines;
	k.shaderKind = 		ShaderKind::Lines;
	k.vertexLayoutKey = VertexLayout::PC;
	k.fillMode = 		Material::FillMode::Lines;
	k.doubleSided = 	true;
	k.depthWrite = 		false;
	k.blendFunction = 	Material::BlendFunction::Disabled;
	k.polygonOffset = 	false;
	ApplyDepthPolicy(k);
	return k;
}

/// Private Static Non-Member Functions ///

PipelineKey MakeBaseKey(const Material& material, VertexLayout layoutKey) {
	PipelineKey k{};
	k.vertexLayoutKey = layoutKey;
	k.fillMode = 		material.fillMode();
	k.blendFunction = 	material.blendFunction();
	k.doubleSided = 	material.doubleSided();
	k.shaderKind = 		ShaderKind::Default;

	switch (material.alphaMode()) {
		case Material::AlphaMode::Opaque:
		case Material::AlphaMode::Mask:
			k.blendFunction = Material::BlendFunction::Disabled;
			break;
		case Material::AlphaMode::Blend:
			if (k.blendFunction == Material::BlendFunction::Disabled) {
				k.blendFunction = Material::BlendFunction::Alpha;
			}
			break;
	}
	return k;
}

void ApplyDepthPolicy(PipelineKey& key) {
	// defaults (main pass)
	key.depthTest = 	true;
	key.depthWrite = 	true;
	key.depthFunc = 	DepthFunc::Less;

	// overlays
	if (key.pass == PassKind::Lines || key.pass == PassKind::Wireframe) {
		key.depthTest = 	true;
		key.depthWrite = 	false;
		key.depthFunc = 	DepthFunc::Lequal;
	}
}
