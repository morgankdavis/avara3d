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
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include "a3d/Execution.h"
#include "a3d/Runner.h"
#include "a3d/scene/Scene.h"

namespace a3d::testing {

	class RunnerTestAccess {

	public:
		using TimePoint = std::chrono::steady_clock::time_point;

		static void start(Runner& runner, TimePoint now) {
			runner.start(now);
		}

		static bool update(Runner& runner, TimePoint now) {
			return runner.update(now);
		}

		static const HostUpdateInfo& hostUpdateInfo(const Runner& runner) {
			return runner._hostUpdateInfo;
		}
	};
}

namespace {

	using TimePoint = a3d::testing::RunnerTestAccess::TimePoint;

	struct CallbackTime {
		double elapsedTime;
		double deltaTime;
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
		std::vector<CallbackTime> times;
		scene.updateCallback(
			[&times](a3d::Scene&, double elapsedTime, double deltaTime) {
				times.push_back({elapsedTime, deltaTime});
			});

		a3d::Runner runner(scene);
		runner.start();

		Expect(
			runner.update(),
			"the running Runner should continue");
		Expect(times.size() == 1, "the Scene callback should run once");
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
		std::vector<CallbackTime> times;
		scene.updateCallback(
			[&times](a3d::Scene&, double elapsedTime, double deltaTime) {
				times.push_back({elapsedTime, deltaTime});
			});

		a3d::Runner runner(scene);
		a3d::testing::RunnerTestAccess::start(
			runner, AtMilliseconds(2000));
		a3d::testing::RunnerTestAccess::update(
			runner, AtMilliseconds(2100));
		a3d::testing::RunnerTestAccess::update(
			runner, AtMilliseconds(2350));
		a3d::testing::RunnerTestAccess::update(
			runner, AtMilliseconds(2900));

		Expect(times.size() == 3, "the Scene callback should run three times");
		ExpectNear(times[0].elapsedTime, 0.1, "first elapsed time");
		ExpectNear(times[1].elapsedTime, 0.35, "second elapsed time");
		ExpectNear(times[2].elapsedTime, 0.9, "third elapsed time");
	}

	void DeltaTimeProgresses() {

		a3d::Scene scene;
		std::vector<CallbackTime> times;
		scene.updateCallback(
			[&times](a3d::Scene&, double elapsedTime, double deltaTime) {
				times.push_back({elapsedTime, deltaTime});
			});

		a3d::Runner runner(scene);
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
		firstScene.updateCallback(
			[&firstTimes](a3d::Scene&, double elapsedTime, double deltaTime) {
				firstTimes.push_back({elapsedTime, deltaTime});
			});

		a3d::Scene secondScene;
		std::vector<CallbackTime> secondTimes;
		secondScene.updateCallback(
			[&secondTimes](a3d::Scene&, double elapsedTime, double deltaTime) {
				secondTimes.push_back({elapsedTime, deltaTime});
			});

		a3d::Runner firstRunner(firstScene);
		a3d::Runner secondRunner(secondScene);
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
		std::size_t callbackCount = 0;
		scene.updateCallback(
			[&callbackCount](a3d::Scene&, double, double) {
				++callbackCount;
			});

		a3d::Runner runner(scene);
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
		Expect(callbackCount == 1, "stop should prevent later Scene callbacks");

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

	void StopDuringUpdate() {

		a3d::Scene scene;
		a3d::Runner runner(scene);
		std::size_t callbackCount = 0;
		scene.updateCallback(
			[&runner, &callbackCount](a3d::Scene&, double, double) {
				++callbackCount;
				runner.stop();
			});

		a3d::testing::RunnerTestAccess::start(
			runner, AtMilliseconds(7000));
		Expect(
			!a3d::testing::RunnerTestAccess::update(
				runner, AtMilliseconds(7100)),
			"an update that stops the Runner should return false");
		Expect(callbackCount == 1, "the stopping callback should run once");
		Expect(
			runner.state() == a3d::Runner::State::Stopped,
			"the Runner should remain stopped");
		Expect(
			a3d::testing::RunnerTestAccess::hostUpdateInfo(runner).updateIndex == 0,
			"the stopping update should retain its update index");

		Expect(
			!a3d::testing::RunnerTestAccess::update(
				runner, AtMilliseconds(7200)),
			"a later update should remain stopped");
		Expect(callbackCount == 1, "a stopped Runner should not invoke the callback again");
	}

	using TestFunction = void (*)();

	const std::pair<std::string_view, TestFunction> Tests[] {
		{"first-update-zero-delta", FirstUpdateHasZeroDelta},
		{"elapsed-time-progression", ElapsedTimeProgresses},
		{"delta-time-progression", DeltaTimeProgresses},
		{"update-index-progression", UpdateIndexProgresses},
		{"independent-runner-clocks", RunnerClocksAreIndependent},
		{"stop-between-updates", StopBetweenUpdates},
		{"stop-during-update", StopDuringUpdate}
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
