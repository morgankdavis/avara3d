//
// Created by mkd on 12/26/25.
//

#ifndef AVARA3D_UTILITYMACROS_H
#define AVARA3D_UTILITYMACROS_H

#include <chrono>

#define A3D_PP_CAT2(a,b) a##b
#define A3D_PP_CAT(a,b)  A3D_PP_CAT2(a,b)

// run only on first invocation per call site
#define A3D_ONCE(...) A3D_ONCE_IMPL(__COUNTER__, __VA_ARGS__)
#define A3D_ONCE_IMPL(ctr, ...) \
do { \
  static std::once_flag A3D_PP_CAT(_a3d_once_flag_, ctr); \
  std::call_once(A3D_PP_CAT(_a3d_once_flag_, ctr), (__VA_ARGS__)); \
} while (0)

namespace a3d::utils::detail {

	template<class Clock, class Rep, class Period>
	inline bool every_tick(typename Clock::time_point& last,
						   std::chrono::duration<Rep, Period> interval) {
		const auto now = Clock::now();
		if (now - last >= interval) {
			last = now; // "at most once" behavior (no catch-up)
			return true;
		}
		return false;
	}

} // namespace a3d::utils::detail

// run at most every 'interval' (duration)
#define A3D_EVERY(interval_expr, ...)                                           \
    do {                                                                        \
        auto&& _a3d_fn = (__VA_ARGS__);                                         \
        if ([&]() -> bool {                                                     \
                struct State {                                                  \
                    std::chrono::steady_clock::time_point last;                 \
                    std::chrono::steady_clock::duration interval;               \
                    explicit State(std::chrono::steady_clock::duration i)       \
                        : last(std::chrono::steady_clock::now() - i),           \
                          interval(i) {}                                        \
                };                                                              \
                static State s{ (interval_expr) };                              \
                return a3d::utils::detail::every_tick<std::chrono::steady_clock>( \
                    s.last, s.interval);                                        \
            }()) {                                                              \
            _a3d_fn();                                                          \
        }                                                                       \
    } while (0)

// run exactly once on 'invocation'th call, not before or after
#define A3D_ON(invocation_expr, ...)                                            \
    do {                                                                        \
        auto&& _a3d_fn = (__VA_ARGS__);                                         \
        if ([&]() -> bool {                                                     \
                struct State {                                                  \
                    long long count = 0;                                        \
                    long long target;                                           \
                    bool done = false;                                          \
                    explicit State(long long t) : target(t < 1 ? 1 : t) {}      \
                };                                                              \
                static State s{ static_cast<long long>(invocation_expr) };      \
                if (s.done) return false;                                       \
                if (++s.count == s.target) { s.done = true; return true; }      \
                return false;                                                   \
            }()) {                                                              \
            _a3d_fn();                                                          \
        }                                                                       \
    } while (0)

// skip the first 'invocations' calls, then run each thereafter
#define A3D_AFTER(invocations_expr, ...)                                        \
    do {                                                                        \
        auto&& _a3d_fn = (__VA_ARGS__);                                         \
        if ([&]() -> bool {                                                     \
                struct State {                                                  \
                    long long count = 0;                                        \
                    long long target;                                           \
                    explicit State(long long t) : target(t) {}                  \
                };                                                              \
                static State s{ static_cast<long long>(invocations_expr) };     \
                return (s.count++ >= s.target);                                 \
            }()) {                                                              \
            _a3d_fn();                                                          \
        }                                                                       \
    } while (0)

// runs 'on_edge()' only on false->true transitions of 'cond'
// runs 'fail_stmt' every time 'cond' is true (e.g. return/continue/break/throw)
// resets when 'cond' becomes false again
#define A3D_EDGE_GUARD(cond, fail_stmt, ...) \
    A3D_EDGE_GUARD_IMPL((cond), fail_stmt, __COUNTER__, __VA_ARGS__)

#define A3D_EDGE_GUARD_IMPL(cond, fail_stmt, ctr, ...) \
do { \
  static bool A3D_PP_CAT(_a3d_latched_, ctr) = false; \
  const bool _a3d_bad = !!(cond); \
  if (_a3d_bad) { \
    if (!A3D_PP_CAT(_a3d_latched_, ctr)) { \
      A3D_PP_CAT(_a3d_latched_, ctr) = true; \
      auto&& _a3d_edge_fn = (__VA_ARGS__); \
      _a3d_edge_fn(); \
    } \
    fail_stmt; \
  } else { \
    A3D_PP_CAT(_a3d_latched_, ctr) = false; \
  } \
} while (0)

#endif //AVARA3D_UTILITYMACROS_H
