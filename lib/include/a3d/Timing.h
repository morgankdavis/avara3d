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
}

#endif //AVARA3D_TIMING_H
