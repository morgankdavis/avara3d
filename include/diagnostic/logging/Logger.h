//
//  Logger.h
//	avara-engine
//
//  Created by Morgan Davis on 1/5/18.
//  Copyright © 2018 Morgan K Davis. All rights reserved.
//
//	I hate this class.
//
//	The template functions need to be visible in the header to generate their variants.
//	This means as long as we want to use template log functions (uh, yes)
//	we have to expose spdlog headers to clients.
//
//	The only solution is to implement the log ourselves.
//

#ifndef Logger_h
#define Logger_h


#include <cstdio>
#include <filesystem>
#include <format>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

#define FMT_HEADER_ONLY
#include "fmt/format.h"

#include "Types.h"
#include "diagnostic/Exception.h"

// https://gcc.gnu.org/onlinedocs/cpp/Variadic-Macros.html

// __PRETTY_FUNCTION__ gives something like:
// "virtual void Renderer::updateTimeStats(Stats &, float)"
// __FUNCTION_NAME__: https://stackoverflow.com/questions/15305310/predefined-macros-for-function-name-func
//#define AE_LOG_T() Logger::MainLogger()->trace(__FILE_NAME__, __LINE__, __FUNCTION__)
//#define AE_LOG_T() Logger::MainLogger()->trace(__FILE_NAME__, __LINE__, __FUNCTION__)
#define AE_LOG_T() Logger::MainLogger()->crumb(__FILE_NAME__, __LINE__, __FUNCTION__)
#define AE_LOG_T(fmtStr, ...) Logger::MainLogger()->trace(true, __FILE_NAME__, __LINE__, __FUNCTION__, std::format(fmtStr, ##__VA_ARGS__).c_str())
#define AE_LOG_D(fmtStr, ...) Logger::MainLogger()->debug(true, __FILE_NAME__, __LINE__, __FUNCTION__, fmt::format(fmtStr, ##__VA_ARGS__).c_str())
#define AE_LOG_I(fmtStr, ...) Logger::MainLogger()->info(true, __FILE_NAME__, __LINE__, __FUNCTION__, fmt::format(fmtStr, ##__VA_ARGS__).c_str())
#define AE_LOG_W(fmtStr, ...) Logger::MainLogger()->warn(true, __FILE_NAME__, __LINE__, __FUNCTION__, fmt::format(fmtStr, ##__VA_ARGS__).c_str())
#define AE_LOG_E(fmtStr, ...) Logger::MainLogger()->error(true, __FILE_NAME__, __LINE__, __FUNCTION__, fmt::format(fmtStr, ##__VA_ARGS__).c_str())
#define AE_LOG_C(fmtStr, ...) Logger::MainLogger()->critical(true, __FILE_NAME__, __LINE__, __FUNCTION__, fmt::format(fmtStr, ##__VA_ARGS__).c_str())

#define AE_LOG_H_T(useHeader, fmtStr, ...) Logger::MainLogger()->trace(useHeader, __FILE_NAME__, __LINE__, __FUNCTION__, fmt::format(fmtStr, ##__VA_ARGS__).c_str())
#define AE_LOG_H_D(useHeader, fmtStr, ...) Logger::MainLogger()->debug(useHeader, __FILE_NAME__, __LINE__, __FUNCTION__, fmt::format(fmtStr, ##__VA_ARGS__).c_str())
#define AE_LOG_H_I(useHeader, fmtStr, ...) Logger::MainLogger()->info(useHeader, __FILE_NAME__, __LINE__, __FUNCTION__, fmt::format(fmtStr, ##__VA_ARGS__).c_str())
#define AE_LOG_H_W(useHeader, fmtStr, ...) Logger::MainLogger()->warn(useHeader, __FILE_NAME__, __LINE__, __FUNCTION__, fmt::format(fmtStr, ##__VA_ARGS__).c_str())
#define AE_LOG_H_E(useHeader, fmtStr, ...) Logger::MainLogger()->error(useHeader, __FILE_NAME__, __LINE__, __FUNCTION__, fmt::format(fmtStr, ##__VA_ARGS__).c_str())
#define AE_LOG_H_C(useHeader, fmtStr, ...) Logger::MainLogger()->critical(useHeader, __FILE_NAME__, __LINE__, __FUNCTION__, fmt::format(fmtStr, ##__VA_ARGS__).c_str())

#define LOG_CRUMB(logger) logger->crumb(__FILE_NAME__, __LINE__, __FUNCTION__)
#define LOG_T(logger, fmtStr, ...) logger->trace(true, __FILE_NAME__, __LINE__, __FUNCTION__, fmt::format(fmtStr, ##__VA_ARGS__).c_str())
#define LOG_D(logger, fmtStr, ...) logger->debug(true, __FILE_NAME__, __LINE__, __FUNCTION__, fmt::format(fmtStr, ##__VA_ARGS__).c_str())
#define LOG_I(logger, fmtStr, ...) logger->info(true, __FILE_NAME__, __LINE__, __FUNCTION__, fmt::format(fmtStr, ##__VA_ARGS__).c_str())
#define LOG_W(logger, fmtStr, ...) logger->warn(true, __FILE_NAME__, __LINE__, __FUNCTION__, fmt::format(fmtStr, ##__VA_ARGS__).c_str())
#define LOG_E(logger, fmtStr, ...) logger->error(true, __FILE_NAME__, __LINE__, __FUNCTION__, fmt::format(fmtStr, ##__VA_ARGS__).c_str())
#define LOG_C(logger, fmtStr, ...) logger->critical(true, __FILE_NAME__, __LINE__, __FUNCTION__, fmt::format(fmtStr, ##__VA_ARGS__).c_str())

