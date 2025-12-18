#ifndef AVARA3D_OPENGLDRAWTIMER_H
#define AVARA3D_OPENGLDRAWTIMER_H

#include <chrono>
#include <cstddef>
#include <cstdint>
#include <vector>

#include "a3d/Configuration.h"

namespace a3d {

	// double/triple/... buffer GL_TIME_ELAPSED queries
	class OpenGLDrawTimer {

	public:
		//explicit OpenGLDrawTimer(int bufferedFrames = config::GL_DRAW_TIMER_BUFFER_SIZE);
		explicit OpenGLDrawTimer(unsigned bufferedFrames);
		~OpenGLDrawTimer();
		OpenGLDrawTimer(const OpenGLDrawTimer&) = delete;
		OpenGLDrawTimer& operator=(const OpenGLDrawTimer&) = delete;
		OpenGLDrawTimer(OpenGLDrawTimer&& other) = delete;
		OpenGLDrawTimer& operator=(OpenGLDrawTimer&& other) = delete;

		void						initialize();
		void 						begin();
		std::chrono::nanoseconds	end();

	private:
		unsigned					_bufferSize;
		std::vector<unsigned> 		_glQueries;
		size_t 						_frame;
		std::chrono::nanoseconds	_lastTime;
		std::vector<uint8_t> 		_issued;
		bool 						_active = false;
		bool 						_initialized = false;
	};
}

#endif //AVARA3D_OPENGLDRAWTIMER_H
