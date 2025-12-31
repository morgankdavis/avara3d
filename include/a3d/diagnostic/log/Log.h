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
#include <functional>
#include <iostream>
#include <memory>
#include <optional>
#include <source_location>
#include <string>
#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>

#include "a3d/Types.h"

#define NOOP ((void)0)

namespace a3d {

	class LogSink;

	class Log {

	public:
		/// Internal Types ///

		/* filename, line, function */
		using SourceInfo = std::tuple<std::string, unsigned, std::string>;

		/// Public Static Member Functions ///

		static Log& AppLog();
		static void AppLog(Log log);

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
		Log(const Log& other) = delete; // copy constructor
		Log& operator=(const Log& other) = delete; // copy assignment
		Log(Log&&) noexcept = default;
		Log& operator=(Log&&) noexcept = default;
		Log();

		/// Public Member Functions ///

		const std::string& 		name() const;

		const std::vector<std::unique_ptr<LogSink>>& sinks() const;

		LogLevel 				level() const;
		void					level(LogLevel level);

		LogLevel 				flushLevel() const;
		void 					flushLevel(LogLevel flushLevel);

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

		static std::optional<Log>						_appLog;

		/// Private Member Variables ///

		std::string										_name;
		std::vector<std::unique_ptr<LogSink>>			_sinks;
		LogLevel										_level;
		LogLevel										_flushLevel;
	};

	// -------------------------------------------------------------------------
	// New ergonomic logging API (no macros needed at call sites)
	// -------------------------------------------------------------------------
	namespace log {

		namespace detail {

			// Map LogLevel to an ordered severity (so we don't depend on enum underlying values).
			constexpr int severity(LogLevel lvl) noexcept {
				switch (lvl) {
					case LogLevel::Trace: return 0;
					case LogLevel::Debug: return 1;
					case LogLevel::Info:  return 2;
					case LogLevel::Warn:  return 3;
					case LogLevel::Error: return 4;
					case LogLevel::Fatal: return 5;
					default:              return 999;
				}
			}

			inline Log::SourceInfo make_source_info(std::source_location loc) {
				// Match your old __FILE_NAME__ vibe: keep only the filename.
				std::string file;
				try {
					file = std::filesystem::path(loc.file_name()).filename().string();
				} catch (...) {
					// filesystem can throw on weird inputs; fall back.
					file = loc.file_name();
				}

				return Log::SourceInfo{
						std::move(file),
						static_cast<unsigned>(loc.line()),
						std::string(loc.function_name())
				};
			}

			inline bool should_log(const Log& logger, LogLevel msgLevel) noexcept {
				// "logger.level()" is the minimum level to emit.
				return severity(msgLevel) >= severity(logger.level());
			}

			// Convert whatever a lazy lambda returns into a std::string.
			template <class T>
			inline std::string to_string(T&& v) {
				using R = std::remove_cvref_t<T>;
				if constexpr (std::is_same_v<R, std::string>) {
					return std::forward<T>(v);
				} else if constexpr (std::is_convertible_v<T, std::string_view>) {
					return std::string(std::string_view(std::forward<T>(v)));
				} else if constexpr (std::is_convertible_v<T, const char*>) {
					return std::string(std::forward<T>(v));
				} else {
					static_assert(std::is_convertible_v<T, std::string>,
								  "lazy logger must return something convertible to std::string or std::string_view");
					return std::string(std::forward<T>(v));
				}
			}

			template <class... Args>
			inline void write_fmt(Log& logger,
								  LogLevel lvl,
								  std::source_location loc,
								  std::format_string<Args...> fmt,
								  Args&&... args)
			{
				if (!should_log(logger, lvl)) return;

				// Note: we want call-site source info, not inside logger, so we pass it down explicitly.
				logger.log(lvl, make_source_info(loc), std::format(fmt, std::forward<Args>(args)...));
			}

