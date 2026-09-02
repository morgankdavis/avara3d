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

// [Public Types]

/** @brief Identifies individual A3D target platforms and platform groups. */
enum class Platform : std::uint8_t {
    None = 0, ///< No platform.

    Linux   = 1 << 0, ///< Linux.
    macOS   = 1 << 1, ///< macOS.
    Windows = 1 << 2, ///< Windows.
    Android = 1 << 3, ///< Android.
    iOS     = 1 << 4, ///< iOS.
    Web     = 1 << 5, ///< WebAssembly/Web.

    Desktop = (1 << 0) | (1 << 1) | (1 << 2), ///< Linux, macOS, or Windows.
    Mobile  = (1 << 3) | (1 << 4)             ///< Android or iOS.
};

namespace env {

    // [Public Constants]

    /** @brief Platform selected for the current build. */
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

    // [Public Functions]

    /** @brief Returns whether the current build platform matches any platform in @p platforms. */
    constexpr bool is(Platform platforms) noexcept {

        return util::bitmask::any(platform, platforms);
    }

} // namespace env

namespace util::bitmask {

    // [Internal Types]

    template<>
    struct enable_ops<Platform> : std::true_type {};

} // namespace util::bitmask

} // namespace a3d

#endif // AVARA3D_ENVIRONMENT_H
