//
//  Timer.h
//  avara3d
//
//  Created by Morgan Davis on 12/6/25.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_PROFILE_TIMER_H
#define AVARA3D_PROFILE_TIMER_H

#include <chrono>

namespace a3d {

    class Timer {

    public:
        // [Public Lifecycle Functions]
        ///
        explicit Timer(bool start = false);

        // [Public Member Functions]

        void                     start();
        std::chrono::nanoseconds stop();
        std::chrono::nanoseconds duration() const;
        double                   durationSeconds() const;
        std::int64_t             durationMilliseconds() const;

    private:
        // [Private Member Variables]

        std::chrono::time_point<std::chrono::steady_clock> _start;
        std::chrono::time_point<std::chrono::steady_clock> _stop;
        std::chrono::nanoseconds                           _duration;
    };

}

#endif //AVARA3D_PROFILE_TIMER_H
