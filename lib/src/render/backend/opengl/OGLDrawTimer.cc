//
//  OpenGLDrawTimer.cc
//  avara3d
//
//  Created by Morgan Davis on 12/24/25.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#include "a3d/render/backend/opengl/OGLDrawTimer.h"

#if defined(A3D_GL_WEB)
    #include <emscripten/html5_webgl.h>
#endif

#include "a3d/Assert.h"
#include "a3d/Math.h"
#include "a3d/log/Log.h"
#include "a3d/render/backend/opengl/gl.h"

using namespace std;

namespace a3d {
namespace {

    // [Private Non-Member Prototypes]

    unsigned MinBufferSize(unsigned bufferedFrames);

} // namespace

// [Internal Lifecycle Functions]

OGLDrawTimer::OGLDrawTimer(unsigned bufferedFrames):
    _mode {Mode::Disabled},
    _queryTarget {},
    _bufferSize(MinBufferSize(bufferedFrames)),
    _glQueries(static_cast<size_t>(_bufferSize), 0),
    _frame {0},
    _lastTime {chrono::nanoseconds {static_cast<chrono::nanoseconds::rep>(0)}} {}

OGLDrawTimer::~OGLDrawTimer() {
    // ! requires a current GL context !
    if (_mode != Mode::Disabled && !_glQueries.empty()) {
#if defined(A3D_GL_WEB)
        if (_mode == Mode::WebDisjointTimerQueryExt) {
            glDeleteQueriesEXT(_bufferSize, _glQueries.data());
            return;
        }
#endif
        glDeleteQueries(_bufferSize, _glQueries.data());
    }
}

// [Internal Member Functions]

bool OGLDrawTimer::initialize() {
    if (_initialized) {
        return _mode != Mode::Disabled;
    }

#if defined(A3D_GL_DESKTOP)
    // TODO: check context?
    _mode = Mode::DesktopTimeElapsed;
    _queryTarget = GL_TIME_ELAPSED;
#elif defined(A3D_GL_WEB)
    // check for EXT_disjoint_timer_query_webgl2 extension
    EMSCRIPTEN_WEBGL_CONTEXT_HANDLE ctx = emscripten_webgl_get_current_context();
    if (ctx) {
        bool queryAvailable = emscripten_webgl_enable_extension(ctx, "EXT_disjoint_timer_query_webgl2");
        if (queryAvailable) {
            _mode = Mode::WebDisjointTimerQuery;
            _queryTarget = GL_TIME_ELAPSED_EXT;
        }
        else {
            queryAvailable = emscripten_webgl_enable_extension(ctx, "EXT_disjoint_timer_query");
            if (queryAvailable) {
                _mode = Mode::WebDisjointTimerQueryExt;
                _queryTarget = GL_TIME_ELAPSED_EXT;
            }
            else {
                log::w()("WebGL timer query extensions are not available.");
            }
        }
    }
#elif defined(A3D_GL_ES)
    // Native GLES timer-query extension loading is separate from the WebGL path.
    _mode = Mode::Disabled;
#endif

    if (_mode != Mode::Disabled) {
#if defined(A3D_GL_WEB)
        if (_mode == Mode::WebDisjointTimerQueryExt) {
            glGenQueriesEXT(_bufferSize, _glQueries.data());
        }
        else
#endif
            glGenQueries(_bufferSize, _glQueries.data());
        _issued.assign(_bufferSize, 0);
    }

    _initialized = true;

    return _mode != Mode::Disabled;
}

void OGLDrawTimer::begin() {
    A3D_ASSERT(_initialized);
    if (_mode == Mode::Disabled) {
        return;
    }

    A3D_ASSERT(!_active);
    const size_t write = _frame % _bufferSize;
    if (_issued[(size_t) write]) {
        resolveQuery(write);
        if (_issued[(size_t) write]) {
            return;
        }
    }

#if defined(A3D_GL_WEB)
    if (_mode == Mode::WebDisjointTimerQueryExt) {
        glBeginQueryEXT(_queryTarget, _glQueries[write]);
    }
    else
#endif
        glBeginQuery(_queryTarget, _glQueries[write]);
    _active = true;
}

chrono::nanoseconds OGLDrawTimer::end() {
    A3D_ASSERT(_initialized);
    if (_mode == Mode::Disabled) {
        return _lastTime;
    }

    const size_t write = _frame % _bufferSize;
    if (_active) {
#if defined(A3D_GL_WEB)
        if (_mode == Mode::WebDisjointTimerQueryExt) {
            glEndQueryEXT(_queryTarget);
        }
        else
#endif
            glEndQuery(_queryTarget);
        _issued[write] = 1;
        _active = false;
        resolveIssuedQueries(write);
    }
    else {
        resolveIssuedQueries(_glQueries.size());
    }

    ++_frame;
    return _lastTime;
}

// [Private Member Functions]

bool OGLDrawTimer::resolveQuery(size_t index) {
    if (index >= _issued.size()) {
        return false;
    }
    if (!_issued[index]) {
        return true;
    }

#if defined(A3D_GL_WEB)
    if (_mode == Mode::WebDisjointTimerQuery || _mode == Mode::WebDisjointTimerQueryExt) {
        GLboolean disjoint = GL_FALSE;
        glGetBooleanv(GL_GPU_DISJOINT_EXT, &disjoint);
        if (disjoint) {
            _issued.assign(_bufferSize, 0);
            return false;
        }
    }
#endif

#if defined(A3D_GL_DESKTOP)
    GLint available = 0;
    glGetQueryObjectiv(_glQueries[index], GL_QUERY_RESULT_AVAILABLE, &available);
#elif defined(A3D_GL_WEB)
    GLuint available = 0;
    if (_mode == Mode::WebDisjointTimerQueryExt) {
        glGetQueryObjectuivEXT(_glQueries[index], GL_QUERY_RESULT_AVAILABLE_EXT, &available);
    }
    else {
        glGetQueryObjectuiv(_glQueries[index], GL_QUERY_RESULT_AVAILABLE, &available);
    }
#else
    return false;
#endif

    if (!available) {
        return false;
    }

    GLuint64 ns = 0;
#if defined(A3D_GL_DESKTOP)
    glGetQueryObjectui64v(_glQueries[index], GL_QUERY_RESULT, &ns);
#elif defined(A3D_GL_WEB)
    if (_mode == Mode::WebDisjointTimerQueryExt) {
        glGetQueryObjectui64vEXT(_glQueries[index], GL_QUERY_RESULT_EXT, &ns);
    }
    else {
        glGetQueryObjectui64vEXT(_glQueries[index], GL_QUERY_RESULT, &ns);
    }
#endif

    _lastTime = chrono::nanoseconds {(chrono::nanoseconds::rep) ns};
    _issued[index] = 0;

    return true;
}

void OGLDrawTimer::resolveIssuedQueries(size_t skipIndex) {
    for (size_t i = 0; i < _issued.size(); ++i) {
        if (i == skipIndex) {
            continue;
        }
        resolveQuery(i);
    }
}

namespace {

    // [Private Non-Member Functions]

    unsigned MinBufferSize(unsigned bufferedFrames) {
#if defined(A3D_GL_WEB)
        return math::max(unsigned(8), bufferedFrames);
#else
        return math::max(unsigned(2), bufferedFrames);
#endif
    }

} // namespace
} // namespace a3d
