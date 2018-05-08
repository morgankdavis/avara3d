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

#include "Exception.h"
#include "Types.h"


//#ifndef ANDROID
#define LOGGER_ENABLED
//#endif


namespace ae {
	
	class Logger : public std::enable_shared_from_this<Logger> {
		
	public:
		
		/**************************************************************************************
		     Public Static
		 **************************************************************************************/
		
		static void Init();
		
		/***************************************************************************************
		     Lifecycle
		 ***************************************************************************************/
		
		Logger(std::string name, LOGGER_SINKS sinks);
		
		/***************************************************************************************
		     Public
		 ***************************************************************************************/
		
		std::string name() const;
		LOGGER_SINKS sinks() const;
		
		
		template<typename T>
		inline void trace(const T& msg) {
#ifdef LOGGER_ENABLED
			m_logger->log(spdlog::level::trace, msg);
#endif
		}
		
		template <typename... Args>
		inline void trace(const wchar_t* fmt, const Args&... args) {
#ifdef LOGGER_ENABLED
			m_logger->log(spdlog::level::trace, fmt, args...);
#endif
		}
		
		template <typename Arg1, typename... Args>
		inline void trace(const char* fmt, const Arg1 &arg1, const Args&... args) {
#ifdef LOGGER_ENABLED
			m_logger->log(spdlog::level::trace, fmt, arg1, args...);
#endif
		}

		template<typename T>
		inline void debug(const T& msg) {
#ifdef LOGGER_ENABLED
			m_logger->log(spdlog::level::debug, msg);
#endif
		}
		
		template <typename... Args>
		inline void debug(const wchar_t* fmt, const Args&... args) {
#ifdef LOGGER_ENABLED
			m_logger->log(spdlog::level::debug, fmt, args...);
#endif
		}
		
		template <typename Arg1, typename... Args>
		inline void debug(const char* fmt, const Arg1 &arg1, const Args&... args) {
#ifdef LOGGER_ENABLED
			m_logger->log(spdlog::level::debug, fmt, arg1, args...);
#endif
		}

		template<typename T>
		inline void info(const T& msg) {
#ifdef LOGGER_ENABLED
			m_logger->log(spdlog::level::info, msg);
#endif
		}

		template <typename... Args>
		inline void info(const wchar_t* fmt, const Args&... args) {
#ifdef LOGGER_ENABLED
			m_logger->log(spdlog::level::info, fmt, args...);
#endif
		}

		template <typename Arg1, typename... Args>
		inline void info(const char* fmt, const Arg1 &arg1, const Args&... args) {
#ifdef LOGGER_ENABLED
			m_logger->log(spdlog::level::info, fmt, arg1, args...);
#endif
		}
		
		template<typename T>
		inline void warn(const T& msg) {
#ifdef LOGGER_ENABLED
			m_logger->log(spdlog::level::warn, msg);
#endif
		}
		
		template <typename... Args>
		inline void warn(const wchar_t* fmt, const Args&... args) {
#ifdef LOGGER_ENABLED
			m_logger->log(spdlog::level::warn, fmt, args...);
#endif
		}
		
		template <typename Arg1, typename... Args>
		inline void warn(const char* fmt, const Arg1 &arg1, const Args&... args) {
#ifdef LOGGER_ENABLED
			m_logger->log(spdlog::level::warn, fmt, arg1, args...);
#endif
		}
		
		template<typename T>
		inline void error(const T& msg) {
#ifdef LOGGER_ENABLED
			m_logger->log(spdlog::level::err, msg);
#endif
		}
		
		template <typename... Args>
		inline void error(const wchar_t* fmt, const Args&... args) {
#ifdef LOGGER_ENABLED
			m_logger->log(spdlog::level::err, fmt, args...);
#endif
		}
		
		template <typename Arg1, typename... Args>
		inline void error(const char* fmt, const Arg1 &arg1, const Args&... args) {
#ifdef LOGGER_ENABLED
			m_logger->log(spdlog::level::err, fmt, arg1, args...);
#endif
		}
		
		template<typename T>
		inline void critical(const T& msg) {
#ifdef LOGGER_ENABLED
			m_logger->log(spdlog::level::critical, msg);
			m_logger->flush();
			std::abort();
#endif
		}
		
		template <typename... Args>
		inline void critical(const wchar_t* fmt, const Args&... args) {
#ifdef LOGGER_ENABLED
			m_logger->log(spdlog::level::critical, fmt, args...);
			m_logger->flush();
			std::abort();
#endif
		}
		
		template <typename Arg1, typename... Args>
		inline void critical(const char* fmt, const Arg1 &arg1, const Args&... args) {
#ifdef LOGGER_ENABLED
			m_logger->log(spdlog::level::critical, fmt, arg1, args...);
			m_logger->flush();
			std::abort();
#endif
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
		     Private
		 ***************************************************************************************/
		
		std::string							m_name;
		LOGGER_SINKS						m_sinks;
		std::shared_ptr<spdlog::logger>		m_logger;
	};
	
	extern std::shared_ptr<ae::Logger>		g_logger;
	#define AE_LOG							g_logger
	
	#define AE_FILE							__FILE__
	#define AE_FUNC							__func__
	#define AE_LINE							__LINE__
}


#endif /* Logger_h */
