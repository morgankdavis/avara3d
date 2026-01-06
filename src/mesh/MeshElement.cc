//
//  MeshElement.cc
//  avara3d
//
//  Created by Morgan Davis on 12/23/16.
//  Copyright © 2024 Morgan K Davis. All rights reserved.
//

#include "a3d/mesh/MeshElement.h"

#include <cstring>
#include <iostream>

#include "a3d/Assert.h"
#include "a3d/Color.h"
#include "a3d/Types.h"
#include "a3d/log/Log.h"
#include "a3d/Math.h"
#include "a3d/mesh/Line.h"
#include "a3d/scene/Node.h"
#include "a3d/render/Renderer.h"
#include "a3d/render/VertexLayoutDesc.h"

using namespace a3d;
using namespace a3d::math;
using namespace std;





static const VertexAttribDesc* FindAttrib(const VertexLayoutDesc& d, VertexSemantic sem);
static inline vec3 ReadVec3(const std::byte* base, uint16_t offset);
static inline void WriteVec3(std::byte* base, uint16_t offset, const vec3& v);








/// Public Lifecycle Functions ///

//MeshElement::MeshElement(const vector<Vertex>& verticies,
//						 const vector<Face>& faces):
//		MeshElement{} {
//
//	_layout = VertexLayout::PNT;
//	_vertices = verticies;
//	_faces = faces;
//
//	genLocalAABB();
//}

MeshElement::MeshElement(const vector<Vertex>& vertices,
						 const vector<Face>& faces):
						 MeshElement{} {
	setVertices(VertexLayout::PNT, vertices);
	setFaces(faces);
}

MeshElement::~MeshElement() {
	log::d()("Destroying MeshElement {:p}", static_cast<void*>(this));
}

/// Internal Member Functions ///

//const vector<Vertex>& MeshElement::vertices() const { // PNT-only legacy path
//	return _vertices;
//}

//const vector<Face>& MeshElement::faces() const {
//	return _faces;
//}

//void MeshElement::burnTransform(const mat4& transform, bool normals) {
//
//	for (auto& vert : _vertices) {
//		vert.position = vec3(transform * vec4(vert.position, 1.0f));
//		if (normals) {
//			vert.normal = normalize(vec3(transform * vec4(vert.normal, 0.0f)));
//		}
//	}
//
//	genLocalAABB();
//
//	A3D_MASK_ADD(_dirtyMask, MeshElementDirtyMask::VertexData);
//}

//void MeshElement::burnTransform(const mat4& transform, bool normals) {
//
//	// If legacy PNT verts exist, keep your fast path
//	if (!_vertices.empty()) {
//		// (your existing code)
//		for (auto& vert : _vertices) {
//			vert.position = vec3(transform * vec4(vert.position, 1.0f));
//			if (normals) {
//				// slightly more correct normal handling:
//				mat3 nmat = transpose(inverse(mat3(transform)));
//				vert.normal = normalize(nmat * vert.normal);
//			}
//		}
//		genLocalAABB();
//		A3D_MASK_ADD(_dirtyMask, MeshElementDirtyMask::VertexData);
//		return;
//	}
//
//	// Generic packed path
//	const uint32_t vcount = vertexCount();
//	if (vcount == 0) return;
//
//	const VertexLayoutDesc& desc = GetVertexLayoutDesc(_layout);
//	const VertexAttribDesc* posA = FindAttrib(desc, VertexSemantic::Position);
//	A3D_ASSERT(posA && posA->format == VertexFormat::F32x3);
//
//	const VertexAttribDesc* nrmA = nullptr;
//	mat3 nmat(1.0f);
//	if (normals) {
//		nrmA = FindAttrib(desc, VertexSemantic::Normal);
//		if (nrmA && nrmA->format == VertexFormat::F32x3) {
//			nmat = transpose(inverse(mat3(transform)));
//		}
//		else {
//			nrmA = nullptr;
//		}
//	}
//
//	auto stride = vertexStride();
//	for (uint32_t i = 0; i < vcount; ++i) {
//		std::byte* base = _vertexData.data() + size_t(i) * stride;
//
//		vec3 p = ReadVec3(base, posA->offset);
//		p = vec3(transform * vec4(p, 1.0f));
//		WriteVec3(base, posA->offset, p);
//
//		if (nrmA) {
//			vec3 n = ReadVec3(base, nrmA->offset);
//			n = normalize(nmat * n);
//			WriteVec3(base, nrmA->offset, n);
//		}
//	}
//
//	genLocalAABB();
//	A3D_MASK_ADD(_dirtyMask, MeshElementDirtyMask::VertexData);
//}

