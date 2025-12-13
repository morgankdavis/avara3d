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
#include <vector>

#include "a3d/Types.h"

#define NOOP ((void)0)

/// Public Macro Functions ///

#ifdef A3D_DEBUG
#define A3D_APP_LOG_T(fmtStr, ...)	a3d::Log::AppLog().log(a3d::LogLevel::Trace, \
										{__FILE_NAME__, __LINE__, __FUNCTION__}, \
										std::format(fmtStr, ##__VA_ARGS__))
#define A3D_APP_LOG_D(fmtStr, ...) 	a3d::Log::AppLog().log(a3d::LogLevel::Debug, \
										{__FILE_NAME__, __LINE__, __FUNCTION__}, \
										std::format(fmtStr, ##__VA_ARGS__))
#else
#define A3D_APP_LOG_T(fmtStr, ...) 	NOOP
#define A3D_APP_LOG_D(fmtStr, ...) 	NOOP
#endif
#define A3D_APP_LOG_I(fmtStr, ...)	a3d::Log::AppLog().log(a3d::LogLevel::Info, \
										{__FILE_NAME__, __LINE__, __FUNCTION__}, \
										std::format(fmtStr, ##__VA_ARGS__))
#define A3D_APP_LOG_W(fmtStr, ...)	a3d::Log::AppLog().log(a3d::LogLevel::Warn, \
										{__FILE_NAME__, __LINE__, __FUNCTION__}, \
										std::format(fmtStr, ##__VA_ARGS__))
#define A3D_APP_LOG_E(fmtStr, ...)	a3d::Log::AppLog().log(a3d::LogLevel::Error, \
										{__FILE_NAME__, __LINE__, __FUNCTION__}, \
										std::format(fmtStr, ##__VA_ARGS__))
#define A3D_APP_LOG_F(fmtStr, ...)	a3d::Log::AppLog().log(a3d::LogLevel::Fatal, \
										{__FILE_NAME__, __LINE__, __FUNCTION__}, \
										std::format(fmtStr, ##__VA_ARGS__))

/// Internal Macro Functions ///

#ifdef A3D_DEBUG
#define A3D_LOG_T(fmtStr, ...) 		a3d::Log::MainLog().log(a3d::LogLevel::Trace, \
										{__FILE_NAME__, __LINE__, __FUNCTION__}, \
										std::format(fmtStr, ##__VA_ARGS__))
#define A3D_LOG_D(fmtStr, ...) 		a3d::Log::MainLog().log(a3d::LogLevel::Debug, \
										{__FILE_NAME__, __LINE__, __FUNCTION__}, \
										std::format(fmtStr, ##__VA_ARGS__))
#else
	#define A3D_LOG_T(fmtStr, ...) 	NOOP
	#define A3D_LOG_D(fmtStr, ...) 	NOOP
#endif
#define A3D_LOG_I(fmtStr, ...) 		a3d::Log::MainLog().log(a3d::LogLevel::Info, \
										{__FILE_NAME__, __LINE__, __FUNCTION__}, \
										std::format(fmtStr, ##__VA_ARGS__))
#define A3D_LOG_W(fmtStr, ...) 		a3d::Log::MainLog().log(a3d::LogLevel::Warn, \
										{__FILE_NAME__, __LINE__, __FUNCTION__}, \
										std::format(fmtStr, ##__VA_ARGS__))
#define A3D_LOG_E(fmtStr, ...) 		a3d::Log::MainLog().log(a3d::LogLevel::Error, \
										{__FILE_NAME__, __LINE__, __FUNCTION__}, \
										std::format(fmtStr, ##__VA_ARGS__))
#define A3D_LOG_F(fmtStr, ...) 		a3d::Log::MainLog().log(a3d::LogLevel::Fatal, \
										{__FILE_NAME__, __LINE__, __FUNCTION__}, \
										std::format(fmtStr, ##__VA_ARGS__))

namespace a3d {

	class LogSink;

	class Log {

	public:
		/// Internal Types ///

		/* filename, line, function */
		using SourceInfo = std::tuple<std::string, unsigned, std::string>;

		/// Public Static Member Functions ///

		static Log& AppLog();
		static void AppLog(std::unique_ptr<Log> log);
//		static void AppLog(const Log& log);

		/// Public Static Member Functions ///

		static Log& MainLog();


		/// Public Lifecycle Functions ///

		Log(const std::string& name,
			std::unique_ptr<LogSink> sink,
			LogLevel level = DEFAULT_LEVEL,
			LogLevel flushLevel = DEFAULT_FLUSH_LEVEL);
		Log(const std::string& name,
			std::vector<std::unique_ptr<LogSink>> sinks,
			LogLevel level = DEFAULT_LEVEL,
			LogLevel flushLevel = DEFAULT_FLUSH_LEVEL);
		Log();

		/// Public Member Functions ///

		const std::string& 		name() const;

		const std::vector<std::unique_ptr<LogSink>>&	sinks() const;

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

	private:
		/// Private Lifecycle ///

		Log(const std::string& name);

		/// Private Member Functions ///

		void 					log(LogLevel level,
									const std::string& msg);

		void 					dispatch(LogLevel level, std::string& output);

		/// Private Constants ///

		static constexpr LogLevel DEFAULT_LEVEL = LogLevel::Debug;
		static constexpr LogLevel DEFAULT_FLUSH_LEVEL = LogLevel::Warn;

		/// Private Static Member Variables ///

		static std::unique_ptr<Log>						_appLog;
//		static Log										_appLog;

		/// Private Member Variables ///

		std::string										_name;
		std::vector<std::unique_ptr<LogSink>>			_sinks;
		LogLevel										_level;
		LogLevel										_flushLevel;
	};
}

#endif /* AVARA3D_LOG_H */
