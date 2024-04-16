//
//  AndroidLoggerSink.cc
//	avara3d
//
//  Created by Morgan Davis on 11/9/23.
//  Copyright © 2024 Morgan K Davis. All rights reserved.
//

#ifdef ANDROID


#include "a3d/diagnostic/logging/sink/platform/android/AndroidLoggerSink.h"


using namespace a3d;
using namespace std;


/*********************************************************************************************
	Static Prototypes
 *********************************************************************************************/

unsigned AndroidPriorityFromLogLevel(LOG_LEVEL level);

/*********************************************************************************************
	Lifecycle
 *********************************************************************************************/

AndroidLoggerSink::AndroidLoggerSink() {

}

AndroidLoggerSink::~AndroidLoggerSink() {

	flush();
}

/*********************************************************************************************
	Public
 *********************************************************************************************/

void AndroidLoggerSink::flush() {

	// no flush in Android.
}

/*********************************************************************************************
	Internal
 *********************************************************************************************/

void AndroidLoggerSink::write(const char* message, const char* tag, LOG_LEVEL level) {

	// https://developer.android.com/ndk/reference/group/logging
	__android_log_write(AndroidPriorityFromLogLevel(level), tag, message);
}

/*********************************************************************************************
	Static
 *********************************************************************************************/

unsigned AndroidPriorityFromLogLevel(LOG_LEVEL level) {

	switch (level) {
		case LOG_LEVEL::TRACE: 	return ANDROID_LOG_VERBOSE;
		case LOG_LEVEL::DEBUG: 	return ANDROID_LOG_DEBUG;
		case LOG_LEVEL::INFO: 		return ANDROID_LOG_INFO;
		case LOG_LEVEL::WARN: 		return ANDROID_LOG_WARN;
		case LOG_LEVEL::ERROR: 	return ANDROID_LOG_ERROR;
		case LOG_LEVEL::CRITICAL: 	return ANDROID_LOG_FATAL;
		case LOG_LEVEL::OFF: 		return ANDROID_LOG_SILENT;
	}
}

#endif

