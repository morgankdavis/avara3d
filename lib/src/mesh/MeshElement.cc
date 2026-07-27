//
//  MeshElement.cc
//  avara3d
//
//  Created by Morgan Davis on 12/23/16.
//  Copyright © 2024 Morgan K Davis. All rights reserved.
//

#include "a3d/mesh/MeshElement.h"

#include <cstring>
#include <stdexcept>

#include "a3d/Assert.h"
#include "a3d/log/Log.h"
#include "a3d/Math.h"
#include "a3d/mesh/PrimitiveTopology.h"
#include "a3d/mesh/VertexAccess.h"
#include "a3d/util/Bitmask.h"

using namespace a3d;
using namespace a3d::math;
using namespace std;

MeshElement::MeshElement(VertexLayout layout,
						 std::span<const std::byte> vbytes,
						 uint32_t vcount,
						 uint16_t vstride,
						 PrimitiveTopology topology,
						 IndexFormat indexFormat,
						 std::span<const std::byte> ibytes,
						 uint32_t icount):
		_topology{topology},
		_vertexLayout{layout},
		_vertexData{},
		_vertexCount{vcount},
		_vertexStride{vstride},
		_indexFormat{indexFormat},
		_indexData{},
		_indexCount{icount},
		_localAABB{AABB::Invalid()},
		_dirtyMask{DirtyMask::All} {

	if (vcount == 0) {
		return;
	}

	const size_t vneeded = size_t(vcount) * size_t(vstride);
	if (vbytes.size() < vneeded) {
		throw std::runtime_error("Vertex byte span too small for vertexCount * stride");
	}

	_vertexData.resize(vneeded);
	memcpy(_vertexData.data(), vbytes.data(), vneeded);

	if (_indexFormat == IndexFormat::None) {
		// bon-indexed mesh: enforce empty indices
		A3D_ASSERT(_indexCount == 0);
		A3D_ASSERT(ibytes.empty());
		_indexCount = 0;
		_indexData.clear();
	}
	else {
		const uint16_t is = IndexStride(_indexFormat);
		const size_t ineeded = size_t(_indexCount) * size_t(is);
		if (ibytes.size() < ineeded) {
			throw std::runtime_error("Index byte span too small for indexCount * indexStride");
		}
		_indexData.resize(ineeded);
		memcpy(_indexData.data(), ibytes.data(), ineeded);
	}

	genLocalAABB();
}


MeshElement::~MeshElement() {
	log::d()("Destroying MeshElement {:p}", static_cast<void*>(this));
}

/// Internal Member Functions ///

PrimitiveTopology MeshElement::topology() const {
	return _topology;
}

VertexLayout MeshElement::vertexLayout() const {
	return _vertexLayout;
}

uint32_t MeshElement::vertexCount() const {
	return _vertexCount;
}

span<const byte> MeshElement::vertexBytes() const {
	return { _vertexData.data(), _vertexData.size() };
}

uint16_t MeshElement::vertexStride() const {
	return _vertexStride;
}

IndexFormat MeshElement::indexFormat() const {
	return _indexFormat;
}

uint32_t MeshElement::indexCount() const {
	return _indexCount;
}

std::span<const std::byte> MeshElement::indexBytes() const {
	return { _indexData.data(), _indexData.size() };
}

AABB MeshElement::localAABB() const {
	return _localAABB;
}

