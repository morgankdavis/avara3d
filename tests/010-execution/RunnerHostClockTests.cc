//
//  RunnerHostClockTests.cc
//  avara3d
//
//  Created by Morgan Davis on 7/27/26.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#include <chrono>
#include <cmath>
#include <cstdint>
#include <iostream>
#include <limits>
#include <memory>
#include <stdexcept>
#include <string>
#include <string_view>
#include <tuple>
#include <utility>
#include <vector>

#include "a3d/Timing.h"
#include "a3d/Runner.h"
#include "a3d/input/InputManager.h"
#include "a3d/physics/PhysicsBody.h"
#include "a3d/physics/PhysicsInventory.h"
#include "a3d/physics/PhysicsWorld.h"
#include "a3d/physics/shape/primitive/SpherePhysicsShape.h"
#include "a3d/profile/FrameStats.h"
#include "a3d/profile/FrameStatsHistory.h"
#include "a3d/profile/Profiler.h"
#include "a3d/scene/Node.h"
#include "a3d/scene/Scene.h"

namespace a3d::testing {

	class RunnerTestAccess {

	public:
		using TimePoint = Runner::Clock::time_point;

		static void start(Runner& runner, TimePoint now) {
			runner.start(now);
		}

		static bool update(Runner& runner, TimePoint now) {
			return runner.update(now);
		}

		static const HostUpdateInfo& hostUpdateInfo(const Runner& runner) {
			return runner._hostUpdateInfo;
		}

		static const FrameStatsHistory& frameStatsHistory(const Runner& runner) {
			return runner._frameStatsHistory;
		}

		static std::uint64_t completedRenderFrameCount(const Runner& runner) {
			return runner._completedRenderFrameCount;
		}

		static RenderFrameInfo renderFrameInfo(const Runner& runner) {
			return runner.makeRenderFrameInfo(runner._hostUpdateInfo);
		}
	};
}

namespace {

	using TimePoint = a3d::testing::RunnerTestAccess::TimePoint;

	struct CallbackTime {
		double elapsedTime;
		double deltaTime;
	};

	struct RecordedStep {
		std::uint64_t	tickIndex;
		double			startTime;
		double			endTime;
		double			deltaTime;
	};

	class RecordingInputManager final : public a3d::InputManager {

	public:
		explicit RecordingInputManager(
			std::vector<std::string_view>& stages):
				_stages{stages},
				_updateCount{} {}

		void update() override {
			_stages.push_back("input");
			++_updateCount;
		}

		void attachedToScene(a3d::Scene&) override {}
		void visualWorldAttachedToScene(a3d::Scene&) override {}

		std::size_t updateCount() const {
			return _updateCount;
		}

		void reset() {
			_updateCount = 0;
		}

	private:
		std::vector<std::string_view>&	_stages;
		std::size_t						_updateCount;
	};

	TimePoint AtMilliseconds(std::int64_t milliseconds) {
		return TimePoint{} + std::chrono::milliseconds(milliseconds);
	}

	TimePoint AtMicroseconds(std::int64_t microseconds) {
		return TimePoint{} + std::chrono::microseconds(microseconds);
	}

	TimePoint AtNanoseconds(std::int64_t nanoseconds) {
		return TimePoint{} + std::chrono::nanoseconds(nanoseconds);
	}

	void Expect(bool condition, std::string_view message) {

		if (!condition) {
			throw std::runtime_error(std::string(message));
		}
	}

	void ExpectNear(double actual,
					double expected,
					std::string_view message,
					double tolerance = 1e-12) {

		if (std::abs(actual - expected) > tolerance) {
			throw std::runtime_error(
				std::string(message)
				+ ": expected "
				+ std::to_string(expected)
				+ ", got "
				+ std::to_string(actual));
		}
	}

	template <typename Function>
	void ExpectInvalidArgument(Function&& function,
							   std::string_view message) {

		bool rejected = false;
		try {
			function();
		}
		catch (const std::invalid_argument&) {
			rejected = true;
		}

		Expect(rejected, message);
	}

	template <typename Function>
	void ExpectRuntimeError(Function&& function,
							std::string_view message) {

		bool rejected = false;
		try {
			function();
		}
		catch (const std::runtime_error&) {
			rejected = true;
		}

		Expect(rejected, message);
	}

	a3d::SimulationConfiguration FixedConfiguration(double deltaTime = 0.125,
													 std::uint32_t maxCatchUpSteps = 8,
													 double timeScale = 1.0) {

		auto configuration = a3d::SimulationConfiguration{};
		configuration.timing = a3d::SimulationTiming::FixedStep;
		configuration.fixedDeltaTime = deltaTime;
		configuration.maxCatchUpSteps = maxCatchUpSteps;
		configuration.timeScale = timeScale;
		return configuration;
	}

	RecordedStep RecordStep(const a3d::SimulationStepInfo& info) {
		return {
			.tickIndex = info.tickIndex,
			.startTime = info.startTime,
			.endTime = info.endTime,
			.deltaTime = info.deltaTime
		};
	}

	void ExpectStep(const RecordedStep& step,
					std::uint64_t tickIndex,
					double startTime,
					double endTime,
					double deltaTime,
					std::string_view message) {

		Expect(
			step.tickIndex == tickIndex,
			std::string(message) + " tick index");
		ExpectNear(
			step.startTime,
			startTime,
			std::string(message) + " start time");
		ExpectNear(
			step.endTime,
			endTime,
			std::string(message) + " end time");
		ExpectNear(
			step.deltaTime,
			deltaTime,
			std::string(message) + " delta time");
	}

	const a3d::FrameStats& LatestFrameStats(const a3d::Runner& runner) {

		const auto& samples =
			a3d::testing::RunnerTestAccess::frameStatsHistory(runner).samples();

		Expect(!samples.empty(), "the Runner should have a statistics sample");
		return std::get<1>(samples.back());
	}

	std::shared_ptr<a3d::Node> AddMovingDynamicBody(a3d::Scene& scene,
													double velocity = 1.0) {

		auto node = std::make_shared<a3d::Node>("moving dynamic body");
		node->physicsBody(std::make_unique<a3d::PhysicsBody>(
			a3d::PhysicsBody::Type::Dynamic,
			std::make_shared<a3d::SpherePhysicsShape>(1.0f)));
		scene.rootNode()->addChild(node);

		auto body = node->physicsBody();
		body->affectedByGravity(false);
		body->allowsResting(false);
		body->linearVelocity({
			static_cast<float>(velocity),
			0.0f,
			0.0f
		});

		return node;
	}

	void FirstUpdateHasZeroDelta() {

		a3d::Scene scene;
		a3d::Runner runner(scene);
		std::vector<CallbackTime> times;
		runner.updateCallback(
			[&times](a3d::Runner&, const a3d::HostUpdateInfo& info) {
				times.push_back({info.elapsedTime, info.deltaTime});
			});

		runner.start();

		Expect(
			runner.update(),
			"the running Runner should continue");
		Expect(times.size() == 1, "the Runner callback should run once");
		Expect(
			times[0].elapsedTime >= 0.0,
			"the first elapsed time should use the monotonic host clock");
		ExpectNear(
			times[0].deltaTime,
			0.0,
			"the first update delta should be zero");
	}

	void ElapsedTimeProgresses() {

		a3d::Scene scene;
		a3d::Runner runner(scene);
		std::vector<CallbackTime> times;
		runner.updateCallback(
			[&times](a3d::Runner&, const a3d::HostUpdateInfo& info) {
				times.push_back({info.elapsedTime, info.deltaTime});
			});

		a3d::testing::RunnerTestAccess::start(
			runner, AtMilliseconds(2000));
		a3d::testing::RunnerTestAccess::update(
			runner, AtMilliseconds(2100));
		a3d::testing::RunnerTestAccess::update(
			runner, AtMilliseconds(2350));
		a3d::testing::RunnerTestAccess::update(
			runner, AtMilliseconds(2900));

		Expect(times.size() == 3, "the Runner callback should run three times");
		ExpectNear(times[0].elapsedTime, 0.1, "first elapsed time");
		ExpectNear(times[1].elapsedTime, 0.35, "second elapsed time");
		ExpectNear(times[2].elapsedTime, 0.9, "third elapsed time");
	}

