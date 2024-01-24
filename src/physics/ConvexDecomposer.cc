//
// Created by mkd on 11/5/23.
//

#include "ae/physics/ConvexDecomposer.h"

#include <utility>

#include "magic_enum.hpp"
#define ENABLE_VHACD_IMPLEMENTATION 1
#include "VHACD.h"

#include "ae/geometry/GeometryElement.h"
#include "ae/diagnostic/logging/Logger.h"


using namespace ae;
using namespace std;
using namespace VHACD;


/*********************************************************************************************
	Lifecycle
 *********************************************************************************************/

//ConvexDecomposer::ConvexDecomposer(vector<shared_ptr<GeometryElement>>&	elements,
//								   Options& options,
//								   bool async) {
//
//}

ConvexDecomposer::ConvexDecomposer(shared_ptr<GeometryElement> element,
								   Options& options):
		_sourceElement(element),
		_options(options) {
		//_vhacd(CreateVHACD()) {
}

/*********************************************************************************************
	Public
 *********************************************************************************************/

//void ConvexDecomposer::run() {
//
//	if (isAsync()) {
//		_vhacd = make_unique<VHACDAsyncImpl>(std::move(VHACD::CreateVHACD_ASYNC()));
//	}
//	else {
//		_vhacd = make_unique<VHACDImpl>(VHACD::CreateVHACD());
//	}
//}

vector<shared_ptr<GeometryElement>> ConvexDecomposer::decompose() {

	VHACD::IVHACD* vhacd = CreateVHACD();

	int aeFillModeUnderlying = magic_enum::enum_integer(_options.fillMode);
	VHACD::FillMode _vhacdFillMode = magic_enum::enum_value<VHACD::FillMode>(aeFillModeUnderlying);

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

	auto element = sourceElement();

	unsigned numVerts = 0;
	unsigned numFaces = 0;
//	for (auto& element : _sourceElements) {
		numVerts += element->vertices().size();
		numFaces += element->faces().size();
//	}

	auto verts = vector<float>();
	verts.reserve((sizeof(float)*3) * numVerts);
	auto faces = vector<uint32_t>();
	faces.reserve((sizeof(uint32_t)*3) * numFaces);

//	for (auto& element : _sourceElements) {
		for (const auto& vert : element->vertices()) {
			verts.push_back(vert.position.x);
			verts.push_back(vert.position.y);
			verts.push_back(vert.position.z);
		}
		for (const auto& face : element->faces()) {
			faces.push_back((uint32_t)face.a);
			faces.push_back((uint32_t)face.b);
			faces.push_back((uint32_t)face.c);
		}
//	}

	vhacd->Compute(verts.data(), verts.size()/3,
				   faces.data(), faces.size()/3,
				   params);

	while (!vhacd->IsReady()) {
		AE_LOG_I("VHACD not ready...");
	}

	auto numHulls = vhacd->GetNConvexHulls();

	auto decomposedElements = vector<shared_ptr<GeometryElement>>();
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
			decomposedFaces.push_back({(int)f.mI0, (int)f.mI1, (int)f.mI2});
		}

		auto decomposedElement = make_shared<GeometryElement>(decomposedVerts, decomposedFaces);
		decomposedElements.push_back(decomposedElement);
	}

	AE_LOG_I("numHulls: {}", numHulls);

	_decomposedElements = decomposedElements;

	return decomposedElements;
}

