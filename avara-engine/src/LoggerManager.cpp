//
//  LoggerManager.cpp
//	avara-engine
//
//  Created by Morgan Davis on 1/5/18.
//  Copyright © 2018 Morgan K Davis. All rights reserved.
//

#include "LoggerManager.h"

#include <iostream>

#include "Logger.h"
#include "Global.h"


using namespace ae;
using namespace spdlog;
using namespace std;


/***************************************************************************************
     Lifecycle
 ***************************************************************************************/

LoggerManager::LoggerManager() {
	
	m_loggers = map<string, shared_ptr<spdlog::logger>>();
			  
#ifdef ANDROID
	m_androidSink = make_shared<sinks::android_sink>("avara-engine", "");
#else
	m_mainFileSink = make_shared<sinks::rotating_file_sink_mt>(string(LOG_MAIN_FILE_NAME) + ".log",
															   LOG_FILE_SIZE,
															   LOG_FILE_ROTATIONS);
	m_stdoutSink = make_shared<sinks::stdout_sink_st>();
#endif
}

/***************************************************************************************
     Internal
 ***************************************************************************************/

std::shared_ptr<spdlog::logger> LoggerManager::addLogger(shared_ptr<Logger> logger) {
	
	// first check loggerNamed()
	
	try {
		set_async_mode(pow(2, LOG_QUEUE_SIZE)); // queue size must be power of 2
		
		vector<sink_ptr> sinks;
		
		LOGGER_SINKS sinksBitmask = logger->sinks();
		if (LOGGER_SINKS_CONTAINS(sinksBitmask, LOGGER_SINKS::NATIVE)) {
#ifdef ANDROID
			sinks.push_back(m_androidSink);
#else
			sinks.push_back(m_stdoutSink);
#endif
		}
#ifndef ANDROID
		if (LOGGER_SINKS_CONTAINS(sinksBitmask, LOGGER_SINKS::MAIN_FILE)) {
			sinks.push_back(m_mainFileSink);
		}
		if (LOGGER_SINKS_CONTAINS(sinksBitmask, LOGGER_SINKS::NAMED_FILE)) {
			// not that since we're not saving this, another sink could be created with the same file name!
			sinks.push_back(make_shared<sinks::rotating_file_sink_mt>(logger->name() + ".log",
																	  LOG_FILE_SIZE,
																	  LOG_FILE_ROTATIONS));
		}
#endif
		
		auto newLogger = make_shared<spdlog::logger>(logger->name(), begin(sinks), end(sinks));
		
		register_logger(newLogger);
		
		set_pattern("[%Y-%d-%m %H:%M:%S.%e] [%n] [%l]\t%v");
		set_level(static_cast<level::level_enum>(LOG_START_LEVEL));
		newLogger->flush_on(static_cast<level::level_enum>(LOG_FLUSH_LEVEL));

		m_loggers[logger->name()] = newLogger;
		
		return newLogger;
	}
	catch (const spdlog_ex& ex) {
		
		cout << "Log initialization failed: " << ex.what() << endl;
	}
	
	return nullptr;
}

shared_ptr<spdlog::logger> LoggerManager::loggerNamed(string& name) {
	if (m_loggers.find(name) == m_loggers.end() ) {
		return nullptr;
	}
	else {
		return m_loggers[name];
	}
}

