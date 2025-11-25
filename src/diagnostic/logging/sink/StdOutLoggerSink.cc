//
//  StdOutLoggerSink.cc
//  avara3d
//
//  Created by Morgan Davis on 111/9/23.
//  Copyright © 2024 Morgan K Davis. All rights reserved.
//

#include "a3d/diagnostic/logging/sink/StdOutLoggerSink.h"

#include <iostream>

#ifdef WINDOWS
#include <windows.h>
#undef ERROR // see note at LOG_LEVEL
#endif

using namespace a3d;
using namespace std;


/*********************************************************************************************
	Public Static Member Functions
 *********************************************************************************************/

bool StdOutLoggerSink::classof(const LoggerSink* o) {
	return o && o->kind() == Kind::StdOut;
}

bool StdOutLoggerSink::classof(const LoggerSink& o) {
	return classof(&o);
}

/*********************************************************************************************
	Public Lifecycle Functions
 *********************************************************************************************/

StdOutLoggerSink::StdOutLoggerSink():
		LoggerSink(Kind::StdOut) {}

StdOutLoggerSink::~StdOutLoggerSink() {
	flush();
}

/*********************************************************************************************
	Public Member Functions
 *********************************************************************************************/

void StdOutLoggerSink::flush() {

	// also flushes cout
	// https://stackoverflow.com/questions/6027034/why-cerr-flushes-the-buffer-of-cout
	cerr.flush();
}

/*********************************************************************************************
	Internal Member Functions
 **************************************************************************************/

void StdOutLoggerSink::write(const string& output, LogLevel level) {

	if (static_cast<underlying_type<LogLevel>::type>(level)
		>= static_cast<underlying_type<LogLevel>::type>(LogLevel::Error)) {
		cerr << output;
	}
	else {
		cout << output;
	}

#ifdef WINDOWS
	OutputDebugStringA((const char*)output.c_str()); // this broke with C++20.  trying to include windows.h ^^
#endif
}
