//
//  Profiling.h
//  avara3d
//
//  Created by Morgan Davis on 12/25/25.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_PROFILE_PROFILE_H
#define AVARA3D_PROFILE_PROFILE_H

#include <functional>
#include <type_traits>
#include <utility>

#include "a3d/profile/Profile.h"
#include "a3d/profile/ScopeTimer.h"

namespace a3d::prof {

// run f() while measuring its duration into `profiler` under `tag`
// supports nesting!
template<class F>
decltype(auto) profile(Profiler& profiler, Profiler::Tag tag, F&& f) {
    ScopeTimer t {profiler, tag};
    return std::invoke(std::forward<F>(f));
}

}

#endif // AVARA3D_PROFILE_PROFILE_H
