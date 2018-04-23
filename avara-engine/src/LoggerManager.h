//
//  LoggerManager.h
//	avara-engine
//
//  Created by Morgan Davis on 1/5/18.
//  Copyright © 2018 Morgan K Davis. All rights reserved.
//
//  ****** This class is unused. See Logger.h for a sob story ******
//

#ifndef LoggerManager_h
#define LoggerManager_h


#include <map>
#include <string>

#include <spdlog/spdlog.h>


namespace ae {
	
	
	class Logger;
	
	
	class LoggerManager {
		
	public:
		
		/***************************************************************************************
		     Lifecycle
		 ***************************************************************************************/
		
		LoggerManager();
		
		/***************************************************************************************
		     Internal
		 ***************************************************************************************/
		
		std::shared_ptr<spdlog::logger> addLogger(std::shared_ptr<Logger> logger);
		std::shared_ptr<spdlog::logger> loggerNamed(std::string& name);
		
	private:

		/***************************************************************************************
		     Private
		 ***************************************************************************************/
		
		std::map<std::string, std::shared_ptr<spdlog::logger>>	m_loggers;
		
		std::shared_ptr<spdlog::sinks::rotating_file_sink_mt>	m_mainFileSink;
		std::shared_ptr<spdlog::sinks::stdout_sink_st>			m_stdoutSink;
		
	};
}


#endif /* LoggerManager_h */
