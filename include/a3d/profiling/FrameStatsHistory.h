
#ifndef AVARA3D_FRAMESTATSHISTORY_H
#define AVARA3D_FRAMESTATSHISTORY_H

#include <deque>
#include <tuple>

#include "a3d/Types.h"

namespace a3d {

	class FrameStatsHistory {

	public:

		using SteadyTimePoint = std::chrono::time_point<std::chrono::steady_clock>;

		explicit FrameStatsHistory(float historySeconds);

		void add(FrameStats stats);

		const std::deque<std::tuple<SteadyTimePoint, FrameStats>>& samples() const;

//		void drawImGui(const char *label) {
//
//			if (_samples.empty())
//				return;
//
//			// ImGui::PlotLines wants a contiguous array of floats.
//			// We only care about Y values; X is just "index".
//			static std::vector<float> tmp; // reuse capacity
//			tmp.resize(_samples.size());
//			for (size_t i = 0; i < _samples.size(); ++i) {
//				tmp[i] = _samples[i].value;
//			}
//
//			ImGui::PlotLines(label,
//							 tmp.data(),
//							 static_cast<int>(tmp.size()),
//							 0, nullptr,
//							 FLT_MAX, FLT_MAX,
//							 ImVec2(0, 80.0f));
//		}

	private:

		float													_historySeconds;
		std::deque<std::tuple<SteadyTimePoint, FrameStats>>		_samples;
	};
}

#endif //AVARA3D_FRAMESTATSHISTORY_H
