//
//  VertexAccess.cc
//  avara3d
//
//  Created by Morgan Davis on 1/5/26.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#include "a3d/mesh/VertexAccess.h"

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <span>

#include "a3d/Assert.h"
#include "a3d/mesh/AABB.h"
#include "a3d/mesh/MeshElement.h"
#include "a3d/mesh/VertexLayoutDesc.h"

using namespace a3d;
using namespace std;

const VertexAttribDesc* VertexAccess::FindAttrib(const VertexLayoutDesc& desc,
												 VertexSemantic semantic) {
	for (const auto& a : desc.attribs) {
		if (a.semantic == semantic) return &a;
	}
	return nullptr;
}

const VertexAttribDesc* VertexAccess::GetPositionAttribF32x3(VertexLayout layout) {
	const VertexLayoutDesc& d = GetVertexLayoutDesc(layout);
	const VertexAttribDesc* posA = FindAttrib(d, VertexSemantic::Position);
	A3D_ASSERT(posA && posA->format == VertexAttribFormat::F32x3);
	return posA;
}

math::vec3 VertexAccess::ReadVec3(const std::byte* base, uint16_t offset) {
	float tmp[3];
	memcpy(tmp, base + offset, sizeof(tmp));
	return {tmp[0], tmp[1], tmp[2]};
}

void VertexAccess::WriteVec3(std::byte* base, uint16_t offset, const math::vec3& vec) {
	float tmp[3] = { vec.x, vec.y, vec.z };
	memcpy(base + offset, tmp, sizeof(tmp));
}

std::optional<VertexStreamView> VertexAccess::GetStreamView(const MeshElement& element,
															VertexSemantic semantic,
															VertexAttribFormat expectedFormat) {

	if (element.vertexCount() == 0) return std::nullopt;

	const auto vb = element.vertexBytes();
	if (vb.empty()) return std::nullopt;

	const VertexLayout layout = element.vertexLayout();
	const VertexLayoutDesc& d = GetVertexLayoutDesc(layout);
	if (d.stride == 0) return std::nullopt;

	// Invariant: element stride must match the canonical layout stride
	A3D_ASSERT(element.vertexStride() == d.stride);
	if (element.vertexStride() != d.stride) return std::nullopt;

	const std::size_t expectedSize =
			std::size_t(element.vertexCount()) * std::size_t(d.stride);

	A3D_ASSERT(vb.size() == expectedSize);
	if (vb.size() != expectedSize) return std::nullopt;

	const VertexAttribDesc* a = FindAttrib(d, semantic);
	if (!a || a->format != expectedFormat) return std::nullopt;

	VertexStreamView out;
	out.base   = vb.data();
	out.count  = element.vertexCount();
	out.offset = a->offset;
	out.layout = layout;

	return out;
}

optional<VertexStreamView> VertexAccess::GetPositionStreamView(const MeshElement& element) {
	return GetStreamView(element, VertexSemantic::Position, VertexAttribFormat::F32x3);
}
