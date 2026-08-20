//
//  ScopeTimer.h
//  avara3d
//
//  Created by Morgan Davis on 12/17/25.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_PROFILE_SCOPETIMER_H
#define AVARA3D_PROFILE_SCOPETIMER_H

#include <chrono>

#include "a3d/profile/Profiler.h"

namespace a3d {

    class Profiler;

    /**
     * @brief RAII timer that accumulates its lifetime into a Profiler tag.
     *
     * ScopeTimer retains the supplied Profiler non-owningly; the Profiler must outlive
     * the timer.
     */
    class ScopeTimer {

    public:
        // [Public Lifecycle Functions]

        /** @brief Begins timing immediately and associates the elapsed duration with @p tag. */
        ScopeTimer(Profiler& profiler, Profiler::Tag tag);

        ScopeTimer(const ScopeTimer&)            = delete;
        ScopeTimer& operator=(const ScopeTimer&) = delete;

        ScopeTimer(ScopeTimer&&)            = delete;
        ScopeTimer& operator=(ScopeTimer&&) = delete;

        /** @brief Adds the elapsed time since construction to the associated Profiler tag. */
        ~ScopeTimer();

    private:
        // [Private Member Variables]

        Profiler*                                          _profiler;
        Profiler::Tag                                      _tag;
        std::chrono::time_point<std::chrono::steady_clock> _start;
    };

}

#endif //AVARA3D_PROFILE_SCOPETIMER_H
