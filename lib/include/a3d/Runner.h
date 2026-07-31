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

#include "a3d/SimulationConfig.h"
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

		struct UpdateInfo {

			// zero-based Runner-update index
			std::uint64_t updateIndex{0};

			// monotonic seconds since Runner::start(), measured at the
			// beginning of this Runner update
			double elapsedTime{0.0};

			// monotonic seconds since the beginning of the previous
			// Runner update. zero on the first update.
			double deltaTime{0.0};
		};

		using UpdateCallback = std::function<void(Runner &runner,
		                                          const UpdateInfo &info)>;

		/// Public Lifecycle Functions ///

		explicit Runner(Scene& scene,
		                SimulationConfig config = {});

		Runner(const Runner&) = delete;
		Runner& operator=(const Runner&) = delete;

		Runner(Runner&&) = delete;
		Runner& operator=(Runner&&) = delete;

		~Runner();

		/// Public Member Functions ///

		void						start();
		bool						update();
		void						stop();

		// a stopped Runner performs no updates. pausing affects automatic
		// simulation ticks only: Runner updates, input, runner callbacks,
		// and rendering continue
		bool						simulationPaused() const;
		void						pauseSimulation();
		void						resumeSimulation();

		// queue one fixed-duration tick for a running, paused simulation.
		// requested ticks use fixedDeltaTime and ignore timeScale.
		void						requestSimulationTick();

		UpdateCallback				updateCallback() const;
		void						updateCallback(UpdateCallback callback);

		const SimulationConfig&		simulationConfig() const;

		double						timeScale() const;
		void						timeScale(double value);

		double						simulationTime() const;
		std::uint64_t				simulationTickCount() const;

		State						state() const;

		Scene&						scene();
		const Scene&				scene() const;

	private:
		/// Private Types ///

		using Clock = std::chrono::steady_clock;

		/// Private Member Functions ///

		void						start(Clock::time_point now);
		bool						update(Clock::time_point now);

		void						validateSimulationConfig() const;
		PhysicsInventory			scheduleSimulation(const UpdateInfo &info,
					                                   FrameStats& stats);
		PhysicsInventory			executeRequestedSimulationTicks(FrameStats& stats);
		PhysicsInventory			executeSimulationTick();
		PhysicsInventory			currentPhysicsInventory();
		void						copyPhysicsInventory(const PhysicsInventory &inventory,
								                         FrameStats& stats);

		bool						renderFrame(const UpdateInfo& info, FrameStats& stats);

		/// Private Member Variables ///

		Scene&						_scene;
		State						_state;
		SimulationConfig			_simulationConfig;
		double						_timeScale;
		double						_simulationAccumulator;
		double						_simulationTime;
		std::uint64_t				_simulationTickCount;
		bool						_simulationPaused;
		std::uint64_t				_pendingSimulationTicks;
		bool						_suppressNextAutomaticSimulationUpdate;
		Clock::time_point			_startTime;
		Clock::time_point			_previousUpdateTime;
		UpdateInfo					_updateInfo;
		bool						_hasUpdated;
		UpdateCallback				_updateCallback;
		std::uint64_t				_completedRenderFrameCount;
		Profiler					_profiler;
		FrameStatsHistory			_frameStatsHistory;

		/// Test Access ///

		friend class testing::RunnerTestAccess;
	};
}

#endif //AVARA3D_RUNNER_H
