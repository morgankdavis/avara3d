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
	};
}

namespace {

	using TimePoint = a3d::testing::RunnerTestAccess::TimePoint;

	struct CallbackTime {
		double elapsedTime;
		double deltaTime;
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

	void Expect(bool condition, std::string_view message) {

		if (!condition) {
			throw std::runtime_error(std::string(message));
		}
	}

	void ExpectNear(double actual,
					double expected,
					std::string_view message) {

		constexpr double tolerance = 1e-12;
		if (std::abs(actual - expected) > tolerance) {
			throw std::runtime_error(
				std::string(message)
				+ ": expected "
				+ std::to_string(expected)
				+ ", got "
				+ std::to_string(actual));
		}
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

		a3d::Runner runner(scene);
		std::vector<std::string_view> stages;
		std::size_t callbackCount = 0;
		std::size_t physicsCount = 0;
		runner.updateCallback(
			[&runner, &callbackCount, &stages](
				a3d::Runner&,
				const a3d::HostUpdateInfo&) {

				stages.push_back("runner");
				++callbackCount;
				runner.stop();
			});
		scene.physicsWorld()->didSimulateCallback(
			[&physicsCount, &stages](a3d::PhysicsWorld&, double, double) {
				stages.push_back("physics");
				++physicsCount;
			});

		a3d::testing::RunnerTestAccess::start(
			runner, AtMilliseconds(7000));
		Expect(
			!a3d::testing::RunnerTestAccess::update(
				runner, AtMilliseconds(7100)),
			"an update that stops the Runner should return false");
		Expect(callbackCount == 1, "the stopping callback should run once");
		Expect(
			physicsCount == 0,
			"stop during the Runner callback should skip the current physics stage");
		Expect(
			stages.size() == 1
				&& stages[0] == "runner",
			"no later stage should follow the stopping Runner callback");
		Expect(
			runner.state() == a3d::Runner::State::Stopped,
			"the Runner should remain stopped");
		Expect(
			a3d::testing::RunnerTestAccess::hostUpdateInfo(runner).updateIndex == 0,
			"the stopping update should retain its update index");
		Expect(
			a3d::testing::RunnerTestAccess::completedRenderFrameCount(runner) == 0,
			"stop during the Runner callback should not complete a render frame");

		Expect(
			!a3d::testing::RunnerTestAccess::update(
				runner, AtMilliseconds(7200)),
			"a later update should remain stopped");
		Expect(callbackCount == 1, "a stopped Runner should not invoke the callback again");
		Expect(physicsCount == 0, "a stopped Runner should not invoke physics");
	}

	void StopDuringDidSimulate() {

		a3d::Scene scene;
		scene.physicsWorld(std::make_unique<a3d::PhysicsWorld>());

		a3d::Runner runner(scene);
		std::vector<std::string_view> stages;
		std::size_t runnerCallbackCount = 0;
		std::size_t physicsCallbackCount = 0;
		runner.updateCallback(
			[&runnerCallbackCount, &stages](
				a3d::Runner&,
				const a3d::HostUpdateInfo&) {

				stages.push_back("runner");
				++runnerCallbackCount;
			});
		scene.physicsWorld()->didSimulateCallback(
			[&runner, &physicsCallbackCount, &stages](
				a3d::PhysicsWorld&,
				double,
				double) {

				stages.push_back("physics");
				++physicsCallbackCount;
				runner.stop();
			});

		a3d::testing::RunnerTestAccess::start(
			runner, AtMilliseconds(7500));
		Expect(
			!a3d::testing::RunnerTestAccess::update(
				runner, AtMilliseconds(7600)),
			"an update stopped during did-simulate should return false");
		Expect(
			runnerCallbackCount == 1,
			"the Runner callback should run before physics");
		Expect(
			physicsCallbackCount == 1,
			"the stopping physics callback should run once");
		Expect(
			stages.size() == 2
				&& stages[0] == "runner"
				&& stages[1] == "physics",
			"did-simulate should follow the Runner callback");
		Expect(
			runner.state() == a3d::Runner::State::Stopped,
			"the Runner should remain stopped after did-simulate");
		Expect(
			a3d::testing::RunnerTestAccess::completedRenderFrameCount(runner) == 0,
			"stopping during did-simulate should not complete a render frame");
	}

