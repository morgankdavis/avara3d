//
//  Logger.cc
//	avara-engine
//
//  Created by Morgan Davis on 1/5/18.
//  Copyright © 2018 Morgan K Davis. All rights reserved.
//

#include "Logger.h"

#include <iostream>

//#include <spdlog/spdlog.h>

#include "Global.h"
#include "Utilities.h"


using namespace ae;
using namespace ae::utils;
using namespace spdlog;
using namespace std;


shared_ptr<Logger>		ae::g_logger;


#ifdef ANDROID
std::shared_ptr<spdlog::sinks::android_sink>			i_spdlogAndroidSink;
#else
std::shared_ptr<spdlog::sinks::rotating_file_sink_mt>	i_spdlogMainFileSink;
std::shared_ptr<spdlog::sinks::stdout_sink_st>			i_spdlogSTDOUTSink;
#endif


/**************************************************************************************
     Public Static
 **************************************************************************************/

void Logger::Init() {

	static bool initialized = false;

	if (!initialized) {

		LOGGER_SINK sinks = LOGGER_SINK::NONE;
		
		if (LOG_ENABLE_NATIVE) sinks = LOGGER_SINK_ADD(sinks, LOGGER_SINK::NATIVE);
		
#ifndef ANDROID
		sinks = LOGGER_SINK_ADD(sinks, LOGGER_SINK::MAIN_FILE);
#endif
		g_logger = make_shared<Logger>("ae", sinks);
		g_logger->info("Init.");

		initialized = true;
	}
}

void Logger::Level(LOG_LEVEL level) {
	set_level(static_cast<level::level_enum>(level));
}

/***************************************************************************************
     Lifecycle
 ***************************************************************************************/

Logger::Logger(string name, LOGGER_SINK sinks):
	m_name(name),
	m_sinks(sinks) {

		static bool initialized = false;
		if (!initialized) {
#ifdef ANDROID
			// auto fileDir = InternalFilesDirectory(); // writing logs to file in Android crashes (?)
			i_spdlogAndroidSink = make_shared<sinks::android_sink>("avara-engine", "");
#else
			auto fileDir = ExecutableDirectory();
			if (fileDir) {
				auto logPath = *fileDir / (string(LOG_MAIN_FILE_NAME) + ".log");
				i_spdlogMainFileSink = make_shared<sinks::rotating_file_sink_mt>(logPath.string(),
																				 LOG_FILE_SIZE,
																				 LOG_FILE_ROTATIONS);
				i_spdlogSTDOUTSink = make_shared<sinks::stdout_sink_st>();
			}
			else {
				AE_LOG->warn("Couldn't locate executable directory.");
			}
#endif
			initialized = true;
		}


		try {
			set_async_mode(pow(2, LOG_QUEUE_SIZE)); // queue size must be power of 2
			
			vector<sink_ptr> sinks;
			
			if (LOGGER_SINK_CONTAINS(m_sinks, LOGGER_SINK::NATIVE)) {
#ifdef ANDROID
				sinks.push_back(i_spdlogAndroidSink);
#else
				sinks.push_back(i_spdlogSTDOUTSink);
#endif
			}
#ifndef ANDROID
			if (LOGGER_SINK_CONTAINS(m_sinks, LOGGER_SINK::MAIN_FILE)) {
				sinks.push_back(i_spdlogMainFileSink);
			}
			if (LOGGER_SINK_CONTAINS(m_sinks, LOGGER_SINK::NAMED_FILE)) {
				// not that since we're not saving this, another sink could be created with the same file name!
				sinks.push_back(make_shared<sinks::rotating_file_sink_mt>(name + ".log",
																		  LOG_FILE_SIZE,
																		  LOG_FILE_ROTATIONS));
			}
#endif
			
			auto logger = make_shared<spdlog::logger>(name, begin(sinks), end(sinks));
			
			register_logger(logger);
			
			set_pattern("%Y-%d-%m %H:%M:%S.%e [%n] [%l] %v");
			set_level(static_cast<level::level_enum>(LOG_START_LEVEL));
			logger->flush_on(static_cast<level::level_enum>(LOG_FLUSH_LEVEL));
			
			m_logger = logger;
		}
		catch (const spdlog_ex& ex) {
			
			cout << "Log initialization failed: " << ex.what() << endl;
		}
}

/***************************************************************************************
     Public
 ***************************************************************************************/

string Logger::name() const {
	return m_name;
}

LOGGER_SINK Logger::sinks() const {
	return m_sinks;
}

void Logger::flush() {
	m_logger->flush();
}