			inline void write_mark(Log& logger,
								   LogLevel lvl,
								   std::source_location loc)
			{
				if (!should_log(logger, lvl)) return;
				// "header only" (empty message) — sinks still see source info + level.
				logger.log(lvl, make_source_info(loc), std::string{});
			}

			template <class F>
			inline void write_lazy(Log& logger,
								   LogLevel lvl,
								   std::source_location loc,
								   F&& fn)
			{
				if (!should_log(logger, lvl)) return;

				static_assert(std::is_invocable_v<F&>,
							  "lazy logger expects a callable with signature: () -> string/string_view");

				logger.log(lvl, make_source_info(loc), to_string(std::invoke(fn)));
			}

		} // namespace detail

#ifdef A3D_DEBUG
		// -------- TRACE --------
		template <class... Args>
		inline void t(std::format_string<Args...> fmt, Args&&... args)
		{
			detail::write_fmt(Log::MainLog(), LogLevel::Trace,
							  std::source_location::current(),
							  fmt, std::forward<Args>(args)...);
		}

		inline void t()
		{
			detail::write_mark(Log::MainLog(), LogLevel::Trace,
							   std::source_location::current());
		}

		template <class F>
		inline void t_lazy(F&& fn)
		{
			detail::write_lazy(Log::MainLog(), LogLevel::Trace,
							   std::source_location::current(),
							   std::forward<F>(fn));
		}

		// explicit-loc versions (for macros)
		template <class... Args>
		inline void t_at(std::source_location loc,
						 std::format_string<Args...> fmt, Args&&... args)
		{
			detail::write_fmt(Log::MainLog(), LogLevel::Trace,
							  loc, fmt, std::forward<Args>(args)...);
		}

		inline void t_at(std::source_location loc)
		{
			detail::write_mark(Log::MainLog(), LogLevel::Trace, loc);
		}

		template <class F>
		inline void t_lazy_at(std::source_location loc, F&& fn)
		{
			detail::write_lazy(Log::MainLog(), LogLevel::Trace,
							   loc, std::forward<F>(fn));
		}

		// -------- DEBUG --------
		template <class... Args>
		inline void d(std::format_string<Args...> fmt, Args&&... args)
		{
			detail::write_fmt(Log::MainLog(), LogLevel::Debug,
							  std::source_location::current(),
							  fmt, std::forward<Args>(args)...);
		}

		inline void d()
		{
			detail::write_mark(Log::MainLog(), LogLevel::Debug,
							   std::source_location::current());
		}

		template <class F>
		inline void d_lazy(F&& fn)
		{
			detail::write_lazy(Log::MainLog(), LogLevel::Debug,
							   std::source_location::current(),
							   std::forward<F>(fn));
		}

		template <class... Args>
		inline void d_at(std::source_location loc,
						 std::format_string<Args...> fmt, Args&&... args)
		{
			detail::write_fmt(Log::MainLog(), LogLevel::Debug,
							  loc, fmt, std::forward<Args>(args)...);
		}

		inline void d_at(std::source_location loc)
		{
			detail::write_mark(Log::MainLog(), LogLevel::Debug, loc);
		}

		template <class F>
		inline void d_lazy_at(std::source_location loc, F&& fn)
		{
			detail::write_lazy(Log::MainLog(), LogLevel::Debug,
							   loc, std::forward<F>(fn));
		}

		// -------- INFO --------
		template <class... Args>
		inline void i(std::format_string<Args...> fmt, Args&&... args)
		{
			detail::write_fmt(Log::MainLog(), LogLevel::Info,
							  std::source_location::current(),
							  fmt, std::forward<Args>(args)...);
		}

		inline void i()
		{
			detail::write_mark(Log::MainLog(), LogLevel::Info,
							   std::source_location::current());
		}

		template <class F>
		inline void i_lazy(F&& fn)
		{
			detail::write_lazy(Log::MainLog(), LogLevel::Info,
							   std::source_location::current(),
							   std::forward<F>(fn));
		}

