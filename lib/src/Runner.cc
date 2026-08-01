//
//  Runner.cc
//  avara3d
//

#include "a3d/Runner.h"

#include <cmath>
#include <limits>
#include <stdexcept>
#include <utility>

#include "a3d/physics/PhysicsInventory.h"
#include "a3d/physics/PhysicsWorld.h"
#include "a3d/profile/FrameStats.h"
#include "a3d/profile/Profile.h"
#include "a3d/scene/Scene.h"
#include "a3d/visual/VisualWorld.h"

using namespace a3d;
using namespace std;

///  Private Constants ///

// duration of sample history to keep
static constexpr std::chrono::milliseconds	FRAME_STATS_HISTORY_DURATION {3000};

/// Public Lifecycle Functions ///

Runner::Runner(Scene& scene,
               SimulationConfig config):
	_scene(scene),
	_state(State::Idle),
	_config{config},
	_timeScale{config.timeScale},
	_simAccum{0.0},
	_simTime{0.0},
	_simStepCount{0},
	_paused{false},
	_pendingSteps{0},
	_suppressNextAutoUpdate{false},
	_startTime{},
	_prevUpdateTime{},
	_updateInfo{},
	_hasUpdated{false},
	_updateCallback{},
	_renderedFrameCount{0},
	_profiler{},
	_frameStatsHistory{FRAME_STATS_HISTORY_DURATION} {
}

Runner::~Runner() {
	stop();
}

/// Public Member Functions ///

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
	_pendingSteps = 0;
}

bool Runner::paused() const {
	return _paused;
}

void Runner::pause() {

	if (_state != State::Running) {
		throw logic_error("Runner::pauseSimulation() requires a running Runner.");
	}

	if (_paused) {
		return;
	}

	_paused = true;
	_simAccum = 0.0;
}

void Runner::resume() {

	if (_state != State::Running) {
		throw logic_error("Runner::resumeSimulation() requires a running Runner.");
	}

	if (!_paused) {
		return;
	}

	_paused = false;
	_pendingSteps = 0;
	_suppressNextAutoUpdate = true;
}

void Runner::requestStep() {

	if (_state != State::Running) {
		throw logic_error("Runner::requestSimulationStep() requires a running Runner.");
	}

	if (!_paused) {
		throw logic_error("Runner::requestSimulationStep() requires paused simulation.");
	}

	if (_pendingSteps == numeric_limits<std::uint64_t>::max()) {
		throw overflow_error("Runner pending simulation-step count overflow.");
	}

	++_pendingSteps;
}

Runner::UpdateCallback Runner::updateCallback() const {
	return _updateCallback;
}

void Runner::updateCallback(UpdateCallback callback) {
	_updateCallback = callback;
}

const SimulationConfig& Runner::config() const {
	return _config;
}

double Runner::timeScale() const {
	return _timeScale;
}

void Runner::timeScale(double value) {

	if (!isfinite(value) || value <= 0.0) {
		throw invalid_argument("Runner time scale must be positive.");
	}

	_timeScale = value;
}

double Runner::simulationTime() const {
	return _simTime;
}

