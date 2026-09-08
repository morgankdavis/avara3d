//
//  FrameStatsHistory.h
//  avara3d
//
//  Created by Morgan Davis on 12/6/25.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_PROFILE_FRAMESTATSHISTORY_H
#define AVARA3D_PROFILE_FRAMESTATSHISTORY_H

#include <chrono>
#include <deque>
#include <tuple>

#include "a3d/profile/FrameStats.h"

namespace a3d {

class FrameStatsHistory {

public:
    // [Internal Types]

    using SteadyTimePoint = std::chrono::time_point<std::chrono::steady_clock>;

    // [Internal Static Member Functions]

    static void GetAverages(const FrameStatsHistory&  history,
                            std::chrono::nanoseconds& frame,
                            std::chrono::nanoseconds& renderPrep,
                            std::chrono::nanoseconds& renderSubmit,
                            std::chrono::nanoseconds& renderGpu,
                            std::chrono::nanoseconds& physics,
                            std::chrono::nanoseconds& appCpu,
                            std::chrono::milliseconds averagingDuration);

    // [Internal Lifecycle Functions]

    explicit FrameStatsHistory(std::chrono::milliseconds historyTime);

    // [Internal Member Functions]

    void                                                       add(FrameStats stats);

    const std::deque<std::tuple<SteadyTimePoint, FrameStats>>& samples() const;

private:
    // [Private Member Variables]

    std::chrono::milliseconds                           _historyTime;
    std::deque<std::tuple<SteadyTimePoint, FrameStats>> _samples;
};

} // namespace a3d

#endif // AVARA3D_PROFILE_FRAMESTATSHISTORY_H
