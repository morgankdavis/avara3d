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

    /**
     * @brief Owns a Scene's host update loop and fixed-step simulation schedule.
     *
     * A Runner converts elapsed host time into zero or more constant-duration
     * simulation steps, applies bounded catch-up through SimulationConfig, and
     * renders the Scene after simulation scheduling completes. Host updates and
     * rendering continue while automatic simulation stepping is paused.
     *
     * A Runner begins in State::Idle, may be started once, and enters the
     * terminal State::Stopped when stop() is called or the update callback stops
     * it.
     *
     * @see SimulationConfig
     */
    class Runner {

    public:
        // [Public Types]

        /** @brief Runner lifecycle states. */
        enum class State {
            Idle,    ///< Constructed but not yet started.
            Running, ///< Accepting host updates and scheduling simulation work.
            Stopped  ///< Permanently stopped; the Runner cannot be restarted.
        };

        /** @brief Timing information supplied to each host update callback. */
        struct UpdateInfo {

            /** Zero-based Runner update index. */
            std::uint64_t updateIndex {0};

            /** Monotonic seconds since start(), sampled at this update's start. */
            double        elapsedTime {0.0};

            /**
             * Monotonic seconds since the previous update's start.
             *
             * This value is zero on the first update.
             */
            double        deltaTime {0.0};
        };

        /**
         * @brief Callback invoked once near the beginning of each host update.
         *
         * The callback runs before event polling, input update, simulation
         * scheduling, and rendering. It may change Runner state, including
         * stopping or pausing the simulation.
         */
        using UpdateCallback = std::function<void(Runner& runner, const UpdateInfo& info)>;

        // [Public Lifecycle Functions]

        /**
         * @brief Creates an idle Runner for a Scene.
         *
         * @param scene Scene whose input, simulation, and rendering are driven.
         * @param config Fixed-step scheduling configuration copied by the Runner.
         */
        explicit Runner(Scene& scene, SimulationConfig config = {});

        Runner(const Runner&)            = delete;
        Runner& operator=(const Runner&) = delete;

        Runner(Runner&&)            = delete;
        Runner& operator=(Runner&&) = delete;

        ~Runner();

        // [Public Member Functions]

        /**
         * @brief Starts the Runner using the current monotonic time.
         *
         * @throws std::logic_error if the Runner is not idle.
         * @throws std::invalid_argument if the configuration is invalid or the
         *         Scene's PhysicsWorld rejects the configured time step.
         */
        void                    start();

        /**
         * @brief Performs one host update using the current monotonic time.
         *
         * @return true when the Runner remains running after the update; false
         *         when it was not running or stopped during the update.
         */
        bool                    update();

        /** @brief Permanently stops the Runner and clears requested steps. */
        void                    stop();

        /** @return true when automatic simulation stepping is paused. */
        bool                    simulationPaused() const;

        /**
         * @brief Pauses automatic simulation stepping.
         *
         * Host updates, input processing, callbacks, and rendering continue.
         *
         * @throws std::logic_error if the Runner is not running.
         */
        void                    pauseSimulation();

        /**
         * @brief Resumes automatic simulation stepping.
         *
         * The first update after resuming discards its host-time delta so time
         * spent paused does not become catch-up work.
         *
         * @throws std::logic_error if the Runner is not running.
         */
        void                    resumeSimulation();

        /**
         * @brief Queues one fixed-duration step for a running, paused simulation.
         *
         * Requested steps use SimulationConfig::timeStep and ignore timeScale().
         *
         * @throws std::logic_error if the Runner is not running or the simulation
         *         is not paused.
         * @throws std::overflow_error if the pending-step counter overflows.
         */
        void                    requestSimulationStep();

        /** @return the currently installed host update callback. */
        UpdateCallback          updateCallback() const;

        /** @brief Replaces the host update callback. */
        void                    updateCallback(UpdateCallback callback);

        /** @return the immutable scheduling configuration copied at construction. */
        const SimulationConfig& config() const;

        /** @return the current automatic simulation-time scale. */
        double                  timeScale() const;

        /**
         * @brief Changes the automatic simulation-time scale.
         *
         * Requested single steps ignore this value.
         *
         * @throws std::invalid_argument if value is non-finite or not positive.
         */
        void                    timeScale(double value);

        /** @return total simulated seconds completed by fixed steps. */
        double                  simulationTime() const;

        /** @return total number of completed fixed simulation steps. */
        std::uint64_t           simulationStepCount() const;

        /** @return the current Runner lifecycle state. */
        State                   state() const;

        /** @return the Scene driven by this Runner. */
        Scene&                  scene();

        /** @return the Scene driven by this Runner. */
        const Scene&            scene() const;

    private:
        // [Private Types]

        using Clock     = std::chrono::steady_clock;
        using TimePoint = Clock::time_point;

        // [Private Member Functions]

        void              start(TimePoint now);
        bool              update(TimePoint now);

        PhysicsInventory  advanceSimulation(const UpdateInfo& info, FrameStats& stats);
        PhysicsInventory  executePendingSimulationSteps(FrameStats& stats);
        PhysicsInventory  executeSimulationStep();
        PhysicsInventory  currentPhysicsInventory();

        bool              renderFrame(const UpdateInfo& info, FrameStats& stats);

        // [Private Member Variables]

        Scene&            _scene;
        State             _state;
        SimulationConfig  _config;
        double            _timeScale;
        double            _simulationTimeAccumulator;
        double            _simulationTime;
        std::uint64_t     _simulationStepCount;
        double            _totalDiscardedSimulationTime;
        bool              _simulationPaused;
        std::uint64_t     _pendingSimulationSteps;
        bool              _skipNextUpdateDelta;
        TimePoint         _startTime;
        TimePoint         _prevUpdateTime;
        std::uint64_t     _updateCount;
        UpdateCallback    _updateCallback;
        std::uint64_t     _renderedFrameCount;
        Profiler          _profiler;
        FrameStatsHistory _frameStatsHistory;

        // [Test Access]

        friend class testing::RunnerTestAccess;
    };

}

#endif //AVARA3D_RUNNER_H
