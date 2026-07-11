//
//  Runner.cc
//  avara3d
//
//  Created by Morgan Davis on 7/10/26.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#include "a3d/Runner.h"

#include <stdexcept>
#include <utility>

#if defined(A3D_WEB)
#include <emscripten.h>
#endif

#include "a3d/scene/Scene.h"

using namespace a3d;
using namespace std;

/// Public Static Member Functions ///

#if defined(A3D_WEB)

int Runner::Run(Runner&& runner) {

    // ! NOTE !
    // this is intentionally heap-owned. on web, Run() returns immediately,
    // so the Runner must outlive the stack frame that called Run().
    auto* webRunner = new Runner(std::move(runner));

    webRunner->begin();
    emscripten_set_main_loop_arg(
        [](void* arg) {
            auto* runner = static_cast<a3d::Runner*>(arg);
            if (!runner->update()) {
                emscripten_cancel_main_loop();
                runner->end();
                delete runner;
            }
        },
        webRunner,
        0,
        false);

    return 0;
}

#else

int Runner::Run(Runner&& runner) {

    runner.begin();
    while (runner.update());
    runner.end();

    return 0;
}

#endif

/// Public Lifecycle Functions ///

Runner::Runner(std::unique_ptr<Scene> scene):
    _scene(std::move(scene)),
    _context{nullptr},
    _state{State::Idle},
    _continueCallback{},
    _shutdownCallback{} {

    if (!_scene) {
        throw std::invalid_argument("a3d::Runner requires a non-null Scene.");
    }
}

Runner::~Runner() {
    end();
}

Runner::Runner(Runner&& other):
    _scene(std::move(other._scene)),
    _context(other._context),
    _state(other._state),
    _continueCallback(std::move(other._continueCallback)),
    _shutdownCallback(std::move(other._shutdownCallback)) {

    other._context = nullptr;
    other._state = State::Stopped;
}

Runner& Runner::operator=(Runner&& other) {

    if (this == &other) {
        return *this;
    }

    end();

    _scene = std::move(other._scene);
    _context = other._context;
    _state = other._state;
    _continueCallback = std::move(other._continueCallback);
    _shutdownCallback = std::move(other._shutdownCallback);

    other._context = nullptr;
    other._state = State::Stopped;

    return *this;
}

/// Public Member Functions ///

void Runner::start() {
    begin();
}

bool Runner::update() {

    if (_state != State::Running || !_scene) {
        return false;
    }

    _scene->update();

    if (_continueCallback &&
        !_continueCallback(*this, *_scene, _context)) {
        stop();
    }

    return _state == State::Running;
}

void Runner::stop() {
    _state = State::Stopping;
}

Runner::State Runner::state() const {
    return _state;
}

const Scene& Runner::scene() const {
    return *_scene;
}

void* Runner::context() const {
    return _context;
}

void Runner::context(void* context) {
    _context = context;
}

Runner::ContinueCallback Runner::continueCallback() const {
    return _continueCallback;
}

void Runner::continueCallback(ContinueCallback function) {
    _continueCallback = std::move(function);
}

Runner::ShutdownCallback Runner::shutdownCallback() const {
    return _shutdownCallback;
}

void Runner::shutdownCallback(ShutdownCallback function) {
    _shutdownCallback = std::move(function);
}

/// Private Member Functions ///

void Runner::begin() {

    if (_state == State::Idle) {
        _state = State::Running;
    }
}

void Runner::end() {

    if (_state == State::Stopped) {
        return;
    }

    _state = State::Stopped;

    // destroy the Scene while the external context/window still exists
    _scene.reset();

    if (_shutdownCallback) {
        try {
            _shutdownCallback(*this, _context);
        }
        catch (...) {
            // destructors / teardown paths must not throw
        }
    }

    _context = nullptr;
}
