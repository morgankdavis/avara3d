//
//  Logger.h
//	avara3d
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
#include <unordered_set>

#include "fmt/format.h"

#include "a3d/Types.h"
//#include "a3d/diagnostic/exceptions/Exception.h"


#define NOOP ((void)0)

#ifdef A3D_DEBUG
//#define A3D_LOG_T() Logger::MainLogger()->crumb(__FILE_NAME__, __LINE__, __FUNCTION__)
#define A3D_LOG_T(fmtStr, ...) Logger::MainLogger()->trace(true, __FILE_NAME__, __LINE__, __FUNCTION__, fmt::format(fmtStr, ##__VA_ARGS__).c_str())
#define A3D_LOG_D(fmtStr, ...) Logger::MainLogger()->debug(true, __FILE_NAME__, __LINE__, __FUNCTION__, fmt::format(fmtStr, ##__VA_ARGS__).c_str())
#else
#define A3D_LOG_T() NOOP
#define A3D_LOG_T(fmtStr, ...) NOOP
#define A3D_LOG_D(fmtStr, ...) NOOP
#endif
#define A3D_LOG_I(fmtStr, ...) Logger::MainLogger()->info(true, __FILE_NAME__, __LINE__, __FUNCTION__, fmt::format(fmtStr, ##__VA_ARGS__).c_str())
#define A3D_LOG_W(fmtStr, ...) Logger::MainLogger()->warn(true, __FILE_NAME__, __LINE__, __FUNCTION__, fmt::format(fmtStr, ##__VA_ARGS__).c_str())
#define A3D_LOG_E(fmtStr, ...) Logger::MainLogger()->error(true, __FILE_NAME__, __LINE__, __FUNCTION__, fmt::format(fmtStr, ##__VA_ARGS__).c_str())
#define A3D_LOG_C(fmtStr, ...) Logger::MainLogger()->critical(true, __FILE_NAME__, __LINE__, __FUNCTION__, fmt::format(fmtStr, ##__VA_ARGS__).c_str())

#ifdef A3D_DEBUG
#define A3D_LOG_H_T(useHeader, fmtStr, ...) Logger::MainLogger()->trace(useHeader, __FILE_NAME__, __LINE__, __FUNCTION__, fmt::format(fmtStr, ##__VA_ARGS__).c_str())
#define A3D_LOG_H_D(useHeader, fmtStr, ...) Logger::MainLogger()->debug(useHeader, __FILE_NAME__, __LINE__, __FUNCTION__, fmt::format(fmtStr, ##__VA_ARGS__).c_str())
#else
#define A3D_LOG_H_T(useHeader, fmtStr, ...) NOOP
#define A3D_LOG_H_D(useHeader, fmtStr, ...) NOOP
#endif
#define A3D_LOG_H_I(useHeader, fmtStr, ...) Logger::MainLogger()->info(useHeader, __FILE_NAME__, __LINE__, __FUNCTION__, fmt::format(fmtStr, ##__VA_ARGS__).c_str())
#define A3D_LOG_H_W(useHeader, fmtStr, ...) Logger::MainLogger()->warn(useHeader, __FILE_NAME__, __LINE__, __FUNCTION__, fmt::format(fmtStr, ##__VA_ARGS__).c_str())
#define A3D_LOG_H_E(useHeader, fmtStr, ...) Logger::MainLogger()->error(useHeader, __FILE_NAME__, __LINE__, __FUNCTION__, fmt::format(fmtStr, ##__VA_ARGS__).c_str())
#define A3D_LOG_H_C(useHeader, fmtStr, ...) Logger::MainLogger()->critical(useHeader, __FILE_NAME__, __LINE__, __FUNCTION__, fmt::format(fmtStr, ##__VA_ARGS__).c_str())

#ifdef A3D_DEBUG
//#define LOG_T(logger) logger->crumb(__FILE_NAME__, __LINE__, __FUNCTION__)
#define LOG_T(logger, fmtStr, ...) logger->trace(true, __FILE_NAME__, __LINE__, __FUNCTION__, fmt::format(fmtStr, ##__VA_ARGS__).c_str())
#define LOG_D(logger, fmtStr, ...) logger->debug(true, __FILE_NAME__, __LINE__, __FUNCTION__, fmt::format(fmtStr, ##__VA_ARGS__).c_str())
#else
#define LOG_T(logger) NOOP
#define LOG_T(logger, fmtStr, ...) NOOP
#define LOG_D(logger, fmtStr, ...) NOOP
#endif
#define LOG_I(logger, fmtStr, ...) logger->info(true, __FILE_NAME__, __LINE__, __FUNCTION__, fmt::format(fmtStr, ##__VA_ARGS__).c_str())
#define LOG_W(logger, fmtStr, ...) logger->warn(true, __FILE_NAME__, __LINE__, __FUNCTION__, fmt::format(fmtStr, ##__VA_ARGS__).c_str())
#define LOG_E(logger, fmtStr, ...) logger->error(true, __FILE_NAME__, __LINE__, __FUNCTION__, fmt::format(fmtStr, ##__VA_ARGS__).c_str())
#define LOG_C(logger, fmtStr, ...) logger->critical(true, __FILE_NAME__, __LINE__, __FUNCTION__, fmt::format(fmtStr, ##__VA_ARGS__).c_str())

