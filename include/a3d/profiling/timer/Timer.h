
#ifndef AVARA3D_TIMER_H
#define AVARA3D_TIMER_H

#include <chrono>

namespace a3d {

	class Timer {

	public:

		explicit Timer(bool start = false);
//		~Timer();

		void start();
		std::chrono::nanoseconds stop();
		std::chrono::nanoseconds duration() const;

	private:

//		bool _started;
//		bool _stopped;
		std::chrono::time_point<std::chrono::steady_clock> _start;
		std::chrono::time_point<std::chrono::steady_clock> _stop;
		std::chrono::nanoseconds _duration;
//		std::time_t _stopTime;
	};
}

#endif //AVARA3D_TIMER_H




//struct CpuScopeTimer {
//	const char* name;
//	std::chrono::time_point<std::chrono::system_clock> start;
//
//	CpuScopeTimer(const char* n)
//			: name(n), start(Now()) {}
//
//	~CpuScopeTimer() {
//		auto end = Now();
//		double ms = ElapsedMs(start, end);
//		a3d::Profiler::AddCpuSample(name, ms); // your profiler storage
//	}
//};