//
//  Runner.cc
//  avara3d
//

#include "a3d/Runner.h"

#include <cmath>
#include <limits>
#include <stdexcept>
#include <utility>

#include "a3d/Configuration.h"
#include "a3d/input/InputContext.h"
#include "a3d/physics/PhysicsInventory.h"
#include "a3d/physics/PhysicsWorld.h"
#include "a3d/profile/FrameStats.h"
#include "a3d/profile/Profile.h"
#include "a3d/scene/Scene.h"
#include "a3d/visual/VisualWorld.h"

using namespace a3d;
using namespace std;

Runner::Runner(Scene& scene,
               SimulationConfig config):
	_scene(scene),
	_state(State::Idle),
	_simulationConfig{config},
	_timeScale{config.timeScale},
	_simulationAccumulator{0.0},
	_simulationTime{0.0},
	_simulationTickCount{0},
	_simulationPaused{false},
	_pendingSimulationTicks{0},
	_suppressNextAutomaticSimulationUpdate{false},
	_startTime{},
	_previousUpdateTime{},
	_updateInfo{},
	_hasUpdated{false},
	_updateCallback{},
	_completedRenderFrameCount{0},
	_profiler{},
	_frameStatsHistory{a3d::config::FRAME_STATS_HISTORY_DURATION} {

	validateSimulationConfig();
}

Runner::~Runner() {
	stop();
}

void Runner::start() {
	start(Clock::now());
}

void Runner::start(Clock::time_point now) {

	if (_state != State::Idle) {
		throw logic_error(
			"Runner::start requires an idle Runner.");
	}

	validateSimulationConfig();

	_timeScale = _simulationConfig.timeScale;
	_simulationAccumulator = 0.0;
	_simulationTime = 0.0;
	_simulationTickCount = 0;
	_simulationPaused = false;
	_pendingSimulationTicks = 0;
	_suppressNextAutomaticSimulationUpdate = false;
	_startTime = now;
	_previousUpdateTime = now;
	_updateInfo = {};
	_hasUpdated = false;
	_completedRenderFrameCount = 0;
	_state = State::Running;
}

bool Runner::update() {

	if (_state != State::Running) {
		return false;
	}

	return update(Clock::now());
}

