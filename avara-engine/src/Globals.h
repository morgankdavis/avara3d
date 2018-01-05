#ifndef Globals_h
#define Globals_h


#include <memory>

#include <spdlog/spdlog.h>


#define MAX_PATH_LEN  1024

#define TRACE(format, ...)		g_logger->trace(format, __VA_ARGS__)
#define DEBUG(format, ...)		g_logger->debug(format, __VA_ARGS__)
#define INFO(format, ...)		g_logger->info(format, __VA_ARGS__)
#define WARN(format, ...)		g_logger->warn(format, __VA_ARGS__)
#define ERROR(format, ...)		g_logger->error(format, __VA_ARGS__)
#define CRITICAL(format, ...)	g_logger->critical(format, __VA_ARGS__)

extern std::shared_ptr<spdlog::logger>		g_logger;


#endif /* Globals_h */
