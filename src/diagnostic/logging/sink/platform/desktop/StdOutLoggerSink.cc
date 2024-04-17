//
//  StdOutLoggerSink.cc
//  avara3d
//
//  Created by Morgan Davis on 111/9/23.
//  Copyright © 2024 Morgan K Davis. All rights reserved.
//

#include "a3d/diagnostic/logging/sink/StdOutLoggerSink.h"

#ifdef WINDOWS
#include <windows.h>
#undef ERROR // see note at LOG_LEVEL
#endif

using namespace a3d;
using namespace std;


/*********************************************************************************************
	Lifecycle
 *********************************************************************************************/

StdOutLoggerSink::StdOutLoggerSink() { }

StdOutLoggerSink::~StdOutLoggerSink() {
	flush();
}

/*********************************************************************************************
	Public
 *********************************************************************************************/

void StdOutLoggerSink::flush() {

	fflush(stdout);
	fflush(stderr);
}

/*********************************************************************************************
	Internal
 **************************************************************************************/

void StdOutLoggerSink::write(const char* message, LogLevel level) {

	if (static_cast<underlying_type<LogLevel>::type>(level)
		>= static_cast<underlying_type<LogLevel>::type>(LogLevel::Error)) {
		fprintf(stderr, "%s\n", message);

	}
	else {
		fprintf(stdout, "%s\n", message);
	}

#ifdef WINDOWS
	const size_t bufSize = strlen(message) + 2;
	auto newLined = (char*)malloc(bufSize);
	//snprintf(newLined, bufSize, "%s\n", message);
	strcpy(newLined, message);
	strcat(newLined, "\n");
	OutputDebugStringA((const char*)newLined); // this broke with C++20.  trying to include windows.h ^^
	free(newLined);
#endif
}
