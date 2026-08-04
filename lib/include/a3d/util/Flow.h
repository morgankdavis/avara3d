//
//  Flow.h
//  avara3d
//
//  Created by Morgan Davis on 1/1/26.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_UTIL_FLOW_H
#define AVARA3D_UTIL_FLOW_H

#include <chrono>
#include <cstdint>
#include <functional>
#include <memory>
#include <mutex>
#include <optional>
#include <source_location>
#include <string_view>
#include <type_traits>
#include <unordered_map>
#include <utility>

// TODO: summary

namespace a3d::util::flow::detail {

    inline std::size_t hash_combine(std::size_t seed, std::size_t v) {
        return seed ^ (v + 0x9e3779b97f4a7c15ull + (seed << 6) + (seed >> 2));
    }

    inline std::size_t callsite_key(std::source_location loc) {
        std::size_t      h    = 0;
        std::string_view file = loc.file_name();
        std::string_view func = loc.function_name();
        h                     = hash_combine(h, std::hash<std::string_view> {}(file));
        h                     = hash_combine(h, std::hash<std::string_view> {}(func));
        h                     = hash_combine(h, std::hash<unsigned> {}(loc.line()));
        h                     = hash_combine(h, std::hash<unsigned> {}(loc.column()));
        return h;
    }

    // Per-(State type, TU) map keyed by callsite.
    template<class State, class... CtorArgs>
    State& state_for(std::source_location loc, CtorArgs&&... ctorArgs) {
        static std::mutex                                              m;
        static std::unordered_map<std::size_t, std::unique_ptr<State>> states;

        const std::size_t                                              key = callsite_key(loc);

        std::lock_guard<std::mutex>                                    lock(m);
        auto&                                                          ptr = states[key];
        if (!ptr) {
            ptr = std::make_unique<State>(std::forward<CtorArgs>(ctorArgs)...);
        }
        return *ptr;
    }

    template<class Clock>
    bool every_tick(typename Clock::time_point& last, typename Clock::duration interval) {
        const auto now = Clock::now();
        if (now - last >= interval) {
            last = now; // "at most once" behavior (no catch-up)
            return true;
        }
        return false;
    }

    inline long long clamp_step(long long n) {
        return (n < 1) ? 1 : n;
    }

} // namespace a3d::util::flow::detail

namespace a3d::util::flow {

    //------------------------------------------------------------------------------
    // guard / edge_guard
    //------------------------------------------------------------------------------
    // Returns:
    //   true  -> condition passed, nothing happened
    //   false -> condition failed, callback ran (every time for guard; latched for edge_guard)

//	template<class Cond, class FailFn>
//	[[nodiscard]] bool guard(Cond&& cond, FailFn&& on_fail_every_time) {
//		const bool ok = static_cast<bool>(std::forward<Cond>(cond)); // supports shared_ptr, etc.
//		if (!ok) std::invoke(std::forward<FailFn>(on_fail_every_time));
//		return ok;
//	}

    template<class Cond, class FailFn>
    [[nodiscard]] bool edge_guard(Cond&&               cond,
                                  FailFn&&             on_fail_once,
                                  std::source_location loc = std::source_location::current()) {
        struct State {
            bool latched = false;
        };

        const bool ok = static_cast<bool>(std::forward<Cond>(cond)); // supports shared_ptr, etc.
        auto&      s  = ::a3d::util::flow::detail::state_for<State>(loc);

        if (ok) {
            s.latched = false;
            return true;
        }

        if (!s.latched) {
            s.latched = true;
            std::invoke(std::forward<FailFn>(on_fail_once));
        }
        return false;
    }

    //------------------------------------------------------------------------------
    // once / once_else
    //------------------------------------------------------------------------------

