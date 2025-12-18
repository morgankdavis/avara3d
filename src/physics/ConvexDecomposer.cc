//
//  ConvexDecomposer.cc
//  avara3d
//
//  Created by Morgan Davis on 11/5/23.
//  Copyright © 2024 Morgan K Davis. All rights reserved.
//

#include "a3d/physics/ConvexDecomposer.h"

#include <utility>

#include "magic_enum/magic_enum.hpp"
#define ENABLE_VHACD_IMPLEMENTATION 1
#include "VHACD.h"

#include "a3d/mesh/MeshElement.h"
#include "a3d/diagnostic/log/Log.h"

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

	VHACD::IVHACD* vhacd = CreateVHACD();

	int a3dFillModeUnderlying = magic_enum::enum_integer(_options.fillMode);
	VHACD::FillMode _vhacdFillMode = magic_enum::enum_value<VHACD::FillMode>(a3dFillModeUnderlying);

	VHACD::IVHACD::Parameters params = {
			nullptr,
			nullptr,
			nullptr,
			_options.maxConvexHulls,
			_options.resolution,
			_options.minimumVolumePercentErrorAllowed,
			_options.maxRecursionDepth,
			_options.shrinkWrap,
			_vhacdFillMode, //_options.fillMode,
			_options.maxNumVerticesPerHull,
			false,//_options.asyncACD,
			_options.minEdgeLength,
			_options.findBestPlane
	};

	// can probably be optimized...

	unsigned numVerts = 0;
	unsigned numFaces = 0;
//	for (auto& element : _sourceElements) {
		numVerts += _sourceElement->vertices().size();
		numFaces += _sourceElement->faces().size();
//	}

	auto verts = vector<float>();
	verts.reserve((sizeof(float)*3) * numVerts);
	auto faces = vector<uint32_t>();
	faces.reserve((sizeof(uint32_t)*3) * numFaces);

//	for (auto& element : _sourceElements) {
		for (const auto& vert : _sourceElement->vertices()) {
			verts.push_back(vert.position.x);
			verts.push_back(vert.position.y);
			verts.push_back(vert.position.z);
		}
		for (const auto& face : _sourceElement->faces()) {
			faces.push_back((uint32_t)face.a);
			faces.push_back((uint32_t)face.b);
			faces.push_back((uint32_t)face.c);
		}
//	}

	vhacd->Compute(verts.data(), verts.size()/3,
				   faces.data(), faces.size()/3,
				   params);

	while (!vhacd->IsReady()) {
		A3D_LOG_I("VHACD not ready...");
	}

	auto numHulls = vhacd->GetNConvexHulls();

	auto decomposedElements = vector<unique_ptr<MeshElement>>();
	decomposedElements.reserve(numHulls);

	for (int h=0; h<numHulls; ++h) {
		VHACD::IVHACD::ConvexHull hull;
		vhacd->GetConvexHull(h, hull);

		auto hacdVerts = hull.m_points;
		auto hacdFaces = hull.m_triangles;

		auto decomposedVerts = vector<Vertex>();
		decomposedVerts.reserve(hacdVerts.size());
		auto decomposedFaces = vector<Face>();
		decomposedFaces.reserve(hacdFaces.size());

		for (auto& v : hacdVerts) {
			decomposedVerts.push_back({{(float)v.mX, (float)v.mY, (float)v.mZ}, {}, {}});
		}
		for (auto& f : hacdFaces) {
			decomposedFaces.push_back({(unsigned)f.mI0, (unsigned)f.mI1, (unsigned)f.mI2});
		}

		auto decomposedElement = make_unique<MeshElement>(decomposedVerts, decomposedFaces);
		decomposedElements.push_back(std::move(decomposedElement));
	}

	A3D_LOG_I("numHulls: {}", numHulls);

//	_decomposedElements = decomposedElements;

	return decomposedElements;
}