#ifdef A3D_DEBUG
#define LOG_H_T(useHeader, logger, fmtStr, ...) logger->trace(useHeader, __FILE_NAME__, __LINE__, __FUNCTION__, fmt::format(fmtStr, ##__VA_ARGS__).c_str())
#define LOG_H_D(useHeader, logger, fmtStr, ...) logger->debug(useHeader, __FILE_NAME__, __LINE__, __FUNCTION__, fmt::format(fmtStr, ##__VA_ARGS__).c_str())
#else
#define LOG_H_T(useHeader, logger, fmtStr, ...) NOOP
#define LOG_H_D(useHeader, logger, fmtStr, ...) NOOP
#endif
#define LOG_H_I(useHeader, logger, fmtStr, ...) logger->info(useHeader, __FILE_NAME__, __LINE__, __FUNCTION__, fmt::format(fmtStr, ##__VA_ARGS__).c_str())
#define LOG_H_W(useHeader, logger, fmtStr, ...) logger->warn(useHeader, __FILE_NAME__, __LINE__, __FUNCTION__, fmt::format(fmtStr, ##__VA_ARGS__).c_str())
#define LOG_H_E(useHeader, logger, fmtStr, ...) logger->error(useHeader, __FILE_NAME__, __LINE__, __FUNCTION__, fmt::format(fmtStr, ##__VA_ARGS__).c_str())
#define LOG_H_C(useHeader, logger, fmtStr, ...) logger->critical(useHeader, __FILE_NAME__, __LINE__, __FUNCTION__, fmt::format(fmtStr, ##__VA_ARGS__).c_str())


namespace a3d {


	class LoggerSink;


	class Logger {

		//static constexpr unsigned char DEFAULT_NAME[] = "ae";
		static constexpr LogLevel DEFAULT_LEVEL = LogLevel::Debug;
		static constexpr LogLevel DEFAULT_FLUSH_LEVEL = LogLevel::Warn;

/**************************************************************************************
	Public Static
 **************************************************************************************/

	public:

		static std::shared_ptr<Logger> MainLogger();

/*********************************************************************************************
	Lifecycle
 *********************************************************************************************/

		Logger(std::string name, std::shared_ptr<LoggerSink> sink,
			   LogLevel level = DEFAULT_LEVEL, LogLevel flushLevel = DEFAULT_FLUSH_LEVEL);
		Logger(std::string name, std::unordered_set<std::shared_ptr<LoggerSink>> sinks,
			   LogLevel level = DEFAULT_LEVEL, LogLevel flushLevel = DEFAULT_FLUSH_LEVEL);
		~Logger();

/*********************************************************************************************
	Public
 *********************************************************************************************/

		std::string name() const;
		std::unordered_set<std::shared_ptr<LoggerSink>> sinks() const;

		LogLevel level() const;
		void level(LogLevel level);

		LogLevel flushLevel() const;
		void flushLevel(LogLevel level);

		// * favor using A3D_LOG_ and LOG_ macros for fancy formatting *

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

		// WORKS
//		template <typename... Args>
//		void f1(const char* format, Args&&... args) {
//			auto together = fmt::vformat(format, fmt::make_format_args(std::forward<Args>(args)...));
//			A3D_LOG_I("WORK {}", together);
//		}

//		template <typename... Args>
//		void f1(const char* format, Args&&... args) {
//
//			auto vec<std::vector<std::any>>();
//		}

//		template <typename... Args>
//		void f1(const char* format, Args&&... args) {
//			f2(format, std::forward<Args>(args)...);
//		}

//		template <typename... Args>
//		void f2(const char* format, Args&&...args);



//
//		void f2(bool useHeader,
//				const char* filename, int line, const char* function,
//				const char* format);


//		template <typename... Args>
//		void f3(std::string_view fmt, Args&&... args);
//
//		template <typename F, typename... Args>
//		void f4(F, Args&&... args);


		void log(LogLevel level,
				 const char* format, va_list args);
		void log(LogLevel level,
				 bool useHeader,
				 const char* filename, int line, const char* function,
				 const char* format, va_list args);
		void log_crumb(const char* filename, int line, const char* function);

		void construct(LogLevel level, const char* message);
		void construct(LogLevel level,
					   const char* filename, int line, const char* function,
					   const char* body);

		void dispatch(LogLevel level, const char* line);

		void flush();

/*********************************************************************************************
	Private
 *********************************************************************************************/

	private:

		std::string 									header();

		std::string										_name;
		std::unordered_set<std::shared_ptr<LoggerSink>>	_sinks;
		LogLevel										_level;
		LogLevel										_flushLevel;
	};
}


#endif /* Logger_h */
