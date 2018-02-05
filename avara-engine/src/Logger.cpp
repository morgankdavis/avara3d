//
//  Logger.cpp
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
//#include "LoggerManager.h"


using namespace ae;
using namespace ae::utils;
using namespace spdlog;
using namespace std;


std::shared_ptr<spdlog::sinks::rotating_file_sink_mt>	i_spdlogMainFileSink;
std::shared_ptr<spdlog::sinks::stdout_sink_st>			i_spdlogSTDOUTSink;


/***************************************************************************************
     MARK:   Lifecycle
 **************************************************************************************/

Logger::Logger(string name, LoggerSink sinks):
	m_name(name),
	m_sinks(sinks) {
		
		//initLog(); // THIS IS A HACK. THIS WHOLE LOG THING IS A HACK!!!
		
		static bool initialized = false;
		if (!initialized) {
			
			auto execDir = ExecutableDirectory();
			if (execDir) {
				auto logPath = *execDir / (string(LOG_MAIN_FILE_NAME) + ".log");
				i_spdlogMainFileSink = make_shared<sinks::rotating_file_sink_mt>(logPath.string().c_str(),
																				 LOG_FILE_SIZE,
																				 LOG_FILE_ROTATIONS);
				i_spdlogSTDOUTSink = make_shared<sinks::stdout_sink_st>();
			}
			else {
				AE_LOG->warn("Couldn't locate executable directory.");
			}
			
			initialized = true;
		}
		

		try {
			set_async_mode(pow(2, LOG_QUEUE_SIZE)); // queue size must be power of 2
			
			vector<sink_ptr> sinks;
			
			if (m_sinks & LoggerSink_STDOUT) {
				sinks.push_back(i_spdlogSTDOUTSink);
			}
			if (m_sinks & LoggerSink_MainFile) {
				sinks.push_back(i_spdlogMainFileSink);
			}
			if (m_sinks & LoggerSink_NamedFile) {
				// not that since we're not saving this, another sink could be created with the same file name!
				sinks.push_back(make_shared<sinks::rotating_file_sink_mt>(name + ".log",
																		  LOG_FILE_SIZE,
																		  LOG_FILE_ROTATIONS));
			}
			
			auto logger = make_shared<spdlog::logger>(name, begin(sinks), end(sinks));
			
			register_logger(logger);
			
			set_pattern("%Y-%d-%m %H:%M:%S.%e [%n] [%l] %v");
			set_level(LOG_LEVEL);
			logger->flush_on(LOG_FLUSH_LEVEL);
			
			m_logger = logger;
		}
		catch (const spdlog_ex& ex) {
			
			cout << "Log initialization failed: " << ex.what() << endl;
		}
}

/***************************************************************************************
     MARK:   Public
 **************************************************************************************/

string Logger::name() const {
	return m_name;
}

LoggerSink Logger::sinks() const {
	return m_sinks;
}

void Logger::flush() {
	m_logger->flush();
}
