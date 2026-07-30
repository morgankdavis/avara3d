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

	enum class SimulationTiming {

		VariableStep,

		FixedStep
	};

	struct SimulationConfig {

		// selects automatic simulation scheduling
		SimulationTiming timing{SimulationTiming::VariableStep};

		// sonstant simulation delta
		//
		// VariableStep: ignored
		// FixedStep:    used
		double fixedDeltaTime{1.0 / 60.0};

		// maximum automatic ticks performed during one Runner::update()
		//
		// VariableStep: ignored
		// FixedStep:    used
		std::uint32_t maxCatchUpSteps{8};

		// initial simulation-time rate relative to monotonic update time
		//
		// VariableStep: used
		// FixedStep:    used
		//
		// Runner owns the mutable runtime value.
		double timeScale{1.0};

		// maximum variable simulation delta.
		//
		// VariableStep: used
		// FixedStep:    ignored
		//
		// this caps automatic simulation advancement after an update stall,
		// suspended browser tab, debugger pause, or similar discontinuity
		double maxVariableStepDeltaTime{0.25};
	};
}

#endif //AVARA3D_SIMULATIONCONFIG_H
