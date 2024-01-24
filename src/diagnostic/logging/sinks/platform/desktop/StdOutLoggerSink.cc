//
// Created by mkd on 11/9/23.
//

#include "diagnostic/logging/sinks/platform/desktop/StdOutLoggerSink.h"

#ifdef WINDOWS
#include <windows.h>
#undef ERROR // see note at LOG_LEVEL
#endif

using namespace ae;
using namespace std;


/*********************************************************************************************
	Lifecycle
 *********************************************************************************************/

StdOutLoggerSink::StdOutLoggerSink() {

}

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

void StdOutLoggerSink::write(const char* message, LOG_LEVEL level) {

	if (static_cast<underlying_type<LOG_LEVEL>::type>(level)
		>= static_cast<underlying_type<LOG_LEVEL>::type>(LOG_LEVEL::ERROR)) {
		fprintf(stderr, "%s\n", message);

	} else {
		fprintf(stdout, "%s\n", message);
	}

#ifdef WINDOWS
	auto newLined = (char*)malloc(strlen(message) + 2);
	//sprintf(newLined, "%s\n", message);
	strcpy(newLined, message);
	strcat(newLined, "\n");
	OutputDebugStringA((const char*)newLined); // this broke with C++20.  trying to include windows.h ^^
	free(newLined);
#endif
}
