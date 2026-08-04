//
//  Assert.h
//  avara3d
//
//  Created by Morgan Davis on 1/5/26.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#include "a3d/Assert.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "a3d/log/Log.h"

namespace a3d::detail {

    static const char* basename(const char* path) {
        if (!path) {
            return "unknown";
        }
        const char* slash = std::strrchr(path, '/');
#if defined(_WIN32)
        const char* bslash = std::strrchr(path, '\\');
        if (bslash && (!slash || bslash > slash)) {
            slash = bslash;
        }
#endif
        return slash ? (slash + 1) : path;
    }

    [[noreturn]] static void trap_or_abort() {
#if defined(_MSC_VER)
        __debugbreak();
#elif defined(__has_builtin)
    #if __has_builtin(__builtin_trap)
        __builtin_trap();
    #else
        std::abort();
    #endif
#elif defined(__GNUC__) || defined(__clang__)
        __builtin_trap();
#else
        std::abort();
#endif
    }

    [[noreturn]] void assert_fail(const char* expr, const char* file, int line, const char* func) {
        const char* f = basename(file);

        // Log through your system
        log::e()("A3D_ASSERT FAILED: expr={} at {}:{} func={}", expr, f, line, func);

        // Also dump to stderr as a last-resort (helps if logging isn't initialized)
        std::fprintf(stderr, "A3D_ASSERT FAILED: %s (%s:%d %s)\n", expr, f, line, func);
        std::fflush(stderr);

        trap_or_abort();
    }

    [[noreturn]] void assert_fail_msg(const char* expr,
                                      const char* file,
                                      int         line,
                                      const char* func,
                                      const char* msg) {
        const char* f = basename(file);

        log::e()("A3D_ASSERT FAILED: expr={} at {}:{} func={} msg={}", expr, f, line, func, (msg ? msg : ""));
        std::fprintf(stderr, "A3D_ASSERT FAILED: %s (%s:%d %s) msg=%s\n", expr, f, line, func,
                     (msg ? msg : ""));
        std::fflush(stderr);

        trap_or_abort();
    }

}
