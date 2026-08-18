//
//  ScopeTimer.cc
//  avara3d
//
//  Created by Morgan Davis on 12/17/25.
//  Copyright © 2025-2026 Morgan K Davis. All rights reserved.
//

#include "a3d/profile/ScopeTimer.h"

#include <chrono>

#include "a3d/profile/Profiler.h"

using namespace a3d;
using namespace std;

ScopeTimer::ScopeTimer(Profiler& profiler, Profiler::Tag tag):
    _profiler {&profiler},
    _tag {tag},
    _start {chrono::steady_clock::now()} {}

ScopeTimer::~ScopeTimer() {
    auto duration = chrono::duration_cast<chrono::microseconds>(chrono::steady_clock::now() - _start);
    _profiler->add(_tag, duration);
}
