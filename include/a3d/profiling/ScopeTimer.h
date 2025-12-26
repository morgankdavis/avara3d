
#ifndef AVARA3D_SCOPETIMER_H
#define AVARA3D_SCOPETIMER_H

#include <chrono>

#include "a3d/profiling/Profiler.h"

namespace a3d {

	class Profiler;

	class ScopeTimer {

	public:
		/// Public Lifecycle Functions ///

		ScopeTimer(Profiler& profiler, Profiler::Tag tag);
		~ScopeTimer();

	private:
		/// Private Member Variables ///

		Profiler*											_profiler;
		Profiler::Tag 										_tag;
		std::chrono::time_point<std::chrono::steady_clock> 	_start;
	};
}

#endif //AVARA3D_SCOPETIMER_H