	void DeltaTimeProgresses() {

		a3d::Scene scene;
		a3d::Runner runner(scene);
		std::vector<CallbackTime> times;
		runner.updateCallback(
			[&times](a3d::Runner&, const a3d::HostUpdateInfo& info) {
				times.push_back({info.elapsedTime, info.deltaTime});
			});

		a3d::testing::RunnerTestAccess::start(
			runner, AtMilliseconds(3000));
		a3d::testing::RunnerTestAccess::update(
			runner, AtMilliseconds(3100));
		a3d::testing::RunnerTestAccess::update(
			runner, AtMilliseconds(3350));
		a3d::testing::RunnerTestAccess::update(
			runner, AtMilliseconds(3900));

		Expect(times.size() == 3, "the Scene callback should run three times");
		ExpectNear(times[0].deltaTime, 0.0, "first delta time");
		ExpectNear(times[1].deltaTime, 0.25, "second delta time");
		ExpectNear(times[2].deltaTime, 0.55, "third delta time");
	}

	void UpdateIndexProgresses() {

		a3d::Scene scene;
		a3d::Runner runner(scene);
		a3d::testing::RunnerTestAccess::start(
			runner, AtMilliseconds(4000));

		a3d::testing::RunnerTestAccess::update(
			runner, AtMilliseconds(4100));
		Expect(
			a3d::testing::RunnerTestAccess::hostUpdateInfo(runner).updateIndex == 0,
			"the first update index should be zero");

		a3d::testing::RunnerTestAccess::update(
			runner, AtMilliseconds(4200));
		Expect(
			a3d::testing::RunnerTestAccess::hostUpdateInfo(runner).updateIndex == 1,
			"the second update index should be one");

		a3d::testing::RunnerTestAccess::update(
			runner, AtMilliseconds(4300));
		Expect(
			a3d::testing::RunnerTestAccess::hostUpdateInfo(runner).updateIndex == 2,
			"the third update index should be two");
	}

	void RunnerClocksAreIndependent() {

		a3d::Scene firstScene;
		std::vector<CallbackTime> firstTimes;
		a3d::Scene secondScene;
		std::vector<CallbackTime> secondTimes;
		a3d::Runner firstRunner(firstScene);
		a3d::Runner secondRunner(secondScene);
		firstRunner.updateCallback(
			[&firstTimes](a3d::Runner&, const a3d::HostUpdateInfo& info) {
				firstTimes.push_back({info.elapsedTime, info.deltaTime});
			});
		secondRunner.updateCallback(
			[&secondTimes](a3d::Runner&, const a3d::HostUpdateInfo& info) {
				secondTimes.push_back({info.elapsedTime, info.deltaTime});
			});

		a3d::testing::RunnerTestAccess::start(
			firstRunner, AtMilliseconds(10000));
		a3d::testing::RunnerTestAccess::start(
			secondRunner, AtMilliseconds(50000));

		a3d::testing::RunnerTestAccess::update(
			firstRunner, AtMilliseconds(10100));
		a3d::testing::RunnerTestAccess::update(
			secondRunner, AtMilliseconds(50700));
		a3d::testing::RunnerTestAccess::update(
			firstRunner, AtMilliseconds(10350));
		a3d::testing::RunnerTestAccess::update(
			secondRunner, AtMilliseconds(51600));

		Expect(firstTimes.size() == 2, "the first callback should run twice");
		Expect(secondTimes.size() == 2, "the second callback should run twice");
		ExpectNear(firstTimes[0].elapsedTime, 0.1, "first Runner first elapsed time");
		ExpectNear(firstTimes[0].deltaTime, 0.0, "first Runner first delta time");
		ExpectNear(firstTimes[1].elapsedTime, 0.35, "first Runner second elapsed time");
		ExpectNear(firstTimes[1].deltaTime, 0.25, "first Runner second delta time");
		ExpectNear(secondTimes[0].elapsedTime, 0.7, "second Runner first elapsed time");
		ExpectNear(secondTimes[0].deltaTime, 0.0, "second Runner first delta time");
		ExpectNear(secondTimes[1].elapsedTime, 1.6, "second Runner second elapsed time");
		ExpectNear(secondTimes[1].deltaTime, 0.9, "second Runner second delta time");
		Expect(
			a3d::testing::RunnerTestAccess::hostUpdateInfo(firstRunner).updateIndex == 1,
			"the first Runner should have its own update index");
		Expect(
			a3d::testing::RunnerTestAccess::hostUpdateInfo(secondRunner).updateIndex == 1,
			"the second Runner should have its own update index");
	}

	void SimulationConfigurationDefaults() {

		const a3d::SimulationConfiguration defaults;

		Expect(
			defaults.timing == a3d::SimulationTiming::FrameDriven,
			"simulation timing should default to FrameDriven");
		ExpectNear(
			defaults.fixedDeltaTime,
			1.0 / 60.0,
			"the default fixed delta");
		Expect(
			defaults.maxCatchUpSteps == 8,
			"the default maximum catch-up count");
		ExpectNear(defaults.timeScale, 1.0, "the default time scale");
		ExpectNear(
			defaults.maxFrameDrivenDeltaTime,
			0.25,
			"the default maximum FrameDriven delta");

		a3d::Scene scene;
		a3d::Runner runner(scene);
		const auto& runnerConfiguration = runner.simulationConfiguration();

		Expect(
			runnerConfiguration.timing == defaults.timing,
			"the default Runner should preserve simulation timing");
		ExpectNear(
			runnerConfiguration.fixedDeltaTime,
			defaults.fixedDeltaTime,
			"the default Runner fixed delta");
		Expect(
			runnerConfiguration.maxCatchUpSteps == defaults.maxCatchUpSteps,
			"the default Runner maximum catch-up count");
		ExpectNear(
			runnerConfiguration.timeScale,
			defaults.timeScale,
			"the default Runner configured time scale");
		ExpectNear(
			runner.timeScale(),
			defaults.timeScale,
			"the default Runner runtime time scale");
		ExpectNear(
			runnerConfiguration.maxFrameDrivenDeltaTime,
			defaults.maxFrameDrivenDeltaTime,
			"the default Runner maximum FrameDriven delta");
	}

	void SimulationConfigurationValidation() {

		auto expectConfigurationRejected =
			[](a3d::SimulationConfiguration configuration,
			   std::string_view message) {

				a3d::Scene scene;
				ExpectInvalidArgument(
					[&] {
						a3d::Runner runner(scene, configuration);
					},
					message);
			};

		auto configuration = a3d::SimulationConfiguration{};
		configuration.fixedDeltaTime = 0.0;
		expectConfigurationRejected(
			configuration,
			"a zero fixed delta should be rejected");

		configuration = {};
		configuration.fixedDeltaTime = -0.125;
		expectConfigurationRejected(
			configuration,
			"a negative fixed delta should be rejected");

		configuration = {};
		configuration.fixedDeltaTime =
			std::numeric_limits<double>::infinity();
		expectConfigurationRejected(
			configuration,
			"an infinite fixed delta should be rejected");

		configuration = {};
		configuration.fixedDeltaTime =
			std::numeric_limits<double>::quiet_NaN();
		expectConfigurationRejected(
			configuration,
			"a NaN fixed delta should be rejected");

		configuration = {};
		configuration.maxCatchUpSteps = 0;
		expectConfigurationRejected(
			configuration,
			"a zero maximum catch-up count should be rejected");

		configuration = {};
		configuration.timeScale = 0.0;
		expectConfigurationRejected(
			configuration,
			"a zero configured time scale should be rejected");

		configuration = {};
		configuration.timeScale =
			std::numeric_limits<double>::quiet_NaN();
		expectConfigurationRejected(
			configuration,
			"a NaN configured time scale should be rejected");

		configuration = {};
		configuration.maxFrameDrivenDeltaTime = 0.0;
		expectConfigurationRejected(
			configuration,
			"a zero maximum FrameDriven delta should be rejected");

		configuration = {};
		configuration.maxFrameDrivenDeltaTime =
			std::numeric_limits<double>::infinity();
		expectConfigurationRejected(
			configuration,
			"an infinite maximum FrameDriven delta should be rejected");

		a3d::Scene scene;
		a3d::Runner runner(scene);
		runner.timeScale(2.0);

		ExpectInvalidArgument(
			[&] {
				runner.timeScale(0.0);
			},
			"a zero runtime time scale should be rejected");
		ExpectInvalidArgument(
			[&] {
				runner.timeScale(-1.0);
			},
			"a negative runtime time scale should be rejected");
		ExpectInvalidArgument(
			[&] {
				runner.timeScale(
					std::numeric_limits<double>::quiet_NaN());
			},
			"a NaN runtime time scale should be rejected");
		ExpectInvalidArgument(
			[&] {
				runner.timeScale(
					std::numeric_limits<double>::infinity());
			},
			"an infinite runtime time scale should be rejected");
		ExpectNear(
			runner.timeScale(),
			2.0,
			"rejected runtime time scales should preserve the current value");

		a3d::Scene physicsScene;
		physicsScene.physicsWorld(std::make_unique<a3d::PhysicsWorld>());
		auto tinyFixedConfiguration = FixedConfiguration(
			static_cast<double>(std::numeric_limits<float>::epsilon()) * 0.5);

		ExpectInvalidArgument(
			[&] {
				a3d::Runner tinyRunner(
					physicsScene,
					tinyFixedConfiguration);
			},
			"a fixed delta that is effectively zero for Bullet should be rejected");
	}