    template<class Fn>
    decltype(auto) once(Fn&& fn, std::source_location loc = std::source_location::current()) {
        using R = std::invoke_result_t<Fn&>;

        if constexpr (std::is_void_v<R>) {
            struct State {
                std::once_flag once;
            };

            auto& s = ::a3d::util::flow::detail::state_for<State>(loc);
            std::call_once(s.once, [&] {
                std::invoke(std::forward<Fn>(fn));
            });
            return;
        }
        else {
            using T = std::decay_t<R>;

            struct State {
                std::once_flag   once;
                std::optional<T> value;
            };

            auto& s = ::a3d::util::flow::detail::state_for<State>(loc);

            std::call_once(s.once, [&] {
                s.value.emplace(std::invoke(std::forward<Fn>(fn)));
            });
            return static_cast<const T&>(*s.value);
        }
    }

    template<class ThenFn, class ElseFn>
    decltype(auto) once_else(ThenFn&&             then_fn,
                             ElseFn&&             else_fn,
                             std::source_location loc = std::source_location::current()) {
        using R1 = std::invoke_result_t<ThenFn&>;
        using R2 = std::invoke_result_t<ElseFn&>;

        static_assert(std::is_void_v<R1> == std::is_void_v<R2>,
                      "once_else: then/else must both return void or both return a value");
        if constexpr (!std::is_void_v<R1>) {
            static_assert(std::is_same_v<std::decay_t<R1>, std::decay_t<R2>>,
                          "once_else: then/else must return the same type");
        }

        struct State {
            std::once_flag once;
        };

        auto& s = ::a3d::util::flow::detail::state_for<State>(loc);

        bool  i_ran_then = false;
        if constexpr (std::is_void_v<R1>) {
            std::call_once(s.once, [&] {
                i_ran_then = true;
                std::invoke(std::forward<ThenFn>(then_fn));
            });
            if (!i_ran_then) {
                std::invoke(std::forward<ElseFn>(else_fn));
            }
            return;
        }
        else {
            using T = std::decay_t<R1>;
            std::optional<T> first_value;

            std::call_once(s.once, [&] {
                i_ran_then = true;
                first_value.emplace(std::invoke(std::forward<ThenFn>(then_fn)));
            });

            if (i_ran_then) {
                return *first_value; // by value (T)
            }
            return std::invoke(std::forward<ElseFn>(else_fn)); // by value (T)
        }
    }

    //------------------------------------------------------------------------------
    // on / on_else
    //------------------------------------------------------------------------------

    template<class Fn>
    decltype(auto) on(long long            invocation,
                      Fn&&                 fn,
                      std::source_location loc = std::source_location::current()) {
        using R = std::invoke_result_t<Fn&>;

        struct State {
            long long count = 0;
            long long target;
            bool      done = false;

            explicit State(long long t):
                target(t < 1 ? 1 : t) {}
        };

        auto& s = ::a3d::util::flow::detail::state_for<State>(loc, invocation);

        if constexpr (std::is_void_v<R>) {
            if (!s.done && (++s.count == s.target)) {
                s.done = true;
                std::invoke(std::forward<Fn>(fn));
            }
            return;
        }
        else {
            using T = std::decay_t<R>;
            if (!s.done && (++s.count == s.target)) {
                s.done = true;
                return std::optional<T> {std::invoke(std::forward<Fn>(fn))};
            }
            return std::optional<T> {};
        }
    }

    template<class ThenFn, class ElseFn>
    decltype(auto) on_else(long long            invocation,
                           ThenFn&&             then_fn,
                           ElseFn&&             else_fn,
                           std::source_location loc = std::source_location::current()) {
        using R1 = std::invoke_result_t<ThenFn&>;
        using R2 = std::invoke_result_t<ElseFn&>;

        static_assert(std::is_void_v<R1> == std::is_void_v<R2>,
                      "on_else: then/else must both return void or both return a value");
        if constexpr (!std::is_void_v<R1>) {
            static_assert(std::is_same_v<std::decay_t<R1>, std::decay_t<R2>>,
                          "on_else: then/else must return the same type");
        }

        struct State {
            long long count = 0;
            long long target;
            bool      done = false;

            explicit State(long long t):
                target(t < 1 ? 1 : t) {}
        };

        auto&      s = ::a3d::util::flow::detail::state_for<State>(loc, invocation);

        const bool fire = (!s.done && (++s.count == s.target));
        if (fire) {
            s.done = true;
        }

        if constexpr (std::is_void_v<R1>) {
            if (fire) {
                std::invoke(std::forward<ThenFn>(then_fn));
            }
            else {
                std::invoke(std::forward<ElseFn>(else_fn));
            }
            return;
        }
        else {
            return fire ? std::invoke(std::forward<ThenFn>(then_fn))
                        : std::invoke(std::forward<ElseFn>(else_fn));
        }
    }

