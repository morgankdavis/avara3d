
#include "a3d/profiling/timer/Timer.h"

using namespace a3d;

Timer::Timer(bool start) {
	if (start) this->start();
}

void Timer::start() {
	_start = std::chrono::steady_clock::now();
}

std::chrono::nanoseconds Timer::stop() {
	_stop = std::chrono::steady_clock::now();
	_duration = std::chrono::duration_cast<std::chrono::microseconds>(_stop - _start);
	return _duration;
}

std::chrono::nanoseconds Timer::duration() const {
	return _duration;
}
