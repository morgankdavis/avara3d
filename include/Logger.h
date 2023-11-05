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
#include <iostream>
#include <memory>
#include <string>
#include <vector>

#define FMT_HEADER_ONLY
#include <fmt/format.h>

#include "Exception.h"
#include "Types.h"


// https://gcc.gnu.org/onlinedocs/cpp/Variadic-Macros.html

// __PRETTY_FUNCTION__ gives something like:
// "virtual void Renderer::updateFrametimeStats(RenderStats &, float)"
// __FUNCTION_NAME__: https://stackoverflow.com/questions/15305310/predefined-macros-for-function-name-func
//#define AE_LOG_T() Logger::MainLogger()->trace(__FILE_NAME__, __LINE__, __FUNCTION__)
#define AE_LOG_T(fmtStr, ...) Logger::MainLogger()->trace(__FILE_NAME__, __LINE__, __FUNCTION__, fmt::format(fmtStr, ##__VA_ARGS__).c_str())
#define AE_LOG_D(fmtStr, ...) Logger::MainLogger()->debug(__FILE_NAME__, __LINE__, __FUNCTION__, fmt::format(fmtStr, ##__VA_ARGS__).c_str())
#define AE_LOG_I(fmtStr, ...) Logger::MainLogger()->info(__FILE_NAME__, __LINE__, __FUNCTION__, fmt::format(fmtStr, ##__VA_ARGS__).c_str())
#define AE_LOG_W(fmtStr, ...) Logger::MainLogger()->warn(__FILE_NAME__, __LINE__, __FUNCTION__, fmt::format(fmtStr, ##__VA_ARGS__).c_str())
#define AE_LOG_E(fmtStr, ...) Logger::MainLogger()->error(__FILE_NAME__, __LINE__, __FUNCTION__, fmt::format(fmtStr, ##__VA_ARGS__).c_str())
#define AE_LOG_C(fmtStr, ...) Logger::MainLogger()->critical(__FILE_NAME__, __LINE__, __FUNCTION__, fmt::format(fmtStr, ##__VA_ARGS__).c_str())

#define LOG_T(logger, fmtStr, ...) logger->trace(__FILE_NAME__, __LINE__, __FUNCTION__, fmt::format(fmtStr, ##__VA_ARGS__).c_str())
#define LOG_D(logger, fmtStr, ...) logger->debug(__FILE_NAME__, __LINE__, __FUNCTION__, fmt::format(fmtStr, ##__VA_ARGS__).c_str())
#define LOG_I(logger, fmtStr, ...) logger->info(__FILE_NAME__, __LINE__, __FUNCTION__, fmt::format(fmtStr, ##__VA_ARGS__).c_str())
#define LOG_W(logger, fmtStr, ...) logger->warn(__FILE_NAME__, __LINE__, __FUNCTION__, fmt::format(fmtStr, ##__VA_ARGS__).c_str())
#define LOG_E(logger, fmtStr, ...) logger->error(__FILE_NAME__, __LINE__, __FUNCTION__, fmt::format(fmtStr, ##__VA_ARGS__).c_str())
#define LOG_C(logger, fmtStr, ...) logger->critical(__FILE_NAME__, __LINE__, __FUNCTION__, fmt::format(fmtStr, ##__VA_ARGS__).c_str())


namespace ae {	

	
	class LoggerSink;
	
	
/*######################################################################################
 #######################################################################################
	Logger
 #######################################################################################
 ######################################################################################*/
	
	class Logger {
		
		
		//static constexpr unsigned char DEFAULT_NAME[] = "ae";
		static constexpr LOG_LEVEL DEFAULT_LEVEL = LOG_LEVEL::DEBUG_;
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

		void trace(const char* filename, int line, const char* function, const char* format, ...);
		void debug(const char* filename, int line, const char* function, const char* format, ...);
		void info(const char* filename, int line, const char* function, const char* format, ...);
		void warn(const char* filename, int line, const char* function, const char* format, ...);
		void error(const char* filename, int line, const char* function, const char* format, ...);
		void critical(const char* filename, int line, const char* function, const char* format, ...);

		void log(LOG_LEVEL level, const char* format, va_list args);
		void log(LOG_LEVEL level, const char* filename, int line, const char* function, const char* format, va_list args);

		void construct(LOG_LEVEL level, const char* message);
		void construct(LOG_LEVEL level, const char* filename, int line, const char* function,const char* body);

		void dispatch(LOG_LEVEL level, const char* line);

		void flush();
		
/*********************************************************************************************
	Private
 *********************************************************************************************/

	private:

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
	
	
	class LoggerSink {
		
/**************************************************************************************
	Lifecycle
 **************************************************************************************/

	public:

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
		
/*********************************************************************************************
	Lifecycle
 *********************************************************************************************/

	public:

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

		
/*********************************************************************************************
	Lifecycle
 *********************************************************************************************/

	public:

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
		
/*********************************************************************************************
	Private
 *********************************************************************************************/

	private:

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