void MeshElement::burnTransform(const mat4& transform, bool normals) {
	if (_vertexCount == 0) return;

	const VertexLayoutDesc& desc = GetVertexLayoutDesc(_layout);

	const VertexAttribDesc* posA = FindAttrib(desc, VertexSemantic::Position);
	A3D_ASSERT(posA && posA->format == VertexFormat::F32x3);

	const VertexAttribDesc* nrmA = nullptr;
	mat3 nmat(1.0f);
	if (normals) {
		nrmA = FindAttrib(desc, VertexSemantic::Normal);
		if (nrmA && nrmA->format == VertexFormat::F32x3) {
			nmat = transpose(inverse(mat3(transform)));
		} else {
			nrmA = nullptr;
		}
	}

	for (uint32_t i = 0; i < _vertexCount; ++i) {
		std::byte* base = _vertexData.data() + size_t(i) * _vertexStride;

		vec3 p = ReadVec3(base, posA->offset);
		p = vec3(transform * vec4(p, 1.0f));
		WriteVec3(base, posA->offset, p);

		if (nrmA) {
			vec3 n = ReadVec3(base, nrmA->offset);
			n = normalize(nmat * n);
			WriteVec3(base, nrmA->offset, n);
		}
	}

	genLocalAABB();
	_dirtyMask |= MeshElementDirtyMask::VertexData;
}

AABB MeshElement::localAABB() const {
	return _localAABB;
}

//AABB MeshElement::worldAABB(const math::mat4& worldTransform, bool vertfit) const {
//
//	// TODO: consolidate (Mesh has the same function)
//
//	// fit over verticies - tighter - slow!
//	if (vertfit) {
//
//		static const float maxFloat = math::f32_max();
//		static const float minFloat = math::f32_lowest();
//		AABB out = { {maxFloat, maxFloat, maxFloat},
//					 {minFloat, minFloat, minFloat} };
//
//		for (const auto& v : vertices()) {
//			vec3 p = vec3(worldTransform * vec4(v.position, 1.0f));
//			out.min.x = math::min(out.min.x, p.x);
//			out.max.x = math::max(out.max.x, p.x);
//			out.min.y = math::min(out.min.y, p.y);
//			out.max.y = math::max(out.max.y, p.y);
//			out.min.z = math::min(out.min.z, p.z);
//			out.max.z = math::max(out.max.z, p.z);
//		}
//
//		return out;
//	}
//		// fit over OBB - looser - fast!
//	else {
//
//		auto localAABB = MeshElement::localAABB();
//
//		const vec3 c = (localAABB.min + localAABB.max) / 2.0f; // local center
//		const vec3 e = (localAABB.max - localAABB.min) / 2.0f; // local half extents
//
//		const vec3 C = vec3{worldTransform * vec4(c, 1.0f)}; // world center
//
//		// linear part (rotation/scale/shear)
//		const mat3 L = math::mat3{worldTransform};
//
//		const mat3 A = math::abs(L);
//
//		const vec3 E = A * e; // world half extents
//
//		return AABB{C - E, C + E};
//	}
//}

