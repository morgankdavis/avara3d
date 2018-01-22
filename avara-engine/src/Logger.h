//
//  Logger.h
//	avara-engine
//
//  Created by Morgan Davis on 1/5/18.
//  Copyright © 2018 Morgan K Davis. All rights reserved.
//
//  The whole point of this class was to shield clients of AE from having to know
//  that spdlog existed... They would just call info(), for example, with a variable
//  number of arguments.
//  Template voodoo has prevented this from working, however, so we do this nasty
//  inline template hack exposing the spdlog header...
//  Someday hopefully we will change this.
//

#ifndef Logger_h
#define Logger_h


#include <memory>
#include <string>

#include <spdlog/spdlog.h>

#include "Types.h"


namespace ae {
	
	class Logger : public std::enable_shared_from_this<Logger> {
		
	public:
		
		/***************************************************************************************
		     MARK:   Lifecycle
		 **************************************************************************************/
		
		Logger(std::string name, LoggerSink sinks);
		
		/***************************************************************************************
		     MARK:   Public
		 **************************************************************************************/
		
		std::string name() const;
		LoggerSink sinks() const;
		
		
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
		}
		
		template <typename... Args>
		inline void critical(const wchar_t* fmt, const Args&... args) {
			m_logger->log(spdlog::level::critical, fmt, args...);
		}
		
		template <typename Arg1, typename... Args>
		inline void critical(const char* fmt, const Arg1 &arg1, const Args&... args) {
			m_logger->log(spdlog::level::critical, fmt, arg1, args...);
		}
		
		
		
		
		
//		template<typename T>
//		void trace(const T& msg);
//
//		template<typename T>
//		void debug(const T& msg);
//
//		template<typename T>
//		void info(const T& msg);
//
//		template<typename T>
//		void warn(const T& msg);
//
//		template<typename T>
//		void error(const T& msg);
//
//		template<typename T>
//		void critical(const T& msg);
		
		
		
		
		

		
		void flush();

		
	private:
		
		/***************************************************************************************
		     MARK:   Private
		 **************************************************************************************/
		
		std::string							m_name;
		LoggerSink							m_sinks;
		std::shared_ptr<spdlog::logger>		m_logger;
	};
	
	extern std::shared_ptr<ae::Logger>		g_aeLogger;
	#define AE_LOG							g_aeLogger
}


#endif /* Logger_h */
