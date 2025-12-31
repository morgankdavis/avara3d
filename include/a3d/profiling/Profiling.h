//
// Created by mkd on 12/25/25.
//

#ifndef AVARA3D_PROFILING_H
#define AVARA3D_PROFILING_H

#include <functional>   // std::invoke
#include <type_traits>
#include <utility>

#include "a3d/profiling/Profiler.h"
#include "a3d/profiling/ScopeTimer.h"

namespace a3d::prof {

	// Runs f() while measuring its duration into `profiler` under `tag`.
	// Supports nesting naturally.
	template <class F>
	decltype(auto) profile(Profiler& profiler, Profiler::Tag tag, F&& f) {
		ScopeTimer t{profiler, tag};
		return std::invoke(std::forward<F>(f));
	}

	// Optional convenience: scope-style usage without a lambda
	// auto _ = a3d::profiling::scoped(profiler, Tag::RenderCpu);
	[[nodiscard]] inline ScopeTimer scoped(Profiler& profiler, Profiler::Tag tag) {
		return ScopeTimer{profiler, tag};
	}

} // namespace a3d::profiling

#endif // AVARA3D_PROFILING_H
