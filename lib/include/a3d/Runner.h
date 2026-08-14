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
         * @param config Initial simulation scheduling configuration copied by the Runner.
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
         * @throws std::invalid_argument if the configuration is invalid.
         */
        void           start();

        /**
         * @brief Performs one host update using the current monotonic time.
         *
         * @return true when the Runner remains running after the update; false
         *         when it was not running or stopped during the update.
         */
        bool           update();

        /** @brief Permanently stops the Runner and clears requested steps. */
        void           stop();

        /** @return true when automatic simulation stepping is paused. */
        bool           simulationPaused() const;

        /**
         * @brief Pauses or resumes automatic simulation stepping.
         *
         * Host updates, input processing, callbacks, and rendering continue
         * while paused. Resuming discards the next host-time delta so time spent
         * paused does not become catch-up work.
         *
         * @param paused true to pause automatic simulation stepping; false to resume it.
         *
         * @throws std::logic_error if the Runner is not running.
         */
        void           simulationPaused(bool paused);

        /**
         * @brief Queues one fixed-duration step for a running, paused simulation.
         *
         * Requested steps use timeStep() and ignore timeScale().
         *
         * @throws std::logic_error if the Runner is not running or the simulation
         *         is not paused.
         * @throws std::overflow_error if the pending-step counter overflows.
         */
        void           requestSimulationStep();

        /**
         * @brief Resets simulation progression to its initial state.
         *
         * Simulation time, completed step count, accumulated/discarded time, and
         * pending requested steps are reset. The current time step, maximum catch-up
         * count, time scale, and pause state are preserved.
         *
         * @throws std::logic_error if the Runner is not running.
         */
        void           resetSimulation();

        /** @return the currently installed host update callback. */
        UpdateCallback updateCallback() const;

        /** @brief Replaces the host update callback. */
        void           updateCallback(UpdateCallback callback);

        /** @return the current simulation step duration in seconds. */
        double         timeStep() const;

        /**
         * @brief Changes the simulation step duration.
         *
         * Existing accumulated simulation time is preserved in seconds.
         *
         * @throws std::invalid_argument if value is non-finite or not positive.
         */
        void           timeStep(double value);

        /** @return the current maximum number of automatic catch-up steps per update. */
        std::uint32_t  maxCatchUpSteps() const;

        /**
         * @brief Changes the maximum number of automatic catch-up steps per update.
         *
         * @throws std::invalid_argument if value is zero.
         */
        void           maxCatchUpSteps(std::uint32_t value);

        /** @return the current automatic simulation-time scale. */
        double         timeScale() const;

        /**
         * @brief Changes the automatic simulation-time scale.
         *
         * Requested single steps ignore this value.
         *
         * @throws std::invalid_argument if value is non-finite or not positive.
         */
        void           timeScale(double value);

        /** @return total simulated seconds completed by fixed steps. */
        double         simulationTime() const;

        /** @return total number of completed fixed simulation steps. */
        std::uint64_t  simulationStepCount() const;

        /** @return the current Runner lifecycle state. */
        State          state() const;

        /** @return the Scene driven by this Runner. */
        Scene&         scene();

        /** @return the Scene driven by this Runner. */
        const Scene&   scene() const;

        // [Internal Member Functions]

        bool           simulationClockSuspended() const;
        void           simulationClockSuspended(bool suspended);

    private:
        // [Private Types]

        using Clock     = std::chrono::steady_clock;
        using TimePoint = Clock::time_point;

        // [Private Member Functions]

        void              start(TimePoint now);
        bool              update(TimePoint now);

        void              advanceSimulation(const UpdateInfo& info, FrameStats& stats);
        void              executePendingSimulationSteps(FrameStats& stats);
        void              executeSimulationStep();

        bool              renderFrame(const UpdateInfo& info, FrameStats& stats);

        // [Private Member Variables]

        Scene&            _scene;
        State             _state;
        SimulationConfig  _config;
        double            _timeStep;
        std::uint32_t     _maxCatchUpSteps;
        double            _timeScale;
        double            _simulationTimeAccumulator;
        double            _simulationTime;
        std::uint64_t     _simulationStepCount;
        double            _totalDiscardedSimulationTime;
        bool              _simulationPaused;
        std::uint64_t     _pendingSimulationSteps;
        bool              _skipNextUpdateDelta;
        bool              _simulationClockSuspended;
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
