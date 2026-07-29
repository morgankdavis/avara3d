//
//  Runner.h
//  avara3d
//
//  Created by Morgan Davis on 7/10/26.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_RUNNER_H
#define AVARA3D_RUNNER_H

#include <chrono>
#include <cstdint>
#include <functional>

#include "a3d/Timing.h"
#include "a3d/profile/FrameStatsHistory.h"
#include "a3d/profile/Profiler.h"

#include "a3d/TestAccessFwd.h"

namespace a3d {

	struct FrameStats;
	struct PhysicsInventory;

	class Scene;

	class Runner {

	public:
		/// Public Types ///

		enum class State {
			Idle,
			Running,
			Stopped
		};

		using UpdateCallback = std::function<void(Runner& runner,
		                                          const HostUpdateInfo& info)>;

		/// Public Lifecycle Functions ///

		explicit Runner(Scene& scene,
		                SimulationConfiguration configuration = {});

		Runner(const Runner&) = delete;
		Runner& operator=(const Runner&) = delete;

		Runner(Runner&&) = delete;
		Runner& operator=(Runner&&) = delete;

		~Runner();

		/// Public Member Functions ///

		void start();
		bool update();
		void stop();

		UpdateCallback updateCallback() const;
		void updateCallback(UpdateCallback callback);

		const SimulationConfiguration& simulationConfiguration() const;

		double timeScale() const;
		void timeScale(double value);

		double simulationTime() const;
		std::uint64_t simulationTickCount() const;

		State state() const;

		Scene& scene();
		const Scene& scene() const;

	private:
		/// Private Types ///

		using Clock = std::chrono::steady_clock;

		/// Private Member Functions ///

		void start(Clock::time_point now);
		bool update(Clock::time_point now);

		void validateSimulationConfiguration() const;
		PhysicsInventory scheduleSimulation(const HostUpdateInfo& info,
		                                    FrameStats& stats);
		PhysicsInventory runSimulationTick(double deltaTime);
		void copyPhysicsInventory(const PhysicsInventory& inventory,
		                          FrameStats& stats);

		RenderFrameInfo makeRenderFrameInfo(const HostUpdateInfo& info) const;
		bool renderFrame(const HostUpdateInfo& info, FrameStats& stats);

		/// Private Member Variables ///

		Scene&				_scene;
		State				_state;
		SimulationConfiguration
							_simulationConfiguration;
		double				_timeScale;
		double				_simulationAccumulator;
		double				_simulationTime;
		std::uint64_t		_simulationTickCount;
		Clock::time_point	_startTime;
		Clock::time_point	_previousUpdateTime;
		HostUpdateInfo		_hostUpdateInfo;
		bool				_hasUpdated;
		UpdateCallback		_updateCallback;
		std::uint64_t		_completedRenderFrameCount;
		Profiler			_profiler;
		FrameStatsHistory	_frameStatsHistory;

		/// Test Access ///

		friend class testing::RunnerTestAccess;
	};
}

#endif //AVARA3D_RUNNER_H
