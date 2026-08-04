//
//  ScopeTimer.h
//  avara3d
//
//  Created by Morgan Davis on 12/17/25.
//  Copyright © 2024 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_PROFILE_SCOPETIMER_H
#define AVARA3D_PROFILE_SCOPETIMER_H

#include <chrono>

#include "a3d/profile/Profiler.h"

namespace a3d {

    class Profiler;

    class ScopeTimer {

    public:
        /// Public Lifecycle Functions ///

        ScopeTimer(Profiler& profiler, Profiler::Tag tag);

        ScopeTimer(const ScopeTimer&)            = delete;
        ScopeTimer& operator=(const ScopeTimer&) = delete;

        ScopeTimer(ScopeTimer&&)            = delete;
        ScopeTimer& operator=(ScopeTimer&&) = delete;

        ~ScopeTimer();

    private:
        /// Private Member Variables ///

        Profiler*                                          _profiler;
        Profiler::Tag                                      _tag;
        std::chrono::time_point<std::chrono::steady_clock> _start;
    };

}

#endif //AVARA3D_PROFILE_SCOPETIMER_H
