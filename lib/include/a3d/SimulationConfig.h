//
//  SimulationConfig.h
//  avara3d
//
//  Created by Morgan Davis on 7/27/26.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_SIMULATIONCONFIG_H
#define AVARA3D_SIMULATIONCONFIG_H

#include <cstdint>

namespace a3d {

	struct SimulationConfig {

		// constant duration of every automatic and requested simulation step
		double			timeStep{1.0 / 60.0};

		// maximum automatic steps performed during one Runner::update()
		std::uint32_t	maxCatchUpSteps{8};

		// initial automatic simulation-time rate relative to monotonic update
		// time. requested steps ignore timeScale.
		//
		// Runner owns the mutable runtime value.
		double			timeScale{1.0};
	};
}

#endif //AVARA3D_SIMULATIONCONFIG_H
