//
//  RunnerExecutionTests.cc
//  avara3d
//
//  Created by Morgan Davis on 7/27/26.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#include <chrono>
#include <cmath>
#include <cstdint>
#include <functional>
#include <iostream>
#include <limits>
#include <memory>
#include <stdexcept>
#include <string>
#include <string_view>
#include <tuple>
#include <utility>
#include <vector>

#include "a3d/Application.h"
#include "a3d/Runner.h"
#include "a3d/SimulationConfig.h"
#include "a3d/input/InputContext.h"
#include "a3d/physics/PhysicsBody.h"
#include "a3d/physics/PhysicsInventory.h"
#include "a3d/physics/PhysicsWorld.h"
#include "a3d/physics/shape/primitive/SpherePhysicsShape.h"
#include "a3d/profile/FrameStats.h"
#include "a3d/profile/FrameStatsHistory.h"
#include "a3d/profile/Profiler.h"
#include "a3d/scene/Node.h"
#include "a3d/scene/Scene.h"
#include "a3d/visual/VisualWorld.h"

namespace a3d::testing {

	class ApplicationTestAccess {

	public:
		static void prepare(Application& application) {
			application.prepare();
		}

		static void shutdown(Application& application) {
			application.shutdown();
		}

		static Runner& runner(Application& application) {
			return *application._runner;
		}

		static void dispatchSimulationWillTick(
				Application& application,
				Scene& scene,
				const Scene::TickInfo& info) {

			application.dispatchSimulationWillTick(scene, info);
		}

		static void dispatchPhysicsWorldWillStep(
				Application& application,
				PhysicsWorld& physicsWorld,
				const PhysicsWorld::StepInfo& info) {

			application.dispatchPhysicsWorldWillStep(physicsWorld, info);
		}

		static void dispatchVisualWorldWillRender(
				Application& application,
				VisualWorld& visualWorld,
				const VisualWorld::RenderInfo& info) {

			application.dispatchVisualWorldWillRender(visualWorld, info);
		}
	};

	class RunnerTestAccess {

	public:
		using TimePoint = Runner::Clock::time_point;

		static void start(Runner& runner, TimePoint now) {
			runner.start(now);
		}

		static bool update(Runner& runner, TimePoint now) {
			return runner.update(now);
		}

		static TimePoint startTime(const Runner& runner) {
			return runner._startTime;
		}

		static const Runner::UpdateInfo& updateInfo(const Runner& runner) {
			return runner._updateInfo;
		}

		static const FrameStatsHistory& frameStatsHistory(const Runner& runner) {
			return runner._frameStatsHistory;
		}

