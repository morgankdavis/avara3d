//
//  ConvexDecomposer.cc
//  avara3d
//
//  Created by Morgan Davis on 11/5/23.
//  Copyright © 2024 Morgan K Davis. All rights reserved.
//

#include "a3d/physics/util/ConvexDecomposer.h"

#include <cstring>
#include <span>
#include <utility>

#include <magic_enum/magic_enum.hpp>
#include <v-hacd/VHACD.h>

#include "a3d/Types.h"
#include "a3d/mesh/MeshElement.h"
#include "a3d/mesh/VertexAccess.h"
#include "a3d/mesh/VertexFormats.h"
#include "a3d/mesh/VertexLayout.h"
#include "a3d/log/Log.h"

using namespace a3d;
using namespace std;
using namespace VHACD;

/// Private Static Non-Member Prototypes ///

static bool GetPositionStream(const MeshElement& e,
							  const std::byte*& base,
							  uint16_t& stride,
							  uint32_t& count,
							  uint16_t& posOffset);

/// Internal Lifecycle Functions ///

ConvexDecomposer::ConvexDecomposer(MeshElement& element,
								   Options& options):
		_sourceElement{&element},
		_options{options} {
}

/// Internal Member Functions ///

//vector<unique_ptr<MeshElement>> ConvexDecomposer::decompose() {
//
//	VHACD::IVHACD* vhacd = CreateVHACD();
//
//	int a3dFillModeUnderlying = magic_enum::enum_integer(_options.fillMode);
//	VHACD::FillMode _vhacdFillMode = magic_enum::enum_value<VHACD::FillMode>(a3dFillModeUnderlying);
//
//	VHACD::IVHACD::Parameters params = {
//			nullptr,
//			nullptr,
//			nullptr,
//			_options.maxConvexHulls,
//			_options.resolution,
//			_options.minVolumePercentErr,
//			_options.maxRecursionDepth,
//			_options.shrinkWrap,
//			_vhacdFillMode, //_options.fillMode,
//			_options.maxNumVerticesPerHull,
//			false,//_options.asyncACD,
//			_options.minEdgeLength,
//			_options.findBestPlane
//	};
//
//	// can probably be optimized...
//
////	unsigned numVerts = 0;
////	unsigned numFaces = 0;
//////	for (auto& element : _sourceElements) {
////		numVerts += _sourceElement->vertices().size();
////		numFaces += _sourceElement->faces().size();
//////	}
////
////	auto verts = vector<float>();
////	verts.reserve((sizeof(float)*3) * numVerts);
////	auto faces = vector<uint32_t>();
////	faces.reserve((sizeof(uint32_t)*3) * numFaces);
////
//////	for (auto& element : _sourceElements) {
////		for (const auto& vert : _sourceElement->vertices()) {
////			verts.push_back(vert.position.x);
////			verts.push_back(vert.position.y);
////			verts.push_back(vert.position.z);
////		}
////		for (const auto& face : _sourceElement->faces()) {
////			faces.push_back((uint32_t)face.a);
////			faces.push_back((uint32_t)face.b);
////			faces.push_back((uint32_t)face.c);
////		}
//////	}
//
//	unsigned numVerts = 0;
//	unsigned numFaces = 0;
//
//	numVerts += _sourceElement->vertexCount();
//	numFaces += (unsigned)_sourceElement->faces().size();
//
//	auto verts = vector<float>();
//	verts.reserve(3u * numVerts);
//
//	auto faces = vector<uint32_t>();
//	faces.reserve(3u * numFaces);
//
//	{ // positions
//
//		const unsigned char* base = nullptr;
//		int stride = 0, count = 0;
//		if (!GetPNTPositionStream(*_sourceElement, base, stride, count)) {
//			vhacd->Release();
//			return {};
//		}
//
//		for (int i = 0; i < count; ++i) {
//			const float* p = reinterpret_cast<const float*>(base + size_t(i) * size_t(stride));
//			verts.push_back(p[0]);
//			verts.push_back(p[1]);
//			verts.push_back(p[2]);
//		}
//	}
//
//	for (const auto& face : _sourceElement->faces()) {
//		faces.push_back((uint32_t)face.a);
//		faces.push_back((uint32_t)face.b);
//		faces.push_back((uint32_t)face.c);
//	}
//
//	vhacd->Compute(verts.data(), verts.size()/3,
//				   faces.data(), faces.size()/3,
//				   params);
//
//	while (!vhacd->IsReady()) {
//		log::i()("VHACD not ready...");
//	}
//
//	auto numHulls = vhacd->GetNConvexHulls();
//
//	auto decomposedElements = vector<unique_ptr<MeshElement>>();
//	decomposedElements.reserve(numHulls);
//
//	for (int h=0; h<numHulls; ++h) {
//		VHACD::IVHACD::ConvexHull hull;
//		vhacd->GetConvexHull(h, hull);
//
//		auto hacdVerts = hull.m_points;
//		auto hacdFaces = hull.m_triangles;
//
//		auto decomposedVerts = vector<VertexPNT>();
//		decomposedVerts.reserve(hacdVerts.size());
//		auto decomposedFaces = vector<Face>();
//		decomposedFaces.reserve(hacdFaces.size());
//
//		for (auto& v : hacdVerts) {
//			decomposedVerts.push_back({{(float)v.mX, (float)v.mY, (float)v.mZ}, {}, {}});
//		}
//		for (auto& f : hacdFaces) {
//			decomposedFaces.push_back({(unsigned)f.mI0, (unsigned)f.mI1, (unsigned)f.mI2});
//		}
//
////		auto decomposedElement = make_unique<MeshElement>(decomposedVerts, decomposedFaces);
//
////		auto decomposedElement = make_unique<MeshElement>();
////		decomposedElement->setVertices(VertexLayout::PNT, decomposedVerts);
////		decomposedElement->setFaces(decomposedFaces);
////		decomposedElements.push_back(std::move(decomposedElement));
//
//		auto vb = std::as_bytes(std::span<const VertexPNT>(decomposedVerts.data(),
//														   decomposedVerts.size()));
//
//		auto decomposedElement2 = make_unique<MeshElement>(
//				VertexLayout::PNT,
//				vb,
//				(uint32_t)decomposedVerts.size(),
//				(uint16_t)sizeof(VertexPNT),
//				std::span<const Face>(decomposedFaces.data(), decomposedFaces.size()));
//	}
//
//	log::i()("numHulls: {}", numHulls);
//
////	_decomposedElements = decomposedElements;
//
//	return decomposedElements;
//}