	void RunnerCallbackOrderWithPhysics() {

		a3d::Scene scene;
		std::vector<std::string_view> stages;
		auto inputManager = std::make_unique<RecordingInputManager>(stages);
		auto* inputManagerPtr = inputManager.get();
		scene.inputManager(std::move(inputManager));
		scene.physicsWorld(std::make_unique<a3d::PhysicsWorld>());

		a3d::Runner runner(scene);
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
		std::vector<CallbackTime> physicsTimes;
		runner.updateCallback(
			[&callbackRunner, &callbackInfo, &runnerTimes, &stages](
				a3d::Runner& callbackRunnerValue,
				const a3d::HostUpdateInfo& info) {

				stages.push_back("runner");
				callbackRunner = &callbackRunnerValue;
				callbackInfo = &info;
				runnerTimes.push_back({info.elapsedTime, info.deltaTime});
			});
		scene.physicsWorld()->didSimulateCallback(
			[&physicsTimes, &stages](
				a3d::PhysicsWorld&,
				double elapsedTime,
				double deltaTime) {

				stages.push_back("physics");
				physicsTimes.push_back({elapsedTime, deltaTime});
			});

		Expect(
			a3d::testing::RunnerTestAccess::update(
				runner, AtMilliseconds(8350)),
			"the observed host update should continue");

		Expect(
			stages.size() == 3
				&& stages[0] == "input"
				&& stages[1] == "runner"
				&& stages[2] == "physics",
			"the pipeline should run input, Runner callback, then physics");
		Expect(
			inputManagerPtr->updateCount() == 1,
			"input should update exactly once in the observed host update");
		Expect(
			runnerTimes.size() == 1,
			"the Runner callback should run exactly once");
		Expect(
			physicsTimes.size() == 1,
			"the physics did-simulate callback should run exactly once");
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
		ExpectNear(
			runnerTimes[0].elapsedTime,
			0.35,
			"the Runner callback elapsed time");
		ExpectNear(
			runnerTimes[0].deltaTime,
			0.25,
			"the Runner callback delta time");
		ExpectNear(
			physicsTimes[0].elapsedTime,
			0.35,
			"the physics callback elapsed time");
		ExpectNear(
			physicsTimes[0].deltaTime,
			0.25,
			"the physics callback delta time");
	}

