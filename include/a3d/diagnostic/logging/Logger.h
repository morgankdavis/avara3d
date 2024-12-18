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
#include <tuple>
#include <unordered_set>

#include "fmt/format.h"

#include "a3d/Types.h"


#define NOOP ((void)0)

/*********************************************************************************************
	Public Macro Functions
 *********************************************************************************************/

#ifdef A3D_DEBUG
//#define LOG_C(logger) 				logger->crumb(__FILE_NAME__, __LINE__, __FUNCTION__)
#define LOG_T(logger, fmtStr, ...)	logger->trace(__FILE_NAME__, __LINE__, __FUNCTION__, \
										fmt::format(fmtStr, ##__VA_ARGS__).c_str())
#define LOG_D(logger, fmtStr, ...) 	logger->debug(__FILE_NAME__, __LINE__, __FUNCTION__, \
										fmt::format(fmtStr, ##__VA_ARGS__).c_str())
#else
#define LOG_C(logger) NOOP
#define LOG_T(logger, fmtStr, ...) 	NOOP
#define LOG_D(logger, fmtStr, ...) 	NOOP
#endif
#define LOG_I(logger, fmtStr, ...)	logger->info(__FILE_NAME__, __LINE__, __FUNCTION__, \
										fmt::format(fmtStr, ##__VA_ARGS__).c_str())
#define LOG_W(logger, fmtStr, ...)	logger->warn(true, __FILE_NAME__, __LINE__, __FUNCTION__, \
										fmt::format(fmtStr, ##__VA_ARGS__).c_str())
#define LOG_E(logger, fmtStr, ...)	logger->error(__FILE_NAME__, __LINE__, __FUNCTION__, \
										fmt::format(fmtStr, ##__VA_ARGS__).c_str())
#define LOG_F(logger, fmtStr, ...)	logger->fatal(__FILE_NAME__, __LINE__, __FUNCTION__, \
										fmt::format(fmtStr, ##__VA_ARGS__).c_str())

/*********************************************************************************************
	Internal Macro Functions
 *********************************************************************************************/

#ifdef A3D_DEBUG
//#define A3D_LOG_C()				Logger::MainLogger().crumb(__FILE_NAME__, __LINE__, __FUNCTION__)
#define A3D_LOG_T(fmtStr, ...) 	Logger::MainLogger().trace(__FILE_NAME__, __LINE__, __FUNCTION__, \
									fmt::format(fmtStr, ##__VA_ARGS__).c_str())
#define A3D_LOG_D(fmtStr, ...) 	Logger::MainLogger().debug(__FILE_NAME__, __LINE__, __FUNCTION__, \
									fmt::format(fmtStr, ##__VA_ARGS__).c_str())
#else
#define A3D_LOG_C() NOOP
#define A3D_LOG_T(fmtStr, ...) 	NOOP
#define A3D_LOG_D(fmtStr, ...) 	NOOP
#endif
#define A3D_LOG_I(fmtStr, ...) 	Logger::MainLogger().info(__FILE_NAME__, __LINE__, __FUNCTION__, \
									fmt::format(fmtStr, ##__VA_ARGS__).c_str())
#define A3D_LOG_W(fmtStr, ...) 	Logger::MainLogger().warn(__FILE_NAME__, __LINE__, __FUNCTION__, \
									fmt::format(fmtStr, ##__VA_ARGS__).c_str())
#define A3D_LOG_E(fmtStr, ...) 	Logger::MainLogger().error(__FILE_NAME__, __LINE__, __FUNCTION__, \
									fmt::format(fmtStr, ##__VA_ARGS__).c_str())
#define A3D_LOG_F(fmtStr, ...) 	Logger::MainLogger().fatal(__FILE_NAME__, __LINE__, __FUNCTION__, \
									fmt::format(fmtStr, ##__VA_ARGS__).c_str())


namespace a3d {


	class LoggerSink;


	class Logger {

/*********************************************************************************************
	Internal Types
 *********************************************************************************************/

	public:

		/* filename, line, function */
		using SourceInfo = std::tuple<std::string, unsigned, std::string>;


/*********************************************************************************************
	Public Static Member Functions
 *********************************************************************************************/

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



		void 					trace(); //TEMPORARY



		void 					trace(const char* format, ...);
		void 					debug(const char* format, ...);
		void 					info(const char* format, ...);
		void 					warn(const char* format, ...);
		void					error(const char* format, ...);
		void 					fatal(const char* format, ...);

		// replace with log()
		void 					crumb(const char* filename, int line, const char* function);
		void 					trace(const char* filename, int line, const char* function,
									  const char* format, ...);
		void 					debug(const char* filename, int line, const char* function,
									  const char* format, ...);
		void 					info(const char* filename, int line, const char* function,
									 const char* format, ...);
		void 					warn(const char* filename, int line, const char* function,
									 const char* format, ...);
		void					error(const char* filename, int line, const char* function,
									  const char* format, ...);
		void 					fatal(const char* filename, int line, const char* function,
									  const char* format, ...);


		void 					log(LogLevel level,
									const char* format, va_list args);
		void 					log(LogLevel level,
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