vector<unique_ptr<MeshElement>> ConvexDecomposer::decompose() {

	VHACD::IVHACD* vhacd = CreateVHACD();

	int a3dFillModeUnderlying = magic_enum::enum_integer(_options.fillMode);
	VHACD::FillMode _vhacdFillMode = magic_enum::enum_value<VHACD::FillMode>(a3dFillModeUnderlying);

	VHACD::IVHACD::Parameters params = {
			nullptr,
			nullptr,
			nullptr,
			_options.maxConvexHulls,
			_options.resolution,
			_options.minVolumePercentErr,
			_options.maxRecursionDepth,
			_options.shrinkWrap,
			_vhacdFillMode, //_options.fillMode,
			_options.maxNumVerticesPerHull,
			false,//_options.asyncACD,
			_options.minEdgeLength,
			_options.findBestPlane
	};

	// Build input vertex float array and index array
	uint32_t vcount = 0;
	uint16_t stride = 0;
	uint16_t posOff = 0;
	const std::byte* vbase = nullptr;

	if (!GetPositionStream(*_sourceElement, vbase, stride, vcount, posOff)) {
		vhacd->Release();
		return {};
	}

	const auto& srcFaces = _sourceElement->faces();

	vector<float> verts;
	verts.reserve(3u * vcount);

	for (uint32_t i = 0; i < vcount; ++i) {
		float xyz[3];
		const std::byte* p = vbase + size_t(i) * size_t(stride) + posOff;
		memcpy(xyz, p, sizeof(xyz));
		verts.push_back(xyz[0]);
		verts.push_back(xyz[1]);
		verts.push_back(xyz[2]);
	}

	vector<uint32_t> indices;
	indices.reserve(3u * (uint32_t)srcFaces.size());
	for (const Face& f : srcFaces) {
		indices.push_back((uint32_t)f.a);
		indices.push_back((uint32_t)f.b);
		indices.push_back((uint32_t)f.c);
	}

	vhacd->Compute(verts.data(), verts.size() / 3,
				   indices.data(), indices.size() / 3,
				   params);

	// If Compute is async in your build, keep this but don't spam logs.
	while (!vhacd->IsReady()) {
		// std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}

	const auto numHulls = (int)vhacd->GetNConvexHulls();

	vector<unique_ptr<MeshElement>> out;
	out.reserve(numHulls);

	for (int h = 0; h < numHulls; ++h) {
		VHACD::IVHACD::ConvexHull hull;
		vhacd->GetConvexHull(h, hull);

		vector<VertexPNT> decomposedVerts;
		decomposedVerts.reserve(hull.m_points.size());

		for (auto& v : hull.m_points) {
			decomposedVerts.push_back({ {(float)v.mX, (float)v.mY, (float)v.mZ}, {}, {} });
		}

		vector<Face> decomposedFaces;
		decomposedFaces.reserve(hull.m_triangles.size());

		for (auto& t : hull.m_triangles) {
			decomposedFaces.push_back({ (uint32_t)t.mI0, (uint32_t)t.mI1, (uint32_t)t.mI2 });
		}

		auto vbSpan = std::span<const VertexPNT>(decomposedVerts.data(), decomposedVerts.size());
		auto vbBytes = std::as_bytes(vbSpan);

		out.push_back(std::make_unique<MeshElement>(
				VertexLayout::PNT,
				vbBytes,
				(uint32_t)decomposedVerts.size(),
				(uint16_t)sizeof(VertexPNT),
				std::span<const Face>(decomposedFaces.data(), decomposedFaces.size())
		));
	}

	vhacd->Release();
	return out;
}

/// Private Static Non-Member Functions ///

bool GetPositionStream(const MeshElement& e,
							  const std::byte*& base,
							  uint16_t& stride,
							  uint32_t& count,
							  uint16_t& posOffset) {

	if (e.vertexCount() == 0) return false;

	const VertexLayoutDesc& d = GetVertexLayoutDesc(e.vertexLayout());
	const VertexAttribDesc* posA = VertexAccess::FindAttrib(d, VertexSemantic::Position);
	if (!posA || posA->format != VertexFormat::F32x3) return false;

	base = e.vertexBytes().data();
	stride = e.vertexStride();
	count = e.vertexCount();
	posOffset = posA->offset;

	return true;
}
