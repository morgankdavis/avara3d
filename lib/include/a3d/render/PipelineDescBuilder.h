//
//  PipelineDescBuilder.h
//  avara3d
//
//  Created by Morgan Davis on 1/4/26.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_RENDER_PIPELINEDESCBUILDER_H
#define AVARA3D_RENDER_PIPELINEDESCBUILDER_H

#include "a3d/mesh/VertexLayout.h"
#include "a3d/render/PipelineDesc.h"

namespace a3d {
class Material;

class PipelineDescBuilder {

public:
    // [Internal Static Member Functions]

    static PipelineDesc MakeBackgroundDesc();
    static PipelineDesc MakeGroundDesc();
    static PipelineDesc MakeOpaqueDesc(const Material& material, VertexLayout layout);
    static PipelineDesc MakeMaskDesc(const Material& material, VertexLayout layout);
    static PipelineDesc MakeTransparentDesc(const Material& material, VertexLayout layout);
    static PipelineDesc MakeWireframeDesc(VertexLayout layout);
    static PipelineDesc MakeLinesDesc();
};
} // namespace a3d

#endif // AVARA3D_RENDER_PIPELINEDESCBUILDER_H
