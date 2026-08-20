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

    /** @brief Accumulates elapsed-time samples by predefined tag or arbitrary string key. */
    class Profiler {

    public:
        // [Public Types]

        /** @brief Predefined timing categories used by A3D's frame profiler. */
        enum class Tag {
            Frame,       ///< Total Runner frame/update work.
            EngineCpu,   ///< A3D engine CPU work.
            RenderCpu,   ///< Rendering CPU work.
            RenderGpu,   ///< Rendering GPU work.
            Physics,     ///< Physics simulation work.
            Application  ///< Application callback work.
        };

        // [Public Lifecycle Functions]

        /** @brief Creates an empty Profiler. */
        Profiler() = default;

        Profiler(const Profiler&)            = delete;
        Profiler& operator=(const Profiler&) = delete;

        Profiler(Profiler&&)            = delete;
        Profiler& operator=(Profiler&&) = delete;

        // [Public Member Functions]

        /** @brief Adds @p ns to the accumulated duration for @p tag. */
        void      add(Tag tag, std::chrono::nanoseconds ns) noexcept;

        /** @brief Adds @p ns to the accumulated duration for @p key. */
        void      add(const std::string& key, std::chrono::nanoseconds ns) noexcept; // ! untested

        // ! TEMPORARY !
//		void subtract(Tag tag, std::chrono::nanoseconds ns); // ! untested
//		void subtract(const std::string& key, std::chrono::nanoseconds ns); // ! untested

        /** @brief Returns the accumulated duration for @p tag, or zero if no sample has been added. */
        std::chrono::nanoseconds time(Tag tag);

        /** @brief Returns the accumulated duration for @p key, or zero if no sample has been added. */
        std::chrono::nanoseconds time(const std::string& key); // ! untested

        /** @brief Removes all accumulated tagged and keyed samples. */
        void                     reset();

    private:
        // [Private Member Variables]

        std::map<Tag, std::chrono::nanoseconds>         _taggedSamples;
        std::map<std::string, std::chrono::nanoseconds> _keyedSamples;
    };

}

#endif //AVARA3D_PROFILING_PROFILER_H
