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

    class PeriodicTrigger {

    public:
        /// Public Lifecycle Functions ///

        explicit PeriodicTrigger(std::chrono::duration<double> interval, bool deferFirstFire = false);

        /// Public Member Functions ///

        template<typename Function>
        std::size_t update(double time, Function&& function) {

            const auto count = dueCount(time);

            for (std::size_t i = 0; i < count; ++i) {
                std::invoke(function);
            }

            return count;
        }

        void reset() noexcept;

    private:
        /// Private Member Functions ///

        std::size_t           dueCount(double time);

        /// Private Member Variables ///

        double                _interval;
        bool                  _deferFirstFire;
        std::optional<double> _nextFireTime;
        std::optional<double> _lastTime;
    };

}

#endif // AVARA3D_UTIL_PERIODICTRIGGER_H
