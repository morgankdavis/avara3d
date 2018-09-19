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
#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include <boost/filesystem.hpp>
#include <spdlog/spdlog.h>

#include "Exception.h"
#include "Types.h"


namespace ae {

	
	class OldLogger : public std::enable_shared_from_this<OldLogger> {
		
	public:
		
		/**************************************************************************************
		     Public Static
		 **************************************************************************************/
		
		static void Init();
		static void Level(LOG_LEVEL level);
		
		/***************************************************************************************
		     Lifecycle
		 ***************************************************************************************/
		
		OldLogger(std::string name, LOGGER_SINK sinks);
		
		/***************************************************************************************
		     Public
		 ***************************************************************************************/
		
		std::string name() const;
		LOGGER_SINK sinks() const;
		
		template<typename T>
		inline void trace(const T& msg) {

			m_logger->log(spdlog::level::trace, msg);
		}
		
		template <typename... Args>
		inline void trace(const wchar_t* fmt, const Args&... args) {
			
			m_logger->log(spdlog::level::trace, fmt, args...);
		}
		
		template <typename Arg1, typename... Args>
		inline void trace(const char* fmt, const Arg1 &arg1, const Args&... args) {

			m_logger->log(spdlog::level::trace, fmt, arg1, args...);
		}

		template<typename T>
		inline void debug(const T& msg) {

			m_logger->log(spdlog::level::debug, msg);
		}
		
		template <typename... Args>
		inline void debug(const wchar_t* fmt, const Args&... args) {

			m_logger->log(spdlog::level::debug, fmt, args...);
		}
		
		template <typename Arg1, typename... Args>
		inline void debug(const char* fmt, const Arg1 &arg1, const Args&... args) {

			m_logger->log(spdlog::level::debug, fmt, arg1, args...);
		}

		template<typename T>
		inline void info(const T& msg) {

			m_logger->log(spdlog::level::info, msg);
		}

		template <typename... Args>
		inline void info(const wchar_t* fmt, const Args&... args) {

			m_logger->log(spdlog::level::info, fmt, args...);
		}

		template <typename Arg1, typename... Args>
		inline void info(const char* fmt, const Arg1 &arg1, const Args&... args) {

			m_logger->log(spdlog::level::info, fmt, arg1, args...);
		}
		
		template<typename T>
		inline void warn(const T& msg) {

			m_logger->log(spdlog::level::warn, msg);
		}
		
		template <typename... Args>
		inline void warn(const wchar_t* fmt, const Args&... args) {

			m_logger->log(spdlog::level::warn, fmt, args...);
		}
		
		template <typename Arg1, typename... Args>
		inline void warn(const char* fmt, const Arg1 &arg1, const Args&... args) {

			m_logger->log(spdlog::level::warn, fmt, arg1, args...);
		}
		
		template<typename T>
		inline void error(const T& msg) {

			m_logger->log(spdlog::level::err, msg);
		}
		
		template <typename... Args>
		inline void error(const wchar_t* fmt, const Args&... args) {

			m_logger->log(spdlog::level::err, fmt, args...);
		}
		
		template <typename Arg1, typename... Args>
		inline void error(const char* fmt, const Arg1 &arg1, const Args&... args) {

			m_logger->log(spdlog::level::err, fmt, arg1, args...);
		}
		
		template<typename T>
		inline void critical(const T& msg) {

			m_logger->log(spdlog::level::critical, msg);
			m_logger->flush();
			std::abort();
		}
		
		template <typename... Args>
		inline void critical(const wchar_t* fmt, const Args&... args) {

			m_logger->log(spdlog::level::critical, fmt, args...);
			m_logger->flush();
			std::abort();
		}
		
		template <typename Arg1, typename... Args>
		inline void critical(const char* fmt, const Arg1 &arg1, const Args&... args) {

			m_logger->log(spdlog::level::critical, fmt, arg1, args...);
			m_logger->flush();
			std::abort();
		}
		
		void flush();
		
	private:
		
		/***************************************************************************************
		     Private
		 ***************************************************************************************/
		
		std::string							m_name;
		LOGGER_SINK							m_sinks;
		std::shared_ptr<spdlog::logger>		m_logger;
	};
	
	
	extern std::shared_ptr<ae::OldLogger>		g_logger;
	#define AE_LOG							g_logger
	
	#define AE_FILE							__FILE__
	#define AE_FUNC							__func__
	#define AE_LINE							__LINE__
	
	
	
	
	
	
	
	
	
	// https://gcc.gnu.org/onlinedocs/cpp/Variadic-Macros.html
	#define LOG_T(fmt, ...) Logger::MainLogger()->trace(fmt, ##__VA_ARGS__)
	#define LOG_D(fmt, ...) Logger::MainLogger()->debug(fmt, ##__VA_ARGS__)
	#define LOG_I(fmt, ...) Logger::MainLogger()->info(fmt, ##__VA_ARGS__)
	#define LOG_W(fmt, ...) Logger::MainLogger()->warn(fmt, ##__VA_ARGS__)
	#define LOG_E(fmt, ...) Logger::MainLogger()->error(fmt, ##__VA_ARGS__)
	#define LOG_C(fmt, ...) Logger::MainLogger()->critical(fmt, ##__VA_ARGS__)
	
	
	
	
	
