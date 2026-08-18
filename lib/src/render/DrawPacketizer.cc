//
//  DrawPacketizer.cc
//  avara3d
//
//  Created by Morgan Davis on 12/28/25.
//  Copyright © 2025-2026 Morgan K Davis. All rights reserved.
//

#include "a3d/render/DrawPacketizer.h"

#include "a3d/mesh/Line.h"
#include "a3d/mesh/Mesh.h"
#include "a3d/render/DrawPacket.h"
#include "a3d/render/GatherOutput.h"
#include "a3d/render/PacketSorter.h"
#include "a3d/render/PipelineDesc.h"
#include "a3d/render/PipelineDescBuilder.h"
#include "a3d/scene/Scene.h"
#include "a3d/visual/material/Material.h"

using namespace a3d;
using namespace a3d::math;
using namespace std;

/// Internal Static Member Functions ///

DrawPacket DrawPacketizer::Packetize(GatherOutput& gatherOutput) {

    DrawPacket packet {};

    packet.mainPassItems.reserve(gatherOutput.renderItems.size());
    packet.wireframePassItems.reserve(gatherOutput.renderItems.size());

    packet.backgroundPass.pipelineId = INVALID_PIPELINE_ID;
    packet.backgroundPass.desc = PipelineDescBuilder::MakeBackgroundDesc();
    packet.backgroundPass.material = gatherOutput.backgroundMaterial;
    packet.backgroundPass.orientation = gatherOutput.backgroundOrientation;

    if (gatherOutput.ground) {
        packet.groundPass.desc = PipelineDescBuilder::MakeGroundDesc();
        packet.groundPass.ground = std::move(gatherOutput.ground);
    }

    for (const RenderItem& ri : gatherOutput.renderItems) {

        // effective style for this instance (refine later?)
        RenderStyle style = ri.style;

        if (style == RenderStyle::Normal || style == RenderStyle::WireframeOverlay) {

            DrawItem di;
            di.elementIndex = ri.elementIndex;
            di.element = ri.element;
            di.material = ri.material;
            di.tint = ri.tint;
            di.model = ri.model;
            di.depth = ri.depth;
            di.renderOrder = ri.renderOrder;

            switch (ri.material->alphaMode()) {
                case Material::AlphaMode::Opaque: {
                    di.pass = PassKind::MainOpaque;
                    di.desc = PipelineDescBuilder::MakeOpaqueDesc(*ri.material, ri.layout);
                    break;
                }
                case Material::AlphaMode::Mask: {
                    di.pass = PassKind::MainMask;
                    di.desc = PipelineDescBuilder::MakeMaskDesc(*ri.material, ri.layout);
                    break;
                }
                case Material::AlphaMode::Blend: {
                    di.pass = PassKind::MainTransparent;
                    di.desc = PipelineDescBuilder::MakeTransparentDesc(*ri.material, ri.layout);
                    break;
                }
                default: /* unreachable */
                    break;
            }

            di.pipelineId = INVALID_PIPELINE_ID; // resolved later
            di.sequence = (uint32_t) packet.mainPassItems.size();

            packet.mainPassItems.push_back(std::move(di));
        }

        if (style == RenderStyle::Wireframe || style == RenderStyle::WireframeOverlay) {

            DrawItem di;
            di.elementIndex = ri.elementIndex;
            di.element = ri.element;
            di.material = ri.material; // optional for wire, fine to keep
            di.tint = ri.tint;
            di.model = ri.model;
            di.depth = ri.depth;
            di.renderOrder = ri.renderOrder;
            di.pass = PassKind::Wireframe;

            di.desc = PipelineDescBuilder::MakeWireframeDesc(ri.layout);
            di.pipelineId = INVALID_PIPELINE_ID; // resolved later
            di.sequence = (uint32_t) packet.wireframePassItems.size();

            packet.wireframePassItems.push_back(std::move(di));
        }
    }

    packet.lightNodes = std::move(gatherOutput.lightNodes);

    packet.linesPass.model = mat4(1.0f);
    packet.linesPass.lines = std::move(gatherOutput.debugLines);

    if (!packet.linesPass.lines.empty()) {
        packet.linesPass.desc = PipelineDescBuilder::MakeLinesDesc();
        packet.linesPass.pipelineId = INVALID_PIPELINE_ID;
    }

    PacketSorter::SortPacket(packet); // ohhh yeaahhhhhh

    return packet;
}
