//
//  FrameStatsHistory.cc
//  avara3d
//
//  Created by Morgan Davis on 12/6/25.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#include "a3d/profile/FrameStatsHistory.h"

using namespace std;

namespace a3d {

// [Public Static Member Functions]

void FrameStatsHistory::GetAverages(const FrameStatsHistory& history,
                                    chrono::nanoseconds&     frame,
                                    chrono::nanoseconds&     renderPrep,
                                    chrono::nanoseconds&     renderSubmit,
                                    chrono::nanoseconds&     renderGpu,
                                    chrono::nanoseconds&     physics,
                                    chrono::nanoseconds&     appCpu,
                                    chrono::milliseconds     averagingDuration) {

    frame = chrono::nanoseconds {0};
    renderPrep = chrono::nanoseconds {0};
    renderSubmit = chrono::nanoseconds {0};
    renderGpu = chrono::nanoseconds {0};
    physics = chrono::nanoseconds {0};
    appCpu = chrono::nanoseconds {0};

    const SteadyTimePoint now = chrono::steady_clock::now();

    unsigned count = 0;

    for (auto it = history._samples.rbegin(); it != history._samples.rend(); ++it) {

        const auto& [sampleTime, sample] = *it;

        if ((now - sampleTime) >= averagingDuration) {
            break;
        }

        frame += sample.frameTime;
        renderPrep += sample.renderPrepTime;
        renderSubmit += sample.renderSubmitTime;
        renderGpu += sample.renderGpuTime;
        physics += sample.physicsTime;
        appCpu += sample.applicationTime;

        ++count;
    }

    if (count > 0) {
        frame /= count;
        renderPrep /= count;
        renderSubmit /= count;
        renderGpu /= count;
        physics /= count;
        appCpu /= count;
    }
}

// [Public Lifecycle Functions]

FrameStatsHistory::FrameStatsHistory(chrono::milliseconds historyTime):
    _historyTime(historyTime) {}

// [Public Member Functions]

void FrameStatsHistory::add(FrameStats stats) {

    const SteadyTimePoint now = chrono::steady_clock::now();

    _samples.push_back({now, stats});

    while (!_samples.empty()
           //&& chrono::duration<float>(now - get<0>(_samples.front())).count() > _historyTime) {
           && ((now - get<0>(_samples.front())) > _historyTime)) {
        _samples.pop_front();
    }
}

const deque<tuple<FrameStatsHistory::SteadyTimePoint, FrameStats>>& FrameStatsHistory::samples() const {
    return _samples;
}

} // namespace a3d
