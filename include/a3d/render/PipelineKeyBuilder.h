//
//  PipelineKeyBuilder.h
//  avara3d
//
//  Created by Morgan Davis on 1/4/26.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_RENDER_PIPELINEKEYBUILDER_H
#define AVARA3D_RENDER_PIPELINEKEYBUILDER_H

#include "a3d/mesh/VertexLayout.h"
#include "a3d/render/PipelineKey.h"

namespace a3d {

	class Material;

	class PipelineKeyBuilder {

	public:
		/// Internal Static Member Functions ///

		static PipelineKey MakeBackgroundKey();
		static PipelineKey MakeOpaqueKey(const Material& material, VertexLayout layoutKey);
		static PipelineKey MakeMaskKey(const Material& material, VertexLayout layoutKey);
		static PipelineKey MakeTransparentKey(const Material& material, VertexLayout layoutKey);
		static PipelineKey MakeWireframeKey(VertexLayout layoutKey);
		static PipelineKey MakeLinesKey();
	};
}

#endif //AVARA3D_RENDER_PIPELINEKEYBUILDER_H
