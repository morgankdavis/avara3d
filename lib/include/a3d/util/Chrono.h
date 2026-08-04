//
//  Chrono.h
//  avara3d
//
//  Created by Morgan Davis on 1/1/26.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_UTIL_CHRONO_H
#define AVARA3D_UTIL_CHRONO_H

#include <chrono>

namespace a3d::util::chrono {

    template<class Rep, class Period>
    constexpr double Seconds(std::chrono::duration<Rep, Period> duration) noexcept {
        return std::chrono::duration<double>(duration).count();
    }

    template<class Rep, class Period>
    constexpr double Milliseconds(std::chrono::duration<Rep, Period> duration) noexcept {
        return std::chrono::duration<double, std::milli>(duration).count();
    }

}

#endif //AVARA3D_UTIL_CHRONO_H