//shared_ptr<GeometryElement> ConvexDecomposer::decompose() {
//
//	VHACD::IVHACD* vhacd = CreateVHACD();
//
//	int aeFillModeUnderlying = magic_enum::enum_integer(_options.fillMode);
//	VHACD::FillMode _vhacdFillMode = magic_enum::enum_value<VHACD::FillMode>(aeFillModeUnderlying);
//
//	VHACD::IVHACD::Parameters params = {
//			nullptr,
//			nullptr,
//			nullptr,
//			_options.maxConvexHulls,
//			_options.resolution,
//			_options.minimumVolumePercentErrorAllowed,
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
//	unsigned numVerts = 0;
//	unsigned numFaces = 0;
//	for (auto& element : _sourceElements) {
//		numVerts += element->vertices().size();
//		numFaces += element->faces().size();
//	}
//
//	auto verts = vector<float>();
//	verts.reserve((sizeof(float)*3) * numVerts);
//	auto faces = vector<uint32_t>();
//	faces.reserve((sizeof(uint32_t)*3) * numFaces);
//
//	for (auto& element : _sourceElements) {
//		for (auto& vert : element->vertices()) {
//			verts.push_back(vert.position.x);
//			verts.push_back(vert.position.y);
//			verts.push_back(vert.position.z);
//		}
//		for (auto& face : element->faces()) {
//			faces.push_back((uint32_t)face.a);
//			faces.push_back((uint32_t)face.b);
//			faces.push_back((uint32_t)face.c);
//		}
//	}
//
//	vhacd->Compute(verts.data(), verts.size()/3,
//				   faces.data(), faces.size()/3,
//				   params);
//
//	while (!vhacd->IsReady()) {
//		AE_LOG_I("VHACD not ready...");
//	}
//
//	auto numHulls = vhacd->GetNConvexHulls();
//
//	auto decomposedElements = vector<shared_ptr<GeometryElement>>();
//	decomposedElements.reserve(numHulls);
//
//	for (int h=0; h<numHulls; ++h) {
//		VHACD::IVHACD::ConvexHull hull;
//		vhacd->GetConvexHull(h, hull);
//
//		auto hacdVerts = hull.m_points;
//		auto hacdFaces = hull.m_triangles;
//
//		auto decomposedVerts = vector<Vertex>();
//		decomposedVerts.reserve(hacdVerts.size());
//		auto decomposedFaces = vector<Face>();
//		decomposedFaces.reserve(hacdFaces.size());
//
//		for (auto& v : hacdVerts) {
//			decomposedVerts.push_back({{(float)v.mX, (float)v.mY, (float)v.mZ}, {}, {}});
//		}
//		for (auto& f : hacdFaces) {
//			decomposedFaces.push_back({(int)f.mI0, (int)f.mI1, (int)f.mI2});
//		}
//
//		auto decomposedElement = make_shared<GeometryElement>(decomposedVerts, decomposedFaces);
//		decomposedElements.push_back(decomposedElement);
//	}
//
//	AE_LOG_I("numHulls: {}", numHulls);
//
//	return decomposedElement;
//}

shared_ptr<GeometryElement> ConvexDecomposer::sourceElement() const {
	return _sourceElement;
}

vector<shared_ptr<GeometryElement>> ConvexDecomposer::decomposedElements() const {
	return _decomposedElements;
}

//bool ConvexDecomposer::running() const {
//
//}
//
//bool ConvexDecomposer::isAsync() const {
//
//}
//
//std::vector<std::shared_ptr<GeometryElement>>& ConvexDecomposer::sourceElements() const {
//
//}
//
//std::vector<std::shared_ptr<GeometryElement>>& ConvexDecomposer::decomposedElements() const {
//
//}
//
//
//ConvexDecomposer::ProgressFunction ConvexDecomposer::progressCallback() const {
//
//}
//
//void ConvexDecomposer::progressCallback(ProgressFunction function) {
//
//}
//
//
//ConvexDecomposer::FinishedFunction ConvexDecomposer::finishedCallback() const {
//
//}
//
//void ConvexDecomposer::finishedCallback(FinishedFunction function) {
//
//}
//
//
//ConvexDecomposer::CanceledFunction ConvexDecomposer::canceledCallback() const {
//
//}
//
//void ConvexDecomposer::canceledCallback(CanceledFunction function) {
//
//}

///*********************************************************************************************
//	IVHACD::IUserCallback
// *********************************************************************************************/
//
//void ConvexDecomposer::Update(const double overallProgress,
//			const double stageProgress,
//			const char* const stage,
//			const char* operation) {
//
//}
//
//void ConvexDecomposer::NotifyVHACDComplete() {
//
//}
//
///*********************************************************************************************
//	IVHACD::IUserLogger
// *********************************************************************************************/
//
//void ConvexDecomposer::Log(const char* const msg) {
//
//}
