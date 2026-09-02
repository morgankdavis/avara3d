//
//  Assert.h
//  avara3d
//
//  Created by Morgan Davis on 1/5/26.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_ASSERT_H
#define AVARA3D_ASSERT_H

#include <cstddef>

// ----------------------------------------------------------------------------
// Branch prediction
// ----------------------------------------------------------------------------
#if defined(__clang__) || defined(__GNUC__)
    #define A3D_LIKELY(x)   __builtin_expect(!!(x), 1)
    #define A3D_UNLIKELY(x) __builtin_expect(!!(x), 0)
#else
    #define A3D_LIKELY(x)   (x)
    #define A3D_UNLIKELY(x) (x)
#endif

// ----------------------------------------------------------------------------
// Debug/Release detect
// ----------------------------------------------------------------------------
#ifndef A3D_DEBUG
    #if defined(NDEBUG)
        #define A3D_DEBUG 0
    #else
        #define A3D_DEBUG 1
    #endif
#endif

namespace a3d::detail {

[[noreturn]] void assert_fail(const char* expr, const char* file, int line, const char* func);

[[noreturn]] void assert_fail_msg(const char* expr,
                                  const char* file,
                                  int         line,
                                  const char* func,
                                  const char* msg);

} // namespace a3d::detail

// ----------------------------------------------------------------------------
// Public macros
// ----------------------------------------------------------------------------
#if A3D_DEBUG

    #define A3D_ASSERT(expr)                                                                                   \
        do {                                                                                                   \
            if (A3D_UNLIKELY(!(expr))) {                                                                       \
                ::a3d::detail::assert_fail(#expr, __FILE__, __LINE__, __func__);                               \
            }                                                                                                  \
        } while (0)

// Message is a plain string here (keep it simple + dependency-free).
// If you want formatting, see note below.
    #define A3D_ASSERT_MSG(expr, msg)                                                                          \
        do {                                                                                                   \
            if (A3D_UNLIKELY(!(expr))) {                                                                       \
                ::a3d::detail::assert_fail_msg(#expr, __FILE__, __LINE__, __func__, (msg));                    \
            }                                                                                                  \
        } while (0)

#else

    #define A3D_ASSERT(expr)                                                                                   \
        do {                                                                                                   \
            (void) sizeof(expr);                                                                               \
        } while (0)
    #define A3D_ASSERT_MSG(expr, msg)                                                                          \
        do {                                                                                                   \
            (void) sizeof(expr);                                                                               \
            (void) sizeof(msg);                                                                                \
        } while (0)

#endif

#endif // AVARA3D_ASSERT_H
