//
//  FrameStats.h
//  avara3d
//
//  Created by Morgan Davis on 1/6/26.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_PROFILING_FRAMESTATE_H
#define AVARA3D_PROFILING_FRAMESTATE_H

#include <chrono>

namespace a3d {

	struct FrameStats {

		std::chrono::nanoseconds frameTime;
		std::chrono::nanoseconds engineCpuTime;
		std::chrono::nanoseconds renderCpuTime;
		std::chrono::nanoseconds renderGpuTime;
		std::chrono::nanoseconds physicsTime;
		std::chrono::nanoseconds applicationTime;

		unsigned numNodes;
		unsigned numMeshes;
		unsigned numElements;
		unsigned numPolygons;
		unsigned numLights;
		unsigned numStaticBodies;
		unsigned numDynamicBodies;
		unsigned numKinematicBodies;
		unsigned numPrimitiveShapes;
		unsigned numBoundingBoxShapes;
		unsigned numConvexHullShapes;
		unsigned numConcavePolyhedronShapes;
	};
}

#endif //AVARA3D_PROFILING_FRAMESTATE_H
