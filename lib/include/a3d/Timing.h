//
//  Timing.h
//  avara3d
//
//  Created by Morgan Davis on 7/27/26.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_TIMING_H
#define AVARA3D_TIMING_H

#include <cstdint>

namespace a3d {

	struct HostUpdateInfo {

		// Zero-based number of this Runner::update() call.
		std::uint64_t	updateIndex{0};

		// Monotonic seconds since Runner::start(), measured at the beginning
		// of this host update.
		double			elapsedTime{0.0};

		// Monotonic seconds since the beginning of the previous host update,
		// or exactly zero on the first update.
		double			deltaTime{0.0};
	};

	struct RenderFrameInfo {

		// Zero-based index assigned to this attempted successful render
		// pipeline. The value is consumed only if rendering succeeds.
		std::uint64_t	frameIndex{0};

		// Host update that produced this render frame.
		std::uint64_t	hostUpdateIndex{0};

		// Monotonic seconds from Runner::start() to the beginning of the
		// current host update.
		double			hostTime{0.0};

		// Monotonic seconds between the beginnings of this host update and
		// the previous host update.
		double			hostDeltaTime{0.0};
	};
}

#endif //AVARA3D_TIMING_H
