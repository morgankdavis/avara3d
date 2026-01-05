//
//  DebugLinesBuilder.h
//  avara3d
//
//  Created by Morgan Davis on 1/4/26.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_RENDER_DEBUGLINESBUILDER_H
#define AVARA3D_RENDER_DEBUGLINESBUILDER_H

#include <vector>

#include "a3d/Types.h"

namespace a3d {

	class Line;

	class DebugLinesBuilder {

	public:
		/// Internal Static Member Functions ///

		static void AppendAABB(std::vector<Line>& out,
							   const AABB& aabb,
							   const Color& color);
		static void AppendOBBFromLocalAABB(std::vector<Line>& out,
										   const AABB& local,
										   const math::mat4& model,
										   const Color& color);
	};
}

#endif //AVARA3D_RENDER_DEBUGLINESBUILDER_H