AABB MeshElement::worldAABB(const mat4& worldTransform, bool vertfit) const {

	if (vertfit) {
		const uint32_t vcount = vertexCount();
		if (vcount == 0) return AABB::Zero();

		const auto vb = vertexBytes();
		const uint16_t stride = vertexStride();

		const VertexLayoutDesc& desc = GetVertexLayoutDesc(_layout);
		const VertexAttribDesc* posA = FindAttrib(desc, VertexSemantic::Position);
		A3D_ASSERT(posA && posA->format == VertexFormat::F32x3);

		static const float maxFloat = math::f32_max();
		static const float minFloat = math::f32_lowest();
		AABB out = { {maxFloat, maxFloat, maxFloat},
					 {minFloat, minFloat, minFloat} };

		for (uint32_t i = 0; i < vcount; ++i) {
			const std::byte* base = vb.data() + size_t(i) * stride;
			vec3 pLocal = ReadVec3(base, posA->offset);
			vec3 p = vec3(worldTransform * vec4(pLocal, 1.0f));

			out.min = min(out.min, p);
			out.max = max(out.max, p);
		}

		return out;
	}

	// OBB-fit path unchanged
	auto localAABB = MeshElement::localAABB();

	const vec3 c = (localAABB.min + localAABB.max) / 2.0f;
	const vec3 e = (localAABB.max - localAABB.min) / 2.0f;

	const vec3 C = vec3{worldTransform * vec4(c, 1.0f)};
	const mat3 L = mat3{worldTransform};
	const mat3 A = abs(L);
	const vec3 E = A * e;

	return AABB{C - E, C + E};
}

//vec3 MeshElement::localExtent() const {
//	auto aabb = MeshElement::localAABB();
//	return { aabb.max.x - aabb.min.x,
//			 aabb.max.y - aabb.min.y,
//			 aabb.max.z - aabb.min.z };
//}

vec3 MeshElement::localExtent() const {
	auto aabb = localAABB();
	return aabb.max - aabb.min;
}

vec3 MeshElement::worldExtent(const mat4& worldTransform) const {
	auto aabb = worldAABB(worldTransform, true);
	return aabb.max - aabb.min;
}

MeshElementDirtyMask MeshElement::dirtyMask() const {
	return _dirtyMask;
}

void MeshElement::dirtyMask(MeshElementDirtyMask mask) {
	_dirtyMask = mask;
}

/// Protected Member Functions ///

//void MeshElement::genLocalAABB() {
//
//	static const float maxFloat = math::f32_max();
//	static const float minFloat = math::f32_lowest();
//
//	AABB aabb = { {maxFloat, maxFloat, maxFloat},
//				  {minFloat, minFloat, minFloat} };
//
//	for (const auto& v : vertices()) {
//		vec3 p = (v.position);
//		aabb.min.x = math::min(aabb.min.x, p.x);
//		aabb.max.x = math::max(aabb.max.x, p.x);
//		aabb.min.y = math::min(aabb.min.y, p.y);
//		aabb.max.y = math::max(aabb.max.y, p.y);
//		aabb.min.z = math::min(aabb.min.z, p.z);
//		aabb.max.z = math::max(aabb.max.z, p.z);
//	}
//
//	_localAABB = aabb;
//}

//void MeshElement::genLocalAABB() {
//
//	const uint32_t vcount = vertexCount();
//	if (vcount == 0) {
//		_localAABB = AABB::Zero();
//		return;
//	}
//
//	const auto vb = vertexBytes();
//	const uint16_t stride = vertexStride();
//
//	const VertexLayoutDesc& desc = GetVertexLayoutDesc(_layout);
//	const VertexAttribDesc* posA = FindAttrib(desc, VertexSemantic::Position);
//	A3D_ASSERT(posA && posA->format == VertexFormat::F32x3);
//
//	static const float maxFloat = math::f32_max();
//	static const float minFloat = math::f32_lowest();
//
//	AABB aabb = { {maxFloat, maxFloat, maxFloat},
//				  {minFloat, minFloat, minFloat} };
//
//	for (uint32_t i = 0; i < vcount; ++i) {
//		const std::byte* base = vb.data() + size_t(i) * stride;
//		vec3 p = ReadVec3(base, posA->offset);
//		aabb.min = min(aabb.min, p);
//		aabb.max = max(aabb.max, p);
//	}
//
//	_localAABB = aabb;
//}

