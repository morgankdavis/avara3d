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

    /**
     * @brief Maintains a rolling wall-clock history of FrameStats samples.
     *
     * Each added sample is timestamped with a steady clock and samples older than
     * the configured history duration are discarded as new samples are added.
     */
    class FrameStatsHistory {

    public:
        // [Public Types]

        /** @brief Steady-clock timestamp associated with a FrameStats sample. */
        using SteadyTimePoint = std::chrono::time_point<std::chrono::steady_clock>;

        // [Public Static Member Functions]

        /**
         * @brief Computes mean timing values from samples within @p averagingDuration of the current time.
         *
         * Each output is set to zero when the selected window contains no samples.
         *
         * @param history History whose recent samples are averaged.
         * @param frame Receives average total frame time.
         * @param engineCpu Receives average engine CPU time.
         * @param renderCpu Receives average render CPU time.
         * @param renderGpu Receives average render GPU time.
         * @param physics Receives average physics time.
         * @param appCpu Receives average application CPU time.
         * @param averagingDuration Wall-clock age window used to select samples.
         */
        static void GetAverages(const FrameStatsHistory&  history,
                                std::chrono::nanoseconds& frame,
                                std::chrono::nanoseconds& engineCpu,
                                std::chrono::nanoseconds& renderCpu,
                                std::chrono::nanoseconds& renderGpu,
                                std::chrono::nanoseconds& physics,
                                std::chrono::nanoseconds& appCpu,
                                std::chrono::milliseconds averagingDuration);
        // [Public Lifecycle Functions]

        /** @brief Creates an empty history that retains samples for approximately @p historyTime. */
        explicit FrameStatsHistory(std::chrono::milliseconds historyTime);

        // [Public Member Functions]

        /** @brief Adds a timestamped FrameStats sample and discards samples older than the history duration. */
        void                                                       add(FrameStats stats);

        /** @brief Returns retained samples in oldest-to-newest order. */
        const std::deque<std::tuple<SteadyTimePoint, FrameStats>>& samples() const;

    private:
        // [Private Member Variables]

        std::chrono::milliseconds                           _historyTime;
        std::deque<std::tuple<SteadyTimePoint, FrameStats>> _samples;
    };

}

#endif //AVARA3D_PROFILE_FRAMESTATSHISTORY_H
