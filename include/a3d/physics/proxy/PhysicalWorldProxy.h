//
//  PhysicalWorldProxy.h
//  avara3d
//
//  Created by Morgan Davis on 12/8/23.
//  Copyright © 2024 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_PHYSICALWORLDPROXY_H
#define AVARA3D_PHYSICALWORLDPROXY_H

#include <memory>
#include <vector>

#include "a3d/Types.h"
#include "a3d/profiling/FrameStats.h"

namespace a3d {

	class Line;
	class PhysicalWorld;
	class PhysicsBody;
	class Profiler;
	class Renderer;
	class RenderContext;

	class PhysicalWorldProxy {

	public:
		/// Internal Lifecycle Functions ///

		explicit PhysicalWorldProxy(PhysicalWorld& world);
		virtual ~PhysicalWorldProxy();

		/// Internal Member Functions ///

		virtual void 				add(PhysicsBody& body) = 0;
		virtual void 				remove(PhysicsBody& body) = 0;

		virtual float				gravity() const = 0;
		virtual void				gravity(float gravity) = 0;

		virtual void				step(double deltaT,
										 float speed,
										 float timestep,
										 FrameStats& stats,
										 Profiler& profiler) = 0;

		virtual void 				updateCollisionPairs() = 0;

		virtual void 				appendDebugLines(std::vector<Line>& out,
													 DebugOptions debugOptions) = 0;
	};
}

#endif //AVARA3D_PHYSICALWORLDPROXY_H