	void FirstUpdateExecutesNoSimulationTicks() {

		a3d::Scene scene;
		scene.physicsWorld(std::make_unique<a3d::PhysicsWorld>());
		auto node = AddMovingDynamicBody(scene);

		std::size_t runnerCallbackCount = 0;
		std::size_t sceneWillCount = 0;
		std::size_t physicsWillCount = 0;
		std::size_t physicsDidCount = 0;
		std::size_t sceneDidCount = 0;

		a3d::Runner runner(scene, FixedConfiguration());
		runner.updateCallback(
			[&runnerCallbackCount](a3d::Runner&,
								   const a3d::HostUpdateInfo&) {

				++runnerCallbackCount;
			});
		scene.willSimulateCallback(
			[&sceneWillCount](a3d::Scene&,
							  const a3d::SimulationStepInfo&) {

				++sceneWillCount;
			});
		scene.physicsWorld()->willStepCallback(
			[&physicsWillCount](a3d::PhysicsWorld&,
								const a3d::SimulationStepInfo&) {

				++physicsWillCount;
			});
		scene.physicsWorld()->didStepCallback(
			[&physicsDidCount](a3d::PhysicsWorld&,
							   const a3d::SimulationStepInfo&) {

				++physicsDidCount;
			});
		scene.didSimulateCallback(
			[&sceneDidCount](a3d::Scene&,
							 const a3d::SimulationStepInfo&) {

				++sceneDidCount;
			});

		a3d::testing::RunnerTestAccess::start(
			runner, AtMilliseconds(9000));
		Expect(
			a3d::testing::RunnerTestAccess::update(
				runner, AtMilliseconds(9500)),
			"the first host update should continue");

		Expect(
			runnerCallbackCount == 1,
			"the first host update should invoke the Runner callback");
		Expect(
			sceneWillCount == 0
				&& physicsWillCount == 0
				&& physicsDidCount == 0
				&& sceneDidCount == 0,
			"the first host update should execute no simulation callbacks");
		Expect(
			runner.simulationTickCount() == 0,
			"the first host update should complete no simulation ticks");
		ExpectNear(
			runner.simulationTime(),
			0.0,
			"the first host update should not advance simulation time");
		ExpectNear(
			node->physicsBody()->centerOfMass().x,
			0.0,
			"the first host update should not advance Bullet",
			1e-6);

		const auto& stats = LatestFrameStats(runner);
		Expect(
			stats.simulationTickCount == 0,
			"the first host statistics sample should report zero ticks");
		ExpectNear(
			stats.discardedSimulationTime,
			0.0,
			"the first host statistics sample should report no discarded time");
		Expect(
			stats.numDynamicBodies == 1,
			"a zero-tick host update should still sample current body inventory");
		Expect(
			stats.numPrimitiveShapes == 1,
			"a zero-tick host update should still sample current shape inventory");
	}

	void FrameDrivenScalesCapsAndReportsDiscard() {

		a3d::Scene scene;
		a3d::Runner runner(scene);
		std::vector<RecordedStep> steps;
		scene.didSimulateCallback(
			[&steps](a3d::Scene&,
					 const a3d::SimulationStepInfo& info) {

				steps.push_back(RecordStep(info));
			});

		a3d::testing::RunnerTestAccess::start(
			runner, AtMilliseconds(12000));
		Expect(
			a3d::testing::RunnerTestAccess::update(
				runner, AtMilliseconds(12100)),
			"the priming FrameDriven update should continue");
		Expect(
			steps.empty(),
			"the first FrameDriven update should execute no simulation tick");
		Expect(
			LatestFrameStats(runner).simulationTickCount == 0,
			"the first FrameDriven update should report zero ticks");

		runner.timeScale(2.0);
		Expect(
			a3d::testing::RunnerTestAccess::update(
				runner, AtMilliseconds(12600)),
			"the capped FrameDriven update should continue");

		Expect(steps.size() == 1, "FrameDriven should execute one capped tick");
		ExpectStep(
			steps[0],
			0,
			0.0,
			0.25,
			0.25,
			"the capped FrameDriven tick");
		ExpectNear(
			runner.simulationTime(),
			0.25,
			"capped FrameDriven simulation time");
		Expect(
			runner.simulationTickCount() == 1,
			"capped FrameDriven completed tick count");

		const auto cappedStats = LatestFrameStats(runner);
		Expect(
			cappedStats.simulationTickCount == 1,
			"the capped host update should report one tick");
		ExpectNear(
			cappedStats.discardedSimulationTime,
			0.75,
			"FrameDriven should report scaled time above its cap");

		Expect(
			a3d::testing::RunnerTestAccess::update(
				runner, AtMilliseconds(12700)),
			"the uncapped FrameDriven update should continue");

		Expect(steps.size() == 2, "FrameDriven should execute a second tick");
		ExpectStep(
			steps[1],
			1,
			0.25,
			0.45,
			0.2,
			"the uncapped FrameDriven tick");
		ExpectNear(
			runner.simulationTime(),
			0.45,
			"discarded FrameDriven time should not advance simulation time");
		Expect(
			runner.simulationTickCount() == 2,
			"FrameDriven should complete two ticks");

		const auto& uncappedStats = LatestFrameStats(runner);
		Expect(
			uncappedStats.simulationTickCount == 1,
			"the uncapped host update should report one tick");
		ExpectNear(
			uncappedStats.discardedSimulationTime,
			0.0,
			"the uncapped host update should report no discarded time");
	}

