//
//  OpenGLDrawTimer.h
//  avara3d
//
//  Created by Morgan Davis on 12/24/25.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_PROFILING_OPENGLDRAWTIMER_H
#define AVARA3D_PROFILING_OPENGLDRAWTIMER_H

#include <chrono>
#include <cstdint>
#include <vector>

#include "a3d/render/backend/opengl/GLTypes.h"

namespace a3d {

class OGLDrawTimer {

public:
    // [Internal Lifecycle Functions]

    explicit OGLDrawTimer(unsigned bufferedFrames);

    OGLDrawTimer(const OGLDrawTimer&)            = delete;
    OGLDrawTimer& operator=(const OGLDrawTimer&) = delete;

    OGLDrawTimer(OGLDrawTimer&& other)            = delete;
    OGLDrawTimer& operator=(OGLDrawTimer&& other) = delete;

    ~OGLDrawTimer();

    // [Internal Member Functions]

    bool                     initialize();
    void                     begin();
    std::chrono::nanoseconds end();

private:
    // [Private Types]

    enum class Mode {
        Disabled,
        DesktopTimeElapsed,
        WebDisjointTimerQuery,
        WebDisjointTimerQueryExt
    };

    // [Private Member Functions]

    bool                     resolveQuery(size_t index);
    void                     resolveIssuedQueries(size_t skipIndex);

    // [Private Member Variables]

    Mode                     _mode;
    gl::enum_t               _queryTarget;
    unsigned                 _bufferSize;
    std::vector<unsigned>    _glQueries;
    size_t                   _frame;
    std::chrono::nanoseconds _lastTime;
    std::vector<uint8_t>     _issued;
    bool                     _active      = false;
    bool                     _initialized = false;
};

}

#endif // AVARA3D_PROFILING_OPENGLDRAWTIMER_H
