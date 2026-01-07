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

/// Internal Lifecycle Functions ///

ConvexDecomposer::ConvexDecomposer(MeshElement& element,
								   Options& options):
		_sourceElement{&element},
		_options{options} {
}

/// Internal Member Functions ///

vector<unique_ptr<MeshElement>> ConvexDecomposer::decompose() {

	log::d()("Performing convex decomposition...");

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

	auto posOpt = VertexAccess::GetPositionStreamView(*_sourceElement);
	if (!posOpt) {
		vhacd->Release();
		return {};
	}
	const VertexStreamView pos = *posOpt;

	const auto& srcFaces = _sourceElement->faces();

//	vector<float> verts;
//	verts.reserve(3u * pos.count);
//
//	for (uint32_t i = 0; i < pos.count; ++i) {
//		float xyz[3];
//		const std::byte* p = pos.base + size_t(i) * size_t(pos.stride) + pos.offset;
//		memcpy(xyz, p, sizeof(xyz));
//		verts.push_back(xyz[0]);
//		verts.push_back(xyz[1]);
//		verts.push_back(xyz[2]);
//	}

	vector<float> verts;
	verts.resize(3u * pos.count);

	for (uint32_t i = 0; i < pos.count; ++i) {
		const std::byte* p = pos.base + size_t(i) * size_t(pos.stride) + pos.offset;
		float xyz[3];
		memcpy(xyz, p, sizeof(xyz));
		verts[3u*i + 0] = xyz[0];
		verts[3u*i + 1] = xyz[1];
		verts[3u*i + 2] = xyz[2];
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

		auto vbSpan  = std::span<const VertexPNT>(decomposedVerts.data(), decomposedVerts.size());
		auto vbBytes = std::as_bytes(vbSpan);

		out.push_back(std::make_unique<MeshElement>(
				VertexLayout::PNT,
				vbBytes,
				(uint32_t)decomposedVerts.size(),
				(uint16_t)sizeof(VertexPNT),
				std::span<const Face>(decomposedFaces.data(), decomposedFaces.size())
		));
	}

	log::d()("Decomposition done.");

	vhacd->Release();

	return out;
}