		template <class... Args>
		inline void i_at(std::source_location loc,
						 std::format_string<Args...> fmt, Args&&... args)
		{
			detail::write_fmt(Log::MainLog(), LogLevel::Info,
							  loc, fmt, std::forward<Args>(args)...);
		}

		inline void i_at(std::source_location loc)
		{
			detail::write_mark(Log::MainLog(), LogLevel::Info, loc);
		}

		template <class F>
		inline void i_lazy_at(std::source_location loc, F&& fn)
		{
			detail::write_lazy(Log::MainLog(), LogLevel::Info,
							   loc, std::forward<F>(fn));
		}

// -------- WARN --------
		template <class... Args>
		inline void w(std::format_string<Args...> fmt, Args&&... args)
		{
			detail::write_fmt(Log::MainLog(), LogLevel::Warn,
							  std::source_location::current(),
							  fmt, std::forward<Args>(args)...);
		}

		inline void w()
		{
			detail::write_mark(Log::MainLog(), LogLevel::Warn,
							   std::source_location::current());
		}

		template <class F>
		inline void w_lazy(F&& fn)
		{
			detail::write_lazy(Log::MainLog(), LogLevel::Warn,
							   std::source_location::current(),
							   std::forward<F>(fn));
		}

		template <class... Args>
		inline void w_at(std::source_location loc,
						 std::format_string<Args...> fmt, Args&&... args)
		{
			detail::write_fmt(Log::MainLog(), LogLevel::Warn,
							  loc, fmt, std::forward<Args>(args)...);
		}

		inline void w_at(std::source_location loc)
		{
			detail::write_mark(Log::MainLog(), LogLevel::Warn, loc);
		}

		template <class F>
		inline void w_lazy_at(std::source_location loc, F&& fn)
		{
			detail::write_lazy(Log::MainLog(), LogLevel::Warn,
							   loc, std::forward<F>(fn));
		}

// -------- ERROR --------
		template <class... Args>
		inline void e(std::format_string<Args...> fmt, Args&&... args)
		{
			detail::write_fmt(Log::MainLog(), LogLevel::Error,
							  std::source_location::current(),
							  fmt, std::forward<Args>(args)...);
		}

		inline void e()
		{
			detail::write_mark(Log::MainLog(), LogLevel::Error,
							   std::source_location::current());
		}

		template <class F>
		inline void e_lazy(F&& fn)
		{
			detail::write_lazy(Log::MainLog(), LogLevel::Error,
							   std::source_location::current(),
							   std::forward<F>(fn));
		}

		template <class... Args>
		inline void e_at(std::source_location loc,
						 std::format_string<Args...> fmt, Args&&... args)
		{
			detail::write_fmt(Log::MainLog(), LogLevel::Error,
							  loc, fmt, std::forward<Args>(args)...);
		}

		inline void e_at(std::source_location loc)
		{
			detail::write_mark(Log::MainLog(), LogLevel::Error, loc);
		}

		template <class F>
		inline void e_lazy_at(std::source_location loc, F&& fn)
		{
			detail::write_lazy(Log::MainLog(), LogLevel::Error,
							   loc, std::forward<F>(fn));
		}

// -------- FATAL --------
		template <class... Args>
		inline void f(std::format_string<Args...> fmt, Args&&... args)
		{
			detail::write_fmt(Log::MainLog(), LogLevel::Fatal,
							  std::source_location::current(),
							  fmt, std::forward<Args>(args)...);
		}

		inline void f()
		{
			detail::write_mark(Log::MainLog(), LogLevel::Fatal,
							   std::source_location::current());
		}

		template <class F>
		inline void f_lazy(F&& fn)
		{
			detail::write_lazy(Log::MainLog(), LogLevel::Fatal,
							   std::source_location::current(),
							   std::forward<F>(fn));
		}

