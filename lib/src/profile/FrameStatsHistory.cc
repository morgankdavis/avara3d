//
//  FrameStatsHistory.cc
//  avara3d
//
//  Created by Morgan Davis on 12/6/25.
//  Copyright © 2024 Morgan K Davis. All rights reserved.
//

#include "a3d/profile/FrameStatsHistory.h"

using namespace a3d;
using namespace std;

/// Public Static Member Functions ///

void FrameStatsHistory::GetAverages(const FrameStatsHistory& history,
                                    chrono::nanoseconds&     frame,
                                    chrono::nanoseconds&     engineCpu,
                                    chrono::nanoseconds&     renderCpu,
                                    chrono::nanoseconds&     renderGpu,
                                    chrono::nanoseconds&     physics,
                                    chrono::nanoseconds&     appCpu,
                                    chrono::milliseconds     averagingDuration) {
    frame = chrono::nanoseconds(0);
    engineCpu = chrono::nanoseconds(0);
    renderCpu = chrono::nanoseconds(0);
    renderGpu = chrono::nanoseconds(0);
    physics = chrono::nanoseconds(0);
    appCpu = chrono::nanoseconds(0);

    const SteadyTimePoint now = std::chrono::steady_clock::now();

    unsigned count = 0;
    for (auto it = history._samples.rbegin(); it != history._samples.rend(); ++it) {
        auto sample = get<1>(*it);
        if ((now - get<0>(*it)) < averagingDuration) {
            frame += sample.frameTime;
            engineCpu += sample.engineCpuTime;
            renderCpu += sample.renderCpuTime;
            renderGpu += sample.renderGpuTime;
            physics += sample.physicsTime;
            appCpu += sample.applicationTime;
            ++count;
        }
        else {
            break;
        }
    }

    if (count > 0) {
        frame /= count;
        engineCpu /= count;
        renderCpu /= count;
        renderGpu /= count;
        physics /= count;
        appCpu /= count;
    }
}

/// Public Lifecycle Functions ///

FrameStatsHistory::FrameStatsHistory(chrono::milliseconds historyTime):
    _historyTime(historyTime) {}

/// Public Member Functions ///

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
