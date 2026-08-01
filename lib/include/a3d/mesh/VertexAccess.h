//
//  VertexAccess.h
//  avara3d
//
//  Created by Morgan Davis on 1/5/26.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_MESH_VERTEXACCESS_H
#define AVARA3D_MESH_VERTEXACCESS_H

#include <cstddef>
#include <cstdint>
#include <optional>
#include <span>

#include "a3d/Math.h"
#include "a3d/mesh/VertexLayout.h"
#include "a3d/mesh/VertexLayoutDesc.h"

namespace a3d {

	class MeshElement;

	struct VertexStreamView {
		const std::byte* base = 	nullptr;
		uint32_t         count  = 	0;
		uint16_t         offset = 	0; // attribute offset
		VertexLayout     layout = 	VertexLayout::None;
	};

	inline uint16_t VertexStreamStride(const VertexStreamView& v) {
		return GetVertexLayoutDesc(v.layout).stride;
	}

	inline const std::byte* VertexBaseAt(const VertexStreamView& v, uint32_t i) {
		return v.base + std::size_t(i) * std::size_t(VertexStreamStride(v));
	}

	struct VertexAccess {

		static const VertexAttribDesc*			FindAttrib(const VertexLayoutDesc& desc,
															 VertexSemantic semantic);
		static const VertexAttribDesc*			GetPositionAttribF32x3(VertexLayout layout);
		static math::vec3 						ReadVec3(const std::byte* base,
														  uint16_t offset);
		static void 							WriteVec3(std::byte* base,
														 uint16_t offset,
														 const math::vec3& vec);
		static std::optional<VertexStreamView>  GetStreamView(const MeshElement& element,
															  VertexSemantic semantic,
															  VertexAttribFormat expectedFormat);

		static std::optional<VertexStreamView>	GetPositionStreamView(const MeshElement& element);
	};
}

#endif //AVARA3D_MESH_VERTEXACCESS_H