AABB MeshElement::worldAABB(const mat4& worldTransform, bool vertfit) const {

	if (vertfit) {

		const uint32_t vcount = vertexCount();
		if (vcount == 0) return AABB::Zero();

		const auto vb = vertexBytes();
		const uint16_t stride = vertexStride();

		const VertexLayoutDesc& desc = GetVertexLayoutDesc(_vertexLayout);
		const VertexAttribDesc* posA = VertexAccess::FindAttrib(desc, VertexSemantic::Position);
		A3D_ASSERT(posA && posA->format == VertexAttribFormat::F32x3);

		static const float maxFloat = math::f32_max();
		static const float minFloat = math::f32_lowest();
		AABB out = { {maxFloat, maxFloat, maxFloat},
					 {minFloat, minFloat, minFloat} };

		for (uint32_t i = 0; i < vcount; ++i) {
			const std::byte* base = vb.data() + size_t(i) * stride;
			vec3 pLocal = VertexAccess::ReadVec3(base, posA->offset);
			vec3 p = vec3(worldTransform * vec4(pLocal, 1.0f));

			out.min = min(out.min, p);
			out.max = max(out.max, p);
		}

		return out;
	}
	else {

		auto localAABB = MeshElement::localAABB();

		const vec3 c = (localAABB.min + localAABB.max) / 2.0f;
		const vec3 e = (localAABB.max - localAABB.min) / 2.0f;

		const vec3 C = vec3{worldTransform * vec4(c, 1.0f)};
		const mat3 L = mat3{worldTransform};
		const mat3 A = abs(L);
		const vec3 E = A * e;

		return AABB{C - E, C + E};
	}
}

vec3 MeshElement::localExtent() const {
	auto aabb = localAABB();
	return aabb.max - aabb.min;
}

vec3 MeshElement::worldExtent(const mat4& worldTransform) const {
	auto aabb = worldAABB(worldTransform, true);
	return aabb.max - aabb.min;
}

void MeshElement::beginBuild(VertexLayout layout,
							 uint16_t vertexStride,
							 PrimitiveTopology topology,
							 IndexFormat indexFormat,
							 uint32_t reserveVerts,
							 uint32_t reserveIndices) {

	_vertexLayout = layout;
	_vertexStride = vertexStride;
	_vertexCount = 0;
	_vertexData.clear();

	_topology = topology;
	_indexFormat = indexFormat;
	_indexCount = 0;
	_indexData.clear();

//	_faces.clear(); // legacy bridge (remove in Phase B)

	if (reserveVerts) {
		_vertexData.reserve(size_t(reserveVerts) * size_t(vertexStride));
	}

	if (reserveIndices && indexFormat != IndexFormat::None) {
		_indexData.reserve(size_t(reserveIndices) * size_t(IndexStride(indexFormat)));
	}

	_dirtyMask = util::bitmask::add(_dirtyMask, DirtyMask::VertexData);
	_dirtyMask = util::bitmask::add(_dirtyMask, DirtyMask::IndexData);
	_localAABB = AABB::Invalid();
}

void MeshElement::appendVertexBytes(const void* vertexBytes) {

	A3D_ASSERT(_vertexStride != 0);

	const size_t oldSize = _vertexData.size();
	_vertexData.resize(oldSize + _vertexStride);
	memcpy(_vertexData.data() + oldSize, vertexBytes, _vertexStride);

	++_vertexCount;
}

void MeshElement::appendIndex(uint32_t idx) {
	A3D_ASSERT(_indexFormat != IndexFormat::None);

	if (_indexFormat == IndexFormat::U16) {
		A3D_ASSERT(idx <= 0xFFFFu);
		uint16_t v = (uint16_t)idx;
		const std::byte* p = reinterpret_cast<const std::byte*>(&v);
		_indexData.insert(_indexData.end(), p, p + sizeof(v));
	}
	else {
		uint32_t v = idx;
		const std::byte* p = reinterpret_cast<const std::byte*>(&v);
		_indexData.insert(_indexData.end(), p, p + sizeof(v));
	}

	++_indexCount;
}

void MeshElement::appendTriangle(uint32_t a, uint32_t b, uint32_t c) {
	A3D_ASSERT(_topology == PrimitiveTopology::Triangles);
	appendIndex(a);
	appendIndex(b);
	appendIndex(c);
}

