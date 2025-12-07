
#include "a3d/profiling/FrameStatsHistory.h"

#include "a3d/diagnostic/log/Log.h"

using namespace a3d;
using namespace std;

FrameStatsHistory::FrameStatsHistory(float historySeconds):
		_historySeconds(historySeconds) {}

void FrameStatsHistory::add(FrameStats stats) {

	const SteadyTimePoint now = std::chrono::steady_clock::now();

	_samples.push_back({now, stats});

	while (!_samples.empty()
		   && chrono::duration<float>(now - get<0>(_samples.front())).count() > _historySeconds) {
		_samples.pop_front();
	}
}

const deque<tuple<FrameStatsHistory::SteadyTimePoint, FrameStats>>&
FrameStatsHistory::samples() const {
	return _samples;
}
