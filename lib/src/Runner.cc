//
//  Runner.cc
//  avara3d
//

#include "a3d/Runner.h"

#include <stdexcept>

#include "a3d/scene/Scene.h"

using namespace a3d;
using namespace std;

Runner::Runner(Scene& scene):
	_scene(scene),
	_state(State::Idle),
	_startTime{},
	_previousUpdateTime{},
	_hostUpdateInfo{},
	_hasUpdated{false} {
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

	_startTime = now;
	_previousUpdateTime = now;
	_hostUpdateInfo = {};
	_hasUpdated = false;
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

	_scene.update(hostUpdateInfo);

	return _state == State::Running;
}

void Runner::stop() {

	if (_state == State::Stopped) {
		return;
	}

	_state = State::Stopped;
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


// //
// //  Runner.cc
// //  avara3d
// //
// //  Created by Morgan Davis on 7/10/26.
// //  Copyright © 2026 Morgan K Davis. All rights reserved.
// //
//
// #include "a3d/Runner.h"
//
// #include <stdexcept>
// #include <utility>
//
// #if defined(A3D_WEB)
// #include <emscripten.h>
// #endif
//
// #include "a3d/scene/Scene.h"
//
// using namespace a3d;
// using namespace std;
//
// /// Public Static Member Functions ///
//
// #if defined(A3D_WEB)
//
// int Runner::Run(Runner&& runner) {
//
//     // ! NOTE !
//     // this is intentionally heap-owned. on web, Run() returns immediately,
//     // so the Runner must outlive the stack frame that called Run().
//     auto* webRunner = new Runner(std::move(runner));
//
//     webRunner->start();
//     emscripten_set_main_loop_arg(
//         [](void* arg) {
//             auto* runner = static_cast<a3d::Runner*>(arg);
//             if (!runner->update()) {
//                 emscripten_cancel_main_loop();
//                 runner->end();
//                 delete runner;
//             }
//         },
//         webRunner,
//         0,
//         false);
//
//     return 0;
// }
//
// #else
//
// int Runner::Run(Runner&& runner) {
//
//     runner.start();
//     while (runner.update());
//     runner.end();
//
//     return 0;
// }
//
// #endif
//
// /// Public Lifecycle Functions ///
//
// Runner::Runner(std::unique_ptr<Scene> scene):
//     _scene(std::move(scene)),
//     _context{nullptr},
//     _state{State::Idle},
//     _shouldContinuePredicate{},
//     _didShutdownCallback{} {
//
//     if (!_scene) {
//         throw std::invalid_argument("a3d::Runner requires a non-null Scene.");
//     }
// }
//
// Runner::~Runner() {
//     end();
// }
//
// Runner::Runner(Runner&& other):
//     _scene(std::move(other._scene)),
//     _context(other._context),
//     _state(other._state),
//     _shouldContinuePredicate(std::move(other._shouldContinuePredicate)),
//     _didShutdownCallback(std::move(other._didShutdownCallback)) {
//
//     other._context = nullptr;
//     other._state = State::Stopped;
// }
//
// Runner& Runner::operator=(Runner&& other) {
//
//     if (this == &other) {
//         return *this;
//     }
//
//     end();
//
//     _scene = std::move(other._scene);
//     _context = other._context;
//     _state = other._state;
//     _shouldContinuePredicate = std::move(other._shouldContinuePredicate);
//     _didShutdownCallback = std::move(other._didShutdownCallback);
//
//     other._context = nullptr;
//     other._state = State::Stopped;
//
//     return *this;
// }
//
// /// Public Member Functions ///
//
// void Runner::start() {
//
//     if (_state == State::Idle) {
//         _state = State::Running;
//     }
// }
//
// bool Runner::update() {
//
//     if (_state != State::Running || !_scene) {
//         return false;
//     }
//
//     _scene->update();
//
//     if (_shouldContinuePredicate &&
//         !_shouldContinuePredicate(*this, *_scene, _context)) {
//         stop();
//     }
//
//     return _state == State::Running;
// }
//
// void Runner::stop() {
//     _state = State::Stopping;
// }
//
// Runner::State Runner::state() const {
//     return _state;
// }
//
// const Scene& Runner::scene() const {
//     return *_scene;
// }
//
// void* Runner::context() const {
//     return _context;
// }
//
// void Runner::context(void* context) {
//     _context = context;
// }
//
// Runner::ShouldContinuePredicate Runner::shouldContinuePredicate() const {
//     return _shouldContinuePredicate;
// }
//
// void Runner::shouldContinuePredicate(ShouldContinuePredicate function) {
//     _shouldContinuePredicate = std::move(function);
// }
//
// Runner::DidShutdownCallback Runner::didShutdownCallback() const {
//     return _didShutdownCallback;
// }
//
// void Runner::didShutdownCallback(DidShutdownCallback function) {
//     _didShutdownCallback = std::move(function);
// }
//
// /// Private Member Functions ///
//
// void Runner::end() {
//
//     if (_state == State::Stopped) {
//         return;
//     }
//
//     _state = State::Stopped;
//
//     // destroy the Scene while the external context/window still exists
//     _scene.reset();
//
//     if (_didShutdownCallback) {
//         try {
//             _didShutdownCallback(*this, _context);
//         }
//         catch (...) {
//             // destructors / teardown paths must not throw
//         }
//     }
//
//     _context = nullptr;
// }