	void FixedStepRetainsFractionalAccumulator() {

		a3d::Scene scene;
		a3d::Runner runner(scene, FixedConfiguration());
		std::vector<RecordedStep> steps;
		scene.didSimulateCallback(
			[&steps](a3d::Scene&,
					 const a3d::SimulationStepInfo& info) {

				steps.push_back(RecordStep(info));
			});

		a3d::testing::RunnerTestAccess::start(
			runner, AtMicroseconds(0));
		Expect(
			a3d::testing::RunnerTestAccess::update(
				runner, AtMicroseconds(10000)),
			"the priming FixedStep update should continue");
		Expect(
			a3d::testing::RunnerTestAccess::update(
				runner, AtMicroseconds(72500)),
			"the first fractional FixedStep update should continue");
		Expect(
			steps.empty(),
			"insufficient fixed-step time should execute no tick");
		Expect(
			LatestFrameStats(runner).simulationTickCount == 0,
			"insufficient fixed-step time should report zero ticks");

		Expect(
			a3d::testing::RunnerTestAccess::update(
				runner, AtMicroseconds(135000)),
			"the second fractional FixedStep update should continue");
		Expect(
			steps.size() == 1,
			"two retained half-step contributions should execute one tick");
		ExpectStep(
			steps[0],
			0,
			0.0,
			0.125,
			0.125,
			"the accumulated FixedStep tick");
		Expect(
			LatestFrameStats(runner).simulationTickCount == 1,
			"the accumulated host update should report one tick");
		ExpectNear(
			runner.simulationTime(),
			0.125,
			"the accumulated FixedStep simulation time");
		Expect(
			runner.simulationTickCount() == 1,
			"the accumulated FixedStep completed tick count");

		runner.timeScale(2.0);
		Expect(
			a3d::testing::RunnerTestAccess::update(
				runner, AtMicroseconds(197500)),
			"the scaled FixedStep update should continue");
		Expect(
			steps.size() == 2,
			"FixedStep time scale should change tick accrual");
		ExpectStep(
			steps[1],
			1,
			0.125,
			0.25,
			0.125,
			"the scaled FixedStep tick");
		ExpectNear(
			runner.simulationTime(),
			0.25,
			"the scaled FixedStep simulation time");
		Expect(
			runner.simulationTickCount() == 2,
			"the scaled FixedStep completed tick count");
		Expect(
			LatestFrameStats(runner).simulationTickCount == 1,
			"the scaled host update should report one fixed tick");

		const auto& samples =
			a3d::testing::RunnerTestAccess::frameStatsHistory(runner).samples();
		Expect(
			samples.size() == 4,
			"FixedStep should commit one statistics record per host update");
	}

	void FixedStepLimitsCatchUpAndReportsDiscard() {

		a3d::Scene scene;
		a3d::Runner runner(scene, FixedConfiguration(0.125, 2));
		std::vector<RecordedStep> steps;
		scene.didSimulateCallback(
			[&steps](a3d::Scene&,
					 const a3d::SimulationStepInfo& info) {

				steps.push_back(RecordStep(info));
			});

		a3d::testing::RunnerTestAccess::start(
			runner, AtMicroseconds(0));
		Expect(
			a3d::testing::RunnerTestAccess::update(
				runner, AtMicroseconds(10000)),
			"the priming catch-up update should continue");
		Expect(
			a3d::testing::RunnerTestAccess::update(
				runner, AtMicroseconds(822500)),
			"the bounded catch-up update should continue");

		Expect(
			steps.size() == 2,
			"the catch-up limit should execute exactly two ticks");
		ExpectStep(
			steps[0],
			0,
			0.0,
			0.125,
			0.125,
			"the first catch-up tick");
		ExpectStep(
			steps[1],
			1,
			0.125,
			0.25,
			0.125,
			"the second catch-up tick");
		ExpectNear(
			runner.simulationTime(),
			0.25,
			"only completed catch-up ticks should advance simulation time");
		Expect(
			runner.simulationTickCount() == 2,
			"the bounded catch-up completed tick count");

		const auto catchUpStats = LatestFrameStats(runner);
		Expect(
			catchUpStats.simulationTickCount == 2,
			"the bounded catch-up host update should report two ticks");
		ExpectNear(
			catchUpStats.discardedSimulationTime,
			0.5,
			"catch-up overflow should discard only whole-step demand");

		Expect(
			a3d::testing::RunnerTestAccess::update(
				runner, AtMicroseconds(885000)),
			"the retained-remainder update should continue");
		Expect(
			steps.size() == 3,
			"the retained fractional remainder should complete the next tick");
		ExpectStep(
			steps[2],
			2,
			0.25,
			0.375,
			0.125,
			"the retained-remainder tick");
		ExpectNear(
			runner.simulationTime(),
			0.375,
			"the retained-remainder simulation time");
		Expect(
			runner.simulationTickCount() == 3,
			"the retained-remainder completed tick count");

		const auto& remainderStats = LatestFrameStats(runner);
		Expect(
			remainderStats.simulationTickCount == 1,
			"the retained-remainder host update should report one tick");
		ExpectNear(
			remainderStats.discardedSimulationTime,
			0.0,
			"discarded time should reset for each host statistics batch");
	}

	void StopBetweenUpdates() {

		a3d::Scene scene;
		a3d::Runner runner(scene);
		std::size_t callbackCount = 0;
		runner.updateCallback(
			[&callbackCount](a3d::Runner&, const a3d::HostUpdateInfo&) {
				++callbackCount;
			});

		a3d::testing::RunnerTestAccess::start(
			runner, AtMilliseconds(6000));
		Expect(
			a3d::testing::RunnerTestAccess::update(
				runner, AtMilliseconds(6100)),
			"the Runner should continue before stop");

		const auto infoBeforeStop =
			a3d::testing::RunnerTestAccess::hostUpdateInfo(runner);
		runner.stop();

		Expect(
			!a3d::testing::RunnerTestAccess::update(
				runner, AtMilliseconds(6500)),
			"a stopped Runner should not update");
		Expect(callbackCount == 1, "stop should prevent later Runner callbacks");

		const auto& infoAfterStop =
			a3d::testing::RunnerTestAccess::hostUpdateInfo(runner);
		Expect(
			infoAfterStop.updateIndex == infoBeforeStop.updateIndex,
			"stop should not advance the update index");
		ExpectNear(
			infoAfterStop.elapsedTime,
			infoBeforeStop.elapsedTime,
			"stop should not advance elapsed time");
		ExpectNear(
			infoAfterStop.deltaTime,
			infoBeforeStop.deltaTime,
			"stop should not change delta time");
	}

	void StopDuringRunnerUpdate() {

		a3d::Scene scene;
		scene.physicsWorld(std::make_unique<a3d::PhysicsWorld>());

		a3d::Runner runner(scene, FixedConfiguration());
		std::vector<std::string_view> stages;
		std::size_t callbackCount = 0;
		std::size_t simulationCount = 0;

		a3d::testing::RunnerTestAccess::start(
			runner, AtMicroseconds(7000000));
		Expect(
			a3d::testing::RunnerTestAccess::update(
				runner, AtMicroseconds(7100000)),
			"the priming update should continue before the stopping callback");

		runner.updateCallback(
			[&runner, &callbackCount, &stages](a3d::Runner&,
											   const a3d::HostUpdateInfo&) {

				stages.push_back("runner");
				++callbackCount;
				runner.stop();
			});
		scene.didSimulateCallback(
			[&simulationCount, &stages](a3d::Scene&,
										const a3d::SimulationStepInfo&) {

				stages.push_back("scene-did");
				++simulationCount;
			});

		Expect(
			!a3d::testing::RunnerTestAccess::update(
				runner, AtMicroseconds(7475000)),
			"an update that stops the Runner should return false");
		Expect(callbackCount == 1, "the stopping callback should run once");
		Expect(
			simulationCount == 0,
			"stop during the Runner callback should skip pending simulation ticks");
		Expect(
			stages.size() == 1
				&& stages[0] == "runner",
			"no later stage should follow the stopping Runner callback");
		Expect(
			runner.state() == a3d::Runner::State::Stopped,
			"the Runner should remain stopped");
		Expect(
			a3d::testing::RunnerTestAccess::hostUpdateInfo(runner).updateIndex == 1,
			"the stopping update should retain its update index");
		Expect(
			LatestFrameStats(runner).simulationTickCount == 0,
			"the stopping host update should report zero simulation ticks");
		Expect(
			runner.simulationTickCount() == 0,
			"stop during the Runner callback should complete no simulation tick");
		Expect(
			a3d::testing::RunnerTestAccess::completedRenderFrameCount(runner) == 0,
			"stop during the Runner callback should not complete a render frame");

		Expect(
			!a3d::testing::RunnerTestAccess::update(
				runner, AtMicroseconds(7600000)),
			"a later update should remain stopped");
		Expect(callbackCount == 1, "a stopped Runner should not invoke the callback again");
		Expect(simulationCount == 0, "a stopped Runner should not invoke simulation");
	}

