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

	enum class SimulationTiming {

		FrameDriven,

		FixedStep
	};

	struct SimulationConfiguration {

		// Selects automatic simulation scheduling.
		SimulationTiming timing{SimulationTiming::FrameDriven};

		// Constant simulation delta used by FixedStep.
		//
		// Existing physics applications must explicitly preserve their
		// current PhysicsWorld timestep when migrated.
		double fixedDeltaTime{1.0 / 60.0};

		// Maximum automatic FixedStep ticks performed during one
		// Runner::update().
		std::uint32_t maxCatchUpSteps{8};

		// Initial simulation-time rate relative to monotonic host time.
		//
		// Runner owns the mutable runtime value.
		double timeScale{1.0};

		// Maximum variable simulation delta accepted by FrameDriven.
		//
		// This caps automatic simulation advancement after a host stall,
		// suspended browser tab, debugger pause, or similar discontinuity.
		double maxFrameDrivenDeltaTime{0.25};
	};

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

	struct SimulationStepInfo {

		// Zero-based index of the simulation tick currently executing.
		std::uint64_t	tickIndex{0};

		// Simulation time immediately before the current tick.
		double			startTime{0.0};

		// Simulation time reached when the current tick completes.
		double			endTime{0.0};

		// Amount of simulation time advanced by this tick.
		//
		// FrameDriven:
		//     variable and capped
		//
		// FixedStep:
		//     exactly fixedDeltaTime
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

		// Time reached by the most recently completed simulation tick.
		double			simulationTime{0.0};

		// Total number of completed simulation ticks.
		std::uint64_t	completedSimulationTicks{0};
	};
}

#endif //AVARA3D_TIMING_H
