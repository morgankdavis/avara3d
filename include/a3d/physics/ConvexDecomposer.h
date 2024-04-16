//
//  ConvexDecomposer.h
//  avara3d
//
//  Created by Morgan Davis on 11/5/23.
//  Copyright © 2024 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_CONVEXDECOMPOSER_H
#define AVARA3D_CONVEXDECOMPOSER_H


#include <functional>
#include <memory>
#include <vector>

////#define ENABLE_VHACD_IMPLEMENTATION TRUE
//#include <VHACD.h>


class IVHACD;


namespace a3d {


	class MeshElement;


//	class ConvexDecomposer:
//			VHACD::IVHACD,
//			VHACD::IVHACD::IUserCallback,
//			VHACD::IVHACD::IUserLogger {
	class ConvexDecomposer {

/*********************************************************************************************
	Types
 *********************************************************************************************/

	public:

		enum class FILL_MODE {
			FLOOD_FILL,
			SURFACE_ONLY,
			RAYCAST_FILL
		};

		struct Options {
			uint32_t	maxConvexHulls {64};
			uint32_t	resolution {400000};
			double		minimumVolumePercentErrorAllowed {1};
			uint32_t	maxRecursionDepth {10};
			bool		shrinkWrap {true};
			FILL_MODE	fillMode {FILL_MODE::FLOOD_FILL};
			uint32_t	maxNumVerticesPerHull {64};
//			bool		asyncACD {true};
			uint32_t	minEdgeLength {2};
			bool		findBestPlane {false};
		};

//		using ProgressFunction = std::function<void(ConvexDecomposer& decomposer, float progress)>;
//		using FinishedFunction = std::function<void(ConvexDecomposer& decomposer)>;
//		using CanceledFunction = std::function<void(ConvexDecomposer& decomposer)>;

/*********************************************************************************************
	Lifecycle
 *********************************************************************************************/

//		ConvexDecomposer(std::vector<std::shared_ptr<MeshElement>>& elements,
//						 Options& options,
//						 bool async);
//		ConvexDecomposer(std::vector<std::shared_ptr<MeshElement>>& elements,
//						 Options& options);
		ConvexDecomposer(MeshElement& element,
						 Options& options);

/*********************************************************************************************
	Public
 *********************************************************************************************/

		std::vector<std::unique_ptr<MeshElement>>	decompose();

//		void 									run();
//		bool  									running() const;
//		bool  									isAsync() const;
//
//		std::vector<std::shared_ptr<GeometryElement>>&		sourceElements() const;
//		std::vector<std::shared_ptr<GeometryElement>>&		decomposedElements() const;

//		MeshElement*								sourceElement() const;
//		std::vector<std::unique_ptr<MeshElement>>&	decomposedElements();
//
//		ProgressFunction 						progressCallback() const;
//		void 									progressCallback(ProgressFunction function);
//
//		FinishedFunction 						finishedCallback() const;
//		void 									finishedCallback(FinishedFunction function);
//
//		CanceledFunction 						canceledCallback() const;
//		void 									canceledCallback(CanceledFunction function);

/*********************************************************************************************
	IVHACD::IUserCallback
 *********************************************************************************************/

//		void Update(const double overallProgress,
//					const double stageProgress,
//					const char* const stage,
//					const char* operation) override;
//		void NotifyVHACDComplete() override;

/*********************************************************************************************
	IVHACD::IUserLogger
 *********************************************************************************************/

//		void Log(const char* const msg) override;

/*********************************************************************************************
	Private
 *********************************************************************************************/

	private:

		// TODO: make this unique_ptr
//		std::unique_ptr<IVHACD>							_vhacd;
//		IVHACD*											_vhacd;

		Options 										_options;

//		std::vector<std::shared_ptr<GeometryElement>>	_sourceElements;
//		std::vector<std::shared_ptr<GeometryElement>>	_decomposedElements;


		MeshElement*									_sourceElement;
//		std::vector<std::unique_ptr<MeshElement>>		_decomposedElements;



//		bool											_running;
//		bool											_isAsync;

//		ProgressFunction								_progressFunction;
//		FinishedFunction								_finishedFunction;
//		CanceledFunction 								_canceledFunction;
	};
}


#endif //AVARA3D_CONVEXDECOMPOSER_H
