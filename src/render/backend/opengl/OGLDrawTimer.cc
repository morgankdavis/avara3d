//
//  OpenGLDrawTimer.cc
//  avara3d
//
//  Created by Morgan Davis on 12/24/25.
//  Copyright © 2024 Morgan K Davis. All rights reserved.
//

#include "a3d/render/backend/opengl/OGLDrawTimer.h"

#include "a3d/render/backend/opengl/gl.h"

#include "a3d/Assert.h"
#include "a3d/Math.h"

using namespace a3d;
using namespace std;

OGLDrawTimer::OGLDrawTimer(unsigned bufferedFrames):
		_bufferSize(math::max(unsigned(2), bufferedFrames)),
		_glQueries(static_cast<size_t>(_bufferSize), 0),
		_frame{0},
		_lastTime{chrono::nanoseconds{static_cast<chrono::nanoseconds::rep>(0)}} { }

OGLDrawTimer::~OGLDrawTimer() {
	// ! requires a current GL context !
	if (!_glQueries.empty()) {
		glDeleteQueries(_bufferSize, _glQueries.data());
	}
}

void OGLDrawTimer::initialize() {
	glGenQueries(_bufferSize, _glQueries.data());
	_issued.assign(_bufferSize, 0);
	_initialized = true;
}

void OGLDrawTimer::begin() {
	A3D_ASSERT(_initialized);
	A3D_ASSERT(!_active);
	const int write = _frame % _bufferSize;
	glBeginQuery(GL_TIME_ELAPSED, _glQueries[(size_t)write]);
	_active = true;
}

chrono::nanoseconds OGLDrawTimer::end() {
	A3D_ASSERT(_initialized && _active);
	glEndQuery(GL_TIME_ELAPSED);
	const int write = _frame % _bufferSize;
	_issued[(size_t)write] = 1;
	_active = false;

	//if (!_initialized) return _lastTime;

	const int read = (_frame + 1) % _bufferSize; // N-1 frames old slot
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