    //------------------------------------------------------------------------------
    // after / after_else
    //------------------------------------------------------------------------------

    template<class Fn>
    decltype(auto) after(long long            invocations,
                         Fn&&                 fn,
                         std::source_location loc = std::source_location::current()) {
        using R = std::invoke_result_t<Fn&>;

        struct State {
            long long count = 0;
            long long target;

            explicit State(long long t):
                target(t < 0 ? 0 : t) {}
        };

        auto&      s = ::a3d::util::flow::detail::state_for<State>(loc, invocations);

        const bool fire = (s.count++ >= s.target);

        if constexpr (std::is_void_v<R>) {
            if (fire) {
                std::invoke(std::forward<Fn>(fn));
            }
            return;
        }
        else {
            using T = std::decay_t<R>;
            if (fire) {
                return std::optional<T> {std::invoke(std::forward<Fn>(fn))};
            }
            return std::optional<T> {};
        }
    }

    template<class ThenFn, class ElseFn>
    decltype(auto) after_else(long long            invocations,
                              ThenFn&&             then_fn,
                              ElseFn&&             else_fn,
                              std::source_location loc = std::source_location::current()) {
        using R1 = std::invoke_result_t<ThenFn&>;
        using R2 = std::invoke_result_t<ElseFn&>;

        static_assert(std::is_void_v<R1> == std::is_void_v<R2>,
                      "after_else: then/else must both return void or both return a value");
        if constexpr (!std::is_void_v<R1>) {
            static_assert(std::is_same_v<std::decay_t<R1>, std::decay_t<R2>>,
                          "after_else: then/else must return the same type");
        }

        struct State {
            long long count = 0;
            long long target;

            explicit State(long long t):
                target(t < 0 ? 0 : t) {}
        };

        auto&      s = ::a3d::util::flow::detail::state_for<State>(loc, invocations);

        const bool fire = (s.count++ >= s.target);

        if constexpr (std::is_void_v<R1>) {
            if (fire) {
                std::invoke(std::forward<ThenFn>(then_fn));
            }
            else {
                std::invoke(std::forward<ElseFn>(else_fn));
            }
            return;
        }
        else {
            return fire ? std::invoke(std::forward<ThenFn>(then_fn))
                        : std::invoke(std::forward<ElseFn>(else_fn));
        }
    }

    //------------------------------------------------------------------------------
    // every / every_else  (time-based, arbitrary duration)
    //------------------------------------------------------------------------------
    // every(interval, fn):
    //   - runs fn immediately the first call, then at most once per interval thereafter
    //   - returns true if it fired

    template<class Rep, class Period, class Fn, class Clock = std::chrono::steady_clock>
    bool every(std::chrono::duration<Rep, Period> interval,
               Fn&&                               fn,
               std::source_location               loc = std::source_location::current()) {
        using D = typename Clock::duration;

        struct State {
            typename Clock::time_point last;
            D                          interval;

            explicit State(D i):
                last(Clock::now() - i),
                interval(i) {} // fire immediately
        };

        const D i = std::chrono::duration_cast<D>(interval);
        auto&   s = ::a3d::util::flow::detail::state_for<State>(loc, i);

        if (::a3d::util::flow::detail::every_tick<Clock>(s.last, s.interval)) {
            std::invoke(std::forward<Fn>(fn));
            return true;
        }
        return false;
    }

