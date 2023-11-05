//
// Created by mkd on 11/5/23.
//

#ifndef ConvexDecomposer_h
#define ConvexDecomposer_h


#include <functional>
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

		enum class FillMode {
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
			FillMode	fillMode {FillMode::FLOOD_FILL};
			uint32_t	maxNumVerticesPerCH {64};
//			bool		asyncACD {true};
			uint32_t	minEdgeLength {2};
			bool		findBestPlane {false};
		};

		using ProgressFunction = std::function<void(ConvexDecomposer& decomposer, float progress)>;
		using FinishedFunction = std::function<void(ConvexDecomposer& decomposer)>;
		using CanceledFunction = std::function<void(ConvexDecomposer& decomposer)>;

/*********************************************************************************************
	Lifecycle
 *********************************************************************************************/

		ConvexDecomposer(std::vector<std::shared_ptr<GeometryElement>>& elements,
						 Options& options,
						 bool runAsync);

/*********************************************************************************************
	Public
 *********************************************************************************************/

		void 									run();
		bool  									isRunning() const;
		bool  									isAsync() const;

		std::vector<std::shared_ptr<GeometryElement>>&		sourceElements() const;
		std::vector<std::shared_ptr<GeometryElement>>&		decomposedElements() const;

		ProgressFunction 						progressCallback() const;
		void 									progressCallback(ProgressFunction function);

		FinishedFunction 						finishedCallback() const;
		void 									finishedCallback(FinishedFunction function);

		CanceledFunction 						canceledCallback() const;
		void 									canceledCallback(CanceledFunction function);

/*********************************************************************************************
	IVHACD::IUserCallback
 *********************************************************************************************/

		void Update(const double overallProgress,
					const double stageProgress,
					const char* const stage,
					const char* operation) override;
		void NotifyVHACDComplete() override;

/*********************************************************************************************
	IVHACD::IUserLogger
 *********************************************************************************************/

		void Log(const char* const msg) override;

/*********************************************************************************************
	Private
 *********************************************************************************************/

	private:

		std::vector<std::shared_ptr<GeometryElement>>		_sourceElements;
		std::vector<std::shared_ptr<GeometryElement>>		_decomposedElements;

		bool												_isRunning;
		bool												_isAsync;

		ProgressFunction									_progressFunction;
		FinishedFunction									_finishedFunction;
		CanceledFunction 									_canceledFunction;
	};
}


#endif //ConvexDecomposer_h
