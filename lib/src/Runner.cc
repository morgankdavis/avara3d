//
//  Runner.cc
//  avara3d
//
//  Created by Morgan Davis on 7/10/26.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#include "a3d/Runner.h"

#include <cmath>
#include <limits>
#include <stdexcept>
#include <utility>

#include "a3d/input/InputContext.h"
#include "a3d/physics/PhysicsWorld.h"
#include "a3d/profile/FrameStats.h"
#include "a3d/profile/Profile.h"
#include "a3d/scene/Scene.h"
#include "a3d/visual/VisualWorld.h"

using namespace std;

namespace a3d {

namespace {

    // [Private Constants]

    // duration of sample history to keep
    constexpr std::chrono::milliseconds FRAME_STATS_HISTORY_DURATION {3000};

} // namespace

// [Public Lifecycle Functions]

Runner::Runner(Scene& scene, SimulationConfig config):
    _scene(scene),
    _state(State::Idle),
    _config {config},
    _timeStep {config.timeStep},
    _maxCatchUpSteps {config.maxCatchUpSteps},
    _timeScale {config.timeScale},
    _simulationTimeAccumulator {0.0},
    _simulationTime {0.0},
    _simulationStepCount {0},
    _totalDiscardedSimulationTime {0.0},
    _simulationPaused {false},
    _pendingSimulationSteps {0},
    _skipNextUpdateDelta {false},
    _simulationClockSuspended {false},
    _startTime {},
    _prevUpdateTime {},
    _updateCount {0},
    _updateCallback {},
    _renderedFrameCount {0},
    _profiler {},
    _frameStatsHistory {FRAME_STATS_HISTORY_DURATION} {}

Runner::~Runner() {
    stop();
}

// [Public Member Functions]

void Runner::start() {
    start(Clock::now());
}

bool Runner::update() {

    if (_state != State::Running) {
        return false;
    }

    return update(Clock::now());
}

void Runner::stop() {

    if (_state == State::Stopped) {
        return;
    }

    _state = State::Stopped;
    _pendingSimulationSteps = 0;
}

bool Runner::simulationPaused() const {
    return _simulationPaused;
}

void Runner::simulationPaused(bool paused) {

    if (_state != State::Running) {
        throw logic_error("Runner::simulationPaused() requires a running Runner.");
    }

    if (_simulationPaused == paused) {
        return;
    }

    _simulationPaused = paused;

    if (paused) {
        _simulationTimeAccumulator = 0.0;
    }
    else {
        _pendingSimulationSteps = 0;
        _skipNextUpdateDelta = true;
    }
}

void Runner::requestSimulationStep() {

    if (_state != State::Running) {
        throw logic_error("Runner::requestSimulationStep() requires a running Runner.");
    }

    if (!_simulationPaused) {
        throw logic_error("Runner::requestSimulationStep() requires paused simulation.");
    }

    if (_pendingSimulationSteps == numeric_limits<std::uint64_t>::max()) {
        throw overflow_error("Runner pending simulation-step count overflow.");
    }

    ++_pendingSimulationSteps;
}

void Runner::resetSimulation() {
    if (_state != State::Running) {
        throw logic_error("Runner::resetSimulation() requires a running Runner.");
    }

    _simulationTimeAccumulator = 0.0;
    _simulationTime = 0.0;
    _simulationStepCount = 0;
    _totalDiscardedSimulationTime = 0.0;
    _pendingSimulationSteps = 0;

    if (!_simulationPaused) {
        _skipNextUpdateDelta = true;
    }
}

Runner::UpdateCallback Runner::updateCallback() const {
    return _updateCallback;
}

void Runner::updateCallback(UpdateCallback callback) {
    _updateCallback = callback;
}

double Runner::timeStep() const {
    return _timeStep;
}

void Runner::timeStep(double value) {

    if (value <= 0.0) {
        throw invalid_argument("Runner time step must be positive.");
    }

    _timeStep = value;
}

uint32_t Runner::maxCatchUpSteps() const {
    return _maxCatchUpSteps;
}

void Runner::maxCatchUpSteps(uint32_t value) {

    if (value == 0) {
        throw invalid_argument("Runner maximum catch-up steps must be at least one.");
    }

    _maxCatchUpSteps = value;
}

double Runner::timeScale() const {
    return _timeScale;
}

void Runner::timeScale(double value) {

    if (value <= 0.0) {
        throw invalid_argument("Runner time scale must be positive.");
    }

    _timeScale = value;
}

double Runner::simulationTime() const {
    return _simulationTime;
}

uint64_t Runner::simulationStepCount() const {
    return _simulationStepCount;
}

Runner::State Runner::state() const {
    return _state;
}

Scene& Runner::scene() {
    return _scene;
}

const Scene& Runner::scene() const {
    return _scene;
}

// [Internal Member Functions]

bool Runner::simulationClockSuspended() const {
    return _simulationClockSuspended;
}

void Runner::simulationClockSuspended(bool suspended) {

    if (_state != State::Running) {
        throw logic_error("Runner::simulationClockSuspended() requires a running Runner.");
    }

    if (_simulationClockSuspended == suspended) {
        return;
    }

    _simulationClockSuspended = suspended;

    if (!suspended) {
        _skipNextUpdateDelta = true;
    }
}

// [Private Member Functions]

void Runner::start(TimePoint now) {

    if (_state != State::Idle) {
        throw logic_error("Runner::start() requires an idle Runner.");
    }

    if (_config.timeScale <= 0.0) {
        throw invalid_argument("Runner requires a positive time scale.");
    }

    if (_config.timeStep <= 0.0) {
        throw invalid_argument("Runner requires a positive fixed simulation delta.");
    }

    if (_config.maxCatchUpSteps == 0) {
        throw invalid_argument("Runner requires at least one catch-up step.");
    }

    _timeStep = _config.timeStep;
    _maxCatchUpSteps = _config.maxCatchUpSteps;
    _timeScale = _config.timeScale;
    _simulationTimeAccumulator = 0.0;
    _simulationTime = 0.0;
    _simulationStepCount = 0;
    _totalDiscardedSimulationTime = 0.0;
    _simulationPaused = false;
    _pendingSimulationSteps = 0;
    _skipNextUpdateDelta = false;
    _simulationClockSuspended = false;
    _startTime = now;
    _prevUpdateTime = now;
    _updateCount = 0;
    _renderedFrameCount = 0;
    _state = State::Running;
}

bool Runner::update(TimePoint now) {

    if (_state != State::Running) {
        return false;
    }

    const UpdateInfo updateInfo {.updateIndex = _updateCount,
                                 .elapsedTime = chrono::duration<double>(now - _startTime).count(),
                                 .deltaTime = _updateCount != 0
                                                  ? chrono::duration<double>(now - _prevUpdateTime).count()
                                                  : 0.0};

    _prevUpdateTime = now;
    ++_updateCount;

    FrameStats stats {};
    stats.simulationStepCount = _simulationStepCount;
    stats.simulationTime = _simulationTime;
    stats.totalDiscardedSimulationTime = _totalDiscardedSimulationTime;

    prof::profile(_profiler, Profiler::Tag::Frame, [&] {
        if (_state == State::Running) {
            if (auto callback = updateCallback()) {
                prof::profile(_profiler, Profiler::Tag::Application, [&] {
                    callback(*this, updateInfo);
                });
            }
        }

        _scene.pollEvents(_profiler);

        const InputContext::UpdateInfo inputInfo {.updateIndex = updateInfo.updateIndex,
                                                  .elapsedTime = updateInfo.elapsedTime,
                                                  .deltaTime = updateInfo.deltaTime};

        _scene.updateInput(inputInfo, _profiler);

        stats.simulationTimeStep = _timeStep;
        stats.maxCatchUpSteps = _maxCatchUpSteps;

        if (_state == State::Running) {

            if (!_simulationClockSuspended) {
                if (_simulationPaused) {
                    // a new paused scheduling boundary supersedes suppression
                    // from an earlier resume/pause sequence. a resume that occurs
                    // during the requested batch sets this again and interrupts
                    // the local snapshot
                    _skipNextUpdateDelta = false;
                    executePendingSimulationSteps(stats);
                }
                else if (_skipNextUpdateDelta) {
                    _skipNextUpdateDelta = false;
                }
                else {
                    advanceSimulation(updateInfo, stats);
                }
            }

            prof::profile(_profiler, Profiler::Tag::EngineCpu, [&] {
                if (auto physicsWorld = _scene.physicsWorld()) {
                    const auto inventory = physicsWorld->inventory();
                    stats.staticBodies = inventory.staticBodies;
                    stats.dynamicBodies = inventory.dynamicBodies;
                    stats.kinematicBodies = inventory.kinematicBodies;
                    stats.activeContacts = inventory.activeContacts;
                    stats.primitiveShapes = inventory.primitiveShapes;
                    stats.boundingBoxShapes = inventory.boundingBoxShapes;
                    stats.convexHullShapes = inventory.convexHullShapes;
                    stats.concavePolyhedronShapes = inventory.concavePolyhedronShapes;
                }
            });

            stats.simulationStepCount = _simulationStepCount;
            stats.simulationTime = _simulationTime;
            stats.totalDiscardedSimulationTime = _totalDiscardedSimulationTime;

            if (_state == State::Running) {
                renderFrame(updateInfo, stats);
            }
        }
    });

    stats.frameTime = _profiler.time(Profiler::Tag::Frame);
    stats.engineCpuTime = _profiler.time(Profiler::Tag::EngineCpu);
    stats.renderCpuTime = _profiler.time(Profiler::Tag::RenderCpu);
    stats.renderGpuTime = _profiler.time(Profiler::Tag::RenderGpu);
    stats.physicsTime = _profiler.time(Profiler::Tag::Physics);
    stats.applicationTime = _profiler.time(Profiler::Tag::Application);

    _frameStatsHistory.add(stats);
    _profiler.reset();

    return _state == State::Running;
}

void Runner::advanceSimulation(const UpdateInfo& info, FrameStats& stats) {

    _simulationTimeAccumulator += info.deltaTime * _timeScale;

    while (_simulationTimeAccumulator >= _timeStep && stats.simulationStepsThisUpdate < _maxCatchUpSteps
           && _state == State::Running && !_simulationPaused && !_simulationClockSuspended
           && !_skipNextUpdateDelta) {

        const double timeStep = _timeStep;

        executeSimulationStep();

        if (_simulationPaused || _skipNextUpdateDelta) {
            _simulationTimeAccumulator = 0.0;
        }
        else {
            _simulationTimeAccumulator -= timeStep;
        }

        ++stats.simulationStepsThisUpdate;
    }

    if (_state == State::Running && !_simulationClockSuspended
        && stats.simulationStepsThisUpdate >= _maxCatchUpSteps && _simulationTimeAccumulator >= _timeStep) {
        const double remainder = fmod(_simulationTimeAccumulator, _timeStep);
        stats.discardedSimulationTime = _simulationTimeAccumulator - remainder;
        _totalDiscardedSimulationTime += stats.discardedSimulationTime;
        _simulationTimeAccumulator = remainder;
    }
}

void Runner::executePendingSimulationSteps(FrameStats& stats) {

    const auto pendingStepCount = std::exchange(_pendingSimulationSteps, std::uint64_t {0});

    for (uint64_t i = 0; i < pendingStepCount; ++i) {

        executeSimulationStep();
        ++stats.simulationStepsThisUpdate;

        if (_state != State::Running || !_simulationPaused || _simulationClockSuspended
            || _skipNextUpdateDelta) {
            break;
        }
    }
}

void Runner::executeSimulationStep() {

    const double timeStep = _timeStep;

    Scene::StepInfo info {.stepIndex = _simulationStepCount,
                          .startTime = _simulationTime,
                          .endTime = _simulationTime + timeStep,
                          .deltaTime = timeStep};

    _scene.stepSimulation(info, _profiler);

    _simulationTime = info.endTime;
    ++_simulationStepCount;
}

bool Runner::renderFrame(const UpdateInfo& info, FrameStats& stats) {

    auto visualWorld = _scene.visualWorld();
    if (!visualWorld) {
        return false;
    }

    const VisualWorld::RenderInfo renderInfo {.frameIndex = _renderedFrameCount,
                                              .updateIndex = info.updateIndex,
                                              .updateTime = info.elapsedTime,
                                              .updateDeltaTime = info.deltaTime,
                                              .simulationTime = _simulationTime,
                                              .simulationStepCount = _simulationStepCount};

    if (!visualWorld->draw(_scene, _scene.physicsWorld(), renderInfo, _scene.debugOptions(), stats, _profiler,
                           _frameStatsHistory)) {
        return false;
    }

    // this is kind of hack-y, but... the first rendered frame can take some time to upload
    // GPU resources, etc, which can cause a simulation step backlog. so just don't treat
    // that startup work as elapsed simulation time.
    if (_renderedFrameCount == 0) {
        _skipNextUpdateDelta = true;
    }

    ++_renderedFrameCount;
    return true;
}

} // namespace a3d