	void StopDuringSimulationTick() {

		a3d::Scene scene;
		scene.physicsWorld(std::make_unique<a3d::PhysicsWorld>());
		auto node = AddMovingDynamicBody(scene);

		a3d::Runner runner(scene, FixedConfiguration());
		std::vector<std::string_view> stages;
		scene.willSimulateCallback(
			[&runner, &stages](a3d::Scene&,
							   const a3d::SimulationStepInfo&) {

				stages.push_back("scene-will");
				runner.stop();
			});
		scene.physicsWorld()->willStepCallback(
			[&stages](a3d::PhysicsWorld&,
					   const a3d::SimulationStepInfo&) {

				stages.push_back("physics-will");
			});
		scene.physicsWorld()->didStepCallback(
			[&stages](a3d::PhysicsWorld&,
					   const a3d::SimulationStepInfo&) {

				stages.push_back("physics-did");
			});
		scene.didSimulateCallback(
			[&stages](a3d::Scene&,
					   const a3d::SimulationStepInfo&) {

				stages.push_back("scene-did");
			});

		a3d::testing::RunnerTestAccess::start(
			runner, AtMicroseconds(7500000));
		Expect(
			a3d::testing::RunnerTestAccess::update(
				runner, AtMicroseconds(7600000)),
			"the priming update should continue");
		stages.clear();

		Expect(
			!a3d::testing::RunnerTestAccess::update(
				runner, AtMicroseconds(7975000)),
			"an update stopped during a simulation tick should return false");
		Expect(
			stages.size() == 4
				&& stages[0] == "scene-will"
				&& stages[1] == "physics-will"
				&& stages[2] == "physics-did"
				&& stages[3] == "scene-did",
			"stop should allow the current simulation tick to finish coherently");
		Expect(
			runner.simulationTickCount() == 1,
			"stop during a simulation tick should prevent a second tick");
		ExpectNear(
			runner.simulationTime(),
			0.125,
			"the stopped Runner should complete the current tick time");
		ExpectNear(
			node->physicsBody()->centerOfMass().x,
			0.125,
			"Bullet should complete the current tick after stop",
			1e-5);
		Expect(
			LatestFrameStats(runner).simulationTickCount == 1,
			"the stopped host update should report its completed tick");
		Expect(
			runner.state() == a3d::Runner::State::Stopped,
			"the Runner should remain stopped after the simulation tick");
		Expect(
			a3d::testing::RunnerTestAccess::completedRenderFrameCount(runner) == 0,
			"stopping during simulation should not complete a render frame");
	}

	void SimulationCallbacksRunInOrderWithPhysics() {

		a3d::Scene scene;
		std::vector<std::string_view> stages;
		auto inputManager = std::make_unique<RecordingInputManager>(stages);
		auto* inputManagerPtr = inputManager.get();
		scene.inputManager(std::move(inputManager));
		scene.physicsWorld(std::make_unique<a3d::PhysicsWorld>());

		a3d::Runner runner(scene, FixedConfiguration());
		a3d::testing::RunnerTestAccess::start(
			runner, AtMilliseconds(8000));

		// Prime the Runner so the observed host update has a nonzero delta.
		Expect(
			a3d::testing::RunnerTestAccess::update(
				runner, AtMilliseconds(8100)),
			"the priming host update should continue");
		stages.clear();
		inputManagerPtr->reset();

		a3d::Runner* callbackRunner = nullptr;
		const a3d::HostUpdateInfo* callbackInfo = nullptr;
		std::vector<CallbackTime> runnerTimes;
		std::vector<RecordedStep> simulationSteps;
		a3d::Scene* callbackScene = nullptr;
		a3d::PhysicsWorld* callbackPhysicsWorld = nullptr;
		runner.updateCallback(
			[&callbackRunner,
			 &callbackInfo,
			 &runnerTimes,
			 &stages](a3d::Runner& callbackRunnerValue,
					  const a3d::HostUpdateInfo& info) {

				stages.push_back("runner");
				callbackRunner = &callbackRunnerValue;
				callbackInfo = &info;
				runnerTimes.push_back({info.elapsedTime, info.deltaTime});
			});
		scene.willSimulateCallback(
			[&callbackScene,
			 &simulationSteps,
			 &stages](a3d::Scene& callbackSceneValue,
					  const a3d::SimulationStepInfo& info) {

				stages.push_back("scene-will");
				callbackScene = &callbackSceneValue;
				simulationSteps.push_back(RecordStep(info));
			});
		scene.physicsWorld()->willStepCallback(
			[&callbackPhysicsWorld,
			 &simulationSteps,
			 &stages](a3d::PhysicsWorld& callbackWorld,
					  const a3d::SimulationStepInfo& info) {

				stages.push_back("physics-will");
				callbackPhysicsWorld = &callbackWorld;
				simulationSteps.push_back(RecordStep(info));
			});
		scene.physicsWorld()->didStepCallback(
			[&simulationSteps, &stages](a3d::PhysicsWorld&,
										const a3d::SimulationStepInfo& info) {

				stages.push_back("physics-did");
				simulationSteps.push_back(RecordStep(info));
			});
		scene.didSimulateCallback(
			[&runner, &simulationSteps, &stages](a3d::Scene&,
												const a3d::SimulationStepInfo& info) {

				stages.push_back("scene-did");
				simulationSteps.push_back(RecordStep(info));
				ExpectNear(
					runner.simulationTime(),
					0.0,
					"Runner simulation time should not advance inside callbacks");
				Expect(
					runner.simulationTickCount() == 0,
					"Runner tick count should not advance inside callbacks");
			});

		Expect(
			a3d::testing::RunnerTestAccess::update(
				runner, AtMilliseconds(8225)),
			"the observed host update should continue");

		Expect(
			stages.size() == 6
				&& stages[0] == "input"
				&& stages[1] == "runner"
				&& stages[2] == "scene-will"
				&& stages[3] == "physics-will"
				&& stages[4] == "physics-did"
				&& stages[5] == "scene-did",
			"the host and simulation callbacks should run in order");
		Expect(
			inputManagerPtr->updateCount() == 1,
			"input should update exactly once in the observed host update");
		Expect(
			runnerTimes.size() == 1,
			"the Runner callback should run exactly once");
		Expect(
			simulationSteps.size() == 4,
			"each simulation callback should run exactly once");
		Expect(
			callbackRunner == &runner,
			"the callback should receive its owning Runner");
		Expect(
			&callbackRunner->scene() == &scene,
			"the callback Runner should reference the expected Scene");
		Expect(
			callbackInfo
				== &a3d::testing::RunnerTestAccess::hostUpdateInfo(runner),
			"the callback should receive the Runner's current HostUpdateInfo");
		Expect(
			callbackInfo->updateIndex == 1,
			"the observed callback should receive update index one");
		Expect(
			callbackScene == &scene,
			"the Scene callback should receive its owning Scene");
		Expect(
			callbackPhysicsWorld == scene.physicsWorld(),
			"the physics callback should receive its owning PhysicsWorld");
		ExpectNear(
			runnerTimes[0].elapsedTime,
			0.225,
			"the Runner callback elapsed time");
		ExpectNear(
			runnerTimes[0].deltaTime,
			0.125,
			"the Runner callback delta time");
		for (const auto& step : simulationSteps) {
			ExpectStep(
				step,
				0,
				0.0,
				0.125,
				0.125,
				"the callback simulation step");
		}
		ExpectNear(
			runner.simulationTime(),
			0.125,
			"Runner simulation time after the callback pipeline");
		Expect(
			runner.simulationTickCount() == 1,
			"Runner tick count after the callback pipeline");
		Expect(
			LatestFrameStats(runner).simulationTickCount == 1,
			"the callback host update should report one tick");
	}

