//
//  Logger.h
//  avara3d
//
//  Created by Morgan Davis on 1/5/18.
//  Copyright © 2024 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_LOG_H
#define AVARA3D_LOG_H


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
#define A3D_APP_LOG_T(log_, fmtStr, ...)	log_->log(LogLevel::Trace, \
											{__FILE_NAME__, __LINE__, __FUNCTION__}, \
											fmt::format(fmtStr, ##__VA_ARGS__))
#define A3D_APP_LOG_D(log_, fmtStr, ...) 	log_->log(LogLevel::Debug, \
											{__FILE_NAME__, __LINE__, __FUNCTION__}, \
											fmt::format(fmtStr, ##__VA_ARGS__))
#else
#define A3D_APP_LOG_T(log, fmtStr, ...) 	NOOP
#define A3D_APP_LOG_D(log, fmtStr, ...) 	NOOP
#endif
#define A3D_APP_LOG_I(log_, fmtStr, ...)	log_->log(LogLevel::Info, \
											{__FILE_NAME__, __LINE__, __FUNCTION__}, \
											fmt::format(fmtStr, ##__VA_ARGS__))
#define A3D_APP_LOG_W(log_, fmtStr, ...)	log_->log(LogLevel::Warn, \
											{__FILE_NAME__, __LINE__, __FUNCTION__}, \
											fmt::format(fmtStr, ##__VA_ARGS__))
#define A3D_APP_LOG_E(log_, fmtStr, ...)	log_->log(LogLevel::Error, \
											{__FILE_NAME__, __LINE__, __FUNCTION__}, \
											fmt::format(fmtStr, ##__VA_ARGS__))
#define A3D_APP_LOG_F(log_, fmtStr, ...)	log_->log(LogLevel::Fatal, \
											{__FILE_NAME__, __LINE__, __FUNCTION__}, \
											fmt::format(fmtStr, ##__VA_ARGS__))

/*********************************************************************************************
	Internal Macro Functions
 *********************************************************************************************/

#ifdef A3D_DEBUG
#define A3D_LOG_T(fmtStr, ...) 		Log::MainLog().log(LogLevel::Trace, \
										{__FILE_NAME__, __LINE__, __FUNCTION__}, \
										fmt::format(fmtStr, ##__VA_ARGS__))
#define A3D_LOG_D(fmtStr, ...) 		Log::MainLog().log(LogLevel::Debug, \
										{__FILE_NAME__, __LINE__, __FUNCTION__}, \
										fmt::format(fmtStr, ##__VA_ARGS__))
#else
	#define A3D_LOG_T(fmtStr, ...) 	NOOP
	#define A3D_LOG_D(fmtStr, ...) 	NOOP
#endif
#define A3D_LOG_I(fmtStr, ...) 		Log::MainLog().log(LogLevel::Info, \
										{__FILE_NAME__, __LINE__, __FUNCTION__}, \
										fmt::format(fmtStr, ##__VA_ARGS__))
#define A3D_LOG_W(fmtStr, ...) 		Log::MainLog().log(LogLevel::Warn, \
										{__FILE_NAME__, __LINE__, __FUNCTION__}, \
										fmt::format(fmtStr, ##__VA_ARGS__))
#define A3D_LOG_E(fmtStr, ...) 		Log::MainLog().log(LogLevel::Error, \
										{__FILE_NAME__, __LINE__, __FUNCTION__}, \
										fmt::format(fmtStr, ##__VA_ARGS__))
#define A3D_LOG_F(fmtStr, ...) 		Log::MainLog().log(LogLevel::Fatal, \
										{__FILE_NAME__, __LINE__, __FUNCTION__}, \
										fmt::format(fmtStr, ##__VA_ARGS__))


namespace a3d {


	class LogSink;


	class Log {

/*********************************************************************************************
	Internal Types
 *********************************************************************************************/

	public:

		/* filename, line, function */
		using SourceInfo = std::tuple<std::string, unsigned, std::string>;


/*********************************************************************************************
	Public Static Member Functions
 *********************************************************************************************/

		static Log& MainLog();

/*********************************************************************************************
	Public Lifecycle Functions
 *********************************************************************************************/

		Log(const std::string& name,
			std::unique_ptr<LogSink> sink,
			LogLevel level = DEFAULT_LEVEL,
			LogLevel flushLevel = DEFAULT_FLUSH_LEVEL);
		Log(const std::string& name,
			std::unordered_set<std::unique_ptr<LogSink>> sinks,
			LogLevel level = DEFAULT_LEVEL,
			LogLevel flushLevel = DEFAULT_FLUSH_LEVEL);
		Log();

/*********************************************************************************************
	Public Member Functions
 *********************************************************************************************/

		const std::string& 		name() const;

		const std::unordered_set<std::unique_ptr<LogSink>>&	sinks() const;

		LogLevel 				level() const;
		void					level(LogLevel level);

		LogLevel 				flushLevel() const;
		void 					flushLevel(LogLevel level);

		void					trace(const std::string& msg);
		void					debug(const std::string& msg);
		void					info(const std::string& msg);
		void					warn(const std::string& msg);
		void					error(const std::string& msg);
		void					fatal(const std::string& msg);

		void 					log(LogLevel level,
									const SourceInfo& sourceInfo,
									const std::string& msg);

		void 					flush();

/*********************************************************************************************
	Private  Member Functions
 *********************************************************************************************/

	private:

		void 					log(LogLevel level,
									const std::string& msg);

		void 					dispatch(LogLevel level, std::string& output);

/*********************************************************************************************
	Private Constants
 *********************************************************************************************/

		static constexpr LogLevel DEFAULT_LEVEL = LogLevel::Debug;
		static constexpr LogLevel DEFAULT_FLUSH_LEVEL = LogLevel::Warn;

/*********************************************************************************************
	Private Member Variables
 *********************************************************************************************/

		std::string										_name;
		std::unordered_set<std::unique_ptr<LogSink>>	_sinks;
		LogLevel										_level;
		LogLevel										_flushLevel;
	};
}


#endif /* AVARA3D_LOG_H */
