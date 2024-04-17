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


class IVHACD;


namespace a3d {


	class MeshElement;


	class ConvexDecomposer {

/*********************************************************************************************
	Internal Types
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

/*********************************************************************************************
	Internal Lifecycle
 *********************************************************************************************/

		ConvexDecomposer(MeshElement& element,
						 Options& options);

		std::vector<std::unique_ptr<MeshElement>>	decompose();

/*********************************************************************************************
	Private IVars
 *********************************************************************************************/

	private:

		Options 										_options;
		MeshElement*									_sourceElement;
	};
}


#endif //AVARA3D_CONVEXDECOMPOSER_H