	void NeitherWorldPipeline() {

		a3d::Scene scene;
		std::vector<std::string_view> stages;
		auto inputManager = std::make_unique<RecordingInputManager>(stages);
		auto* inputManagerPtr = inputManager.get();
		scene.inputManager(std::move(inputManager));

		Expect(
			scene.physicsWorld() == nullptr,
			"the neither-world fixture should not have a PhysicsWorld");
		Expect(
			scene.visualWorld() == nullptr,
			"the neither-world fixture should not have a VisualWorld");

		a3d::Runner runner(scene);
		std::vector<CallbackTime> runnerTimes;

		a3d::testing::RunnerTestAccess::start(
			runner, AtMilliseconds(10000));
		Expect(
			a3d::testing::RunnerTestAccess::update(
				runner, AtMilliseconds(10100)),
			"the priming neither-world host update should continue");
		stages.clear();
		inputManagerPtr->reset();

		runner.updateCallback(
			[&runnerTimes, &stages](a3d::Runner&,
									const a3d::HostUpdateInfo& info) {

				stages.push_back("runner");
				runnerTimes.push_back({info.elapsedTime, info.deltaTime});
			});
		scene.willSimulateCallback(
			[&stages](a3d::Scene&,
					   const a3d::SimulationStepInfo&) {

				stages.push_back("scene-will");
			});
		scene.didSimulateCallback(
			[&stages](a3d::Scene&,
					   const a3d::SimulationStepInfo&) {

				stages.push_back("scene-did");
			});

		Expect(
			a3d::testing::RunnerTestAccess::update(
				runner, AtMilliseconds(10225)),
			"the observed neither-world host update should continue");

		Expect(
			stages.size() == 4
				&& stages[0] == "input"
				&& stages[1] == "runner"
				&& stages[2] == "scene-will"
				&& stages[3] == "scene-did",
			"Scene simulation should run without either world");
		Expect(
			inputManagerPtr->updateCount() == 1,
			"input should update once without either world");
		Expect(
			runnerTimes.size() == 1,
			"the Runner callback should run once without either world");
		ExpectNear(
			runnerTimes[0].elapsedTime,
			0.225,
			"the neither-world callback elapsed time");
		ExpectNear(
			runnerTimes[0].deltaTime,
			0.125,
			"the neither-world callback delta time");
		Expect(
			runner.simulationTickCount() == 1,
			"the neither-world Scene should complete one simulation tick");
		ExpectNear(
			runner.simulationTime(),
			0.125,
			"the neither-world simulation time");
		Expect(
			LatestFrameStats(runner).simulationTickCount == 1,
			"the neither-world host update should report one tick");
	}

	void NoVisualWorldHasZeroCompletedRenderFrames() {

		a3d::Scene scene;
		a3d::Runner runner(scene);

		Expect(
			scene.visualWorld() == nullptr,
			"the no-visual fixture should not have a VisualWorld");
		Expect(
			a3d::testing::RunnerTestAccess::completedRenderFrameCount(runner) == 0,
			"a new Runner should have no completed render frames");

		a3d::testing::RunnerTestAccess::start(
			runner, AtMilliseconds(10500));
		Expect(
			a3d::testing::RunnerTestAccess::update(
				runner, AtMilliseconds(10600)),
			"the first no-visual host update should continue");
		Expect(
			a3d::testing::RunnerTestAccess::update(
				runner, AtMilliseconds(10800)),
			"the second no-visual host update should continue");

		Expect(
			a3d::testing::RunnerTestAccess::completedRenderFrameCount(runner) == 0,
			"host updates without a VisualWorld should not complete render frames");
	}

	void RenderFrameInfoReportsCompletedSimulation() {

		a3d::Scene scene;
		a3d::Runner runner(scene, FixedConfiguration());

		a3d::testing::RunnerTestAccess::start(
			runner, AtMicroseconds(0));
		Expect(
			a3d::testing::RunnerTestAccess::update(
				runner, AtMicroseconds(10000)),
			"the priming render-metadata update should continue");
		Expect(
			a3d::testing::RunnerTestAccess::update(
				runner, AtMicroseconds(260000)),
			"the render-metadata scheduling update should continue");

		const auto info =
			a3d::testing::RunnerTestAccess::renderFrameInfo(runner);

		Expect(info.frameIndex == 0, "the first render frame index");
		Expect(
			info.hostUpdateIndex == 1,
			"render metadata should identify its host update");
		ExpectNear(info.hostTime, 0.26, "render metadata host time");
		ExpectNear(info.hostDeltaTime, 0.25, "render metadata host delta");
		ExpectNear(
			info.simulationTime,
			0.25,
			"render metadata should report completed simulation time");
		Expect(
			info.completedSimulationTicks == 2,
			"render metadata should report completed simulation ticks");
		Expect(
			a3d::testing::RunnerTestAccess::completedRenderFrameCount(runner) == 0,
			"constructing render metadata should not consume a frame index");
	}

	void SceneRootInvariant() {

		a3d::Scene scene;
		auto originalRoot = scene.rootNode();

		Expect(
			originalRoot != nullptr,
			"Scene should start with a non-null root Node");
		Expect(
			originalRoot->scene() == &scene,
			"the initial root Node should be attached to its Scene");

		scene.rootNode(originalRoot);
		Expect(
			scene.rootNode() == originalRoot,
			"installing the current root should be a no-op");

		bool rejected = false;
		try {
			scene.rootNode(nullptr);
		}
		catch (const std::invalid_argument&) {
			rejected = true;
		}

		Expect(rejected, "Scene should reject a null root Node");
		Expect(
			scene.rootNode() == originalRoot,
			"a rejected root replacement should preserve the original root");
		Expect(
			originalRoot->scene() == &scene,
			"a rejected root replacement should leave the original root attached");
	}

	void PhysicsInventorySamplingIsCurrent() {

		a3d::Scene scene;
		scene.physicsWorld(std::make_unique<a3d::PhysicsWorld>());

		auto staticNode = std::make_shared<a3d::Node>("static body");
		staticNode->physicsBody(std::make_unique<a3d::PhysicsBody>(
			a3d::PhysicsBody::Type::Static,
			std::make_shared<a3d::SpherePhysicsShape>(1.0f)));
		scene.rootNode()->addChild(staticNode);

		auto dynamicNode = std::make_shared<a3d::Node>("dynamic body");
		dynamicNode->physicsBody(std::make_unique<a3d::PhysicsBody>(
			a3d::PhysicsBody::Type::Dynamic,
			std::make_shared<a3d::SpherePhysicsShape>(1.0f)));
		scene.rootNode()->addChild(dynamicNode);

		auto kinematicNode = std::make_shared<a3d::Node>("kinematic body");
		kinematicNode->physicsBody(std::make_unique<a3d::PhysicsBody>(
			a3d::PhysicsBody::Type::Kinematic,
			std::make_shared<a3d::SpherePhysicsShape>(1.0f)));
		scene.rootNode()->addChild(kinematicNode);

		std::size_t callbackCount = 0;
		scene.physicsWorld()->didStepCallback(
			[&callbackCount](a3d::PhysicsWorld&,
							 const a3d::SimulationStepInfo&) {

				++callbackCount;
			});

		const auto first = scene.physicsWorld()->inventory();

		Expect(first.staticBodies == 1, "current static-body inventory");
		Expect(first.dynamicBodies == 1, "current dynamic-body inventory");
		Expect(first.kinematicBodies == 1, "current kinematic-body inventory");
		Expect(first.primitiveShapes == 3, "current primitive-shape inventory");
		Expect(first.boundingBoxShapes == 0, "current bounding-box-shape inventory");
		Expect(first.convexHullShapes == 0, "current convex-hull-shape inventory");
		Expect(
			first.concavePolyhedronShapes == 0,
			"current concave-polyhedron-shape inventory");

		const auto second = scene.physicsWorld()->inventory();

		Expect(
			second.staticBodies == first.staticBodies
				&& second.dynamicBodies == first.dynamicBodies
				&& second.kinematicBodies == first.kinematicBodies,
			"repeated queries should not multiply body inventory");
		Expect(
			second.primitiveShapes == first.primitiveShapes
				&& second.boundingBoxShapes == first.boundingBoxShapes
				&& second.convexHullShapes == first.convexHullShapes
				&& second.concavePolyhedronShapes == first.concavePolyhedronShapes,
			"repeated queries should not multiply shape inventory");
		Expect(
			callbackCount == 0,
			"an inventory query should not invoke the physics callback");
	}