#define LOG_H_T(useHeader, logger, fmtStr, ...) logger->trace(useHeader, __FILE_NAME__, __LINE__, __FUNCTION__, fmt::format(fmtStr, ##__VA_ARGS__).c_str())
#define LOG_H_D(useHeader, logger, fmtStr, ...) logger->debug(useHeader, __FILE_NAME__, __LINE__, __FUNCTION__, fmt::format(fmtStr, ##__VA_ARGS__).c_str())
#define LOG_H_I(useHeader, logger, fmtStr, ...) logger->info(useHeader, __FILE_NAME__, __LINE__, __FUNCTION__, fmt::format(fmtStr, ##__VA_ARGS__).c_str())
#define LOG_H_W(useHeader, logger, fmtStr, ...) logger->warn(useHeader, __FILE_NAME__, __LINE__, __FUNCTION__, fmt::format(fmtStr, ##__VA_ARGS__).c_str())
#define LOG_H_E(useHeader, logger, fmtStr, ...) logger->error(useHeader, __FILE_NAME__, __LINE__, __FUNCTION__, fmt::format(fmtStr, ##__VA_ARGS__).c_str())
#define LOG_H_C(useHeader, logger, fmtStr, ...) logger->critical(useHeader, __FILE_NAME__, __LINE__, __FUNCTION__, fmt::format(fmtStr, ##__VA_ARGS__).c_str())


namespace ae {

	
	class LoggerSink;


	class Logger {
		
		
		//static constexpr unsigned char DEFAULT_NAME[] = "ae";
		static constexpr LOG_LEVEL DEFAULT_LEVEL = LOG_LEVEL::DEBUG;
		static constexpr LOG_LEVEL DEFAULT_FLUSH_LEVEL = LOG_LEVEL::WARN_;
		
/**************************************************************************************
	Public Static
 **************************************************************************************/

	public:

		static std::shared_ptr<Logger> MainLogger();
		
/*********************************************************************************************
	Lifecycle
 *********************************************************************************************/
		
		Logger(std::string name, std::shared_ptr<LoggerSink> sink,
			   LOG_LEVEL level = DEFAULT_LEVEL, LOG_LEVEL flushLevel = DEFAULT_FLUSH_LEVEL);
		Logger(std::string name, std::vector<std::shared_ptr<LoggerSink>> sinks,
			   LOG_LEVEL level = DEFAULT_LEVEL, LOG_LEVEL flushLevel = DEFAULT_FLUSH_LEVEL);
		~Logger();
		
/*********************************************************************************************
	Public
 *********************************************************************************************/
		
		std::string name() const;
		std::vector<std::shared_ptr<LoggerSink>> sinks() const;
		
		LOG_LEVEL level() const;
		void level(LOG_LEVEL level);
		
		LOG_LEVEL flushLevel() const;
		void flushLevel(LOG_LEVEL level);
		
		// * favor using AE_LOG_ and LOG_ macros for fancy formatting *

		void trace(const char* format, ...);
		void debug(const char* format, ...);
		void info(const char* format, ...);
		void warn(const char* format, ...);
		void error(const char* format, ...);
		void critical(const char* format, ...);

		void crumb(const char* filename, int line, const char* function);
		void trace(bool useHeader,
				   const char* filename, int line, const char* function,
				   const char* format, ...);
		void debug(bool useHeader,
				   const char* filename, int line, const char* function,
				   const char* format, ...);
		void info(bool useHeader,
				  const char* filename, int line, const char* function,
				  const char* format, ...);
		void warn(bool useHeader,
				  const char* filename, int line, const char* function,
				  const char* format, ...);
		void error(bool useHeader,
				   const char* filename, int line, const char* function,
				   const char* format, ...);
		void critical(bool useHeader,
					  const char* filename, int line, const char* function,
					  const char* format, ...);

		void log(LOG_LEVEL level,
				 const char* format, va_list args);
		void log(LOG_LEVEL level,
				 bool useHeader,
				 const char* filename, int line, const char* function,
				 const char* format, va_list args);
		void log_crumb(const char* filename, int line, const char* function);

		void construct(LOG_LEVEL level, const char* message);
		void construct(LOG_LEVEL level,
					   const char* filename, int line, const char* function,
					   const char* body);

		void dispatch(LOG_LEVEL level, const char* line);

		void flush();
		
/*********************************************************************************************
	Private
 *********************************************************************************************/

	private:

		std::string 								header();
		
		std::string									_name;
		std::vector<std::shared_ptr<LoggerSink>>	_sinks;
		LOG_LEVEL									_level;
		LOG_LEVEL									_flushLevel;
	};
}


#endif /* Logger_h */