		static std::uint64_t completedRenderFrameCount(const Runner& runner) {
			return runner._completedRenderFrameCount;
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

	class RecordingInputContext final : public a3d::InputContext {

	public:
		explicit RecordingInputContext(
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

	class PassiveInputContext final : public a3d::InputContext {

	public:
		void update() override {}
		void attachedToScene(a3d::Scene&) override {}
		void visualWorldAttachedToScene(a3d::Scene&) override {}
	};

	class RecordingApplication final : public a3d::Application {

	public:
		using RunnerAction =
			std::function<void(a3d::Runner&, const a3d::Runner::UpdateInfo&)>;
		using InputAction =
			std::function<void(a3d::InputContext&,
			                   const a3d::InputContext::UpdateInfo&)>;
		using SimulationAction =
			std::function<void(a3d::Scene&, const a3d::Scene::TickInfo&)>;
		using PhysicsAction =
			std::function<void(a3d::PhysicsWorld&,
			                   const a3d::PhysicsWorld::StepInfo&)>;
		using RenderAction =
			std::function<void(a3d::VisualWorld&,
			                   const a3d::VisualWorld::RenderInfo&)>;

		RecordingApplication():
			Application(1, Arguments()),
			runnerAction{},
			inputAction{},
			simulationWillAction{},
			simulationDidAction{},
			physicsWillAction{},
			physicsDidAction{},
			renderWillAction{},
			renderDidAction{} {}

		void enqueueSimulation(SimulationCommand command) {
			queueSimulationCommand(std::move(command));
		}

		void enqueuePhysics(PhysicsCommand command) {
			queuePhysicsCommand(std::move(command));
		}

		void enqueueRender(RenderCommand command) {
			queueRenderCommand(std::move(command));
		}

		void invokeRunnerUpdate(
				a3d::Runner& runner,
				const a3d::Runner::UpdateInfo& info) {

			runnerUpdate(runner, info);
		}

		void invokeInputContextDidUpdate(
				a3d::InputContext& inputContext,
				const a3d::InputContext::UpdateInfo& info) {

			inputContextDidUpdate(inputContext, info);
		}

		RunnerAction		runnerAction;
		InputAction			inputAction;
		SimulationAction	simulationWillAction;
		SimulationAction	simulationDidAction;
		PhysicsAction		physicsWillAction;
		PhysicsAction		physicsDidAction;
		RenderAction		renderWillAction;
		RenderAction		renderDidAction;

	protected:
		std::unique_ptr<a3d::Scene> init() override {
			return std::make_unique<a3d::Scene>();
		}

		/// Runner Callbacks ///

		void runnerUpdate(
				a3d::Runner& runner,
				const a3d::Runner::UpdateInfo& info) override {

			if (runnerAction) {
				runnerAction(runner, info);
			}
		}

		/// Input Context Callbacks ///

		void inputContextDidUpdate(
				a3d::InputContext& inputContext,
				const a3d::InputContext::UpdateInfo& info) override {

			if (inputAction) {
				inputAction(inputContext, info);
			}
		}

		/// Simulation Callbacks ///

		void simulationWillTick(
				a3d::Scene& scene,
				const a3d::Scene::TickInfo& info) override {

			if (simulationWillAction) {
				simulationWillAction(scene, info);
			}
		}

		void simulationDidTick(
				a3d::Scene& scene,
				const a3d::Scene::TickInfo& info) override {

			if (simulationDidAction) {
				simulationDidAction(scene, info);
			}
		}

		/// Physics World Callbacks ///

		void physicsWorldWillStep(
				a3d::PhysicsWorld& physicsWorld,
				const a3d::PhysicsWorld::StepInfo& info) override {

			if (physicsWillAction) {
				physicsWillAction(physicsWorld, info);
			}
		}

		void physicsWorldDidStep(
				a3d::PhysicsWorld& physicsWorld,
				const a3d::PhysicsWorld::StepInfo& info) override {

			if (physicsDidAction) {
				physicsDidAction(physicsWorld, info);
			}
		}

		/// Visual World Callbacks ///

		void visualWorldWillRender(
				a3d::VisualWorld& visualWorld,
				const a3d::VisualWorld::RenderInfo& info) override {

			if (renderWillAction) {
				renderWillAction(visualWorld, info);
			}
		}

		void visualWorldDidRender(
				a3d::VisualWorld& visualWorld,
				const a3d::VisualWorld::RenderInfo& info) override {

			if (renderDidAction) {
				renderDidAction(visualWorld, info);
			}
		}

	private:
		static char** Arguments() {

			static char executableName[] =
				"a3d-runner-execution-tests";
			static char* arguments[] = {
				executableName
			};

			return arguments;
		}
	};

	struct ApplicationRunnerAccessResult {
		std::size_t	inputCallbackCount;
		std::size_t	runnerCallbackCount;
		bool		constAndMutableAccessMatch;
		bool		callbackAndAccessMatch;
		double		initialTimeScale;
		double		runnerCallbackTimeScale;
	};

	class RunnerAccessApplication final : public a3d::Application {

	public:
		explicit RunnerAccessApplication(
				ApplicationRunnerAccessResult& result):
			Application(1, Arguments()),
			_result{result} {}

		a3d::Runner& runnerBeforeInitialization() {
			return runner();
		}

	protected:
		std::unique_ptr<a3d::Scene> init() override {

			auto scene = std::make_unique<a3d::Scene>();
			scene->inputContext(
				std::make_unique<PassiveInputContext>());
			return scene;
		}

		/// Runner Callbacks ///

		void runnerUpdate(
				a3d::Runner& runner,
				const a3d::Runner::UpdateInfo&) override {

			++_result.runnerCallbackCount;
			_result.callbackAndAccessMatch =
				&runner == &this->runner();
			_result.runnerCallbackTimeScale =
				this->runner().timeScale();

			this->runner().stop();
		}

		/// Input Context Callbacks ///

		void inputContextDidUpdate(
				a3d::InputContext& inputContext,
				const a3d::InputContext::UpdateInfo&) override {

			(void)inputContext;
			++_result.inputCallbackCount;

			const auto& constApplication = *this;
			_result.constAndMutableAccessMatch =
				&constApplication.runner() == &runner();
			_result.initialTimeScale =
				constApplication.runner().timeScale();

			runner().timeScale(0.5);
		}

	private:
		static char** Arguments() {

			static char executableName[] =
				"a3d-runner-execution-tests";
			static char* arguments[] = {
				executableName
			};

			return arguments;
		}

		ApplicationRunnerAccessResult& _result;
	};

	struct StopDuringInputResult {
		std::size_t	inputCallbackCount;
		std::size_t	runnerCallbackCount;
		std::size_t	simulationWillCount;
		std::size_t	simulationDidCount;
		std::size_t	physicsWillCount;
		std::size_t	physicsDidCount;
		bool		inputObservedStoppedState;
	};

	class StopDuringInputApplication final : public a3d::Application {

	public:
		explicit StopDuringInputApplication(
				StopDuringInputResult& result):
			Application(1, Arguments()),
			_result{result},
			_stopDuringInput{false} {}

		void stopDuringNextInputUpdate() {
			_stopDuringInput = true;
		}

	protected:
		std::unique_ptr<a3d::Scene> init() override {

			auto scene = std::make_unique<a3d::Scene>();
			scene->inputContext(
				std::make_unique<PassiveInputContext>());
			scene->physicsWorld(
				std::make_unique<a3d::PhysicsWorld>());
			return scene;
		}

		a3d::SimulationConfig simulationConfig() const override {

			auto config = a3d::SimulationConfig{};
			config.timing = a3d::SimulationTiming::FixedStep;
			config.fixedDeltaTime = 0.125;
			return config;
		}

		/// Runner Callbacks ///

		void runnerUpdate(
				a3d::Runner& runner,
				const a3d::Runner::UpdateInfo&) override {

			(void)runner;
			++_result.runnerCallbackCount;
		}

		/// Input Context Callbacks ///

		void inputContextDidUpdate(
				a3d::InputContext& inputContext,
				const a3d::InputContext::UpdateInfo&) override {

			(void)inputContext;
			++_result.inputCallbackCount;

			if (_stopDuringInput) {
				runner().stop();
				_result.inputObservedStoppedState =
					runner().state() == a3d::Runner::State::Stopped;
			}
		}

		/// Simulation Callbacks ///

		void simulationWillTick(
				a3d::Scene& scene,
				const a3d::Scene::TickInfo&) override {

			(void)scene;
			++_result.simulationWillCount;
		}

		void simulationDidTick(
				a3d::Scene& scene,
				const a3d::Scene::TickInfo&) override {

			(void)scene;
			++_result.simulationDidCount;
		}

		/// Physics World Callbacks ///

		void physicsWorldWillStep(
				a3d::PhysicsWorld& physicsWorld,
				const a3d::PhysicsWorld::StepInfo&) override {

			(void)physicsWorld;
			++_result.physicsWillCount;
		}

		void physicsWorldDidStep(
				a3d::PhysicsWorld& physicsWorld,
				const a3d::PhysicsWorld::StepInfo&) override {

			(void)physicsWorld;
			++_result.physicsDidCount;
		}

	private:
		static char** Arguments() {

			static char executableName[] =
				"a3d-runner-execution-tests";
			static char* arguments[] = {
				executableName
			};

			return arguments;
		}

		StopDuringInputResult&	_result;
		bool					_stopDuringInput;
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

	a3d::SimulationConfig FixedConfiguration(double deltaTime = 0.125,
													 std::uint32_t maxCatchUpSteps = 8,
													 double timeScale = 1.0) {

		auto configuration = a3d::SimulationConfig{};
		configuration.timing = a3d::SimulationTiming::FixedStep;
		configuration.fixedDeltaTime = deltaTime;
		configuration.maxCatchUpSteps = maxCatchUpSteps;
		configuration.timeScale = timeScale;
		return configuration;
	}

	RecordedStep RecordStep(const a3d::Scene::TickInfo& info) {
		return {
			.tickIndex = info.tickIndex,
			.startTime = info.startTime,
			.endTime = info.endTime,
			.deltaTime = info.deltaTime
		};
	}

	RecordedStep RecordStep(const a3d::PhysicsWorld::StepInfo& info) {
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
			[&times](a3d::Runner&, const a3d::Runner::UpdateInfo& info) {
				times.push_back({info.elapsedTime, info.deltaTime});
			});

		runner.start();

		Expect(
			runner.update(),
			"the running Runner should continue");
		Expect(times.size() == 1, "the Runner callback should run once");
		Expect(
			times[0].elapsedTime >= 0.0,
			"the first elapsed time should use the monotonic update clock");
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
			[&times](a3d::Runner&, const a3d::Runner::UpdateInfo& info) {
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
			[&times](a3d::Runner&, const a3d::Runner::UpdateInfo& info) {
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
			a3d::testing::RunnerTestAccess::updateInfo(runner).updateIndex == 0,
			"the first update index should be zero");

		a3d::testing::RunnerTestAccess::update(
			runner, AtMilliseconds(4200));
		Expect(
			a3d::testing::RunnerTestAccess::updateInfo(runner).updateIndex == 1,
			"the second update index should be one");

		a3d::testing::RunnerTestAccess::update(
			runner, AtMilliseconds(4300));
		Expect(
			a3d::testing::RunnerTestAccess::updateInfo(runner).updateIndex == 2,
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
			[&firstTimes](a3d::Runner&, const a3d::Runner::UpdateInfo& info) {
				firstTimes.push_back({info.elapsedTime, info.deltaTime});
			});
		secondRunner.updateCallback(
			[&secondTimes](a3d::Runner&, const a3d::Runner::UpdateInfo& info) {
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
			a3d::testing::RunnerTestAccess::updateInfo(firstRunner).updateIndex == 1,
			"the first Runner should have its own update index");
		Expect(
			a3d::testing::RunnerTestAccess::updateInfo(secondRunner).updateIndex == 1,
			"the second Runner should have its own update index");
	}

	void SimulationConfigDefaults() {

		const a3d::SimulationConfig defaults;

		Expect(
			defaults.timing == a3d::SimulationTiming::VariableStep,
			"simulation timing should default to VariableStep");
		ExpectNear(
			defaults.fixedDeltaTime,
			1.0 / 60.0,
			"the default fixed delta");
		Expect(
			defaults.maxCatchUpSteps == 8,
			"the default maximum catch-up count");
		ExpectNear(defaults.timeScale, 1.0, "the default time scale");
		ExpectNear(
			defaults.maxVariableStepDeltaTime,
			0.25,
			"the default maximum VariableStep delta");

		a3d::Scene scene;
		a3d::Runner runner(scene);
		const auto& runnerConfiguration = runner.simulationConfig();

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
			runnerConfiguration.maxVariableStepDeltaTime,
			defaults.maxVariableStepDeltaTime,
			"the default Runner maximum VariableStep delta");
	}

	void SimulationConfigValidation() {

		auto expectConfigurationRejected =
			[](a3d::SimulationConfig configuration,
			   std::string_view message) {

				a3d::Scene scene;
				ExpectInvalidArgument(
					[&] {
						a3d::Runner runner(scene, configuration);
					},
					message);
			};

		{
			auto configuration = a3d::SimulationConfig{};
			configuration.fixedDeltaTime =
				std::numeric_limits<double>::quiet_NaN();

			a3d::Scene scene;
			scene.physicsWorld(
				std::make_unique<a3d::PhysicsWorld>());
			a3d::Runner runner(scene, configuration);

			Expect(
				std::isnan(
					runner.simulationConfig().fixedDeltaTime),
				"VariableStep should preserve its ignored fixed delta");
		}

		{
			auto configuration = a3d::SimulationConfig{};
			configuration.maxCatchUpSteps = 0;

			a3d::Scene scene;
			a3d::Runner runner(scene, configuration);

			Expect(
				runner.simulationConfig().maxCatchUpSteps == 0,
				"VariableStep should preserve its ignored catch-up limit");
		}

		auto configuration = a3d::SimulationConfig{};
		configuration.maxVariableStepDeltaTime = 0.0;
		expectConfigurationRejected(
			configuration,
			"VariableStep should reject a zero maximum delta");

		configuration = {};
		configuration.maxVariableStepDeltaTime =
			std::numeric_limits<double>::infinity();
		expectConfigurationRejected(
			configuration,
			"VariableStep should reject an infinite maximum delta");

		{
			configuration = FixedConfiguration();
			configuration.maxVariableStepDeltaTime =
				std::numeric_limits<double>::quiet_NaN();

			a3d::Scene scene;
			a3d::Runner runner(scene, configuration);

			Expect(
				std::isnan(
					runner.simulationConfig().
						maxVariableStepDeltaTime),
				"FixedStep should preserve its ignored VariableStep maximum");
		}

		configuration = FixedConfiguration(0.0);
		expectConfigurationRejected(
			configuration,
			"FixedStep should reject a zero fixed delta");

		configuration = FixedConfiguration(-0.125);
		expectConfigurationRejected(
			configuration,
			"FixedStep should reject a negative fixed delta");

		configuration = FixedConfiguration(
			std::numeric_limits<double>::infinity());
		expectConfigurationRejected(
			configuration,
			"FixedStep should reject an infinite fixed delta");

		configuration = FixedConfiguration(
			std::numeric_limits<double>::quiet_NaN());
		expectConfigurationRejected(
			configuration,
			"FixedStep should reject a NaN fixed delta");

		configuration = FixedConfiguration(0.125, 0);
		expectConfigurationRejected(
			configuration,
			"FixedStep should reject a zero maximum catch-up count");

		configuration = {};
		configuration.timeScale = 0.0;
		expectConfigurationRejected(
			configuration,
			"VariableStep should reject a zero configured time scale");

		configuration = {};
		configuration.timeScale =
			std::numeric_limits<double>::quiet_NaN();
		expectConfigurationRejected(
			configuration,
			"VariableStep should reject a NaN configured time scale");

		configuration = FixedConfiguration(
			0.125,
			8,
			0.0);
		expectConfigurationRejected(
			configuration,
			"FixedStep should reject a zero configured time scale");

		configuration = FixedConfiguration(
			0.125,
			8,
			std::numeric_limits<double>::infinity());
		expectConfigurationRejected(
			configuration,
			"FixedStep should reject an infinite configured time scale");

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
		std::size_t simulationWillCount = 0;
		std::size_t physicsWillCount = 0;
		std::size_t physicsDidCount = 0;
		std::size_t simulationDidCount = 0;

		a3d::Runner runner(scene, FixedConfiguration());
		runner.updateCallback(
			[&runnerCallbackCount](a3d::Runner&,
								   const a3d::Runner::UpdateInfo&) {

				++runnerCallbackCount;
			});
		scene.willTickCallback(
			[&simulationWillCount](a3d::Scene&,
							  const a3d::Scene::TickInfo&) {

				++simulationWillCount;
			});
		scene.physicsWorld()->willStepCallback(
			[&physicsWillCount](a3d::PhysicsWorld&,
								const a3d::PhysicsWorld::StepInfo&) {

				++physicsWillCount;
			});
		scene.physicsWorld()->didStepCallback(
			[&physicsDidCount](a3d::PhysicsWorld&,
							   const a3d::PhysicsWorld::StepInfo&) {

				++physicsDidCount;
			});
		scene.didTickCallback(
			[&simulationDidCount](a3d::Scene&,
							 const a3d::Scene::TickInfo&) {

				++simulationDidCount;
			});

		a3d::testing::RunnerTestAccess::start(
			runner, AtMilliseconds(9000));
		Expect(
			a3d::testing::RunnerTestAccess::update(
				runner, AtMilliseconds(9500)),
			"the first Runner update should continue");

		Expect(
			runnerCallbackCount == 1,
			"the first Runner update should invoke the Runner callback");
		Expect(
			simulationWillCount == 0
				&& physicsWillCount == 0
				&& physicsDidCount == 0
				&& simulationDidCount == 0,
			"the first Runner update should execute no simulation callbacks");
		Expect(
			runner.simulationTickCount() == 0,
			"the first Runner update should complete no simulation ticks");
		ExpectNear(
			runner.simulationTime(),
			0.0,
			"the first Runner update should not advance simulation time");
		ExpectNear(
			node->physicsBody()->centerOfMass().x,
			0.0,
			"the first Runner update should not advance Bullet",
			1e-6);

		const auto& stats = LatestFrameStats(runner);
		Expect(
			stats.simulationTickCount == 0,
			"the first update statistics sample should report zero ticks");
		ExpectNear(
			stats.discardedSimulationTime,
			0.0,
			"the first update statistics sample should report no discarded time");
		Expect(
			stats.numDynamicBodies == 1,
			"a zero-tick Runner update should still sample current body inventory");
		Expect(
			stats.numPrimitiveShapes == 1,
			"a zero-tick Runner update should still sample current shape inventory");
	}

	void VariableStepScalesCapsAndReportsDiscard() {

		a3d::Scene scene;
		a3d::Runner runner(scene);
		std::vector<RecordedStep> steps;
		scene.didTickCallback(
			[&steps](a3d::Scene&,
					 const a3d::Scene::TickInfo& info) {

				steps.push_back(RecordStep(info));
			});

		a3d::testing::RunnerTestAccess::start(
			runner, AtMilliseconds(12000));
		Expect(
			a3d::testing::RunnerTestAccess::update(
				runner, AtMilliseconds(12100)),
			"the priming VariableStep update should continue");
		Expect(
			steps.empty(),
			"the first VariableStep update should execute no simulation tick");
		Expect(
			LatestFrameStats(runner).simulationTickCount == 0,
			"the first VariableStep update should report zero ticks");

		runner.timeScale(2.0);
		Expect(
			a3d::testing::RunnerTestAccess::update(
				runner, AtMilliseconds(12600)),
			"the capped VariableStep update should continue");

		Expect(steps.size() == 1, "VariableStep should execute one capped tick");
		ExpectStep(
			steps[0],
			0,
			0.0,
			0.25,
			0.25,
			"the capped VariableStep tick");
		ExpectNear(
			runner.simulationTime(),
			0.25,
			"capped VariableStep simulation time");
		Expect(
			runner.simulationTickCount() == 1,
			"capped VariableStep completed tick count");

		const auto cappedStats = LatestFrameStats(runner);
		Expect(
			cappedStats.simulationTickCount == 1,
			"the capped Runner update should report one tick");
		ExpectNear(
			cappedStats.discardedSimulationTime,
			0.75,
			"VariableStep should report scaled time above its cap");

		Expect(
			a3d::testing::RunnerTestAccess::update(
				runner, AtMilliseconds(12700)),
			"the uncapped VariableStep update should continue");

		Expect(steps.size() == 2, "VariableStep should execute a second tick");
		ExpectStep(
			steps[1],
			1,
			0.25,
			0.45,
			0.2,
			"the uncapped VariableStep tick");
		ExpectNear(
			runner.simulationTime(),
			0.45,
			"discarded VariableStep time should not advance simulation time");
		Expect(
			runner.simulationTickCount() == 2,
			"VariableStep should complete two ticks");

		const auto& uncappedStats = LatestFrameStats(runner);
		Expect(
			uncappedStats.simulationTickCount == 1,
			"the uncapped Runner update should report one tick");
		ExpectNear(
			uncappedStats.discardedSimulationTime,
			0.0,
			"the uncapped Runner update should report no discarded time");
	}

	void FixedStepRetainsFractionalAccumulator() {

		a3d::Scene scene;
		a3d::Runner runner(scene, FixedConfiguration());
		std::vector<RecordedStep> steps;
		scene.didTickCallback(
			[&steps](a3d::Scene&,
					 const a3d::Scene::TickInfo& info) {

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
			"the accumulated Runner update should report one tick");
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
			"the scaled Runner update should report one fixed tick");

		const auto& samples =
			a3d::testing::RunnerTestAccess::frameStatsHistory(runner).samples();
		Expect(
			samples.size() == 4,
			"FixedStep should commit one statistics record per Runner update");
	}

	void FixedStepLimitsCatchUpAndReportsDiscard() {

		a3d::Scene scene;
		a3d::Runner runner(scene, FixedConfiguration(0.125, 2));
		std::vector<RecordedStep> steps;
		scene.didTickCallback(
			[&steps](a3d::Scene&,
					 const a3d::Scene::TickInfo& info) {

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
			"the bounded catch-up Runner update should report two ticks");
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
			"the retained-remainder Runner update should report one tick");
		ExpectNear(
			remainderStats.discardedSimulationTime,
			0.0,
			"discarded time should reset for each update statistics batch");
	}

	void StopBetweenUpdates() {

		a3d::Scene scene;
		a3d::Runner runner(scene);
		std::size_t callbackCount = 0;
		runner.updateCallback(
			[&callbackCount](a3d::Runner&, const a3d::Runner::UpdateInfo&) {
				++callbackCount;
			});

		a3d::testing::RunnerTestAccess::start(
			runner, AtMilliseconds(6000));
		Expect(
			a3d::testing::RunnerTestAccess::update(
				runner, AtMilliseconds(6100)),
			"the Runner should continue before stop");

		const auto infoBeforeStop =
			a3d::testing::RunnerTestAccess::updateInfo(runner);
		runner.stop();

		Expect(
			!a3d::testing::RunnerTestAccess::update(
				runner, AtMilliseconds(6500)),
			"a stopped Runner should not update");
		Expect(callbackCount == 1, "stop should prevent later Runner callbacks");

		const auto& infoAfterStop =
			a3d::testing::RunnerTestAccess::updateInfo(runner);
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

	void StopDuringInputUpdate() {

		StopDuringInputResult result{};
		auto application =
			std::make_unique<StopDuringInputApplication>(result);

		a3d::testing::ApplicationTestAccess::prepare(*application);

		auto& runner =
			a3d::testing::ApplicationTestAccess::runner(*application);
		const auto startTime =
			a3d::testing::RunnerTestAccess::startTime(runner);

		Expect(
			a3d::testing::RunnerTestAccess::update(
				runner,
				startTime),
			"the priming input update should continue");

		const auto sampleCountBeforeStop =
			a3d::testing::RunnerTestAccess::
				frameStatsHistory(runner).samples().size();

		result = {};
		application->stopDuringNextInputUpdate();

		Expect(
			!a3d::testing::RunnerTestAccess::update(
				runner,
				startTime + std::chrono::milliseconds(125)),
			"an update stopped from inputContextDidUpdate should return false");
		Expect(
			result.inputCallbackCount == 1,
			"the stopping input callback should run exactly once");
		Expect(
			result.inputObservedStoppedState,
			"inputContextDidUpdate should observe the Runner as stopped");
		Expect(
			result.runnerCallbackCount == 0,
			"stop during inputContextDidUpdate should skip runnerUpdate");
		Expect(
			result.simulationWillCount == 0
				&& result.simulationDidCount == 0,
			"stop during inputContextDidUpdate should skip simulation");
		Expect(
			result.physicsWillCount == 0
				&& result.physicsDidCount == 0,
			"stop during inputContextDidUpdate should skip physics");
		Expect(
			runner.simulationTickCount() == 0,
			"stop during inputContextDidUpdate should complete no simulation tick");
		Expect(
			a3d::testing::RunnerTestAccess::completedRenderFrameCount(runner) == 0,
			"stop during inputContextDidUpdate should complete no render frame");
		Expect(
			LatestFrameStats(runner).simulationTickCount == 0,
			"the stopping input update should commit zero-tick statistics");
		Expect(
			a3d::testing::RunnerTestAccess::
				frameStatsHistory(runner).samples().size()
					== sampleCountBeforeStop + 1,
			"the stopping input update should commit one statistics sample");
		Expect(
			runner.state() == a3d::Runner::State::Stopped,
			"the Runner should remain stopped after the input callback");

		Expect(
			!a3d::testing::RunnerTestAccess::update(
				runner,
				startTime + std::chrono::milliseconds(250)),
			"a later update should remain stopped");
		Expect(
			result.inputCallbackCount == 1,
			"a stopped Runner should not update input again");

		a3d::testing::ApplicationTestAccess::shutdown(*application);
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
											   const a3d::Runner::UpdateInfo&) {

				stages.push_back("runner");
				++callbackCount;
				runner.stop();
			});
		scene.didTickCallback(
			[&simulationCount, &stages](a3d::Scene&,
										const a3d::Scene::TickInfo&) {

				stages.push_back("simulation-did");
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
			a3d::testing::RunnerTestAccess::updateInfo(runner).updateIndex == 1,
			"the stopping update should retain its update index");
		Expect(
			LatestFrameStats(runner).simulationTickCount == 0,
			"the stopping Runner update should report zero simulation ticks");
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
		scene.willTickCallback(
			[&runner, &stages](a3d::Scene&,
							   const a3d::Scene::TickInfo&) {

				stages.push_back("simulation-will");
				runner.stop();
			});
		scene.physicsWorld()->willStepCallback(
			[&stages](a3d::PhysicsWorld&,
					   const a3d::PhysicsWorld::StepInfo&) {

				stages.push_back("physics-will");
			});
		scene.physicsWorld()->didStepCallback(
			[&stages](a3d::PhysicsWorld&,
					   const a3d::PhysicsWorld::StepInfo&) {

				stages.push_back("physics-did");
			});
		scene.didTickCallback(
			[&stages](a3d::Scene&,
					   const a3d::Scene::TickInfo&) {

				stages.push_back("simulation-did");
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
				&& stages[0] == "simulation-will"
				&& stages[1] == "physics-will"
				&& stages[2] == "physics-did"
				&& stages[3] == "simulation-did",
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
			"the stopped Runner update should report its completed tick");
		Expect(
			runner.state() == a3d::Runner::State::Stopped,
			"the Runner should remain stopped after the simulation tick");
		Expect(
			a3d::testing::RunnerTestAccess::completedRenderFrameCount(runner) == 0,
			"stopping during simulation should not complete a render frame");
	}

	void InputCallbacksRunBeforeRunnerAndSimulation() {

		a3d::Scene scene;
		std::vector<std::string_view> stages;
		auto inputContext =
			std::make_unique<RecordingInputContext>(stages);
		auto* inputContextPtr = inputContext.get();
		scene.inputContext(std::move(inputContext));

		RecordingApplication application;
		a3d::InputContext* callbackInputContext = nullptr;
		std::vector<a3d::InputContext::UpdateInfo> inputUpdates;

		application.inputAction =
			[&](a3d::InputContext& callbackContext,
			    const a3d::InputContext::UpdateInfo& info) {

				stages.push_back("input-callback");
				callbackInputContext = &callbackContext;
				inputUpdates.push_back(info);
			};
		application.runnerAction =
			[&](a3d::Runner&, const a3d::Runner::UpdateInfo&) {
				stages.push_back("runner");
			};
		application.simulationWillAction =
			[&](a3d::Scene&, const a3d::Scene::TickInfo&) {
				stages.push_back("simulation-will");
			};

		inputContextPtr->didUpdateCallback(
			[&](a3d::InputContext& callbackContext,
			    const a3d::InputContext::UpdateInfo& info) {

				application.invokeInputContextDidUpdate(
					callbackContext,
					info);
			});

		a3d::Runner runner(scene, FixedConfiguration());
		runner.updateCallback(
			[&](a3d::Runner& callbackRunner,
			    const a3d::Runner::UpdateInfo& info) {

				application.invokeRunnerUpdate(callbackRunner, info);
			});
		scene.willTickCallback(
			[&](a3d::Scene& callbackScene,
			    const a3d::Scene::TickInfo& info) {

				a3d::testing::ApplicationTestAccess::
					dispatchSimulationWillTick(
						application,
						callbackScene,
						info);
			});
		scene.didTickCallback(
			[&](a3d::Scene&, const a3d::Scene::TickInfo&) {
				stages.push_back("simulation-did");
			});

		a3d::testing::RunnerTestAccess::start(
			runner,
			AtMilliseconds(2000));
		Expect(
			a3d::testing::RunnerTestAccess::update(
				runner,
				AtMilliseconds(2100)),
			"the priming input-callback update should continue");
		Expect(
			stages.size() == 3
				&& stages[0] == "input"
				&& stages[1] == "input-callback"
				&& stages[2] == "runner",
			"input update and callback should precede the Runner callback");

		stages.clear();
		Expect(
			a3d::testing::RunnerTestAccess::update(
				runner,
				AtMilliseconds(2225)),
			"the scheduled input-callback update should continue");
		Expect(
			stages.size() == 5
				&& stages[0] == "input"
				&& stages[1] == "input-callback"
				&& stages[2] == "runner"
				&& stages[3] == "simulation-will"
				&& stages[4] == "simulation-did",
			"input callback and Runner callback should precede simulation");
		Expect(
			callbackInputContext == inputContextPtr,
			"the input callback should receive its owning InputContext");
		Expect(
			inputUpdates.size() == 2,
			"the input callback should run once per Runner update");
		Expect(
			inputUpdates.back().updateIndex == 1,
			"the second input callback should receive update index one");
		ExpectNear(
			inputUpdates.back().elapsedTime,
			0.225,
			"the second input callback elapsed time");
		ExpectNear(
			inputUpdates.back().deltaTime,
			0.125,
			"the second input callback delta time");
	}

	void NoInputContextSkipsInputCallback() {

		a3d::Scene scene;
		Expect(
			scene.inputContext() == nullptr,
			"the no-input fixture should not have an InputContext");

		RecordingApplication application;
		std::size_t inputCallbackCount = 0;
		std::size_t runnerCallbackCount = 0;
		std::size_t simulationTickCount = 0;

		application.inputAction =
			[&](a3d::InputContext&,
			    const a3d::InputContext::UpdateInfo&) {
				++inputCallbackCount;
			};
		application.runnerAction =
			[&](a3d::Runner&, const a3d::Runner::UpdateInfo&) {
				++runnerCallbackCount;
			};

		a3d::Runner runner(scene, FixedConfiguration());
		runner.updateCallback(
			[&](a3d::Runner& callbackRunner,
			    const a3d::Runner::UpdateInfo& info) {

				application.invokeRunnerUpdate(callbackRunner, info);
			});
		scene.didTickCallback(
			[&](a3d::Scene&, const a3d::Scene::TickInfo&) {
				++simulationTickCount;
			});

		a3d::testing::RunnerTestAccess::start(
			runner,
			AtMilliseconds(3000));
		Expect(
			a3d::testing::RunnerTestAccess::update(
				runner,
				AtMilliseconds(3100)),
			"the priming no-input update should continue");
		Expect(
			a3d::testing::RunnerTestAccess::update(
				runner,
				AtMilliseconds(3225)),
			"the scheduled no-input update should continue");

		Expect(
			inputCallbackCount == 0,
			"no InputContext should skip inputContextDidUpdate");
		Expect(
			runnerCallbackCount == 2,
			"runnerUpdate should run without an InputContext");
		Expect(
			simulationTickCount == 1,
			"simulation should remain scheduled without an InputContext");
	}

	void ApplicationRunnerAccessUsesPreparedRunner() {

		ApplicationRunnerAccessResult result{};

		auto application =
			std::make_unique<RunnerAccessApplication>(result);

		bool uninitializedAccessRejected = false;
		try {
			(void)application->runnerBeforeInitialization();
		}
		catch (const std::logic_error&) {
			uninitializedAccessRejected = true;
		}

		Expect(
			uninitializedAccessRejected,
			"Application Runner access should reject an uninitialized Runner");

		const int status =
			a3d::Application::Run(std::move(application));

		Expect(
			status == 0,
			"the Runner-access Application should exit successfully");
		Expect(
			result.inputCallbackCount == 1,
			"the Runner-access input callback should run once");
		Expect(
			result.runnerCallbackCount == 1,
			"the Runner-access Runner callback should run once");
		Expect(
			result.constAndMutableAccessMatch,
			"const and mutable Application Runner access should match");
		Expect(
			result.callbackAndAccessMatch,
			"runnerUpdate should receive the Application's Runner");
		ExpectNear(
			result.initialTimeScale,
			1.0,
			"the Runner-access initial time scale");
		ExpectNear(
			result.runnerCallbackTimeScale,
			0.5,
			"runnerUpdate should observe the input callback's time scale");
	}

	void SimulationCommandsRespectTickBoundaries() {

		a3d::Scene scene;
		std::vector<std::string_view> inputStages;
		auto inputContext =
			std::make_unique<RecordingInputContext>(inputStages);
		auto* inputContextPtr = inputContext.get();
		scene.inputContext(std::move(inputContext));

		RecordingApplication application;
		std::vector<int> events;
		std::size_t commandExecutionCount = 0;

		application.inputAction =
			[&](a3d::InputContext&,
			    const a3d::InputContext::UpdateInfo& info) {

				if (info.updateIndex != 0) {
					return;
				}

				application.enqueueSimulation(
					[&](a3d::Scene& commandScene) {

						Expect(
							&commandScene == &scene,
							"a simulation command should receive the Scene");
						events.push_back(1);
						++commandExecutionCount;

						application.enqueueSimulation(
							[&](a3d::Scene& deferredScene) {

								Expect(
									&deferredScene == &scene,
									"a deferred simulation command should receive the Scene");
								events.push_back(3);
								++commandExecutionCount;
							});
					});
			};
		application.runnerAction =
			[&](a3d::Runner&, const a3d::Runner::UpdateInfo& info) {

				if (info.updateIndex == 0) {
					application.enqueueSimulation(
						[&](a3d::Scene& commandScene) {

							Expect(
								&commandScene == &scene,
								"a queued simulation command should receive the Scene");
							events.push_back(2);
							++commandExecutionCount;
						});
				}
			};
		application.simulationWillAction =
			[&](a3d::Scene&, const a3d::Scene::TickInfo& info) {
				events.push_back(10 + static_cast<int>(info.tickIndex));
			};

		inputContextPtr->didUpdateCallback(
			[&](a3d::InputContext& callbackContext,
			    const a3d::InputContext::UpdateInfo& info) {

				application.invokeInputContextDidUpdate(
					callbackContext,
					info);
			});

		a3d::Runner runner(scene, FixedConfiguration());
		runner.updateCallback(
			[&](a3d::Runner& callbackRunner,
			    const a3d::Runner::UpdateInfo& info) {

				application.invokeRunnerUpdate(callbackRunner, info);
			});
		scene.willTickCallback(
			[&](a3d::Scene& callbackScene,
			    const a3d::Scene::TickInfo& info) {

				a3d::testing::ApplicationTestAccess::
					dispatchSimulationWillTick(
						application,
						callbackScene,
						info);
			});
		scene.didTickCallback(
			[&](a3d::Scene&, const a3d::Scene::TickInfo& info) {
				events.push_back(20 + static_cast<int>(info.tickIndex));
			});

		a3d::testing::RunnerTestAccess::start(
			runner,
			AtMilliseconds(4000));
		Expect(
			a3d::testing::RunnerTestAccess::update(
				runner,
				AtMilliseconds(4100)),
			"the zero-tick command-queue update should continue");
		Expect(
			events.empty(),
			"simulation commands should survive a zero-tick Runner update");

		Expect(
			a3d::testing::RunnerTestAccess::update(
				runner,
				AtMilliseconds(4350)),
			"the catch-up command-queue update should continue");

		const std::vector<int> expectedCatchUpEvents {
			1,
			2,
			10,
			20,
			3,
			11,
			21
		};
		Expect(
			events == expectedCatchUpEvents,
			"simulation commands should be FIFO, boundary-scoped, and deferred by batch");
		Expect(
			commandExecutionCount == 3,
			"each simulation command should execute exactly once");

		Expect(
			a3d::testing::RunnerTestAccess::update(
				runner,
				AtMilliseconds(4475)),
			"the command-free simulation update should continue");

		const std::vector<int> expectedFinalEvents {
			1,
			2,
			10,
			20,
			3,
			11,
			21,
			12,
			22
		};
		Expect(
			events == expectedFinalEvents,
			"later ticks should not repeat executed simulation commands");
		Expect(
			commandExecutionCount == 3,
			"simulation commands should remain exactly-once after later ticks");
	}

	void PhysicsCommandsRespectStepBoundaries() {

		a3d::Scene scene;
		scene.physicsWorld(std::make_unique<a3d::PhysicsWorld>());

		RecordingApplication application;
		std::vector<int> events;
		std::size_t commandExecutionCount = 0;

		application.enqueuePhysics(
			[&](a3d::PhysicsWorld& commandWorld) {

				Expect(
					&commandWorld == scene.physicsWorld(),
					"a physics command should receive the PhysicsWorld");
				events.push_back(1);
				++commandExecutionCount;

				application.enqueuePhysics(
					[&](a3d::PhysicsWorld& deferredWorld) {

						Expect(
							&deferredWorld == scene.physicsWorld(),
							"a deferred physics command should receive the PhysicsWorld");
						events.push_back(3);
						++commandExecutionCount;
					});
			});
		application.simulationWillAction =
			[&](a3d::Scene&, const a3d::Scene::TickInfo& info) {

				if (info.tickIndex == 0) {
					application.enqueuePhysics(
						[&](a3d::PhysicsWorld& commandWorld) {

							Expect(
								&commandWorld == scene.physicsWorld(),
								"a simulation-queued physics command should receive the PhysicsWorld");
							events.push_back(2);
							++commandExecutionCount;
						});
				}
			};
		application.physicsWillAction =
			[&](a3d::PhysicsWorld&,
			    const a3d::PhysicsWorld::StepInfo& info) {
				events.push_back(10 + static_cast<int>(info.tickIndex));
			};

		scene.willTickCallback(
			[&](a3d::Scene& callbackScene,
			    const a3d::Scene::TickInfo& info) {

				a3d::testing::ApplicationTestAccess::
					dispatchSimulationWillTick(
						application,
						callbackScene,
						info);
			});
		scene.physicsWorld()->willStepCallback(
			[&](a3d::PhysicsWorld& callbackWorld,
			    const a3d::PhysicsWorld::StepInfo& info) {

				a3d::testing::ApplicationTestAccess::
					dispatchPhysicsWorldWillStep(
						application,
						callbackWorld,
						info);
			});
		scene.physicsWorld()->didStepCallback(
			[&](a3d::PhysicsWorld&,
			    const a3d::PhysicsWorld::StepInfo& info) {
				events.push_back(20 + static_cast<int>(info.tickIndex));
			});

		a3d::Runner runner(scene, FixedConfiguration());
		a3d::testing::RunnerTestAccess::start(
			runner,
			AtMilliseconds(5000));
		Expect(
			a3d::testing::RunnerTestAccess::update(
				runner,
				AtMilliseconds(5100)),
			"the zero-step physics-command update should continue");
		Expect(
			events.empty(),
			"physics commands should survive a Runner update with no physics step");

		Expect(
			a3d::testing::RunnerTestAccess::update(
				runner,
				AtMilliseconds(5350)),
			"the catch-up physics-command update should continue");

		const std::vector<int> expectedCatchUpEvents {
			1,
			2,
			10,
			20,
			3,
			11,
			21
		};
		Expect(
			events == expectedCatchUpEvents,
			"physics commands should be FIFO, pre-step, and deferred by batch");
		Expect(
			commandExecutionCount == 3,
			"each physics command should execute exactly once");

		Expect(
			a3d::testing::RunnerTestAccess::update(
				runner,
				AtMilliseconds(5475)),
			"the command-free physics update should continue");

		const std::vector<int> expectedFinalEvents {
			1,
			2,
			10,
			20,
			3,
			11,
			21,
			12,
			22
		};
		Expect(
			events == expectedFinalEvents,
			"later physics steps should not repeat executed commands");
		Expect(
			commandExecutionCount == 3,
			"physics commands should remain exactly-once after later steps");
	}

	void RenderCommandsWaitWithoutAValidRender() {

		RecordingApplication application;
		std::size_t commandExecutionCount = 0;
		application.enqueueRender(
			[&](a3d::VisualWorld&) {
				++commandExecutionCount;
			});

		a3d::Scene scene;
		Expect(
			scene.visualWorld() == nullptr,
			"the render-command fixture should not have a VisualWorld");

		a3d::Runner runner(scene);
		a3d::testing::RunnerTestAccess::start(
			runner,
			AtMilliseconds(6000));
		Expect(
			a3d::testing::RunnerTestAccess::update(
				runner,
				AtMilliseconds(6100)),
			"the first no-render command update should continue");
		Expect(
			a3d::testing::RunnerTestAccess::update(
				runner,
				AtMilliseconds(6200)),
			"the second no-render command update should continue");
		Expect(
			commandExecutionCount == 0,
			"a render command should not execute without a valid render boundary");
	}

	void SimulationCallbacksRunInOrderWithPhysics() {

		a3d::Scene scene;
		std::vector<std::string_view> stages;
		auto inputContext = std::make_unique<RecordingInputContext>(stages);
		auto* inputContextPtr = inputContext.get();
		scene.inputContext(std::move(inputContext));
		scene.physicsWorld(std::make_unique<a3d::PhysicsWorld>());

		a3d::Runner runner(scene, FixedConfiguration());
		a3d::testing::RunnerTestAccess::start(
			runner, AtMilliseconds(8000));

		// Prime the Runner so the observed Runner update has a nonzero delta.
		Expect(
			a3d::testing::RunnerTestAccess::update(
				runner, AtMilliseconds(8100)),
			"the priming Runner update should continue");
		stages.clear();
		inputContextPtr->reset();

		a3d::Runner* callbackRunner = nullptr;
		const a3d::Runner::UpdateInfo* callbackInfo = nullptr;
		std::vector<CallbackTime> runnerTimes;
		std::vector<RecordedStep> simulationSteps;
		a3d::Scene* callbackScene = nullptr;
		a3d::PhysicsWorld* callbackPhysicsWorld = nullptr;
		runner.updateCallback(
			[&callbackRunner,
			 &callbackInfo,
			 &runnerTimes,
			 &stages](a3d::Runner& callbackRunnerValue,
					  const a3d::Runner::UpdateInfo& info) {

				stages.push_back("runner");
				callbackRunner = &callbackRunnerValue;
				callbackInfo = &info;
				runnerTimes.push_back({info.elapsedTime, info.deltaTime});
			});
		scene.willTickCallback(
			[&callbackScene,
			 &simulationSteps,
			 &stages](a3d::Scene& callbackSceneValue,
					  const a3d::Scene::TickInfo& info) {

				stages.push_back("simulation-will");
				callbackScene = &callbackSceneValue;
				simulationSteps.push_back(RecordStep(info));
			});
		scene.physicsWorld()->willStepCallback(
			[&callbackPhysicsWorld,
			 &simulationSteps,
			 &stages](a3d::PhysicsWorld& callbackWorld,
					  const a3d::PhysicsWorld::StepInfo& info) {

				stages.push_back("physics-will");
				callbackPhysicsWorld = &callbackWorld;
				simulationSteps.push_back(RecordStep(info));
			});
		scene.physicsWorld()->didStepCallback(
			[&simulationSteps, &stages](a3d::PhysicsWorld&,
										const a3d::PhysicsWorld::StepInfo& info) {

				stages.push_back("physics-did");
				simulationSteps.push_back(RecordStep(info));
			});
		scene.didTickCallback(
			[&runner, &simulationSteps, &stages](a3d::Scene&,
												const a3d::Scene::TickInfo& info) {

				stages.push_back("simulation-did");
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
			"the observed Runner update should continue");

		Expect(
			stages.size() == 6
				&& stages[0] == "input"
				&& stages[1] == "runner"
				&& stages[2] == "simulation-will"
				&& stages[3] == "physics-will"
				&& stages[4] == "physics-did"
				&& stages[5] == "simulation-did",
			"the Runner and simulation callbacks should run in order");
		Expect(
			inputContextPtr->updateCount() == 1,
			"input should update exactly once in the observed Runner update");
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
				== &a3d::testing::RunnerTestAccess::updateInfo(runner),
			"the callback should receive the Runner's current Runner::UpdateInfo");
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
				"the callback simulation tick");
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
			"the callback Runner update should report one tick");
	}

	void NeitherWorldPipeline() {

		a3d::Scene scene;
		std::vector<std::string_view> stages;
		auto inputContext = std::make_unique<RecordingInputContext>(stages);
		auto* inputContextPtr = inputContext.get();
		scene.inputContext(std::move(inputContext));

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
			"the priming neither-world Runner update should continue");
		stages.clear();
		inputContextPtr->reset();

		runner.updateCallback(
			[&runnerTimes, &stages](a3d::Runner&,
									const a3d::Runner::UpdateInfo& info) {

				stages.push_back("runner");
				runnerTimes.push_back({info.elapsedTime, info.deltaTime});
			});
		scene.willTickCallback(
			[&stages](a3d::Scene&,
					   const a3d::Scene::TickInfo&) {

				stages.push_back("simulation-will");
			});
		scene.didTickCallback(
			[&stages](a3d::Scene&,
					   const a3d::Scene::TickInfo&) {

				stages.push_back("simulation-did");
			});

		Expect(
			a3d::testing::RunnerTestAccess::update(
				runner, AtMilliseconds(10225)),
			"the observed neither-world Runner update should continue");

		Expect(
			stages.size() == 4
				&& stages[0] == "input"
				&& stages[1] == "runner"
				&& stages[2] == "simulation-will"
				&& stages[3] == "simulation-did",
			"Scene simulation should run without either world");
		Expect(
			inputContextPtr->updateCount() == 1,
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
			"the neither-world Runner update should report one tick");
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
			"the first no-visual Runner update should continue");
		Expect(
			a3d::testing::RunnerTestAccess::update(
				runner, AtMilliseconds(10800)),
			"the second no-visual Runner update should continue");

		Expect(
			a3d::testing::RunnerTestAccess::completedRenderFrameCount(runner) == 0,
			"Runner updates without a VisualWorld should not complete render frames");
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
							 const a3d::PhysicsWorld::StepInfo&) {

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
							 const a3d::PhysicsWorld::StepInfo&) {

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
			"the multi-tick Runner update should execute three physics steps");
		const auto& stats = LatestFrameStats(runner);
		Expect(
			stats.simulationTickCount == 3,
			"the multi-tick Runner update should report three ticks");
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
			"multiple physics ticks should commit one update statistics sample");
	}