	void PhysicsInventoryAcrossMultipleTicks() {

		a3d::Scene scene;
		scene.physicsWorld(std::make_unique<a3d::PhysicsWorld>());
		AddMovingDynamicBody(scene, 0.0);

		std::size_t callbackCount = 0;
		scene.physicsWorld()->didStepCallback(
			[&callbackCount](a3d::PhysicsWorld&,
							 const a3d::SimulationStepInfo&) {

				++callbackCount;
			});

		a3d::Runner runner(scene, FixedConfiguration());
		a3d::testing::RunnerTestAccess::start(
			runner, AtMicroseconds(0));
		Expect(
			a3d::testing::RunnerTestAccess::update(
				runner, AtMicroseconds(10000)),
			"the priming inventory update should continue");
		Expect(
			a3d::testing::RunnerTestAccess::update(
				runner, AtMicroseconds(385000)),
			"the multi-tick inventory update should continue");

		Expect(
			callbackCount == 3,
			"the multi-tick host update should execute three physics steps");
		const auto& stats = LatestFrameStats(runner);
		Expect(
			stats.simulationTickCount == 3,
			"the multi-tick host update should report three ticks");
		Expect(
			stats.numDynamicBodies == 1,
			"multiple physics ticks should not multiply body inventory");
		Expect(
			stats.numPrimitiveShapes == 1,
			"multiple physics ticks should not multiply shape inventory");

		const auto& samples =
			a3d::testing::RunnerTestAccess::frameStatsHistory(runner).samples();
		Expect(
			samples.size() == 2,
			"multiple physics ticks should commit one host statistics sample");
	}

	void PhysicsInventoryUsesLatestTick() {

		a3d::Scene scene;
		scene.physicsWorld(std::make_unique<a3d::PhysicsWorld>());
		auto dynamicNode = AddMovingDynamicBody(scene, 0.0);

		scene.physicsWorld()->didStepCallback(
			[&dynamicNode](a3d::PhysicsWorld&,
						   const a3d::SimulationStepInfo& info) {

				if (info.tickIndex == 0) {
					dynamicNode->physicsBody(
						std::unique_ptr<a3d::PhysicsBody>{});
				}
			});

		a3d::Runner runner(scene, FixedConfiguration());
		a3d::testing::RunnerTestAccess::start(
			runner, AtMicroseconds(0));
		Expect(
			a3d::testing::RunnerTestAccess::update(
				runner, AtMicroseconds(10000)),
			"the priming latest-inventory update should continue");
		Expect(
			a3d::testing::RunnerTestAccess::update(
				runner, AtMicroseconds(260000)),
			"the two-tick latest-inventory update should continue");

		const auto& stats = LatestFrameStats(runner);
		Expect(
			stats.simulationTickCount == 2,
			"the latest-inventory update should report two ticks");
		Expect(
			stats.numDynamicBodies == 0,
			"host statistics should retain the latest physics inventory");
		Expect(
			stats.numPrimitiveShapes == 0,
			"host statistics should retain the latest shape inventory");
	}

	void PhysicsInventoryPrecedesDidStep() {

		a3d::Scene scene;
		scene.physicsWorld(std::make_unique<a3d::PhysicsWorld>());

		auto dynamicNode = std::make_shared<a3d::Node>("dynamic body");
		dynamicNode->physicsBody(std::make_unique<a3d::PhysicsBody>(
			a3d::PhysicsBody::Type::Dynamic,
			std::make_shared<a3d::SpherePhysicsShape>(1.0f)));
		scene.rootNode()->addChild(dynamicNode);

		std::size_t callbackCount = 0;
		scene.physicsWorld()->didStepCallback(
			[&callbackCount, &dynamicNode](a3d::PhysicsWorld&,
										  const a3d::SimulationStepInfo& info) {

				++callbackCount;
				ExpectStep(
					RecordStep(info),
					0,
					0.0,
					0.125,
					0.125,
					"the inventory-order physics step");
				dynamicNode->physicsBody(std::unique_ptr<a3d::PhysicsBody>{});
			});

		a3d::Runner runner(scene, FixedConfiguration());
		a3d::testing::RunnerTestAccess::start(
			runner, AtMicroseconds(11000000));
		Expect(
			a3d::testing::RunnerTestAccess::update(
				runner, AtMicroseconds(11100000)),
			"the priming inventory-order update should continue");
		Expect(
			a3d::testing::RunnerTestAccess::update(
				runner, AtMicroseconds(11225000)),
			"the inventory-order host update should continue");

		Expect(
			callbackCount == 1,
			"the physics callback should run exactly once");

		const auto& samples =
			a3d::testing::RunnerTestAccess::frameStatsHistory(runner).samples();
		Expect(
			samples.size() == 2,
			"each inventory-order host update should commit one statistics sample");

		const auto& stats = std::get<1>(samples.back());
		Expect(
			stats.numDynamicBodies == 1,
			"the completed-step inventory should precede callback mutation");
		Expect(
			stats.numPrimitiveShapes == 1,
			"the completed-step shape inventory should precede callback mutation");

		const auto current = scene.physicsWorld()->inventory();
		Expect(
			current.dynamicBodies == 0,
			"the callback mutation should affect the current body inventory");
		Expect(
			current.primitiveShapes == 0,
			"the callback mutation should affect the current shape inventory");
	}

	void BulletAdvancesExactlyOncePerSimulationTick() {

		a3d::Scene scene;
		scene.physicsWorld(std::make_unique<a3d::PhysicsWorld>());
		auto node = AddMovingDynamicBody(scene);

		std::size_t physicsStepCount = 0;
		scene.physicsWorld()->didStepCallback(
			[&physicsStepCount](a3d::PhysicsWorld&,
								const a3d::SimulationStepInfo&) {

				++physicsStepCount;
			});

		a3d::Runner runner(scene, FixedConfiguration());
		a3d::testing::RunnerTestAccess::start(
			runner, AtMicroseconds(0));
		Expect(
			a3d::testing::RunnerTestAccess::update(
				runner, AtMicroseconds(10000)),
			"the priming exact-step update should continue");
		Expect(
			a3d::testing::RunnerTestAccess::update(
				runner, AtMicroseconds(135000)),
			"the exact-step update should continue");

		Expect(
			physicsStepCount == 1,
			"one simulation tick should perform one physics callback sequence");
		ExpectNear(
			node->physicsBody()->centerOfMass().x,
			0.125,
			"one simulation tick should advance Bullet by the full requested delta",
			1e-5);

		node->physicsBody()->applyForce({4.0f, 0.0f, 0.0f}, false);
		ExpectNear(
			node->physicsBody()->totalForce().x,
			4.0,
			"the force fixture should hold a force before stepping",
			1e-6);

		Expect(
			a3d::testing::RunnerTestAccess::update(
				runner, AtMicroseconds(260000)),
			"the force-clearing exact-step update should continue");
		Expect(
			physicsStepCount == 2,
			"the second simulation tick should perform one physics callback sequence");
		ExpectNear(
			node->physicsBody()->totalForce().x,
			0.0,
			"an exact Bullet step should preserve force clearing",
			1e-6);
	}

	void BulletRejectsInvalidStepDelta() {

		a3d::Scene scene;
		scene.physicsWorld(std::make_unique<a3d::PhysicsWorld>());
		a3d::Profiler profiler;
		const a3d::SimulationStepInfo tinyStep {
			.tickIndex = 0,
			.startTime = 0.0,
			.endTime = 1e-9,
			.deltaTime = 1e-9
		};

		ExpectInvalidArgument(
			[&] {
				scene.physicsWorld()->step(tinyStep, profiler);
			},
			"PhysicsWorld should reject a delta that is effectively zero for Bullet");

		a3d::Runner runner(scene);
		a3d::testing::RunnerTestAccess::start(
			runner, AtNanoseconds(0));
		Expect(
			a3d::testing::RunnerTestAccess::update(
				runner, AtNanoseconds(1000000)),
			"the priming tiny-delta update should continue");
		ExpectRuntimeError(
			[&] {
				(void)a3d::testing::RunnerTestAccess::update(
					runner, AtNanoseconds(1000001));
			},
			"FrameDriven should reject a delta that is effectively zero for Bullet");
	}

