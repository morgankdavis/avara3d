//
//  Log.h
//  avara3d
//
//  Created by Morgan Davis on 1/5/18.
//  Copyright © 2024 Morgan K Davis. All rights reserved.
//

#ifndef A3D_LOG_H
#define A3D_LOG_H

#include <format>
#include <memory>
#include <optional>
#include <source_location>
#include <string>
#include <string_view>
#include <tuple>
#include <utility>
#include <vector>

namespace a3d {

	class LogSink;

	class Log {

	public:
		/// Public Types ///

		enum class Level : uint8_t {
			Trace =		0,
			Debug =		1,
			Info =		2,
			Warn =		3,
			Error =		4,
			Fatal = 	5,
			Off = 		6
		};

		/// Internal Types ///

		struct SourceInfo {
			std::string_view filename;
			unsigned line;
			std::string_view function;
		};

		class Entry {
		public:
			Entry(Log& logger, Log::Level lvl, SourceInfo src);

			void operator()(std::string_view msg) const;

			template<class... Args>
			void operator()(std::format_string<Args...> fmt, Args&&... args) const {
				nl(fmt, std::forward<Args>(args)...);
			}

			void nl(std::string_view msg) const;

			template<class... Args>
			void nl(std::format_string<Args...> fmt, Args&&... args) const
			requires (sizeof...(Args) > 0) {
				nlf(fmt, std::forward<Args>(args)...);
			}

			template<class... Args>
			void nlf(std::format_string<Args...> fmt, Args&&... args) const {
				if (!_logger->enabled(_level)) return;
				_logger->log(_level, _source, std::format(fmt, std::forward<Args>(args)...));
			}

			void raw(std::string_view msg) const;

			template<class... Args>
			void raw(std::format_string<Args...> fmt, Args&&... args) const
			requires (sizeof...(Args) > 0) {
				rawf(fmt, std::forward<Args>(args)...);
			}

			template<class... Args>
			void rawf(std::format_string<Args...> fmt, Args&&... args) const {
				if (!_logger->enabled(_level)) return;
				_logger->write(_level, _source, std::format(fmt, std::forward<Args>(args)...));
			}

		private:
			Log*       	_logger;
			Log::Level 	_level;
			SourceInfo 	_source;
		};

		/// Public Static Member Functions ///

		static Log& AppLog();
		static void AppLog(Log log);
		static Log& MainLog();

		/// Public Lifecycle Functions ///

		Log(const std::string& name,
			std::unique_ptr<LogSink> sink,
			Log::Level level = DEFAULT_LEVEL,
			Log::Level flushLevel = DEFAULT_FLUSH_LEVEL);

		Log(const std::string& name,
			std::vector<std::unique_ptr<LogSink>> sinks,
			Log::Level level = DEFAULT_LEVEL,
			Log::Level flushLevel = DEFAULT_FLUSH_LEVEL);

		Log(const Log& other) = delete;
		Log& operator=(const Log& other) = delete;
		Log(Log&&) noexcept = default;
		Log& operator=(Log&&) noexcept = default;

		~Log();

		Log();

		/// Public Member Functions ///

		const std::string& name() const;

		const std::vector<std::unique_ptr<LogSink>>& sinks() const;

		Log::Level level() const;
		void level(Log::Level level);

		Log::Level flushLevel() const;
		void flushLevel(Log::Level flushLevel);

		void trace(const std::string& msg);
		void debug(const std::string& msg);
		void info (const std::string& msg);
		void warn (const std::string& msg);
		void error(const std::string& msg);
		void fatal(const std::string& msg);

		void log(Log::Level level,
				 const SourceInfo& sourceInfo,
				 const std::string& msg);

		void flush();

		/// Internal Member Functions ///

		Entry trace(std::source_location where = std::source_location::current());
		Entry debug(std::source_location where = std::source_location::current());
		Entry info (std::source_location where = std::source_location::current());
		Entry warn (std::source_location where = std::source_location::current());
		Entry error(std::source_location where = std::source_location::current());
		Entry fatal(std::source_location where = std::source_location::current());

		void write(Log::Level level, const SourceInfo& sourceInfo, std::string_view msg);
		void write(Log::Level level, std::string_view msg);

	private:
		/// Private Static Member Functions ///

		static std::string_view Basename(std::string_view p);
		static std::string_view ShortFunction(std::string_view s, int keepScopes = 2);
		static SourceInfo MakeSourceInfo(const std::source_location& where);

		/// Private Lifecycle ///

		Log(const std::string& name);

		/// Private Member Functions ///

		void log(Log::Level level, const std::string& msg);
		void dispatch(Log::Level level, std::string& output);

		bool enabled(Log::Level lvl) const;

		/// Private Constants ///

		static constexpr Log::Level DEFAULT_LEVEL       = Log::Level::Debug;
		static constexpr Log::Level DEFAULT_FLUSH_LEVEL = Log::Level::Warn;

		/// Private Static Member Variables ///

		static std::optional<Log> _appLog;

		/// Private Member Variables ///

		std::string								_name;
		std::vector<std::unique_ptr<LogSink>> 	_sinks;
		Log::Level 								_level;
		Log::Level 								_flushLevel;
	};

	namespace log {

		inline Log::Entry t(std::source_location where = std::source_location::current()) { return Log::MainLog().trace(where); }
		inline Log::Entry d(std::source_location where = std::source_location::current()) { return Log::MainLog().debug(where); }
		inline Log::Entry i(std::source_location where = std::source_location::current()) { return Log::MainLog().info(where);  }
		inline Log::Entry w(std::source_location where = std::source_location::current()) { return Log::MainLog().warn(where);  }
		inline Log::Entry e(std::source_location where = std::source_location::current()) { return Log::MainLog().error(where); }
		inline Log::Entry f(std::source_location where = std::source_location::current()) { return Log::MainLog().fatal(where); }

		namespace app {

			inline Log::Entry t(std::source_location where = std::source_location::current()) { return Log::AppLog().trace(where); }
			inline Log::Entry d(std::source_location where = std::source_location::current()) { return Log::AppLog().debug(where); }
			inline Log::Entry i(std::source_location where = std::source_location::current()) { return Log::AppLog().info(where);  }
			inline Log::Entry w(std::source_location where = std::source_location::current()) { return Log::AppLog().warn(where);  }
			inline Log::Entry e(std::source_location where = std::source_location::current()) { return Log::AppLog().error(where); }
			inline Log::Entry f(std::source_location where = std::source_location::current()) { return Log::AppLog().fatal(where); }
		} // namespace app
	}

} // namespace a3d

#endif // A3D_LOG_H
