//
//  FrameStatsHistory.h
//  avara3d
//
//  Created by Morgan Davis on 12/6/25.
//  Copyright © 2024 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_FRAMESTATSHISTORY_H
#define AVARA3D_FRAMESTATSHISTORY_H

#include <deque>
#include <tuple>

#include "a3d/Types.h"

namespace a3d {

	class FrameStatsHistory {

	public:
		/// Public Types ///

		using SteadyTimePoint = std::chrono::time_point<std::chrono::steady_clock>;

		/// Public Lifecycle Functions ///

		explicit FrameStatsHistory(float historySeconds);

		/// Public Member Functions ///

		void add(FrameStats stats);
		const std::deque<std::tuple<SteadyTimePoint, FrameStats>>& samples() const;

	private:
		/// Private Member Variables ///

		float													_historySeconds;
		std::deque<std::tuple<SteadyTimePoint, FrameStats>>		_samples;
	};
}

#endif //AVARA3D_FRAMESTATSHISTORY_H
