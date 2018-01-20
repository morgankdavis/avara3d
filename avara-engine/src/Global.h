//
//  Global.h
//	avara-engine
//
//  Created by Morgan Davis on 1/5/18.
//  Copyright © 2018 Morgan K Davis. All rights reserved.
//

#ifndef Global_h
#define Global_h


#include <memory>

//#include <GL/glew.h>
#include <spdlog/spdlog.h>


#define MAX_DYNAMIC_LIGHTS	8

#define LOG_MAIN_FILE_NAME	"ae"
#define LOG_QUEUE_SIZE	    12
#define LOG_FILE_SIZE 		1024 * 1024 * 5
#define LOG_FILE_ROTATIONS	3
#define LOG_ENABLE_STDOUT	true
#define LOG_LEVEL			level::debug
#define LOG_FLUSH_LEVEL		level::warn

#define MAX_PATH_LEN  		1024


namespace ae {
	
	
//	class Logger;
//	class LoggerManager;
	
	
//	extern std::shared_ptr<LoggerManager>	g_loggerManager;

	
//	extern std::shared_ptr<spdlog::logger> 	g_logger; // TEMPORARY
	
	
//	extern std::shared_ptr<spdlog::sinks::rotating_file_sink_mt>	g_spdlogMainFileSink;
//	extern std::shared_ptr<spdlog::sinks::stdout_sink_st>			g_spdlogSTDOUTSink;

	
	

	extern int 		g_glfwLastErrorCode;
	extern char* 	g_glfwLastErrorDescription;

	
	
	
	
	

	// gross. wish we knew how to properly overload these...
//	#define AE_LOG->trace(msg)				g_logger->trace(msg)
//	#define AE_LOG->trace_F(fmt, ...)		g_logger->trace(fmt, __VA_ARGS__)
//	#define AE_LOG->debug(msg)				g_logger->debug(msg)
//	#define AE_LOG->debug(fmt, ...)		g_logger->debug(fmt, __VA_ARGS__)
//	#define AE_LOG->info(msg)				g_logger->info(msg)
//	#define AE_LOG->info(fmt, ...)		g_logger->info(fmt, __VA_ARGS__)
//	#define AE_LOG->warn(msg)				g_logger->warn(msg)
//	#define AE_LOG->warn(fmt, ...)		g_logger->warn(fmt, __VA_ARGS__)
//	#define AE_LOG->error(msg)				g_logger->error(msg)
//	#define AE_LOG->error(fmt, ...)		g_logger->error(fmt, __VA_ARGS__)
//	#define AE_LOG->critical(msg)			g_logger->critical(msg)
//	#define AE_LOG->critical(fmt, ...)	g_logger->critical(fmt, __VA_ARGS__)


	int initLog();
	int initGLFW();
	int initGLEW();
}

#endif /* Global_h */
