//
//  Environment.h
//  avara3d
//
//  Created by Morgan Davis on 8/19/26.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_ENVIRONMENT_H
#define AVARA3D_ENVIRONMENT_H

#include <cstdint>

#include "a3d/util/Bitmask.h"

namespace a3d {

    enum class Platform : std::uint8_t {
        None = 0,

        Linux   = 1 << 0,
        macOS   = 1 << 1,
        Windows = 1 << 2,
        Android = 1 << 3,
        iOS     = 1 << 4,
        Web     = 1 << 5,

        Desktop = (1 << 0) | (1 << 1) | (1 << 2),
        Mobile  = (1 << 3) | (1 << 4)
    };

    namespace env {

#if defined(A3D_LINUX)
        inline constexpr Platform platform = Platform::Linux;
#elif defined(A3D_MACOS)
        inline constexpr Platform platform = Platform::macOS;
#elif defined(A3D_WINDOWS)
        inline constexpr Platform platform = Platform::Windows;
#elif defined(A3D_ANDROID)
        inline constexpr Platform platform = Platform::Android;
#elif defined(A3D_IOS)
        inline constexpr Platform platform = Platform::iOS;
#elif defined(A3D_WEB)
        inline constexpr Platform platform = Platform::Web;
#else
    #error Unsupported platform.
#endif

        constexpr bool is(Platform platforms) noexcept {

            return util::bitmask::any(platform, platforms);
        }

    }

    namespace util::bitmask {

        template<>
        struct enable_ops<Platform> : std::true_type {};

    }

}

#endif // AVARA3D_ENVIRONMENT_H
