//
// Created by mkd on 11/5/23.
//

#ifndef ConvexDecomposer_h
#define ConvexDecomposer_h


#include <memory>
#include <vector>

#define ENABLE_VHACD_IMPLEMENTATION TRUE
#include <VHACD.h>


namespace ae {


	class GeometryElement;


	class ConvexDecomposer:
			VHACD::IVHACD,
			VHACD::IVHACD::IUserCallback,
			VHACD::IVHACD::IUserLogger {

/*********************************************************************************************
	Types
 *********************************************************************************************/

	public:

		using ProgressFunction = std::function<void(ConvexDecomposer& decomposer, float progress)>;
		using FinishedFunction = std::function<void(ConvexDecomposer& decomposer)>;
		using CanceledFunction = std::function<void(ConvexDecomposer& decomposer)>;

/*********************************************************************************************
	Lifecycle
 *********************************************************************************************/

//		uint32_t            m_maxConvexHulls{ 64 };         // The maximum number of convex hulls to produce
//		uint32_t            m_resolution{ 400000 };         // The voxel resolution to use
//		double              m_minimumVolumePercentErrorAllowed{ 1 }; // if the voxels are within 1% of the volume of the hull, we consider this a close enough approximation
//		uint32_t            m_maxRecursionDepth{ 10 };        // The maximum recursion depth
//		bool                m_shrinkWrap{true};             // Whether or not to shrinkwrap the voxel positions to the source mesh on output
//		FillMode            m_fillMode{ FillMode::FLOOD_FILL }; // How to fill the interior of the voxelized mesh
//		uint32_t            m_maxNumVerticesPerCH{ 64 };    // The maximum number of vertices allowed in any output convex hull
//		bool                m_asyncACD{ true };             // Whether or not to run asynchronously, taking advantage of additional cores
//		uint32_t            m_minEdgeLength{ 2 };           // Once a voxel patch has an edge length of less than 4 on all 3 sides, we don't keep recursing
//		bool                m_findBestPlane{ false };

		ConvexDecomposer(std::vector<std::shared_ptr<GeometryElement>>&	elements);

/*********************************************************************************************
	Public
 *********************************************************************************************/

		void begin();
		bool isReady();
		void cancel();

		std::vector<std::shared_ptr<GeometryElement>>&		sourceElements();
		std::vector<std::shared_ptr<GeometryElement>>&		decomposedElements();

		ProgressFunction 						progressCallback() const;
		void 									progressCallback(ProgressFunction function);

		FinishedFunction 						finishedCallback() const;
		void 									finishedCallback(FinishedFunction function);

		CanceledFunction 						canceledCallback() const;
		void 									canceledCallback(CanceledFunction function);

/*********************************************************************************************
	Private
 *********************************************************************************************/

	private:

		std::vector<std::shared_ptr<GeometryElement>>		_sourceElements;
		std::vector<std::shared_ptr<GeometryElement>>		_decomposedElements;

		ProgressFunction									_progressFunction;
		FinishedFunction									_finishedFunction;
		CanceledFunction 									_canceledFunction;
	};
}


#endif //ConvexDecomposer_h
