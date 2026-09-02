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
 * @brief Drives host updates, fixed-step simulation, and rendering for a Scene.
 *
 * A Runner converts elapsed host time into zero or more constant-duration
 * simulation steps according to SimulationConfig. Catch-up work is bounded by
 * maxCatchUpSteps(); excess accumulated simulation time is discarded while the
 * fractional remainder is retained.
 *
 * Automatic simulation stepping may be paused while host updates, input
 * processing, host-update and input callbacks, and rendering continue. While
 * paused, fixed-duration simulation steps may be requested explicitly.
 *
 * A Runner begins in State::Idle. start() may be called once; stop() places the
 * Runner in the terminal State::Stopped state.
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
     * @brief Callback invoked near the beginning of each host update.
     *
     * The callback runs before event polling, input update, simulation
     * scheduling, and rendering. It may change Runner state, including stopping
     * the Runner or pausing automatic simulation stepping.
     */
    using UpdateCallback = std::function<void(Runner& runner, const UpdateInfo& info)>;

    // [Public Lifecycle Functions]

        /**
     * @brief Creates an idle Runner for @p scene.
     *
     * The Runner retains @p scene by reference; the Scene must outlive the Runner.
     * The supplied configuration is copied and validated when start() is called.
     *
     * @param scene Scene whose input, simulation, and rendering are driven.
     * @param config initial simulation scheduling configuration.
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
     * Starting resets simulation progression and host-update counters to their
     * initial values from the Runner configuration.
     *
     * @throws std::logic_error if the Runner is not idle.
     * @throws std::invalid_argument if the initial simulation configuration is invalid.
     */
    void           start();

        /**
     * @brief Performs one host update using the current monotonic time.
     *
     * A running update invokes the host callback, polls events, updates input,
     * advances or explicitly steps the simulation as appropriate, and renders
     * when the Scene has a VisualWorld.
     *
     * @return true when the Runner remains running after the update; false when
     *         it was not running or stopped during the update.
     */
    bool           update();

        /**
     * @brief Permanently stops the Runner and clears pending requested steps.
     *
     * Calling stop() on an already stopped Runner has no effect.
     */
    void           stop();

        /** @brief Returns true when automatic simulation stepping is paused. */
    bool           simulationPaused() const;

        /**
     * @brief Pauses or resumes automatic simulation stepping.
     *
     * Host updates, input processing, host-update and input callbacks, and
     * rendering continue while paused. Scene simulation-step callbacks run only
     * when explicitly requested steps are executed. Pausing clears accumulated
     * automatic simulation time. Resuming clears pending requested steps and
     * discards the next host-time delta so time spent paused does not become
     * catch-up work.
     *
     * @param paused true to pause automatic simulation stepping; false to resume it.
     *
     * @throws std::logic_error if the Runner is not running.
     */
    void           simulationPaused(bool paused);

        /**
     * @brief Queues one fixed-duration simulation step while paused.
     *
     * Requested steps use timeStep() and are not scaled by timeScale(). Multiple
     * calls queue multiple steps for the next eligible host update.
     *
     * @throws std::logic_error if the Runner is not running or the simulation is not paused.
     * @throws std::overflow_error if the pending-step counter overflows.
     */
    void           requestSimulationStep();

        /**
     * @brief Resets simulation progression without changing scheduling settings.
     *
     * Simulation time, completed step count, accumulated/discarded time, and
     * pending requested steps are reset. The current time step, maximum catch-up
     * count, time scale, and pause state are preserved.
     *
     * When automatic stepping is active, the next host-time delta is discarded.
     *
     * @throws std::logic_error if the Runner is not running.
     */
    void           resetSimulation();

        /** @brief Returns the currently installed host update callback. */
    UpdateCallback updateCallback() const;

        /** @brief Replaces the host update callback; an empty callback disables it. */
    void           updateCallback(UpdateCallback callback);

        /** @brief Returns the fixed simulation step duration in seconds. */
    double         timeStep() const;

        /**
     * @brief Changes the fixed simulation step duration.
     *
     * Existing accumulated simulation time is preserved in seconds.
     *
     * @throws std::invalid_argument if @p value is less than or equal to zero.
     */
    void           timeStep(double value);

        /** @brief Returns the maximum number of automatic catch-up steps permitted per host update. */
    std::uint32_t  maxCatchUpSteps() const;

        /**
     * @brief Changes the maximum number of automatic catch-up steps per host update.
     *
     * @throws std::invalid_argument if @p value is zero.
     */
    void           maxCatchUpSteps(std::uint32_t value);

        /** @brief Returns the scale applied to elapsed host time for automatic simulation stepping. */
    double         timeScale() const;

        /**
     * @brief Changes the scale applied to elapsed host time for automatic simulation stepping.
     *
     * Requested steps are not affected by this value.
     *
     * @throws std::invalid_argument if @p value is less than or equal to zero.
     */
    void           timeScale(double value);

        /** @brief Returns total simulated time in seconds completed by fixed simulation steps. */
    double         simulationTime() const;

        /** @brief Returns the total number of completed fixed simulation steps. */
    std::uint64_t  simulationStepCount() const;

        /** @brief Returns the current Runner lifecycle state. */
    State          state() const;

        /** @brief Returns the Scene driven by this Runner. */
    Scene&         scene();

        /** @brief Returns the Scene driven by this Runner. */
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

#endif // AVARA3D_RUNNER_H
