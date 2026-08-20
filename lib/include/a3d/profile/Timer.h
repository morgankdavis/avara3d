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

    /** @brief Manually controlled steady-clock timer that stores its most recently measured duration. */
    class Timer {

    public:
        // [Public Lifecycle Functions]

        /**
         * @brief Creates a Timer and optionally starts it immediately.
         *
         * The stored duration is initially zero. If @p start is false, call start()
         * before measuring with stop().
         */
        explicit Timer(bool start = false);

        // [Public Member Functions]

        /** @brief Starts or restarts timing from the current steady-clock time. */
        void                     start();

        /** @brief Stops timing, stores the elapsed duration since the most recent start(), and returns it. */
        std::chrono::nanoseconds stop();

        /** @brief Returns the most recently stored duration in nanoseconds. */
        std::chrono::nanoseconds duration() const;

        /** @brief Returns the most recently stored duration as fractional seconds. */
        double                   durationSeconds() const;

        /** @brief Returns the most recently stored duration as whole milliseconds. */
        std::int64_t             durationMilliseconds() const;

    private:
        // [Private Member Variables]

        std::chrono::time_point<std::chrono::steady_clock> _start;
        std::chrono::time_point<std::chrono::steady_clock> _stop;
        std::chrono::nanoseconds                           _duration;
    };

}

#endif //AVARA3D_PROFILE_TIMER_H
