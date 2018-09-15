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
	
	
	
	
	class LoggerSink;
	
	
	
	class Logger : public std::enable_shared_from_this<Logger> {
		
	public:
		
		/**************************************************************************************
		     Public Static
		 **************************************************************************************/
		
		static std::shared_ptr<Logger> MainLogger();
		
		/***************************************************************************************
		     Lifecycle
		 ***************************************************************************************/
		
		Logger(std::string name, std::vector<std::shared_ptr<LoggerSink>> sinks);
		~Logger();
		
		/***************************************************************************************
		     Public
		 ***************************************************************************************/
		
		std::string name() const;
		std::vector<std::shared_ptr<LoggerSink>> sinks() const;
		
		LOG_LEVEL level() const;
		void level(LOG_LEVEL level);
		
		void log(LOG_LEVEL level, std::string& message);
		void log(LOG_LEVEL level, std::string& format, ...);
		
		void trace(std::string& message);
		void trace(std::string& format, ...);
		
		void debug(std::string& message);
		void debug(std::string& format, ...);

		void info(std::string& message);
		void info(std::string& format, ...);
		
		void warn(std::string& message);
		void warn(std::string& format, ...);

		void error(std::string& message);
		void error(std::string& format, ...);

		void critical(std::string& message);
		void critical(std::string& format, ...);
		
		void flush();
		
	private:
		
		/***************************************************************************************
		     Private
		 ***************************************************************************************/
		
		std::string									m_name;
		std::vector<std::shared_ptr<LoggerSink>>	m_sinks;
	};
	
	
	
	class LoggerSink : public std::enable_shared_from_this<LoggerSink> {
		
	public:
		
		/***************************************************************************************
		     Lifecycle
		 ***************************************************************************************/
		
		LoggerSink();
//		~NewLoggerSink();
		
		/***************************************************************************************
		     Public
		 ***************************************************************************************/
		
		virtual void flush();
		
	private:
		
		/***************************************************************************************
		     Private
		 ***************************************************************************************/
		
	};
	
	
	
	
	class NativeLoggerSink : public LoggerSink {
		
	public:
		
		/***************************************************************************************
		     Lifecycle
		 ***************************************************************************************/
		
		NativeLoggerSink();
		~NativeLoggerSink();
		
		/***************************************************************************************
		     Public
		 ***************************************************************************************/
		
		void flush() override;
		
	private:
		
		/***************************************************************************************
		     Private
		 ***************************************************************************************/
		
	};
	
	
	
	
	
	class FileLoggerSink : public LoggerSink {
		
	public:
		
		/***************************************************************************************
		     Lifecycle
		 ***************************************************************************************/
		
		FileLoggerSink(boost::filesystem::path filepath);
		~FileLoggerSink();
		
		/***************************************************************************************
		     Public
		 ***************************************************************************************/
		
		boost::filesystem::path filepath() const;
		
		unsigned maxFiles() const;
		unsigned maxFilesize() const;
		
		void flush() override;
		
	private:
		
		/***************************************************************************************
		     Private
		 ***************************************************************************************/
		
		boost::filesystem::path		m_filepath;
		unsigned					m_maxFiles;
		unsigned					m_maxFilesize;
	};
	
	
	
}

#endif /* Logger_h */