	void PhysicsInventoryUsesLatestTick() {

		a3d::Scene scene;
		scene.physicsWorld(std::make_unique<a3d::PhysicsWorld>());
		auto dynamicNode = AddMovingDynamicBody(scene, 0.0);

		scene.physicsWorld()->didStepCallback(
			[&dynamicNode](a3d::PhysicsWorld&,
						   const a3d::PhysicsWorld::StepInfo& info) {

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
			"update statistics should retain the latest physics inventory");
		Expect(
			stats.numPrimitiveShapes == 0,
			"update statistics should retain the latest shape inventory");
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
										  const a3d::PhysicsWorld::StepInfo& info) {

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
			"the inventory-order Runner update should continue");

		Expect(
			callbackCount == 1,
			"the physics callback should run exactly once");

		const auto& samples =
			a3d::testing::RunnerTestAccess::frameStatsHistory(runner).samples();
		Expect(
			samples.size() == 2,
			"each inventory-order Runner update should commit one statistics sample");

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
								const a3d::PhysicsWorld::StepInfo&) {

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
		const a3d::PhysicsWorld::StepInfo tinyStep {
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
			"VariableStep should reject a delta that is effectively zero for Bullet");
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
		scene.didTickCallback(
			[&result](a3d::Scene&,
					  const a3d::Scene::TickInfo& info) {

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
			"fixed-tick physics state should not depend on update grouping",
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
		{"simulation-config-defaults", SimulationConfigDefaults},
		{"simulation-config-validation", SimulationConfigValidation},
		{"first-update-zero-simulation-ticks", FirstUpdateExecutesNoSimulationTicks},
		{"variable-step-scale-cap-discard", VariableStepScalesCapsAndReportsDiscard},
		{"fixed-step-fractional-accumulator", FixedStepRetainsFractionalAccumulator},
		{"fixed-step-catch-up-overflow", FixedStepLimitsCatchUpAndReportsDiscard},
		{"stop-between-updates", StopBetweenUpdates},
		{"stop-during-input-update", StopDuringInputUpdate},
		{"stop-during-runner-update", StopDuringRunnerUpdate},
		{"stop-during-simulation-tick", StopDuringSimulationTick},
		{"input-callback-order", InputCallbacksRunBeforeRunnerAndSimulation},
		{"no-input-context", NoInputContextSkipsInputCallback},
		{"application-runner-access", ApplicationRunnerAccessUsesPreparedRunner},
		{"simulation-callback-order-physics", SimulationCallbacksRunInOrderWithPhysics},
		{"simulation-command-boundaries", SimulationCommandsRespectTickBoundaries},
		{"physics-command-boundaries", PhysicsCommandsRespectStepBoundaries},
		{"render-command-no-valid-render", RenderCommandsWaitWithoutAValidRender},
		{"neither-world-pipeline", NeitherWorldPipeline},
		{"no-visual-completed-render-count", NoVisualWorldHasZeroCompletedRenderFrames},
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