	class LoggerSink;
	
	
	/*######################################################################################
	 #######################################################################################
	     Logger
	 #######################################################################################
	 ######################################################################################*/
	
	
	class Logger : public std::enable_shared_from_this<Logger> {
		
		
		static constexpr LOG_LEVEL DEFAULT_LEVEL = LOG_LEVEL::INFO_;
		static constexpr LOG_LEVEL DEFAULT_FLUSH_LEVEL = LOG_LEVEL::WARN_;
		
		
	public:
		
		/**************************************************************************************
		     Public Static
		 **************************************************************************************/
		
		static std::shared_ptr<Logger> MainLogger();
		
		/***************************************************************************************
		     Lifecycle
		 ***************************************************************************************/
		
		Logger(std::string name, std::shared_ptr<LoggerSink> sink,
			   LOG_LEVEL level = DEFAULT_LEVEL, LOG_LEVEL flushLevel = DEFAULT_FLUSH_LEVEL);
		Logger(std::string name, std::vector<std::shared_ptr<LoggerSink>> sinks,
			   LOG_LEVEL level = DEFAULT_LEVEL, LOG_LEVEL flushLevel = DEFAULT_FLUSH_LEVEL);
		~Logger();
		
		/***************************************************************************************
		     Public
		 ***************************************************************************************/
		
		std::string name() const;
		std::vector<std::shared_ptr<LoggerSink>> sinks() const;
		
		LOG_LEVEL level() const;
		void level(LOG_LEVEL level);
		
		LOG_LEVEL flushLevel() const;
		void flushLevel(LOG_LEVEL level);
		
		void log(LOG_LEVEL level, const char* message);
		void log(LOG_LEVEL level, const char* format, va_list args);
		
//		void trace(std::string& format, ...);
		void trace(const char* format, ...);
//		void debug(std::string& format, ...);
		void debug(const char* format, ...);
//		void info(std::string& format, ...);
		void info(const char* format, ...);
//		void warn(std::string& format, ...);
		void warn(const char* format, ...);
//		void error(std::string& format, ...);
		void error(const char* format, ...);
//		void critical(std::string& format, ...);
		void critical(const char* format, ...);
		
		void flush();
		
	private:
		
		/***************************************************************************************
		     Private
		 ***************************************************************************************/
		
//		void trace(const char* format, va_list args);
//		void debug(const char* format, va_list args);
//		void info(const char* format, va_list args);
//		void warn(const char* format, va_list args);
//		void error(const char* format, va_list args);
//		void critical(const char* format, va_list args);
		
		std::string header();
		
		std::string									m_name;
		std::vector<std::shared_ptr<LoggerSink>>	m_sinks;
		LOG_LEVEL									m_level;
		LOG_LEVEL									m_flushLevel;
	};
	
	
	/*######################################################################################
	 #######################################################################################
	     LoggerSink
	 #######################################################################################
	 ######################################################################################*/
	
	
	class LoggerSink : public std::enable_shared_from_this<LoggerSink> {
		
	public:
		
		/***************************************************************************************
		     Public
		 ***************************************************************************************/
		
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
		
		/***************************************************************************************
		     Lifecycle
		 ***************************************************************************************/
		
		STDLoggerSink();
		~STDLoggerSink();
		
		/***************************************************************************************
		     Public
		 ***************************************************************************************/
		
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
		
		/***************************************************************************************
		     Lifecycle
		 ***************************************************************************************/
		
		AndroidLoggerSink();
		~AndroidLoggerSink();
		
		/***************************************************************************************
		     Public
		 ***************************************************************************************/

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
		
		/***************************************************************************************
		     Lifecycle
		 ***************************************************************************************/
		
		FileLoggerSink(boost::filesystem::path filepath,
					   unsigned maxFiles = DEFAULT_MAX_FILES,
					   unsigned maxFilesize = DEFAULT_MAX_FILESIZE);
		~FileLoggerSink();
		
		/***************************************************************************************
		     Public
		 ***************************************************************************************/
		
		boost::filesystem::path filepath() const;
		
		unsigned maxFiles() const;
		unsigned maxFilesize() const;
		
		void flush() override;
		
		/**************************************************************************************
		     Internal
		 **************************************************************************************/
		
		void write(const char* message);
		
	private:
		
		/***************************************************************************************
		     Private
		 ***************************************************************************************/
		
		void openStream();
		void checkRotate();
		void rotate();
		
		boost::filesystem::path				m_filepath;
		unsigned							m_maxFiles;
		unsigned							m_maxFilesize;
		std::shared_ptr<std::ofstream>		m_fileStream;
	};
}

#endif /* Logger_h */