bool Runner::update(Clock::time_point now) {

	if (_state != State::Running) {
		return false;
	}

	const UpdateInfo updateInfo {
		.updateIndex = _hasUpdated
			? _updateInfo.updateIndex + 1 : 0,
		.elapsedTime = chrono::duration<double>(now - _startTime).count(),
		.deltaTime = _hasUpdated
			? chrono::duration<double>(now - _previousUpdateTime).count() : 0.0
	};

	_previousUpdateTime = now;
	_updateInfo = updateInfo;
	_hasUpdated = true;

	const InputContext::UpdateInfo inputInfo {
		.updateIndex = updateInfo.updateIndex,
		.elapsedTime = updateInfo.elapsedTime,
		.deltaTime = updateInfo.deltaTime
	};

	FrameStats stats{};

	prof::profile(_profiler, Profiler::Tag::Frame, [&] {
		_scene.pollEvents(_profiler);
		_scene.updateInput(inputInfo, _profiler);

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
				inventory = executeRequestedSimulationTicks(stats);
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

void Runner::validateSimulationConfig() const {

	const auto& config = _simulationConfig;

	if (!isfinite(config.timeScale)
		|| config.timeScale <= 0.0) {

		throw invalid_argument(
			"Runner requires a finite, positive initial time scale.");
	}

	if (!isfinite(config.fixedDeltaTime) || config.fixedDeltaTime <= 0.0) {

		throw invalid_argument(
			"Runner requires a finite, positive fixed simulation delta.");
	}

	if (config.maxCatchUpSteps == 0) {
		throw invalid_argument("Runner requires at least one maximum catch-up step.");
	}

	if (auto physicsWorld = _scene.physicsWorld();
		physicsWorld && !physicsWorld->acceptsStepDelta(config.fixedDeltaTime)) {

		throw invalid_argument(
			"Runner fixed simulation delta is not accepted by its PhysicsWorld backend.");
	}
}

PhysicsInventory Runner::scheduleSimulation(const UpdateInfo& info,
                                            FrameStats& stats) {

	PhysicsInventory inventory{};
	const double fixedDeltaTime = _simulationConfig.fixedDeltaTime;

	_simulationAccumulator += info.deltaTime * _timeScale;

	while (_simulationAccumulator >= fixedDeltaTime
		&& stats.simulationTickCount < _simulationConfig.maxCatchUpSteps
		&& _state == State::Running
		&& !_simulationPaused
		&& !_suppressNextAutomaticSimulationUpdate) {

		inventory = executeSimulationTick();

		if (_simulationPaused || _suppressNextAutomaticSimulationUpdate) {
			_simulationAccumulator = 0.0;
		}
		else {
			_simulationAccumulator -= fixedDeltaTime;
		}

		++stats.simulationTickCount;
	}

	if (_state == State::Running && _simulationAccumulator >= fixedDeltaTime) {
		const double remainder = fmod(_simulationAccumulator, fixedDeltaTime);

		stats.discardedSimulationTime = _simulationAccumulator - remainder;

		_simulationAccumulator = remainder;
	}

	if (stats.simulationTickCount == 0) {
		inventory = currentPhysicsInventory();
	}

	return inventory;
}

PhysicsInventory Runner::executeRequestedSimulationTicks(
		FrameStats& stats) {

	PhysicsInventory inventory{};
	const auto requestedTickCount =
		exchange(_pendingSimulationTicks, std::uint64_t{0});

	for (std::uint64_t tickIndex = 0;
		 tickIndex < requestedTickCount;
		 ++tickIndex) {

		inventory = executeSimulationTick();
		++stats.simulationTickCount;

		if (_state != State::Running
			|| !_simulationPaused
			|| _suppressNextAutomaticSimulationUpdate) {

			break;
		}
	}

	if (stats.simulationTickCount == 0) {
		inventory = currentPhysicsInventory();
	}

	return inventory;
}

PhysicsInventory Runner::executeSimulationTick() {

	if (auto physicsWorld = _scene.physicsWorld();
		physicsWorld && !physicsWorld->acceptsStepDelta(
				_simulationConfig.fixedDeltaTime)) {

		throw runtime_error(
			"Runner simulation delta is not accepted by its PhysicsWorld backend.");
	}

	Scene::TickInfo info {
		.tickIndex = _simulationTickCount,
		.startTime = static_cast<double>(_simulationTickCount)
			* _simulationConfig.fixedDeltaTime,
		.endTime = static_cast<double>(_simulationTickCount + 1)
			* _simulationConfig.fixedDeltaTime,
		.deltaTime = _simulationConfig.fixedDeltaTime
	};

	auto inventory = _scene.tickSimulation(info, _profiler);

	_simulationTime = info.endTime;
	++_simulationTickCount;

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

void Runner::copyPhysicsInventory(const PhysicsInventory& inventory,
                                  FrameStats& stats) {

	prof::profile(_profiler, Profiler::Tag::EngineCpu, [&] {
		stats.numStaticBodies = inventory.staticBodies;
		stats.numDynamicBodies = inventory.dynamicBodies;
		stats.numKinematicBodies = inventory.kinematicBodies;
		stats.numPrimitiveShapes = inventory.primitiveShapes;
		stats.numBoundingBoxShapes = inventory.boundingBoxShapes;
		stats.numConvexHullShapes = inventory.convexHullShapes;
		stats.numConcavePolyhedronShapes = inventory.concavePolyhedronShapes;
	});
}

bool Runner::renderFrame(const UpdateInfo& info,
                         FrameStats& stats) {

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
		.simulationTickCount = _simulationTickCount
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

void Runner::stop() {

	if (_state == State::Stopped) {
		return;
	}

	_state = State::Stopped;
	_pendingSimulationTicks = 0;
}

bool Runner::simulationPaused() const {
	return _simulationPaused;
}

void Runner::pauseSimulation() {

	if (_state != State::Running) {
		throw logic_error(
			"Runner::pauseSimulation() requires a running Runner.");
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
	_pendingSimulationTicks = 0;
	_suppressNextAutomaticSimulationUpdate = true;
}

void Runner::requestSimulationTick() {

	if (_state != State::Running) {
		throw logic_error("Runner::requestSimulationTick() requires a running Runner.");
	}

	if (!_simulationPaused) {
		throw logic_error("Runner::requestSimulationTick() requires paused simulation.");
	}

	if (_pendingSimulationTicks
		== numeric_limits<std::uint64_t>::max()) {

		throw overflow_error("Runner pending simulation-tick count overflow.");
	}

	++_pendingSimulationTicks;
}

Runner::UpdateCallback Runner::updateCallback() const {
	return _updateCallback;
}

void Runner::updateCallback(UpdateCallback callback) {
	_updateCallback = callback;
}

const SimulationConfig& Runner::simulationConfig() const {
	return _simulationConfig;
}

double Runner::timeScale() const {
	return _timeScale;
}

void Runner::timeScale(double value) {

	if (!isfinite(value) || value <= 0.0) {
		throw invalid_argument("Runner time scale must be finite and positive.");
	}

	_timeScale = value;
}

double Runner::simulationTime() const {
	return _simulationTime;
}

uint64_t Runner::simulationTickCount() const {
	return _simulationTickCount;
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
