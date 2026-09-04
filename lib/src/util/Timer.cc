//
//  Timer.cc
//  avara3d
//
//  Created by Morgan Davis on 12/6/25.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#include "a3d/util/Timer.h"

#include <stdexcept>

using namespace std;

namespace a3d::util {

// [Public Member Functions]

Timer::Timer(bool start):
    _start {},
    _duration {} {

    if (start) {
        this->start();
    }
}

void Timer::start() {
    _start = chrono::steady_clock::now();
}

chrono::nanoseconds Timer::stop() {

    if (!_start) {
        throw logic_error("Timer has not been started.");
    }

    _duration = chrono::duration_cast<chrono::nanoseconds>(chrono::steady_clock::now() - *_start);

    return _duration;
}

chrono::nanoseconds Timer::duration() const {
    return _duration;
}

double Timer::durationSeconds() const {
    return std::chrono::duration<double>(_duration).count();
}

std::int64_t Timer::durationMilliseconds() const {
    return std::chrono::duration_cast<std::chrono::milliseconds>(_duration).count();
}

} // namespace a3d::util