		template <class... Args>
		inline void f_at(std::source_location loc,
						 std::format_string<Args...> fmt, Args&&... args)
		{
			detail::write_fmt(Log::MainLog(), LogLevel::Fatal,
							  loc, fmt, std::forward<Args>(args)...);
		}

		inline void f_at(std::source_location loc)
		{
			detail::write_mark(Log::MainLog(), LogLevel::Fatal, loc);
		}

		template <class F>
		inline void f_lazy_at(std::source_location loc, F&& fn)
		{
			detail::write_lazy(Log::MainLog(), LogLevel::Fatal,
							   loc, std::forward<F>(fn));
		}

#else
		// If you want, you can provide no-op versions for release here.
#endif

		namespace app {

#ifdef A3D_DEBUG
			// -------- TRACE --------
			template <class... Args>
			inline void t_at(std::source_location loc,
							 std::format_string<Args...> fmt, Args&&... args)
			{
				detail::write_fmt(Log::AppLog(), LogLevel::Trace, loc, fmt, std::forward<Args>(args)...);
			}

			inline void t_at(std::source_location loc)
			{
				detail::write_mark(Log::AppLog(), LogLevel::Trace, loc);
			}

			template <class F>
			inline void t_lazy_at(std::source_location loc, F&& fn)
			{
				detail::write_lazy(Log::AppLog(), LogLevel::Trace, loc, std::forward<F>(fn));
			}

			// -------- DEBUG --------
			template <class... Args>
			inline void d_at(std::source_location loc,
							 std::format_string<Args...> fmt, Args&&... args)
			{
				detail::write_fmt(Log::AppLog(), LogLevel::Debug, loc, fmt, std::forward<Args>(args)...);
			}

			inline void d_at(std::source_location loc)
			{
				detail::write_mark(Log::AppLog(), LogLevel::Debug, loc);
			}

			template <class F>
			inline void d_lazy_at(std::source_location loc, F&& fn)
			{
				detail::write_lazy(Log::AppLog(), LogLevel::Debug, loc, std::forward<F>(fn));
			}
#else
			// no-ops for release
	template <class... Args>
	inline void t_at(std::source_location, std::format_string<Args...>, Args&&...) {}
	inline void t_at(std::source_location) {}
	template <class F>
	inline void t_lazy_at(std::source_location, F&&) {}

	template <class... Args>
	inline void d_at(std::source_location, std::format_string<Args...>, Args&&...) {}
	inline void d_at(std::source_location) {}
	template <class F>
	inline void d_lazy_at(std::source_location, F&&) {}
#endif

			// -------- INFO --------
			template <class... Args>
			inline void i_at(std::source_location loc,
							 std::format_string<Args...> fmt, Args&&... args)
			{
				detail::write_fmt(Log::AppLog(), LogLevel::Info, loc, fmt, std::forward<Args>(args)...);
			}

			inline void i_at(std::source_location loc)
			{
				detail::write_mark(Log::AppLog(), LogLevel::Info, loc);
			}

			template <class F>
			inline void i_lazy_at(std::source_location loc, F&& fn)
			{
				detail::write_lazy(Log::AppLog(), LogLevel::Info, loc, std::forward<F>(fn));
			}

			// -------- WARN --------
			template <class... Args>
			inline void w_at(std::source_location loc,
							 std::format_string<Args...> fmt, Args&&... args)
			{
				detail::write_fmt(Log::AppLog(), LogLevel::Warn, loc, fmt, std::forward<Args>(args)...);
			}

			inline void w_at(std::source_location loc)
			{
				detail::write_mark(Log::AppLog(), LogLevel::Warn, loc);
			}

			template <class F>
			inline void w_lazy_at(std::source_location loc, F&& fn)
			{
				detail::write_lazy(Log::AppLog(), LogLevel::Warn, loc, std::forward<F>(fn));
			}

