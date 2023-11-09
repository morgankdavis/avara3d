//
// Created by mkd on 11/5/23.
//

#include "physics/ConvexDecomposer.h"

#include <utility>

#include "magic_enum-0.9.3/include/magic_enum.hpp"
#define ENABLE_VHACD_IMPLEMENTATION 1
#include "v-hacd-4.1.0/VHACD.h"

#include "geometry/GeometryElement.h"
#include "diagnostic/logging/Logger.h"


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

ConvexDecomposer::ConvexDecomposer(vector<shared_ptr<GeometryElement>>&	elements,
								   Options& options):
		_sourceElements(elements),
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

vector<shared_ptr<GeometryElement>>	ConvexDecomposer::decompose() {

	VHACD::IVHACD* vhacd = CreateVHACD();
//	virtual bool Compute(const float* const points,
//						 const uint32_t countPoints,
//						 const uint32_t* const triangles,
//						 const uint32_t countTriangles,
//						 const Parameters& params) = 0;

//class Parameters
//{
//	public:
//	IUserCallback*      m_callback{nullptr};            // Optional user provided callback interface for progress
//	IUserLogger*        _logger{nullptr};              // Optional user provided callback interface for log messages
//	IUserTaskRunner*    m_taskRunner{nullptr};          // Optional user provided interface for creating tasks
//	uint32_t            m_maxConvexHulls{ 64 };         // The maximum number of convex hulls to produce
//	uint32_t            m_resolution{ 400000 };         // The voxel resolution to use
//	double              m_minimumVolumePercentErrorAllowed{ 1 }; // if the voxels are within 1% of the volume of the hull, we consider this a close enough approximation
//	uint32_t            m_maxRecursionDepth{ 10 };        // The maximum recursion depth
//	bool                m_shrinkWrap{true};             // Whether or not to shrinkwrap the voxel positions to the source mesh on output
//	FillMode            m_fillMode{ FillMode::FLOOD_FILL }; // How to fill the interior of the voxelized mesh
//	uint32_t            m_maxNumVerticesPerCH{ 64 };    // The maximum number of vertices allowed in any output convex hull
//	bool                m_asyncACD{ true };             // Whether or not to run asynchronously, taking advantage of additional cores
//	uint32_t            m_minEdgeLength{ 2 };           // Once a voxel patch has an edge length of less than 4 on all 3 sides, we don't keep recursing
//	bool                m_findBestPlane{ false };       // Whether or not to attempt to split planes along the best location. Experimental feature. False by default.
//};

	//int ogFillModeUnderlying = to_underlying<VHACD::FillMode>()
	//int aeFillModeUnderlying = static_cast<std::underlying_type<FILL_MODE>::type>(_options.fillMode);
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

	unsigned numVerts = 0;
	unsigned numFaces = 0;
	for (auto& element : _sourceElements) {
		numVerts += element->vertices().size();
		numFaces += element->faces().size();
	}

	auto verts = vector<float>();
	verts.reserve((sizeof(float)*3) * numVerts);
	auto faces = vector<uint32_t>();
	faces.reserve((sizeof(uint32_t)*3) * numFaces);

	for (auto& element : _sourceElements) {
		for (auto& vert : element->vertices()) {
			verts.push_back(vert.position.x);
			verts.push_back(vert.position.y);
			verts.push_back(vert.position.z);
		}
		for (auto& face : element->faces()) {
			faces.push_back((uint32_t)face.a);
			faces.push_back((uint32_t)face.b);
			faces.push_back((uint32_t)face.c);
		}
	}

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

//		sm.mIndices = (const uint32_t *)&ch.m_triangles[0];
//		sm.mVertices = (const double *)&ch.m_points[0];
//		sm.mTriangleCount = uint32_t(ch.m_triangles.size());
//		sm.mVertexCount = uint32_t(ch.m_points.size());

//		std::vector<VHACD::Vertex>      m_points;
//		std::vector<VHACD::Triangle>    m_triangles;

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

	return decomposedElements;
}

//bool ConvexDecomposer::isRunning() const {
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
