//
//  Timer.cc
//  avara3d
//
//  Created by Morgan Davis on 12/6/25.
//  Copyright © 2024 Morgan K Davis. All rights reserved.
//

#include "a3d/profile/Timer.h"

using namespace a3d;
using namespace std;

/// Public Member Functions ///

Timer::Timer(bool start):
		_start{},
		_stop{},
		_duration{} {

	if (start) this->start();
}

void Timer::start() {
	_start = chrono::steady_clock::now();
}

chrono::nanoseconds Timer::stop() {
	_stop = chrono::steady_clock::now();
	_duration = chrono::duration_cast<chrono::nanoseconds>(_stop - _start);
	return _duration;
}

chrono::nanoseconds Timer::duration() const {
	return _duration;
}
