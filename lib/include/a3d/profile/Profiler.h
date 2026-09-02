//
//  Profiler.h
//  avara3d
//
//  Created by Morgan Davis on 12/6/25.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_PROFILE_PROFILER_H
#define AVARA3D_PROFILE_PROFILER_H

#include <chrono>
#include <map>
#include <string>

namespace a3d {

class Profiler {

public:
    // [Internal Types]

    enum class Tag {
        Frame,
        EngineCpu,
        RenderCpu,
        RenderGpu,
        Physics,
        Application
    };

    // [Internal Lifecycle Functions]

    Profiler() = default;

    Profiler(const Profiler&)            = delete;
    Profiler& operator=(const Profiler&) = delete;

    Profiler(Profiler&&)                           = delete;
    Profiler&                operator=(Profiler&&) = delete;

    // [Internal Member Functions]

    void                     add(Tag tag, std::chrono::nanoseconds ns) noexcept;
    void                     add(const std::string& key, std::chrono::nanoseconds ns) noexcept; // ! untested

    // ! TEMPORARY !
//		void subtract(Tag tag, std::chrono::nanoseconds ns); // ! untested
//		void subtract(const std::string& key, std::chrono::nanoseconds ns); // ! untested

    std::chrono::nanoseconds time(Tag tag);
    std::chrono::nanoseconds time(const std::string& key); // ! untested

    void                     reset();

private:
    // [Private Member Variables]

    std::map<Tag, std::chrono::nanoseconds>         _taggedSamples;
    std::map<std::string, std::chrono::nanoseconds> _keyedSamples;
};

}

#endif // AVARA3D_PROFILE_PROFILER_H
