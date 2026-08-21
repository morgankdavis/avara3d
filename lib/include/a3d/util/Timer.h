//
//  Timer.h
//  avara3d
//
//  Created by Morgan Davis on 12/6/25.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_UTIL_TIMER_H
#define AVARA3D_UTIL_TIMER_H

#include <chrono>
#include <optional>

namespace a3d::util {

    /** @brief Manually controlled steady-clock timer that stores its most recently measured duration. */
    class Timer {

    public:
        // [Public Lifecycle Functions]

        /** @brief Creates a Timer, starting it immediately unless @p start is false. */
        explicit Timer(bool start = true);

        // [Public Member Functions]

        /** @brief Starts or restarts timing from the current steady-clock time. */
        void                     start();

        /**
         * @brief Stores and returns the elapsed duration since the most recent start().
         *
         * @throws std::logic_error if the Timer has not been started.
         */
        std::chrono::nanoseconds stop();

        /** @brief Returns the most recently stored duration in nanoseconds. */
        std::chrono::nanoseconds duration() const;

        /** @brief Returns the most recently stored duration as fractional seconds. */
        double                   durationSeconds() const;

        /** @brief Returns the most recently stored duration as whole milliseconds. */
        std::int64_t             durationMilliseconds() const;

    private:
        // [Private Member Variables]

        std::optional<std::chrono::steady_clock::time_point> _start;
        std::chrono::nanoseconds                             _duration;
    };

}

#endif // AVARA3D_UTIL_TIMER_H