void MeshElement::endBuild(bool recomputeAABB) {

	const VertexLayoutDesc& vld = GetVertexLayoutDesc(_vertexLayout);

	if (_vertexCount == 0) {
		A3D_ASSERT(_vertexData.empty());
		// stride can be 0 - layout can be None
	}
	else {
		A3D_ASSERT(vld.stride != 0);
		A3D_ASSERT(_vertexStride != 0);

		A3D_ASSERT(_vertexStride == vld.stride);

		const size_t expectedVB =
				size_t(_vertexCount) * size_t(_vertexStride);
		A3D_ASSERT(_vertexData.size() == expectedVB);
	}

	const uint16_t is = IndexStride(_indexFormat);

	if (_indexFormat == IndexFormat::None) {
		A3D_ASSERT(is == 0);

		A3D_ASSERT(_indexCount == 0);
		A3D_ASSERT(_indexData.empty());

		if (_topology == PrimitiveTopology::Triangles) {
			A3D_ASSERT((_vertexCount % 3u) == 0u);
		}
	}
	else {
		A3D_ASSERT(is == 2u || is == 4u);

		A3D_ASSERT(_indexCount > 0);
		const size_t expectedIB =
				size_t(_indexCount) * size_t(is);
		A3D_ASSERT(_indexData.size() == expectedIB);

		if (_topology == PrimitiveTopology::Triangles) {
			A3D_ASSERT((_indexCount % 3u) == 0u);
		}
	}

	if (recomputeAABB) genLocalAABB();

	_dirtyMask = util::bitmask::add(_dirtyMask, DirtyMask::VertexData);
	_dirtyMask = util::bitmask::add(_dirtyMask, DirtyMask::IndexData);
}


void MeshElement::burnTransform(const mat4& transform, bool normals) {
	if (_vertexCount == 0) return;

	const VertexLayoutDesc& desc = GetVertexLayoutDesc(_vertexLayout);

	const VertexAttribDesc* posA = VertexAccess::FindAttrib(desc, VertexSemantic::Position);
	A3D_ASSERT(posA && posA->format == VertexAttribFormat::F32x3);

	const VertexAttribDesc* nrmA = nullptr;
	mat3 nmat(1.0f);
	if (normals) {
		nrmA = VertexAccess::FindAttrib(desc, VertexSemantic::Normal);
		if (nrmA && nrmA->format == VertexAttribFormat::F32x3) {
			nmat = transpose(inverse(mat3(transform)));
		}
		else {
			nrmA = nullptr;
		}
	}

	for (uint32_t i = 0; i < _vertexCount; ++i) {
		std::byte* base = _vertexData.data() + size_t(i) * _vertexStride;

		vec3 p = VertexAccess::ReadVec3(base, posA->offset);
		p = vec3(transform * vec4(p, 1.0f));
		VertexAccess::WriteVec3(base, posA->offset, p);

		if (nrmA) {
			vec3 n = VertexAccess::ReadVec3(base, nrmA->offset);
			n = normalize(nmat * n);
			VertexAccess::WriteVec3(base, nrmA->offset, n);
		}
	}

	genLocalAABB();
	_dirtyMask = util::bitmask::add(_dirtyMask, DirtyMask::VertexData);
}

MeshElement::DirtyMask MeshElement::dirtyMask() const {
	return _dirtyMask;
}

void MeshElement::dirtyMask(DirtyMask mask) {
	_dirtyMask = mask;
}

/// Protected Member Functions ///

void MeshElement::genLocalAABB() {
	if (_vertexCount == 0) {
		_localAABB = AABB::Invalid();
		return;
	}

	const VertexAttribDesc* posA = VertexAccess::GetPositionAttribF32x3(_vertexLayout);
	const auto vb = vertexBytes();

	_localAABB = AABB::Invalid();

	for (uint32_t i = 0; i < _vertexCount; ++i) {
		const std::byte* base = vb.data() + size_t(i) * size_t(_vertexStride);
		const vec3 pos = VertexAccess::ReadVec3(base, posA->offset);
		_localAABB |= pos;
	}
}

/// Protected Lifecycle ///

MeshElement::MeshElement():
		_topology{PrimitiveTopology::Triangles},
		_vertexLayout{VertexLayout::None},
		_vertexData{},
		_vertexCount{0},
		_vertexStride{0},
		_indexFormat{IndexFormat::None},
		_indexData{},
		_indexCount{0},
		_localAABB{AABB::Invalid()},
		_dirtyMask{DirtyMask::All} {}
