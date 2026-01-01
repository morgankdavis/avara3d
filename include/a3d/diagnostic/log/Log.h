// Log.h
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

#include "a3d/Types.h"

// example:
// log::w()("This is a normal log line");     // wraps .nl()
// log::w().nl("This is a normal log line");  // explicit newline
// log::w().raw("progress: 10%");             // no newline

#if defined(_MSC_VER)
static_assert(_MSVC_LANG >= 202002L, "Need C++20 (/std:c++20) for std::source_location");
#else
static_assert(__cplusplus >= 202002L, "Need C++20 for std::source_location");
#endif

namespace a3d {

	class LogSink;

	class Log {

	public:
		/// Internal Types ///

		/* filename, line, function */
		using SourceInfo = std::tuple<std::string, unsigned, std::string>;
		// using SourceInfo = std::tuple<std::string_view, unsigned, std::string_view>;

		/// Public Static Member Functions ///

		static Log& AppLog();
		static void AppLog(Log log);
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

		Log(const Log& other) = delete;            // copy constructor
		Log& operator=(const Log& other) = delete; // copy assignment
		Log(Log&&) noexcept = default;
		Log& operator=(Log&&) noexcept = default;

		~Log(); // must be out-of-line because LogSink is incomplete here

		Log();

		/// Public Member Functions ///

		const std::string& name() const;
		const std::vector<std::unique_ptr<LogSink>>& sinks() const;

		LogLevel level() const;
		void level(LogLevel level);

		LogLevel flushLevel() const;
		void flushLevel(LogLevel flushLevel);

		// ---- Original API (kept verbatim) ----
		void trace(const std::string& msg);
		void debug(const std::string& msg);
		void info (const std::string& msg);
		void warn (const std::string& msg);
		void error(const std::string& msg);
		void fatal(const std::string& msg);

		void log(LogLevel level,
				 const SourceInfo& sourceInfo,
				 const std::string& msg);

		void flush();

		/// Internal Member Functions ///

		void write(LogLevel level, const SourceInfo& sourceInfo, std::string_view msg);
		void write(LogLevel level, std::string_view msg);

		class Entry {
		public:
			Entry(Log& logger, LogLevel lvl, SourceInfo src);

			// operator() wraps nl()
			void operator()(std::string_view msg) const;

			template<class... Args>
			void operator()(std::format_string<Args...> fmt, Args&&... args) const {
				nlf(fmt, std::forward<Args>(args)...);
			}

			// ---- main dudes ----

			// newline-terminated (old behavior)
			void nl(std::string_view msg) const;

			template<class... Args>
			void nlf(std::format_string<Args...> fmt, Args&&... args) const {
				if (!_logger->enabled(_lvl)) return;
				_logger->log(_lvl, _src, std::format(fmt, std::forward<Args>(args)...));
			}

			// no newline
			void raw(std::string_view msg) const;

			template<class... Args>
			void rawf(std::format_string<Args...> fmt, Args&&... args) const {
				if (!_logger->enabled(_lvl)) return;
				_logger->write(_lvl, _src, std::format(fmt, std::forward<Args>(args)...));
			}

		private:
			Log*       _logger;
			LogLevel   _lvl;
			SourceInfo _src;
		};

		// Overloads (same names) that capture source location.
		// Default arg is evaluated at the *caller* (this is the trick).
		Entry trace(std::source_location where = std::source_location::current());
		Entry debug(std::source_location where = std::source_location::current());
		Entry info (std::source_location where = std::source_location::current());
		Entry warn (std::source_location where = std::source_location::current());
		Entry error(std::source_location where = std::source_location::current());
		Entry fatal(std::source_location where = std::source_location::current());

	private:
		/// Private Lifecycle ///

		Log(const std::string& name);

		/// Private Member Functions ///

		void log(LogLevel level, const std::string& msg);
		void dispatch(LogLevel level, std::string& output);

		bool enabled(LogLevel lvl) const;

		// moved out-of-line
		static std::string_view Basename(std::string_view p);
		static std::string_view ShortFunction(std::string_view s, int keepScopes = 2);
		static SourceInfo MakeSourceInfo(const std::source_location& where);

		/// Private Constants ///

		static constexpr LogLevel DEFAULT_LEVEL       = LogLevel::Debug;
		static constexpr LogLevel DEFAULT_FLUSH_LEVEL = LogLevel::Warn;

		/// Private Static Member Variables ///

		static std::optional<Log> _appLog;

		/// Private Member Variables ///

		std::string _name;
		std::vector<std::unique_ptr<LogSink>> _sinks;
		LogLevel _level;
		LogLevel _flushLevel;
	};

	// Convenience free functions
	// (Default argument captures call-site correctly.)
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