	struct FixedScheduleResult {
		std::vector<RecordedStep>	steps;
		double						bodyPosition;
		double						simulationTime;
		std::uint64_t				simulationTickCount;
	};

	FixedScheduleResult RunFixedPhysicsSchedule(const std::vector<std::int64_t>& updateDeltasMicroseconds) {

		a3d::Scene scene;
		scene.physicsWorld(std::make_unique<a3d::PhysicsWorld>());
		auto node = AddMovingDynamicBody(scene);
		a3d::Runner runner(scene, FixedConfiguration());

		FixedScheduleResult result;
		scene.didSimulateCallback(
			[&result](a3d::Scene&,
					  const a3d::SimulationStepInfo& info) {

				result.steps.push_back(RecordStep(info));
			});

		std::int64_t now = 10000;
		a3d::testing::RunnerTestAccess::start(
			runner, AtMicroseconds(0));
		Expect(
			a3d::testing::RunnerTestAccess::update(
				runner, AtMicroseconds(now)),
			"the repeatability priming update should continue");

		for (const auto delta : updateDeltasMicroseconds) {
			now += delta;
			Expect(
				a3d::testing::RunnerTestAccess::update(
					runner, AtMicroseconds(now)),
				"the repeatability scheduled update should continue");
		}

		result.bodyPosition = node->physicsBody()->centerOfMass().x;
		result.simulationTime = runner.simulationTime();
		result.simulationTickCount = runner.simulationTickCount();
		return result;
	}

	void FixedStepPhysicsIsRepeatable() {

		const auto catchUp = RunFixedPhysicsSchedule({375000});
		const auto distributed = RunFixedPhysicsSchedule({
			125000,
			125000,
			125000
		});

		Expect(
			catchUp.steps.size() == 3,
			"the catch-up schedule should execute three ticks");
		Expect(
			distributed.steps.size() == 3,
			"the distributed schedule should execute three ticks");

		for (std::size_t index = 0; index < catchUp.steps.size(); ++index) {
			const double startTime = static_cast<double>(index) * 0.125;
			const double endTime = static_cast<double>(index + 1) * 0.125;

			ExpectStep(
				catchUp.steps[index],
				index,
				startTime,
				endTime,
				0.125,
				"the catch-up repeatability step");
			ExpectStep(
				distributed.steps[index],
				index,
				startTime,
				endTime,
				0.125,
				"the distributed repeatability step");
		}

		ExpectNear(
			catchUp.bodyPosition,
			distributed.bodyPosition,
			"fixed-tick physics state should not depend on host grouping",
			1e-5);
		ExpectNear(
			catchUp.bodyPosition,
			0.375,
			"three exact fixed ticks should advance the body three deltas",
			1e-5);
		ExpectNear(
			catchUp.simulationTime,
			distributed.simulationTime,
			"repeatable schedules should reach the same simulation time");
		Expect(
			catchUp.simulationTickCount
				== distributed.simulationTickCount
				&& catchUp.simulationTickCount == 3,
			"repeatable schedules should complete the same tick count");
	}

	void FixedStepAtOneHundredTwentyHertzDoesNotDrift() {

		constexpr double FIXED_DELTA_TIME =
			1.0 / 120.0;

		a3d::Scene scene;

		a3d::Runner runner(
			scene,
			FixedConfiguration(FIXED_DELTA_TIME));

		a3d::testing::RunnerTestAccess::start(
			runner,
			AtMicroseconds(0));

		Expect(
			a3d::testing::RunnerTestAccess::update(
				runner,
				AtMicroseconds(0)),
			"the priming 120 Hz update should continue");

		for (std::int64_t millisecond = 1;
			 millisecond <= 1000;
			 ++millisecond) {

			Expect(
				a3d::testing::RunnerTestAccess::update(
					runner,
					AtMicroseconds(millisecond * 1000)),
				"the 120 Hz accumulation update should continue");
			 }

		Expect(
			runner.simulationTickCount() == 120,
			"one second should execute 120 fixed simulation ticks");

		ExpectNear(
			runner.simulationTime(),
			1.0,
			"120 fixed ticks should advance one simulation second",
			1e-12);
	}

	using TestFunction = void (*)();

	const std::pair<std::string_view, TestFunction> Tests[] {
		{"first-update-zero-delta", FirstUpdateHasZeroDelta},
		{"elapsed-time-progression", ElapsedTimeProgresses},
		{"delta-time-progression", DeltaTimeProgresses},
		{"update-index-progression", UpdateIndexProgresses},
		{"independent-runner-clocks", RunnerClocksAreIndependent},
		{"simulation-configuration-defaults", SimulationConfigurationDefaults},
		{"simulation-configuration-validation", SimulationConfigurationValidation},
		{"first-update-zero-simulation-ticks", FirstUpdateExecutesNoSimulationTicks},
		{"frame-driven-scale-cap-discard", FrameDrivenScalesCapsAndReportsDiscard},
		{"fixed-step-fractional-accumulator", FixedStepRetainsFractionalAccumulator},
		{"fixed-step-catch-up-overflow", FixedStepLimitsCatchUpAndReportsDiscard},
		{"stop-between-updates", StopBetweenUpdates},
		{"stop-during-runner-update", StopDuringRunnerUpdate},
		{"stop-during-simulation-tick", StopDuringSimulationTick},
		{"simulation-callback-order-physics", SimulationCallbacksRunInOrderWithPhysics},
		{"neither-world-pipeline", NeitherWorldPipeline},
		{"no-visual-completed-render-count", NoVisualWorldHasZeroCompletedRenderFrames},
		{"render-frame-simulation-metadata", RenderFrameInfoReportsCompletedSimulation},
		{"scene-root-invariant", SceneRootInvariant},
		{"physics-inventory-sampling", PhysicsInventorySamplingIsCurrent},
		{"physics-inventory-multiple-ticks", PhysicsInventoryAcrossMultipleTicks},
		{"physics-inventory-latest-tick", PhysicsInventoryUsesLatestTick},
		{"physics-inventory-before-did-step", PhysicsInventoryPrecedesDidStep},
		{"bullet-exact-step", BulletAdvancesExactlyOncePerSimulationTick},
		{"bullet-invalid-step-delta", BulletRejectsInvalidStepDelta},
		{"fixed-step-repeatability", FixedStepPhysicsIsRepeatable},
		{"fixed-step-120hz-long-run", FixedStepAtOneHundredTwentyHertzDoesNotDrift}
	};

	bool RunTest(
		std::string_view name,
		TestFunction function) {

		try {
			function();
			std::cout << "[PASS] " << name << '\n';
			return true;
		}
		catch (const std::exception& exception) {
			std::cerr
				<< "[FAIL] "
				<< name
				<< ": "
				<< exception.what()
				<< '\n';

			return false;
		}
		catch (...) {
			std::cerr
				<< "[FAIL] "
				<< name
				<< ": unknown exception\n";

			return false;
		}
	}

	void PrintAvailableTests() {

		std::cerr << "Available tests:\n";

		for (const auto& [name, function] : Tests) {
			std::cerr << "  " << name << '\n';
		}
	}
}

int main(int argc, char* argv[]) {

	if (argc > 2) {
		std::cerr << "Usage: " << argv[0] << " [test-name]\n";
		PrintAvailableTests();
		return 2;
	}


	// No test name: run the complete suite.

	if (argc == 1) {

		std::size_t passed = 0;
		std::size_t failed = 0;

		for (const auto& [name, function] : Tests) {

			if (RunTest(name, function)) {
				++passed;
			}
			else {
				++failed;
			}
		}

		std::cout
			<< '\n'
			<< passed
			<< " passed, "
			<< failed
			<< " failed.\n";

		return failed == 0 ? 0 : 1;
	}


	// One test name: run only that test.
	// CTest continues to use this path.

	const std::string_view requestedTest = argv[1];

	for (const auto& [name, function] : Tests) {

		if (name == requestedTest) {
			return RunTest(name, function) ? 0 : 1;
		}
	}

	std::cerr << "Unknown test: " << requestedTest << '\n';
	PrintAvailableTests();

	return 2;
}
