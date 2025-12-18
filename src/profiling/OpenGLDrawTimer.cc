
#include "a3d/profiling/OpenGLDrawTimer.h"

#include <cassert>

#ifdef A3D_GL_ES
#include <EGL/egl.h>
#include <GLES3/gl3.h>
#else
#include "glad/glad.h"
#endif

#include "a3d/Math.h"

using namespace a3d;
using namespace std;

OpenGLDrawTimer::OpenGLDrawTimer(int bufferedFrames):
		_size(math::max(2, bufferedFrames)),
		_glQueries(static_cast<size_t>(_size), 0),
		_frame{0},
		_lastTime{chrono::nanoseconds{static_cast<chrono::nanoseconds::rep>(0)}} { }

OpenGLDrawTimer::~OpenGLDrawTimer() {
	// ! requires a current GL context !
	if (!_glQueries.empty()) {
		glDeleteQueries(_size, _glQueries.data());
	}
}

void OpenGLDrawTimer::initialize() {
	glGenQueries(_size, _glQueries.data());
	_issued.assign(_size, 0);
	_initialized = true;
}

void OpenGLDrawTimer::begin() {
	assert(_initialized);
	assert(!_active);
	const int write = _frame % _size;
	glBeginQuery(GL_TIME_ELAPSED, _glQueries[(size_t)write]);
	_active = true;
}

chrono::nanoseconds OpenGLDrawTimer::end() {
	assert(_initialized && _active);
	glEndQuery(GL_TIME_ELAPSED);
	const int write = _frame % _size;
	_issued[(size_t)write] = 1;
	_active = false;

	//if (!_initialized) return _lastTime;

	const int read = (_frame + 1) % _size; // N-1 frames old slot
	if (_issued[(size_t)read]) {
		GLint available = 0;
		glGetQueryObjectiv(_glQueries[(size_t)read], GL_QUERY_RESULT_AVAILABLE, &available);
		if (available) {
			GLuint64 ns = 0;
			glGetQueryObjectui64v(_glQueries[(size_t)read], GL_QUERY_RESULT, &ns);
			_lastTime = chrono::nanoseconds{ (chrono::nanoseconds::rep)ns };
		}
	}

	++_frame;
	return _lastTime;
}

//chrono::nanoseconds OpenGLDrawTimer::resolve() {
//	if (!_initialized) return _lastTime;
//
//	const int read = (_frame + 1) % _size; // N-1 frames old slot
//	if (_issued[(size_t)read]) {
//		GLint available = 0;
//		glGetQueryObjectiv(_glQueries[(size_t)read], GL_QUERY_RESULT_AVAILABLE, &available);
//		if (available) {
//			GLuint64 ns = 0;
//			glGetQueryObjectui64v(_glQueries[(size_t)read], GL_QUERY_RESULT, &ns);
//			_lastTime = chrono::nanoseconds{ (chrono::nanoseconds::rep)ns };
//		}
//	}
//
//	++_frame;
//	return _lastTime;
//}

//chrono::nanoseconds OpenGLDrawTimer::lastTime() const {
//	return _lastTime;
//}