			// -------- ERROR --------
			template <class... Args>
			inline void e_at(std::source_location loc,
							 std::format_string<Args...> fmt, Args&&... args)
			{
				detail::write_fmt(Log::AppLog(), LogLevel::Error, loc, fmt, std::forward<Args>(args)...);
			}

			inline void e_at(std::source_location loc)
			{
				detail::write_mark(Log::AppLog(), LogLevel::Error, loc);
			}

			template <class F>
			inline void e_lazy_at(std::source_location loc, F&& fn)
			{
				detail::write_lazy(Log::AppLog(), LogLevel::Error, loc, std::forward<F>(fn));
			}

			// -------- FATAL --------
			template <class... Args>
			inline void f_at(std::source_location loc,
							 std::format_string<Args...> fmt, Args&&... args)
			{
				detail::write_fmt(Log::AppLog(), LogLevel::Fatal, loc, fmt, std::forward<Args>(args)...);
			}

			inline void f_at(std::source_location loc)
			{
				detail::write_mark(Log::AppLog(), LogLevel::Fatal, loc);
			}

			template <class F>
			inline void f_lazy_at(std::source_location loc, F&& fn)
			{
				detail::write_lazy(Log::AppLog(), LogLevel::Fatal, loc, std::forward<F>(fn));
			}

		} // namespace app
	} // namespace log
} // namespace a3d





#ifdef A3D_DEBUG
#define A3D_APP_LOG_T(fmtStr, ...) ::a3d::log::app::t_at(std::source_location::current(), (fmtStr) __VA_OPT__(,) __VA_ARGS__)
#define A3D_APP_LOG_D(fmtStr, ...) ::a3d::log::app::d_at(std::source_location::current(), (fmtStr) __VA_OPT__(,) __VA_ARGS__)
#else
#define A3D_APP_LOG_T(fmtStr, ...) NOOP
	#define A3D_APP_LOG_D(fmtStr, ...) NOOP
#endif

#define A3D_APP_LOG_I(fmtStr, ...) ::a3d::log::app::i_at(std::source_location::current(), (fmtStr) __VA_OPT__(,) __VA_ARGS__)
#define A3D_APP_LOG_W(fmtStr, ...) ::a3d::log::app::w_at(std::source_location::current(), (fmtStr) __VA_OPT__(,) __VA_ARGS__)
#define A3D_APP_LOG_E(fmtStr, ...) ::a3d::log::app::e_at(std::source_location::current(), (fmtStr) __VA_OPT__(,) __VA_ARGS__)
#define A3D_APP_LOG_F(fmtStr, ...) ::a3d::log::app::f_at(std::source_location::current(), (fmtStr) __VA_OPT__(,) __VA_ARGS__)

#ifdef A3D_DEBUG
#define A3D_LOG_T(fmtStr, ...) ::a3d::log::t_at(std::source_location::current(), (fmtStr) __VA_OPT__(,) __VA_ARGS__)
#define A3D_LOG_D(fmtStr, ...) ::a3d::log::d_at(std::source_location::current(), (fmtStr) __VA_OPT__(,) __VA_ARGS__)
#else
#define A3D_LOG_T(fmtStr, ...) NOOP
#define A3D_LOG_D(fmtStr, ...) NOOP
#endif

#define A3D_LOG_I(fmtStr, ...) ::a3d::log::i_at(std::source_location::current(), (fmtStr) __VA_OPT__(,) __VA_ARGS__)
#define A3D_LOG_W(fmtStr, ...) ::a3d::log::w_at(std::source_location::current(), (fmtStr) __VA_OPT__(,) __VA_ARGS__)
#define A3D_LOG_E(fmtStr, ...) ::a3d::log::e_at(std::source_location::current(), (fmtStr) __VA_OPT__(,) __VA_ARGS__)
#define A3D_LOG_F(fmtStr, ...) ::a3d::log::f_at(std::source_location::current(), (fmtStr) __VA_OPT__(,) __VA_ARGS__)

#endif /* AVARA3D_LOG_H */
