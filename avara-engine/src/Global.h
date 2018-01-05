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

#include <spdlog/spdlog.h>




#define LOG_FILE_NAME	    "ae.log"
#define LOG_QUEUE_SIZE	    12
#define LOG_FILE_SIZE 		1024 * 1024 * 5
#define LOG_FILE_ROTATIONS	3
#define LOG_ENABLE_STDOUT	true
#define LOG_LEVEL			level::debug
#define LOG_FLUSH_LEVEL		level::warn




#define MAX_PATH_LEN  1024

namespace ae {

extern std::shared_ptr<spdlog::logger> g_logger;

	extern int 		g_glfwLastErrorCode;
	extern char* 	g_glfwLastErrorDescription;


	// gross. wish we knew how to properly overload these...
	#define TRACE(msg)				g_logger->trace(msg)
	#define TRACE_F(fmt, ...)		g_logger->trace(fmt, __VA_ARGS__)
	#define DEBUG(msg)				g_logger->debug(msg)
	#define DEBUG_F(fmt, ...)		g_logger->debug(fmt, __VA_ARGS__)
	#define INFO(msg)				g_logger->info(msg)
	#define INFO_F(fmt, ...)		g_logger->info(fmt, __VA_ARGS__)
	#define WARN(msg)				g_logger->warn(msg)
	#define WARN_F(fmt, ...)		g_logger->warn(fmt, __VA_ARGS__)
	#define ERROR(msg)				g_logger->error(msg)
	#define ERROR_F(fmt, ...)		g_logger->error(fmt, __VA_ARGS__)
	#define CRITICAL(msg)			g_logger->critical(msg)
	#define CRITICAL_F(fmt, ...)	g_logger->critical(fmt, __VA_ARGS__)


	int initLog();
	int initGLFW();
	int initGLEW();
}

#endif /* Global_h */