uint64_t Runner::simulationStepCount() const {
	return _simStepCount;
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

/// Private Member Functions ///

void Runner::start(TimePoint now) {

	if (_state != State::Idle) {
		throw logic_error("Runner::start() requires an idle Runner.");
	}

	if (!isfinite(_config.timeScale) || _config.timeScale <= 0.0) {
		throw invalid_argument("Runner requires a positive initial time scale.");
	}

	if (!isfinite(_config.fixedDeltaTime) || _config.fixedDeltaTime <= 0.0) {
		throw invalid_argument("Runner requires a positive fixed simulation delta.");
	}

	if (_config.maxCatchUpSteps == 0) {
		throw invalid_argument("Runner requires at least one catch-up step.");
	}

	if (auto physicsWorld = _scene.physicsWorld();
		physicsWorld && !physicsWorld->acceptsStepDelta(_config.fixedDeltaTime)) {
		throw invalid_argument("Runner simulation delta not accepted by its PhysicsWorld.");
	}

	_timeScale = _config.timeScale;
	_simAccum = 0.0;
	_simTime = 0.0;
	_simStepCount = 0;
	_paused = false;
	_pendingSteps = 0;
	_suppressNextAutoUpdate = false;
	_startTime = now;
	_prevUpdateTime = now;
	_updateInfo = {};
	_hasUpdated = false;
	_renderedFrameCount = 0;
	_state = State::Running;
}

bool Runner::update(TimePoint now) {

	if (_state != State::Running) {
		return false;
	}

	const UpdateInfo updateInfo {
		.updateIndex = _hasUpdated ? _updateInfo.updateIndex + 1 : 0,
		.elapsedTime = chrono::duration<double>(now - _startTime).count(),
		.deltaTime = _hasUpdated ? chrono::duration<double>(now-_prevUpdateTime).count() : 0.0
	};

	_prevUpdateTime = now;
	_updateInfo = updateInfo;
	_hasUpdated = true;

	FrameStats stats{};

	prof::profile(_profiler, Profiler::Tag::Frame, [&] {
		_scene.pollEvents(_profiler);
		_scene.updateInput(_profiler);

		if (_state == State::Running) {
			if (auto callback = updateCallback()) {
				prof::profile(_profiler, Profiler::Tag::Application, [&] {
					callback(*this, _updateInfo);
				});
			}
		}

		if (_state == State::Running) {
			PhysicsInventory inventory{};

			if (_paused) {
				// a new paused scheduling boundary supersedes suppression
				// from an earlier resume/pause sequence. a resume that occurs
				// during the requested batch sets this again and interrupts
				// the local snapshot
				_suppressNextAutoUpdate = false;
				inventory = executeRequestedSteps(stats);
			}
			else if (_suppressNextAutoUpdate) {
				_suppressNextAutoUpdate = false;
				inventory = currentPhysicsInventory();
			}
			else {
				inventory = scheduleSimulation(updateInfo, stats);
			}

			prof::profile(_profiler, Profiler::Tag::EngineCpu, [&] {
				stats.staticBodies = inventory.staticBodies;
				stats.dynamicBodies = inventory.dynamicBodies;
				stats.kinematicBodies = inventory.kinematicBodies;
				stats.primitiveShapes = inventory.primitiveShapes;
				stats.boundingBoxShapes = inventory.boundingBoxShapes;
				stats.convexHullShapes = inventory.convexHullShapes;
				stats.concavePolyhedronShapes = inventory.concavePolyhedronShapes;
			});

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

PhysicsInventory Runner::scheduleSimulation(const UpdateInfo& info, FrameStats& stats) {

	PhysicsInventory inventory{};
	const double fixedDeltaTime = _config.fixedDeltaTime;

	_simAccum += info.deltaTime * _timeScale;

	while (_simAccum >= fixedDeltaTime
	       && stats.simulationStepCount < _config.maxCatchUpSteps
	       && _state == State::Running
	       && !_paused
	       && !_suppressNextAutoUpdate) {

		inventory = executeSimulationStep();

		if (_paused || _suppressNextAutoUpdate) {
			_simAccum = 0.0;
		}
		else {
			_simAccum -= fixedDeltaTime;
		}

		++stats.simulationStepCount;
	}

	if (_state == State::Running && _simAccum >= fixedDeltaTime) {
		const double remainder = fmod(_simAccum, fixedDeltaTime);

		stats.discardedSimulationTime = _simAccum - remainder;

		_simAccum = remainder;
	}

	if (stats.simulationStepCount == 0) {
		inventory = currentPhysicsInventory();
	}

	return inventory;
}

PhysicsInventory Runner::executeRequestedSteps(FrameStats& stats) {

	PhysicsInventory inventory{};
	const auto requestedStepCount = exchange(_pendingSteps, std::uint64_t{0});

	for (uint64_t i = 0; i < requestedStepCount; ++i) {

		inventory = executeSimulationStep();
		++stats.simulationStepCount;

		if (_state != State::Running || !_paused || _suppressNextAutoUpdate) {
			break;
		}
	}

	if (stats.simulationStepCount == 0) {
		inventory = currentPhysicsInventory();
	}

	return inventory;
}

PhysicsInventory Runner::executeSimulationStep() {

	if (auto physicsWorld = _scene.physicsWorld();
		physicsWorld && !physicsWorld->acceptsStepDelta(_config.fixedDeltaTime)) {
		throw runtime_error("Runner simulation delta not accepted by its PhysicsWorld backend.");
	}

	Scene::StepInfo info {
		.stepIndex = _simStepCount,
		.startTime = static_cast<double>(_simStepCount) * _config.fixedDeltaTime,
		.endTime = static_cast<double>(_simStepCount + 1) * _config.fixedDeltaTime,
		.deltaTime = _config.fixedDeltaTime
	};

	auto inventory = _scene.stepSimulation(info, _profiler);

	_simTime = info.endTime;
	++_simStepCount;

	return inventory;
}

PhysicsInventory Runner::currentPhysicsInventory() {

	if (auto physicsWorld = _scene.physicsWorld()) {
		return prof::profile(_profiler,
		                     Profiler::Tag::EngineCpu,
		                     [&] {
			                     return physicsWorld->inventory();
		                     });
	}

	return {};
}

bool Runner::renderFrame(const UpdateInfo& info, FrameStats& stats) {

	auto visualWorld = _scene.visualWorld();
	if (!visualWorld) {
		return false;
	}

	const VisualWorld::RenderInfo renderInfo {
		.frameIndex = _renderedFrameCount,
		.updateIndex = info.updateIndex,
		.updateTime = info.elapsedTime,
		.updateDeltaTime = info.deltaTime,
		.simulationTime = _simTime,
		.simulationStepCount = _simStepCount
	};

	if (!visualWorld->draw(_scene,
						   _scene.physicsWorld(),
						   renderInfo,
						   _scene.debugOptions(),
						   stats,
						   _profiler,
						   _frameStatsHistory)) {
		return false;
	}

	++_renderedFrameCount;
	return true;
}
