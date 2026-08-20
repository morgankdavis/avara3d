//
//  PeriodicTrigger.cc
//  avara3d
//
//  Created by Morgan Davis on 8/4/26.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#include "a3d/util/PeriodicTrigger.h"

#include <cmath>
#include <stdexcept>

using namespace a3d::util;
using namespace std;

// [Public Lifecycle Functions]

PeriodicTrigger::PeriodicTrigger(chrono::duration<double> interval, bool deferFirstFire):
    _interval {interval.count()},
    _deferFirstFire {deferFirstFire},
    _nextFireTime {},
    _lastTime {} {

    if (!isfinite(_interval) || _interval <= 0.0) {
        throw invalid_argument("PeriodicTrigger interval must be positive and finite.");
    }
}

// [Public Member Functions]

void PeriodicTrigger::reset() noexcept {

    _nextFireTime.reset();
    _lastTime.reset();
}

// [Private Member Functions]

size_t PeriodicTrigger::dueCount(double time) {

    if (!isfinite(time)) {
        throw invalid_argument("PeriodicTrigger time must be finite.");
    }

    if (_lastTime && time < *_lastTime) {
        reset();
    }

    _lastTime = time;

    if (!_nextFireTime) {
        _nextFireTime = time + (_deferFirstFire ? _interval : 0.0);
    }

    size_t count = 0;

    while (time >= *_nextFireTime) {
        *_nextFireTime += _interval;
        ++count;
    }

    return count;
}
