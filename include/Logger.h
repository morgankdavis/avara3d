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
//	The only solution is to implement the log outselves.
//

#ifndef Logger_h
#define Logger_h


#include <cstdio>
#include <filesystem>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

//#include <boost/filesystem.hpp>
#define FMT_HEADER_ONLY
#include <fmt/format.h>

#include "Exception.h"
#include "Types.h"


// https://gcc.gnu.org/onlinedocs/cpp/Variadic-Macros.html

#define AE_LOG_T(fmtStr, ...) Logger::MainLogger()->trace(fmt::format(fmtStr, ##__VA_ARGS__).c_str())
#define AE_LOG_D(fmtStr, ...) Logger::MainLogger()->debug(fmt::format(fmtStr, ##__VA_ARGS__).c_str())
#define AE_LOG_I(fmtStr, ...) Logger::MainLogger()->info(fmt::format(fmtStr, ##__VA_ARGS__).c_str())
#define AE_LOG_W(fmtStr, ...) Logger::MainLogger()->warn(fmt::format(fmtStr, ##__VA_ARGS__).c_str())
#define AE_LOG_E(fmtStr, ...) Logger::MainLogger()->error(fmt::format(fmtStr, ##__VA_ARGS__).c_str())
#define AE_LOG_C(fmtStr, ...) Logger::MainLogger()->critical(fmt::format(fmtStr, ##__VA_ARGS__).c_str())

#define LOG_T(logger, fmtStr, ...) logger->trace(fmt::format(fmtStr, ##__VA_ARGS__).c_str())
#define LOG_D(logger, fmtStr, ...) logger->debug(fmt::format(fmtStr, ##__VA_ARGS__).c_str())
#define LOG_I(logger, fmtStr, ...) logger->info(fmt::format(fmtStr, ##__VA_ARGS__).c_str())
#define LOG_W(logger, fmtStr, ...) logger->warn(fmt::format(fmtStr, ##__VA_ARGS__).c_str())
#define LOG_E(logger, fmtStr, ...) logger->error(fmt::format(fmtStr, ##__VA_ARGS__).c_str())
#define LOG_C(logger, fmtStr, ...) logger->critical(fmt::format(fmtStr, ##__VA_ARGS__).c_str())


namespace ae {	

	
	class LoggerSink;
	
	
	/*######################################################################################
	 #######################################################################################
		Logger
	 #######################################################################################
	 ######################################################################################*/
	
	
	class Logger : public std::enable_shared_from_this<Logger> {
		
		
		//static constexpr unsigned char DEFAULT_NAME[] = "ae";
		static constexpr LOG_LEVEL DEFAULT_LEVEL = LOG_LEVEL::DEBUG_;
		static constexpr LOG_LEVEL DEFAULT_FLUSH_LEVEL = LOG_LEVEL::WARN_;
		
		
	public:
		
/**************************************************************************************
	Public Static
 **************************************************************************************/
		
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
		
		void log(LOG_LEVEL level, const char* message);
		void log(LOG_LEVEL level, const char* format, va_list args);
		
		void trace(const char* format, ...);
		void debug(const char* format, ...);
		void info(const char* format, ...);
		void warn(const char* format, ...);
		void error(const char* format, ...);
		void critical(const char* format, ...);
		
		void flush();
		
	private:
		
/*********************************************************************************************
	Private
 *********************************************************************************************/

		std::string header();
		
		std::string									_name;
		std::vector<std::shared_ptr<LoggerSink>>	_sinks;
		LOG_LEVEL									_level;
		LOG_LEVEL									_flushLevel;
	};
	
	
	/*######################################################################################
	 #######################################################################################
		LoggerSink
	 #######################################################################################
	 ######################################################################################*/
	
	
	class LoggerSink : public std::enable_shared_from_this<LoggerSink> {
		
	public:
		
/**************************************************************************************
	Lifecycle
 **************************************************************************************/
		
		virtual ~LoggerSink();
		
/*********************************************************************************************
	Public
 *********************************************************************************************/
		
		virtual void flush();
	};
	
	
	/*######################################################################################
	 #######################################################################################
		STDLoggerSink
	 #######################################################################################
	 ######################################################################################*/
	
	
#ifdef DESKTOP
	class STDLoggerSink : public LoggerSink {
		
	public:
		
/*********************************************************************************************
	Lifecycle
 *********************************************************************************************/
		
		STDLoggerSink();
		~STDLoggerSink();
		
/*********************************************************************************************
	Public
 *********************************************************************************************/
		
		void flush() override;
		
/**************************************************************************************
	Internal
 **************************************************************************************/
		
		void write(const char* message, LOG_LEVEL level);
	};
#endif
	
	
	/*######################################################################################
	 #######################################################################################
		AndroidLoggerSink
	 #######################################################################################
	 ######################################################################################*/
	
	
#ifdef ANDROID
	class AndroidLoggerSink : public LoggerSink {
		
	public:
		
/*********************************************************************************************
	Lifecycle
 *********************************************************************************************/
		
		AndroidLoggerSink();
		~AndroidLoggerSink();
		
/*********************************************************************************************
	Public
 *********************************************************************************************/

		void flush() override;
		
/**************************************************************************************
	Internal
 **************************************************************************************/
		
		void write(const char* message, const char* tag, LOG_LEVEL level);
	};
#endif
	
	
	/*######################################################################################
	 #######################################################################################
		FileLoggerSink
	 #######################################################################################
	 ######################################################################################*/
	
	
	class FileLoggerSink : public LoggerSink {
		
		
		static constexpr unsigned DEFAULT_MAX_FILES = 3;
		static constexpr unsigned DEFAULT_MAX_FILESIZE = 1024 * 1024 * 1; // 1MB
		
		
	public:
		
/*********************************************************************************************
	Lifecycle
 *********************************************************************************************/

		// desktop log paths are relative to the executable
		// android log paths are relative to the app's internal storage directory
		FileLoggerSink(std::filesystem::path relPath,
					   unsigned maxFiles = DEFAULT_MAX_FILES,
					   unsigned maxFilesize = DEFAULT_MAX_FILESIZE);
		~FileLoggerSink();
		
/*********************************************************************************************
	Public
 *********************************************************************************************/
		
		std::filesystem::path filepath() const;
		
		unsigned maxFiles() const;
		unsigned maxFilesize() const;
		
		void flush() override;
		
/**************************************************************************************
	Internal
 **************************************************************************************/
		
		void write(const char* message);
		
	private:
		
	/*********************************************************************************************
		Private
	 *********************************************************************************************/
		
		void openStream();
		void checkRotate();
		void rotate();
		
		std::filesystem::path				_filepath;
		unsigned							_maxFiles;
		unsigned							_maxFilesize;
		std::shared_ptr<std::ofstream>		_fileStream;
	};
}

#endif /* Logger_h */
