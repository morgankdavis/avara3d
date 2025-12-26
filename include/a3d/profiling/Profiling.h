//
// Created by mkd on 12/25/25.
//

#ifndef AVARA3D_PROFILING_H
#define AVARA3D_PROFILING_H

#include "a3d/profiling/FrameStatsHistory.h"
#include "a3d/profiling/OpenGLDrawTimer.h"
#include "a3d/profiling/Profiler.h"
#include "a3d/profiling/ScopeTimer.h"
#include "a3d/profiling/Timer.h"

namespace a3d {
	template <class F>
	decltype(auto) ProfileScope(Profiler& profiler, Profiler::Tag tag, F&& f) {
		ScopeTimer t{profiler, tag};
		return std::forward<F>(f)();
	}

#define A3D_PROFILE(profiler, tag, ...) \
		::a3d::ProfileScope((profiler), (tag), __VA_ARGS__)
}

#endif //AVARA3D_PROFILING_H
