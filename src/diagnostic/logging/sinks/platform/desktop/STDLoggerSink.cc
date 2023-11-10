//
// Created by mkd on 11/9/23.
//

#include "diagnostic/logging/sinks/platform/desktop/STDLoggerSink.h"


using namespace ae;
using namespace std;


/*********************************************************************************************
	Lifecycle
 *********************************************************************************************/

STDLoggerSink::STDLoggerSink() {

}

STDLoggerSink::~STDLoggerSink() {

	flush();
}

/*********************************************************************************************
	Public
 *********************************************************************************************/

void STDLoggerSink::flush() {

	fflush(stdout);
	fflush(stderr);
}

/*********************************************************************************************
	Internal
 **************************************************************************************/

void STDLoggerSink::write(const char* message, LOG_LEVEL level) {

	if (static_cast<underlying_type<LOG_LEVEL>::type>(level)
		>= static_cast<underlying_type<LOG_LEVEL>::type>(LOG_LEVEL::ERROR_)) {
		fprintf(stderr, "%s\n", message);

	} else {
		fprintf(stdout, "%s\n", message);
	}

#ifdef WINDOWS
	auto newLined = (char*)malloc(strlen(message) + 2);
	//sprintf(newLined, "%s\n", message);
	strcpy(newLined, message);
	strcat(newLined, "\n");
	OutputDebugStringA((const char*)newLined);
	free(newLined);
#endif
}
