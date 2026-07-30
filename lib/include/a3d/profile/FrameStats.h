//
//  FrameStats.h
//  avara3d
//
//  Created by Morgan Davis on 1/6/26.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_PROFILE_FRAMESTATE_H
#define AVARA3D_PROFILE_FRAMESTATE_H

#include <chrono>

namespace a3d {

	struct FrameStats {

		bool isRenderGpuTimeAvailable = false;

		std::chrono::nanoseconds frameTime{};
		std::chrono::nanoseconds engineCpuTime{};
		std::chrono::nanoseconds renderCpuTime{};
		std::chrono::nanoseconds renderGpuTime{};
		std::chrono::nanoseconds physicsTime{};
		std::chrono::nanoseconds applicationTime{};

		// simulation ticks completed during this Runner update
		unsigned simulationTickCount{0};

		// scaled simulation-time demand discarded during this Runner update
		double discardedSimulationTime{0.0};

		unsigned numNodes = 0;
		unsigned numMeshes = 0;
		unsigned numElements = 0;
		unsigned numPolygons = 0;
		unsigned numLights = 0;
		unsigned numStaticBodies = 0;
		unsigned numDynamicBodies = 0;
		unsigned numKinematicBodies = 0;
		unsigned numPrimitiveShapes = 0;
		unsigned numBoundingBoxShapes = 0;
		unsigned numConvexHullShapes = 0;
		unsigned numConcavePolyhedronShapes = 0;
	};
}

#endif //AVARA3D_PROFILE_FRAMESTATE_H
