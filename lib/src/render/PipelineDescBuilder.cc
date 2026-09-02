//
//  PipelineDescBuilder.cc
//  avara3d
//
//  Created by Morgan Davis on 1/4/26.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#include "a3d/render/PipelineDescBuilder.h"

#include "a3d/render/GatherOutput.h"
#include "a3d/visual/material/Material.h"

using namespace a3d;
using namespace std;

// [Private Non-Member Prototypes]

static PipelineDesc MakeBaseDesc(const Material& material, VertexLayout layout);

namespace a3d {

// [Internal Static Member Functions]

PipelineDesc PipelineDescBuilder::MakeBackgroundDesc() {
    PipelineDesc desc {};
    desc.passKind = PassKind::Background;
    desc.shaderKind = ShaderKind::Skybox;
    desc.vertexLayoutKey = VertexLayout::PNT;
    desc.fillMode = Material::FillMode::Fill;
    desc.doubleSided = true; // change winding order instead??
    desc.depthTest = true;
    desc.depthWrite = false;
    desc.depthFunc = DepthFunc::Lequal;
    desc.blendFunction = Material::BlendFunction::Disabled;
    desc.polygonOffset = false;
    return desc;
}

PipelineDesc PipelineDescBuilder::MakeGroundDesc() {
    PipelineDesc desc {};
    desc.passKind = PassKind::Ground;
    desc.shaderKind = ShaderKind::Ground;
    desc.vertexLayoutKey = VertexLayout::None;
    desc.fillMode = Material::FillMode::Fill;
    desc.doubleSided = true;
    desc.depthTest = true;
    desc.depthWrite = true;
    desc.depthFunc = DepthFunc::Lequal;
    desc.blendFunction = Material::BlendFunction::Disabled;
    desc.polygonOffset = false;
    return desc;
}

PipelineDesc PipelineDescBuilder::MakeOpaqueDesc(const Material& material, VertexLayout layout) {
    PipelineDesc desc = MakeBaseDesc(material, layout);
    desc.passKind = PassKind::MainOpaque;
    desc.polygonOffset = false;
    return desc;
}

PipelineDesc PipelineDescBuilder::MakeMaskDesc(const Material& material, VertexLayout layout) {
    PipelineDesc desc = MakeBaseDesc(material, layout);
    desc.passKind = PassKind::MainMask;
    desc.polygonOffset = false;
    return desc;
}

PipelineDesc PipelineDescBuilder::MakeTransparentDesc(const Material& material, VertexLayout layout) {
    PipelineDesc desc = MakeBaseDesc(material, layout);
    desc.passKind = PassKind::MainTransparent;
    desc.polygonOffset = false;
    return desc;
}

PipelineDesc PipelineDescBuilder::MakeWireframeDesc(VertexLayout layout) {
    PipelineDesc desc {};
    desc.passKind = PassKind::Wireframe;
    desc.shaderKind = ShaderKind::Wireframe;
    desc.vertexLayoutKey = layout;
    desc.fillMode = Material::FillMode::Lines;
    desc.doubleSided = true;
    desc.depthTest = true;
    desc.depthWrite = false;
    desc.depthFunc = DepthFunc::Lequal;
    desc.blendFunction = Material::BlendFunction::Disabled;
    desc.polygonOffset = true;
    return desc;
}

PipelineDesc PipelineDescBuilder::MakeLinesDesc() {
    PipelineDesc desc {};
    desc.passKind = PassKind::Lines;
    desc.shaderKind = ShaderKind::Lines;
    desc.vertexLayoutKey = VertexLayout::PC;
    desc.fillMode = Material::FillMode::Fill;
    desc.doubleSided = true;
    desc.depthTest = true;
    desc.depthWrite = false;
    desc.depthFunc = DepthFunc::Lequal;
    desc.blendFunction = Material::BlendFunction::Disabled;
    desc.polygonOffset = false;
    return desc;
}

} // namespace a3d

// [Private Non-Member Functions]

PipelineDesc MakeBaseDesc(const Material& material, VertexLayout layout) {
    PipelineDesc desc {};
    desc.vertexLayoutKey = layout;
    desc.fillMode = material.fillMode();
    desc.blendFunction = material.blendFunction();
    desc.doubleSided = material.doubleSided();
    desc.depthTest = material.depthTestEnabled();
    desc.depthWrite = material.depthWriteEnabled();
    desc.depthFunc = DepthFunc::Less;
    desc.shaderKind = ShaderKind::Default;

    switch (material.alphaMode()) {
        case Material::AlphaMode::Opaque:
        case Material::AlphaMode::Mask:
            desc.blendFunction = Material::BlendFunction::Disabled;
            break;

        case Material::AlphaMode::Blend:
            if (desc.blendFunction == Material::BlendFunction::Disabled) {
                desc.blendFunction = Material::BlendFunction::Alpha;
            }
            break;
    }

    return desc;
}
