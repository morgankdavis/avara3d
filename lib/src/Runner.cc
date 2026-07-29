//
//  Runner.cc
//  avara3d
//

#include "a3d/Runner.h"

#include <algorithm>
#include <cmath>
#include <stdexcept>

#include "a3d/Configuration.h"
#include "a3d/physics/PhysicsInventory.h"
#include "a3d/physics/PhysicsWorld.h"
#include "a3d/profile/FrameStats.h"
#include "a3d/profile/Profile.h"
#include "a3d/scene/Scene.h"
#include "a3d/visual/VisualWorld.h"

using namespace a3d;
using namespace std;

Runner::Runner(Scene& scene,
               SimulationConfiguration configuration):
	_scene(scene),
	_state(State::Idle),
	_simulationConfiguration{configuration},
	_timeScale{configuration.timeScale},
	_simulationAccumulator{0.0},
	_simulationTime{0.0},
	_simulationTickCount{0},
	_startTime{},
	_previousUpdateTime{},
	_hostUpdateInfo{},
	_hasUpdated{false},
	_updateCallback{},
	_completedRenderFrameCount{0},
	_profiler{},
	_frameStatsHistory{a3d::config::FRAME_STATS_HISTORY_DURATION} {

	validateSimulationConfiguration();
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

	validateSimulationConfiguration();

	_timeScale = _simulationConfiguration.timeScale;
	_simulationAccumulator = 0.0;
	_simulationTime = 0.0;
	_simulationTickCount = 0;
	_startTime = now;
	_previousUpdateTime = now;
	_hostUpdateInfo = {};
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

	const HostUpdateInfo hostUpdateInfo {
		.updateIndex = _hasUpdated
			? _hostUpdateInfo.updateIndex + 1
			: 0,
		.elapsedTime = chrono::duration<double>(
			now - _startTime).count(),
		.deltaTime = _hasUpdated
			? chrono::duration<double>(
				now - _previousUpdateTime).count()
			: 0.0
	};

	_previousUpdateTime = now;
	_hostUpdateInfo = hostUpdateInfo;
	_hasUpdated = true;

	FrameStats stats{};

	prof::profile(_profiler, Profiler::Tag::Frame, [&] {
		_scene.updateHostEvents(_profiler);
		_scene.updateInput(_profiler);

		if (auto callback = updateCallback()) {
			prof::profile(_profiler, Profiler::Tag::Application, [&] {
				callback(*this, _hostUpdateInfo);
			});
		}

		if (_state == State::Running) {
			const auto inventory = scheduleSimulation(hostUpdateInfo, stats);
			copyPhysicsInventory(inventory, stats);

			if (_state == State::Running) {
				renderFrame(hostUpdateInfo, stats);
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

void Runner::validateSimulationConfiguration() const {

	const auto& configuration = _simulationConfiguration;

	if (!isfinite(configuration.fixedDeltaTime)
		|| configuration.fixedDeltaTime <= 0.0) {

		throw invalid_argument(
			"Runner requires a finite, positive fixed simulation delta.");
	}

	if (configuration.maxCatchUpSteps == 0) {
		throw invalid_argument(
			"Runner requires at least one maximum catch-up step.");
	}

	if (!isfinite(configuration.timeScale)
		|| configuration.timeScale <= 0.0) {

		throw invalid_argument(
			"Runner requires a finite, positive initial time scale.");
	}

	if (!isfinite(configuration.maxFrameDrivenDeltaTime)
		|| configuration.maxFrameDrivenDeltaTime <= 0.0) {

		throw invalid_argument(
			"Runner requires a finite, positive maximum FrameDriven delta.");
	}

	if (auto physicsWorld = _scene.physicsWorld();
		physicsWorld
		&& !physicsWorld->acceptsStepDelta(configuration.fixedDeltaTime)) {

		throw invalid_argument(
			"Runner fixed simulation delta is not accepted by its PhysicsWorld backend.");
	}
}

PhysicsInventory Runner::scheduleSimulation(const HostUpdateInfo& info,
                                            FrameStats& stats) {

	PhysicsInventory inventory{};

	if (_simulationConfiguration.timing == SimulationTiming::FrameDriven) {

		const double scaledDelta = info.deltaTime * _timeScale;
		const double simulationDelta = min(
			scaledDelta,
			_simulationConfiguration.maxFrameDrivenDeltaTime);

		if (scaledDelta > simulationDelta) {
			stats.discardedSimulationTime =
				scaledDelta - simulationDelta;
		}

		if (simulationDelta > 0.0) {
			inventory = runSimulationTick(simulationDelta);
			stats.simulationTickCount = 1;
		}
	}
	else {
		const double fixedDeltaTime = _simulationConfiguration.fixedDeltaTime;

		_simulationAccumulator += info.deltaTime * _timeScale;

		while (_simulationAccumulator >= fixedDeltaTime
			&& stats.simulationTickCount < _simulationConfiguration.maxCatchUpSteps
		       && _state == State::Running) {

			inventory = runSimulationTick(fixedDeltaTime);

			_simulationAccumulator -= fixedDeltaTime;

			++stats.simulationTickCount;
		}

		if (_state == State::Running && _simulationAccumulator >= fixedDeltaTime) {
			const double remainder = fmod(_simulationAccumulator,
						fixedDeltaTime);

			stats.discardedSimulationTime =
					_simulationAccumulator - remainder;

			_simulationAccumulator = remainder;
		}
	}

	if (stats.simulationTickCount == 0) {
		if (auto physicsWorld = _scene.physicsWorld()) {
			inventory = prof::profile(
				_profiler,
				Profiler::Tag::EngineCpu,
				[&] {
					return physicsWorld->inventory();
				});
		}
	}

	return inventory;
}

PhysicsInventory Runner::runSimulationTick(double deltaTime) {

	if (auto physicsWorld = _scene.physicsWorld();
		physicsWorld
			&& !physicsWorld->acceptsStepDelta(deltaTime)) {

		throw runtime_error(
			"Runner simulation delta is not accepted by its PhysicsWorld backend.");
	}

	SimulationStepInfo info {
		.tickIndex = _simulationTickCount
	};

	if (_simulationConfiguration.timing == SimulationTiming::FixedStep) {
		info.startTime =
			static_cast<double>(info.tickIndex)
			* _simulationConfiguration.fixedDeltaTime;
		info.endTime =
			static_cast<double>(info.tickIndex + 1)
			* _simulationConfiguration.fixedDeltaTime;
		info.deltaTime = _simulationConfiguration.fixedDeltaTime;
	}
	else {
		info.startTime = _simulationTime;
		info.endTime = info.startTime + deltaTime;
		info.deltaTime = deltaTime;
	}

	auto inventory = _scene.simulate(info, _profiler);

	_simulationTime = info.endTime;
	++_simulationTickCount;

	return inventory;
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

RenderFrameInfo Runner::makeRenderFrameInfo(const HostUpdateInfo& info) const {

	return {
		.frameIndex = _completedRenderFrameCount,
		.hostUpdateIndex = info.updateIndex,
		.hostTime = info.elapsedTime,
		.hostDeltaTime = info.deltaTime,
		.simulationTime = _simulationTime,
		.completedSimulationTicks = _simulationTickCount
	};
}

bool Runner::renderFrame(const HostUpdateInfo& info,
                         FrameStats& stats) {

	auto visualWorld = _scene.visualWorld();
	if (!visualWorld) {
		return false;
	}

	const RenderFrameInfo frameInfo = makeRenderFrameInfo(info);

	if (!visualWorld->draw(_scene,
						   _scene.physicsWorld(),
						   frameInfo,
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
}

Runner::UpdateCallback Runner::updateCallback() const {
	return _updateCallback;
}

void Runner::updateCallback(UpdateCallback callback) {
	_updateCallback = callback;
}

const SimulationConfiguration& Runner::simulationConfiguration() const {
	return _simulationConfiguration;
}

double Runner::timeScale() const {
	return _timeScale;
}

void Runner::timeScale(double value) {

	if (!isfinite(value) || value <= 0.0) {
		throw invalid_argument(
			"Runner time scale must be finite and positive.");
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
