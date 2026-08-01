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
static constexpr std::chrono::milliseconds	FRAME_STATS_HISTORY_DURATION 		{3000};

/// Public Lifecycle Functions ///

Runner::Runner(Scene& scene,
               SimulationConfig config):
	_scene(scene),
	_state(State::Idle),
	_config{config},
	_timeScale{config.timeScale},
	_simulationAccumulator{0.0},
	_simulationTime{0.0},
	_simulationStepCount{0},
	_simulationPaused{false},
	_pendingSimulationSteps{0},
	_suppressNextAutomaticSimulationUpdate{false},
	_startTime{},
	_previousUpdateTime{},
	_updateInfo{},
	_hasUpdated{false},
	_updateCallback{},
	_completedRenderFrameCount{0},
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
	_pendingSimulationSteps = 0;
}

bool Runner::simulationPaused() const {
	return _simulationPaused;
}

void Runner::pauseSimulation() {

	if (_state != State::Running) {
		throw logic_error("Runner::pauseSimulation() requires a running Runner.");
	}

	if (_simulationPaused) {
		return;
	}

	_simulationPaused = true;
	_simulationAccumulator = 0.0;
}

void Runner::resumeSimulation() {

	if (_state != State::Running) {
		throw logic_error("Runner::resumeSimulation() requires a running Runner.");
	}

	if (!_simulationPaused) {
		return;
	}

	_simulationPaused = false;
	_pendingSimulationSteps = 0;
	_suppressNextAutomaticSimulationUpdate = true;
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
	_simulationAccumulator = 0.0;
	_simulationTime = 0.0;
	_simulationStepCount = 0;
	_simulationPaused = false;
	_pendingSimulationSteps = 0;
	_suppressNextAutomaticSimulationUpdate = false;
	_startTime = now;
	_previousUpdateTime = now;
	_updateInfo = {};
	_hasUpdated = false;
	_completedRenderFrameCount = 0;
	_state = State::Running;
}

bool Runner::update(TimePoint now) {

	if (_state != State::Running) {
		return false;
	}

	const UpdateInfo updateInfo {
		.updateIndex = _hasUpdated ? _updateInfo.updateIndex + 1 : 0,
		.elapsedTime = chrono::duration<double>(now - _startTime).count(),
		.deltaTime = _hasUpdated ? chrono::duration<double>(now-_previousUpdateTime).count() : 0.0
	};

	_previousUpdateTime = now;
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

			if (_simulationPaused) {
				// a new paused scheduling boundary supersedes suppression
				// from an earlier resume/pause sequence. a resume that occurs
				// during the requested batch sets this again and interrupts
				// the local snapshot
				_suppressNextAutomaticSimulationUpdate = false;
				inventory = executeRequestedSteps(stats);
			}
			else if (_suppressNextAutomaticSimulationUpdate) {
				_suppressNextAutomaticSimulationUpdate = false;
				inventory = currentPhysicsInventory();
			}
			else {
				inventory = scheduleSimulation(updateInfo, stats);
			}

			copyPhysicsInventory(inventory, stats);

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

PhysicsInventory Runner::scheduleSimulation(const UpdateInfo& info,
                                            FrameStats& stats) {

	PhysicsInventory inventory{};
	const double fixedDeltaTime = _config.fixedDeltaTime;

	_simulationAccumulator += info.deltaTime * _timeScale;

	while (_simulationAccumulator >= fixedDeltaTime
	       && stats.simulationStepCount < _config.maxCatchUpSteps
	       && _state == State::Running
	       && !_simulationPaused
	       && !_suppressNextAutomaticSimulationUpdate) {

		inventory = executeSimulationStep();

		if (_simulationPaused || _suppressNextAutomaticSimulationUpdate) {
			_simulationAccumulator = 0.0;
		}
		else {
			_simulationAccumulator -= fixedDeltaTime;
		}

		++stats.simulationStepCount;
	}

	if (_state == State::Running && _simulationAccumulator >= fixedDeltaTime) {
		const double remainder = fmod(_simulationAccumulator, fixedDeltaTime);

		stats.discardedSimulationTime = _simulationAccumulator - remainder;

		_simulationAccumulator = remainder;
	}

	if (stats.simulationStepCount == 0) {
		inventory = currentPhysicsInventory();
	}

	return inventory;
}

PhysicsInventory Runner::executeRequestedSteps(
		FrameStats& stats) {

	PhysicsInventory inventory{};
	const auto requestedStepCount = exchange(_pendingSimulationSteps, std::uint64_t{0});

	for (std::uint64_t requestedStepIndex = 0;
		 requestedStepIndex < requestedStepCount;
		 ++requestedStepIndex) {

		inventory = executeSimulationStep();
		++stats.simulationStepCount;

		if (_state != State::Running
			|| !_simulationPaused
			|| _suppressNextAutomaticSimulationUpdate) {

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
		.stepIndex = _simulationStepCount,
		.startTime = static_cast<double>(_simulationStepCount) * _config.fixedDeltaTime,
		.endTime = static_cast<double>(_simulationStepCount + 1) * _config.fixedDeltaTime,
		.deltaTime = _config.fixedDeltaTime
	};

	auto inventory = _scene.stepSimulation(info, _profiler);

	_simulationTime = info.endTime;
	++_simulationStepCount;

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

void Runner::copyPhysicsInventory(const PhysicsInventory& inventory, FrameStats& stats) {

	prof::profile(_profiler, Profiler::Tag::EngineCpu, [&] {
		stats.staticBodies = inventory.staticBodies;
		stats.dynamicBodies = inventory.dynamicBodies;
		stats.kinematicBodies = inventory.kinematicBodies;
		stats.primitiveShapes = inventory.primitiveShapes;
		stats.boundingBoxShapes = inventory.boundingBoxShapes;
		stats.convexHullShapes = inventory.convexHullShapes;
		stats.concavePolyhedronShapes = inventory.concavePolyhedronShapes;
	});
}

bool Runner::renderFrame(const UpdateInfo& info, FrameStats& stats) {

	auto visualWorld = _scene.visualWorld();
	if (!visualWorld) {
		return false;
	}

	const VisualWorld::RenderInfo renderInfo {
		.frameIndex = _completedRenderFrameCount,
		.updateIndex = info.updateIndex,
		.updateTime = info.elapsedTime,
		.updateDeltaTime = info.deltaTime,
		.simulationTime = _simulationTime,
		.simulationStepCount = _simulationStepCount
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

	++_completedRenderFrameCount;
	return true;
}
