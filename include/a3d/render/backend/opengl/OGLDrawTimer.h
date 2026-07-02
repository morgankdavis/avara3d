//
//  OpenGLDrawTimer.h
//  avara3d
//
//  Created by Morgan Davis on 12/24/25.
//  Copyright © 2024 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_PROFILING_OPENGLDRAWTIMER_H
#define AVARA3D_PROFILING_OPENGLDRAWTIMER_H

#include <chrono>
#include <cstddef>
#include <cstdint>
#include <vector>

#include "a3d/Configuration.h"
#include "a3d/render/backend/opengl/GLTypes.h"

namespace a3d {

	// double/triple/... buffer GL_TIME_ELAPSED queries
	class OGLDrawTimer {

	public:

		//explicit OpenGLDrawTimer(int bufferedFrames = config::GL_DRAW_TIMER_BUFFER_SIZE);
		explicit OGLDrawTimer(unsigned bufferedFrames);
		~OGLDrawTimer();
		OGLDrawTimer(const OGLDrawTimer&) = delete;
		OGLDrawTimer& operator=(const OGLDrawTimer&) = delete;
		OGLDrawTimer(OGLDrawTimer&& other) = delete;
		OGLDrawTimer& operator=(OGLDrawTimer&& other) = delete;

		void						initialize();
		bool						isAvailable() const; // always returns false before calling initialize()
		void 						begin();
		std::chrono::nanoseconds	end();

	private:

		enum class Mode {
			Disabled,
			DesktopTimeElapsed,
			WebDisjointTimerQuery,
			WebDisjointTimerQueryExt
		};

		bool						resolveQuery(size_t index);
		void						resolveIssuedQueries(size_t skipIndex);

		bool						_isAvailable;
		Mode						_mode;
		gl::enum_t					_queryTarget;
		unsigned					_bufferSize;
		std::vector<unsigned> 		_glQueries;
		size_t 						_frame;
		std::chrono::nanoseconds	_lastTime;
		std::vector<uint8_t> 		_issued;
		bool 						_active = false;
		bool 						_initialized = false;
	};
}

#endif //AVARA3D_PROFILING_OPENGLDRAWTIMER_H