    template<class Rep, class Period, class ThenFn, class ElseFn, class Clock = std::chrono::steady_clock>
    decltype(auto) every_else(std::chrono::duration<Rep, Period> interval,
                              ThenFn&&                           then_fn,
                              ElseFn&&                           else_fn,
                              std::source_location               loc = std::source_location::current()) {
        using R1 = std::invoke_result_t<ThenFn&>;
        using R2 = std::invoke_result_t<ElseFn&>;

        static_assert(std::is_void_v<R1> == std::is_void_v<R2>,
                      "every_else: then/else must both return void or both return a value");
        if constexpr (!std::is_void_v<R1>) {
            static_assert(std::is_same_v<std::decay_t<R1>, std::decay_t<R2>>,
                          "every_else: then/else must return the same type");
        }

        using D = typename Clock::duration;

        struct State {
            typename Clock::time_point last;
            D                          interval;

            explicit State(D i):
                last(Clock::now() - i),
                interval(i) {} // fire immediately
        };

        const D    i = std::chrono::duration_cast<D>(interval);
        auto&      s = ::a3d::util::flow::detail::state_for<State>(loc, i);

        const bool fire = ::a3d::util::flow::detail::every_tick<Clock>(s.last, s.interval);

        if constexpr (std::is_void_v<R1>) {
            if (fire) {
                std::invoke(std::forward<ThenFn>(then_fn));
            }
            else {
                std::invoke(std::forward<ElseFn>(else_fn));
            }
            return;
        }
        else {
            return fire ? std::invoke(std::forward<ThenFn>(then_fn))
                        : std::invoke(std::forward<ElseFn>(else_fn));
        }
    }

    //------------------------------------------------------------------------------
    // NEW: every(n, fn) and every(interval, n, fn)
    //------------------------------------------------------------------------------
    // every(n, fn):
    //   - runs fn immediately the first call
    //   - then runs again every n-th call AFTER that (n<=0 treated as 1)
    //   - returns true if it fired
    //
    // Example (n=10): fires on calls 1, 11, 21, ...

    template<class Fn>
    bool every(long long n, Fn&& fn, std::source_location loc = std::source_location::current()) {
        struct State {
            bool      first = true;
            long long step;
            long long since = 0; // calls since last fire (not counting the firing call)

            explicit State(long long s):
                step(::a3d::util::flow::detail::clamp_step(s)) {}
        };

        auto& s = ::a3d::util::flow::detail::state_for<State>(loc, n);

        if (s.first) {
            s.first = false;
            s.since = 0;
            std::invoke(std::forward<Fn>(fn));
            return true;
        }

        ++s.since;
        if (s.since >= s.step) {
            s.since = 0;
            std::invoke(std::forward<Fn>(fn));
            return true;
        }
        return false;
    }

    // Optional symmetry: every_else(n, then, else) and every_else(interval, n, then, else)

    template<class ThenFn, class ElseFn>
    decltype(auto) every_else(long long            n,
                              ThenFn&&             then_fn,
                              ElseFn&&             else_fn,
                              std::source_location loc = std::source_location::current()) {
        using R1 = std::invoke_result_t<ThenFn&>;
        using R2 = std::invoke_result_t<ElseFn&>;

        static_assert(std::is_void_v<R1> == std::is_void_v<R2>,
                      "every_else(n): then/else must both return void or both return a value");
        if constexpr (!std::is_void_v<R1>) {
            static_assert(std::is_same_v<std::decay_t<R1>, std::decay_t<R2>>,
                          "every_else(n): then/else must return the same type");
        }

        const bool fire = every(
            n,
            [&] {
                std::invoke(std::forward<ThenFn>(then_fn));
            },
            loc);

        if constexpr (std::is_void_v<R1>) {
            if (!fire) {
                std::invoke(std::forward<ElseFn>(else_fn));
            }
            return;
        }
        else {
            if (fire) {
                return std::invoke(std::forward<ThenFn>(then_fn));
            }
            return std::invoke(std::forward<ElseFn>(else_fn));
        }
    }

} // namespace a3d::util::flow

#endif //AVARA3D_UTIL_FLOW_H
