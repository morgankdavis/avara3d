//
//  PeriodicTrigger.h
//  avara3d
//
//  Created by Morgan Davis on 8/4/26.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_UTIL_PERIODICTRIGGER_H
#define AVARA3D_UTIL_PERIODICTRIGGER_H

#include <chrono>
#include <cstddef>
#include <functional>
#include <optional>

namespace a3d::util {

    /**
     * @brief Invokes a callback at fixed intervals along a caller-supplied time line.
     *
     * update() catches up missed intervals by invoking the callback once for every
     * elapsed trigger time. Supplying a time earlier than the previous update resets
     * the schedule automatically.
     */
    class PeriodicTrigger {

    public:
        // [Public Lifecycle Functions]

        /**
         * @brief Creates a trigger with @p interval between firings.
         *
         * By default, the first update fires immediately. When @p deferFirstFire is
         * true, the first firing is scheduled one interval after the first update time.
         *
         * @throws std::invalid_argument if @p interval is not positive and finite.
         */
        explicit PeriodicTrigger(std::chrono::duration<double> interval, bool deferFirstFire = false);

        // [Public Member Functions]

        /**
         * @brief Advances the trigger to @p time and invokes @p function once for each due firing.
         *
         * @param time Current caller-defined time in seconds.
         * @param function Callable invoked for every due interval.
         * @return Number of callback invocations due at this update.
         *
         * @throws std::invalid_argument if @p time is not finite.
         */
        template<typename Function>
        std::size_t update(double time, Function&& function) {

            const auto count = dueCount(time);

            for (std::size_t i = 0; i < count; ++i) {
                std::invoke(function);
            }

            return count;
        }

        /** @brief Clears timing history so the next update establishes a new schedule. */
        void reset() noexcept;

    private:
        // [Private Member Functions]

        std::size_t           dueCount(double time);

        // [Private Member Variables]

        double                _interval;
        bool                  _deferFirstFire;
        std::optional<double> _nextFireTime;
        std::optional<double> _lastTime;
    };

}

#endif // AVARA3D_UTIL_PERIODICTRIGGER_H