	void FirstZeroDeltaReachesPhysics() {

		a3d::Scene scene;
		scene.physicsWorld(std::make_unique<a3d::PhysicsWorld>());

		std::vector<CallbackTime> physicsTimes;
		scene.physicsWorld()->didSimulateCallback(
			[&physicsTimes](
				a3d::PhysicsWorld&,
				double elapsedTime,
				double deltaTime) {

				physicsTimes.push_back({elapsedTime, deltaTime});
			});

		a3d::Runner runner(scene);
		a3d::testing::RunnerTestAccess::start(
			runner, AtMilliseconds(9000));
		Expect(
			a3d::testing::RunnerTestAccess::update(
				runner, AtMilliseconds(9100)),
			"the first host update should continue");

		Expect(
			physicsTimes.size() == 1,
			"the first host update should reach the existing physics callback");
		ExpectNear(
			physicsTimes[0].elapsedTime,
			0.1,
			"the first physics callback elapsed time");
		ExpectNear(
			physicsTimes[0].deltaTime,
			0.0,
			"the first physics callback delta time should remain zero");
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
		runner.updateCallback(
			[&runnerTimes, &stages](
				a3d::Runner&,
				const a3d::HostUpdateInfo& info) {

				stages.push_back("runner");
				runnerTimes.push_back({info.elapsedTime, info.deltaTime});
			});

		a3d::testing::RunnerTestAccess::start(
			runner, AtMilliseconds(10000));
		Expect(
			a3d::testing::RunnerTestAccess::update(
				runner, AtMilliseconds(10150)),
			"the neither-world host update should continue");

		Expect(
			stages.size() == 2
				&& stages[0] == "input"
				&& stages[1] == "runner",
			"input and the Runner callback should run without either world");
		Expect(
			inputManagerPtr->updateCount() == 1,
			"input should update once without either world");
		Expect(
			runnerTimes.size() == 1,
			"the Runner callback should run once without either world");
		ExpectNear(
			runnerTimes[0].elapsedTime,
			0.15,
			"the neither-world callback elapsed time");
		ExpectNear(
			runnerTimes[0].deltaTime,
			0.0,
			"the neither-world callback first delta time");
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
		scene.physicsWorld()->didSimulateCallback(
			[&callbackCount](a3d::PhysicsWorld&, double, double) {
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

	void PhysicsInventoryPrecedesDidSimulate() {

		a3d::Scene scene;
		scene.physicsWorld(std::make_unique<a3d::PhysicsWorld>());

		auto dynamicNode = std::make_shared<a3d::Node>("dynamic body");
		dynamicNode->physicsBody(std::make_unique<a3d::PhysicsBody>(
			a3d::PhysicsBody::Type::Dynamic,
			std::make_shared<a3d::SpherePhysicsShape>(1.0f)));
		scene.rootNode()->addChild(dynamicNode);

		std::size_t callbackCount = 0;
		scene.physicsWorld()->didSimulateCallback(
			[&callbackCount, &dynamicNode](
				a3d::PhysicsWorld&,
				double,
				double deltaTime) {

				++callbackCount;
				ExpectNear(
					deltaTime,
					0.0,
					"the first physics callback delta should remain zero");
				dynamicNode->physicsBody(std::unique_ptr<a3d::PhysicsBody>{});
			});

		a3d::Runner runner(scene);
		a3d::testing::RunnerTestAccess::start(
			runner, AtMilliseconds(11000));
		Expect(
			a3d::testing::RunnerTestAccess::update(
				runner, AtMilliseconds(11100)),
			"the inventory-order host update should continue");

		Expect(
			callbackCount == 1,
			"the physics callback should run exactly once");

		const auto& samples =
			a3d::testing::RunnerTestAccess::frameStatsHistory(runner).samples();
		Expect(
			samples.size() == 1,
			"the host update should commit one statistics sample");

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

	using TestFunction = void (*)();

	const std::pair<std::string_view, TestFunction> Tests[] {
		{"first-update-zero-delta", FirstUpdateHasZeroDelta},
		{"elapsed-time-progression", ElapsedTimeProgresses},
		{"delta-time-progression", DeltaTimeProgresses},
		{"update-index-progression", UpdateIndexProgresses},
		{"independent-runner-clocks", RunnerClocksAreIndependent},
		{"stop-between-updates", StopBetweenUpdates},
		{"stop-during-runner-update", StopDuringRunnerUpdate},
		{"stop-during-did-simulate", StopDuringDidSimulate},
		{"runner-callback-order-physics", RunnerCallbackOrderWithPhysics},
		{"first-zero-delta-reaches-physics", FirstZeroDeltaReachesPhysics},
		{"neither-world-pipeline", NeitherWorldPipeline},
		{"no-visual-completed-render-count", NoVisualWorldHasZeroCompletedRenderFrames},
		{"scene-root-invariant", SceneRootInvariant},
		{"physics-inventory-sampling", PhysicsInventorySamplingIsCurrent},
		{"physics-inventory-before-did-simulate", PhysicsInventoryPrecedesDidSimulate}
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
