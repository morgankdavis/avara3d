//
//  Logger.h
//  avara3d
//
//  Created by Morgan Davis on 1/5/18.
//  Copyright © 2024 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_LOGGER_H
#define AVARA3D_LOGGER_H


#include <cstdio>
#include <filesystem>
#include <format>
#include <iostream>
#include <memory>
#include <string>
#include <unordered_set>

#include "fmt/format.h"

#include "a3d/Types.h"


#define NOOP ((void)0)

#ifdef A3D_DEBUG
#define A3D_LOG_C() Logger::MainLogger().crumb(__FILE_NAME__, __LINE__, __FUNCTION__)
#define A3D_LOG_T(fmtStr, ...) Logger::MainLogger().trace(true, __FILE_NAME__, __LINE__, __FUNCTION__, fmt::format(fmtStr, ##__VA_ARGS__).c_str())
#define A3D_LOG_D(fmtStr, ...) Logger::MainLogger().debug(true, __FILE_NAME__, __LINE__, __FUNCTION__, fmt::format(fmtStr, ##__VA_ARGS__).c_str())
#else
#define A3D_LOG_C() NOOP
#define A3D_LOG_T(fmtStr, ...) NOOP
#define A3D_LOG_D(fmtStr, ...) NOOP
#endif
#define A3D_LOG_I(fmtStr, ...) Logger::MainLogger().info(true, __FILE_NAME__, __LINE__, __FUNCTION__, fmt::format(fmtStr, ##__VA_ARGS__).c_str())
#define A3D_LOG_W(fmtStr, ...) Logger::MainLogger().warn(true, __FILE_NAME__, __LINE__, __FUNCTION__, fmt::format(fmtStr, ##__VA_ARGS__).c_str())
#define A3D_LOG_E(fmtStr, ...) Logger::MainLogger().error(true, __FILE_NAME__, __LINE__, __FUNCTION__, fmt::format(fmtStr, ##__VA_ARGS__).c_str())
#define A3D_LOG_F(fmtStr, ...) Logger::MainLogger().fatal(true, __FILE_NAME__, __LINE__, __FUNCTION__, fmt::format(fmtStr, ##__VA_ARGS__).c_str())

#ifdef A3D_DEBUG
#define A3D_LOG_H_T(useHeader, fmtStr, ...) Logger::MainLogger().trace(useHeader, __FILE_NAME__, __LINE__, __FUNCTION__, fmt::format(fmtStr, ##__VA_ARGS__).c_str())
#define A3D_LOG_H_D(useHeader, fmtStr, ...) Logger::MainLogger().debug(useHeader, __FILE_NAME__, __LINE__, __FUNCTION__, fmt::format(fmtStr, ##__VA_ARGS__).c_str())
#else
#define A3D_LOG_H_T(useHeader, fmtStr, ...) NOOP
#define A3D_LOG_H_D(useHeader, fmtStr, ...) NOOP
#endif
#define A3D_LOG_H_I(useHeader, fmtStr, ...) Logger::MainLogger().info(useHeader, __FILE_NAME__, __LINE__, __FUNCTION__, fmt::format(fmtStr, ##__VA_ARGS__).c_str())
#define A3D_LOG_H_W(useHeader, fmtStr, ...) Logger::MainLogger().warn(useHeader, __FILE_NAME__, __LINE__, __FUNCTION__, fmt::format(fmtStr, ##__VA_ARGS__).c_str())
#define A3D_LOG_H_E(useHeader, fmtStr, ...) Logger::MainLogger().error(useHeader, __FILE_NAME__, __LINE__, __FUNCTION__, fmt::format(fmtStr, ##__VA_ARGS__).c_str())
#define A3D_LOG_H_F(useHeader, fmtStr, ...) Logger::MainLogger().fatal(useHeader, __FILE_NAME__, __LINE__, __FUNCTION__, fmt::format(fmtStr, ##__VA_ARGS__).c_str())

#ifdef A3D_DEBUG
//#define LOG_T(logger) logger->crumb(__FILE_NAME__, __LINE__, __FUNCTION__)
#define LOG_T(logger, fmtStr, ...) logger->trace(true, __FILE_NAME__, __LINE__, __FUNCTION__, fmt::format(fmtStr, ##__VA_ARGS__).c_str())
#define LOG_D(logger, fmtStr, ...) logger->debug(true, __FILE_NAME__, __LINE__, __FUNCTION__, fmt::format(fmtStr, ##__VA_ARGS__).c_str())
#else
#define LOG_C(logger) NOOP
#define LOG_T(logger, fmtStr, ...) NOOP
#define LOG_D(logger, fmtStr, ...) NOOP
#endif
#define LOG_I(logger, fmtStr, ...) logger->info(true, __FILE_NAME__, __LINE__, __FUNCTION__, fmt::format(fmtStr, ##__VA_ARGS__).c_str())
#define LOG_W(logger, fmtStr, ...) logger->warn(true, __FILE_NAME__, __LINE__, __FUNCTION__, fmt::format(fmtStr, ##__VA_ARGS__).c_str())
#define LOG_E(logger, fmtStr, ...) logger->error(true, __FILE_NAME__, __LINE__, __FUNCTION__, fmt::format(fmtStr, ##__VA_ARGS__).c_str())
#define LOG_F(logger, fmtStr, ...) logger->fatal(true, __FILE_NAME__, __LINE__, __FUNCTION__, fmt::format(fmtStr, ##__VA_ARGS__).c_str())

