//
//  VertexAccess.h
//  avara3d
//
//  Created by Morgan Davis on 1/5/26.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_MESH_VERTEXACCESS_H
#define AVARA3D_MESH_VERTEXACCESS_H

#include <cstdint>
#include <optional>
#include <span>

#include "a3d/Math.h"
#include "a3d/mesh/VertexLayout.h"
#include "a3d/render/VertexLayoutDesc.h"

namespace a3d {

	class MeshElement;

	struct VertexStreamView {
		const std::byte* base = 	nullptr; // start of vertex buffer
		uint16_t         stride = 	0;
		uint32_t         count  = 	0;
		uint16_t         offset = 	0; // attribute offset within vertex
	};

	struct VertexAccess {

		static const VertexAttribDesc*			FindAttrib(const VertexLayoutDesc &desc,
															 VertexSemantic semantic);
		static const VertexAttribDesc*			GetPositionAttribF32x3(VertexLayout layout);
		static math::vec3 						ReadVec3(const std::byte *base,
														  uint16_t offset);
		static void 							WriteVec3(std::byte *base,
														 uint16_t offset,
														 const math::vec3 &vec);
		static std::optional<VertexStreamView>	GetStreamView(const MeshElement& element,
																VertexSemantic semantic,
																VertexFormat expectedFormat);

		static std::optional<VertexStreamView>	GetPositionStreamView(const MeshElement& element);
	};
}

#endif //AVARA3D_MESH_VERTEXACCESS_H
