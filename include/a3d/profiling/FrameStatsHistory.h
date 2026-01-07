//
//  FrameStatsHistory.h
//  avara3d
//
//  Created by Morgan Davis on 12/6/25.
//  Copyright © 2024 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_PROFILING_FRAMESTATSHISTORY_H
#define AVARA3D_PROFILING_FRAMESTATSHISTORY_H

#include <chrono>
#include <deque>
#include <tuple>

#include "a3d/Types.h"
#include "a3d/profiling/FrameStats.h"

namespace a3d {

	class FrameStatsHistory {

	public:
		/// Public Types ///

		using SteadyTimePoint = std::chrono::time_point<std::chrono::steady_clock>;

		/// Public Static Member Functions ///

		static void GetAverages(const FrameStatsHistory& history,
								std::chrono::nanoseconds& frame,
								std::chrono::nanoseconds& engineCpu,
								std::chrono::nanoseconds& renderCpu,
								std::chrono::nanoseconds& renderGpu,
								std::chrono::nanoseconds& physics,
								std::chrono::nanoseconds& appCpu,
								std::chrono::milliseconds averagingDuration);
		/// Public Lifecycle Functions ///

		explicit FrameStatsHistory(std::chrono::milliseconds historyTime);

		/// Public Member Functions ///

		void add(FrameStats stats);
		const std::deque<std::tuple<SteadyTimePoint, FrameStats>>& samples() const;

	private:
		/// Private Member Variables ///

		std::chrono::milliseconds								_historyTime;
		std::deque<std::tuple<SteadyTimePoint, FrameStats>>		_samples;
	};
}

#endif //AVARA3D_PROFILING_FRAMESTATSHISTORY_H