#ifdef A3D_DEBUG
#define LOG_H_T(useHeader, g_logger, fmtStr, ...) logger->trace(useHeader, __FILE_NAME__, __LINE__, __FUNCTION__, fmt::format(fmtStr, ##__VA_ARGS__).c_str())
#define LOG_H_D(useHeader, g_logger, fmtStr, ...) logger->debug(useHeader, __FILE_NAME__, __LINE__, __FUNCTION__, fmt::format(fmtStr, ##__VA_ARGS__).c_str())
#else
#define LOG_H_T(useHeader, logger, fmtStr, ...) NOOP
#define LOG_H_D(useHeader, logger, fmtStr, ...) NOOP
#endif
#define LOG_H_I(useHeader, g_logger, fmtStr, ...) logger->info(useHeader, __FILE_NAME__, __LINE__, __FUNCTION__, fmt::format(fmtStr, ##__VA_ARGS__).c_str())
#define LOG_H_W(useHeader, g_logger, fmtStr, ...) logger->warn(useHeader, __FILE_NAME__, __LINE__, __FUNCTION__, fmt::format(fmtStr, ##__VA_ARGS__).c_str())
#define LOG_H_E(useHeader, g_logger, fmtStr, ...) logger->error(useHeader, __FILE_NAME__, __LINE__, __FUNCTION__, fmt::format(fmtStr, ##__VA_ARGS__).c_str())
#define LOG_H_F(useHeader, g_logger, fmtStr, ...) logger->fatal(useHeader, __FILE_NAME__, __LINE__, __FUNCTION__, fmt::format(fmtStr, ##__VA_ARGS__).c_str())


namespace a3d {


	class LoggerSink;


	class Logger {

/*********************************************************************************************
	Public Static Member Functions
 *********************************************************************************************/

	public:

		static Logger& MainLogger();

/**************************************************************************************
	Public Lifecycle Functions
 **************************************************************************************/

		Logger(const std::string& name,
			   std::unique_ptr<LoggerSink> sink,
			   LogLevel level = DEFAULT_LEVEL,
			   LogLevel flushLevel = DEFAULT_FLUSH_LEVEL);
		Logger(const std::string& name,
			   std::unordered_set<std::unique_ptr<LoggerSink>> sinks,
			   LogLevel level = DEFAULT_LEVEL,
			   LogLevel flushLevel = DEFAULT_FLUSH_LEVEL);
		~Logger();

/*********************************************************************************************
	Public Member Functions
 *********************************************************************************************/

		const std::string& 		name() const;

		const std::unordered_set<std::unique_ptr<LoggerSink>>&	sinks() const;

		LogLevel 				level() const;
		void					level(LogLevel level);

		LogLevel 				flushLevel() const;
		void 					flushLevel(LogLevel level);

		void 					trace(const char* format, ...);
		void 					debug(const char* format, ...);
		void 					info(const char* format, ...);
		void 					warn(const char* format, ...);
		void					error(const char* format, ...);
		void 					fatal(const char* format, ...);

		void 					crumb(const char* filename, int line, const char* function);
		void 					trace(bool useHeader,
									  const char* filename, int line, const char* function,
									  const char* format, ...);
		void 					debug(bool useHeader,
									  const char* filename, int line, const char* function,
									  const char* format, ...);
		void 					info(bool useHeader,
									 const char* filename, int line, const char* function,
									 const char* format, ...);
		void 					warn(bool useHeader,
									 const char* filename, int line, const char* function,
									 const char* format, ...);
		void					error(bool useHeader,
									  const char* filename, int line, const char* function,
									  const char* format, ...);
		void 					fatal(bool useHeader,
									  const char* filename, int line, const char* function,
									  const char* format, ...);

		void 					log(LogLevel level,
									const char* format, va_list args);
		void 					log(LogLevel level,
									bool useHeader,
									const char* filename, int line, const char* function,
									const char* format, va_list args);
		void 					log_crumb(const char* filename, int line, const char* function);

		void					construct(LogLevel level, const char* message);
		void 					construct(LogLevel level,
										  const char* filename, int line, const char* function,
										  const char* body);

		void 					dispatch(LogLevel level, const char* line);

		void 					flush();

/*********************************************************************************************
	Private Constants
 *********************************************************************************************/

	private:

		static constexpr LogLevel DEFAULT_LEVEL = LogLevel::Debug;
		static constexpr LogLevel DEFAULT_FLUSH_LEVEL = LogLevel::Warn;

/*********************************************************************************************
	Private Member Variables
 *********************************************************************************************/

		std::string										_name;
		std::unordered_set<std::unique_ptr<LoggerSink>>	_sinks;
		LogLevel										_level;
		LogLevel										_flushLevel;
	};
}


#endif /* AVARA3D_LOGGER_H */