void MeshElement::genLocalAABB() {
	if (_vertexCount == 0) {
		_localAABB = AABB::Zero();
		return;
	}

	const VertexLayoutDesc& desc = GetVertexLayoutDesc(_layout);
	const VertexAttribDesc* posA = FindAttrib(desc, VertexSemantic::Position);
	A3D_ASSERT(posA && posA->format == VertexFormat::F32x3);

	static const float maxFloat = math::f32_max();
	static const float minFloat = math::f32_lowest();

	AABB aabb = { {maxFloat, maxFloat, maxFloat},
				  {minFloat, minFloat, minFloat} };

	const auto vb = vertexBytes();
	for (uint32_t i = 0; i < _vertexCount; ++i) {
		const std::byte* base = vb.data() + size_t(i) * _vertexStride;
		vec3 p = ReadVec3(base, posA->offset);
		aabb.min = min(aabb.min, p);
		aabb.max = max(aabb.max, p);
	}

	_localAABB = aabb;
}

/// Protected Lifecycle ///

//MeshElement::MeshElement():
//		_layout{VertexLayout::PNT},
//		//_vertices{},
//		//_faces{},
//		//_aabbLines{},
//		_dirtyMask{MeshElementDirtyMask::All} { }


MeshElement::MeshElement():
		_layout{VertexLayout::None},
		_dirtyMask{MeshElementDirtyMask::All} {}














VertexLayout MeshElement::vertexLayout() const {
	return _layout;
}

void MeshElement::vertexLayout(VertexLayout layout) {
	_layout = layout;
}


void MeshElement::setVertexData(VertexLayout layout,
							   span<const byte> bytes,
							   uint32_t vertexCount,
							   uint16_t stride) {
	_layout = layout;
	_vertexCount = vertexCount;
	_vertexStride = stride;

	_vertexData.assign(bytes.begin(), bytes.end());

	genLocalAABB();
	_dirtyMask |= MeshElementDirtyMask::VertexData;
}

void MeshElement::setFaces(span<const Face> faces) {
	_faces.assign(faces.begin(), faces.end());
	_dirtyMask |= MeshElementDirtyMask::VertexData; // triggers VAO+EBO rebuild
}



//uint32_t MeshElement::vertexCount() const {
//	return (uint32_t)_vertices.size();
//}
//
//span<const byte> MeshElement::vertexBytes() const {
//	auto s = span<const Vertex>{ _vertices.data(), _vertices.size() };
//	return as_bytes(s);
//}
//
//uint16_t MeshElement::vertexStride() const {
//	return (uint16_t)sizeof(Vertex);
//}

//uint32_t MeshElement::vertexCount() const {
//	// If legacy verts exist, trust them
//	if (!_vertices.empty()) return (uint32_t)_vertices.size();
//	return _vertexCount;
//}
//
//span<const byte> MeshElement::vertexBytes() const {
//	if (!_vertexData.empty()) {
//		return span<const byte>(_vertexData.data(), _vertexData.size());
//	}
//	auto s = span<const Vertex>{ _vertices.data(), _vertices.size() };
//	return as_bytes(s);
//}
//
//uint16_t MeshElement::vertexStride() const {
//	if (!_vertexData.empty()) return _vertexStride;
//	return (uint16_t)sizeof(Vertex);
//}

uint32_t MeshElement::vertexCount() const {
	return _vertexCount;
}

span<const byte> MeshElement::vertexBytes() const {
	return { _vertexData.data(), _vertexData.size() };
}

uint16_t MeshElement::vertexStride() const {
	return _vertexStride;
}

const vector<Face>& MeshElement::faces() const {
	return _faces;
}











const VertexAttribDesc* FindAttrib(const VertexLayoutDesc& d, VertexSemantic sem) {
	for (const auto& a : d.attribs) if (a.semantic == sem) return &a;
	return nullptr;
}

vec3 ReadVec3(const std::byte* base, uint16_t offset) {
	float tmp[3];
	std::memcpy(tmp, base + offset, sizeof(tmp));
	return vec3(tmp[0], tmp[1], tmp[2]);
}

void WriteVec3(std::byte* base, uint16_t offset, const vec3& v) {
	float tmp[3] = { v.x, v.y, v.z };
	std::memcpy(base + offset, tmp, sizeof(tmp));
}