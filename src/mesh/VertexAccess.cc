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
#include "a3d/render/VertexLayoutDesc.h"

using namespace a3d;
using namespace std;

const VertexAttribDesc* VertexAccess::FindAttrib(const VertexLayoutDesc& desc,
												 VertexSemantic semantic) {
	for (const auto& a : desc.attribs) {
		if (a.semantic == semantic) return &a;
	}
	return nullptr;
}

const VertexAttribDesc* VertexAccess::GetPosAttribOrDie(VertexLayout layout) {
	const VertexLayoutDesc& d = GetVertexLayoutDesc(layout);
	const VertexAttribDesc* posA = FindAttrib(d, VertexSemantic::Position);
	A3D_ASSERT(posA && posA->format == VertexFormat::F32x3);
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
