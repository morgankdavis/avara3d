//
//  Execution.h
//  avara3d
//
//  Created by Morgan Davis on 7/27/26.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_EXECUTION_H
#define AVARA3D_EXECUTION_H

#include <cstdint>

namespace a3d {

	struct HostUpdateInfo {

		// Zero-based number of this Runner::update() call.
		std::uint64_t	updateIndex{0};

		// Wall-clock seconds since Runner::start().
		double			elapsedTime{0.0};

		// Wall-clock seconds since the previous update, or zero on the first.
		double			deltaTime{0.0};
	};
}

#endif //AVARA3D_EXECUTION_H
