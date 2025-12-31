// Log.cc
#include "a3d/diagnostic/log/Log.h"

#include <cstring>
#include <ctime>
#include <format>
#include <type_traits>
#include <utility>

#ifdef A3D_POSIX
#include <sys/time.h>
#endif

#include "magic_enum/magic_enum.hpp"

#include "a3d/diagnostic/log/sink/LogSink.h"
#include "a3d/diagnostic/log/sink/StdOutLogSink.h" // default MainLog sink

namespace a3d {

	/// Private Static Prototypes ///
	static std::string TimestampString();
	static std::string HeaderString(const std::string& logName, LogLevel level,
									const Log::SourceInfo& sourceInfo);
	static std::string HeaderString(const std::string& logName, LogLevel level);

	std::optional<Log> Log::_appLog{};

	static constexpr auto to_u(LogLevel lvl) -> std::underlying_type_t<LogLevel> {
		return static_cast<std::underlying_type_t<LogLevel>>(lvl);
	}

	/// Public Static Member Functions ///

	Log& Log::AppLog() {
		if (_appLog.has_value()) return *_appLog;
		return MainLog();
	}

	void Log::AppLog(Log log) {
		_appLog = std::move(log);
	}

	Log& Log::MainLog() {
		static Log main{
				"a3d",
				std::make_unique<StdOutLogSink>(),
				DEFAULT_LEVEL,
				DEFAULT_FLUSH_LEVEL
		};
		return main;
	}

	/// Public Lifecycle Functions ///

	Log::Log(const std::string& name,
			 std::unique_ptr<LogSink> sink,
			 LogLevel level,
			 LogLevel flushLevel)
			: _name(name),
			  _level(level),
			  _flushLevel(flushLevel) {
		if (sink) _sinks.emplace_back(std::move(sink));
	}

	Log::Log(const std::string& name,
			 std::vector<std::unique_ptr<LogSink>> sinks,
			 LogLevel level,
			 LogLevel flushLevel)
			: _name(name),
			  _sinks(std::move(sinks)),
			  _level(level),
			  _flushLevel(flushLevel) {}

	Log::Log()
			: _name("unnamed"),
			  _level(DEFAULT_LEVEL),
			  _flushLevel(DEFAULT_FLUSH_LEVEL) {}

	Log::Log(const std::string& name)
			: _name(name),
			  _level(DEFAULT_LEVEL),
			  _flushLevel(DEFAULT_FLUSH_LEVEL) {}

	Log::~Log() = default;

	/// Public Member Functions ///

	const std::string& Log::name() const { return _name; }
	const std::vector<std::unique_ptr<LogSink>>& Log::sinks() const { return _sinks; }

	LogLevel Log::level() const { return _level; }
	void Log::level(LogLevel level) { _level = level; }

	LogLevel Log::flushLevel() const { return _flushLevel; }
	void Log::flushLevel(LogLevel flushLevel) { _flushLevel = flushLevel; }

	bool Log::enabled(LogLevel lvl) const {
		if (lvl == LogLevel::Off) return false;
		if (_level == LogLevel::Off) return false;
		return to_u(lvl) >= to_u(_level);
	}

	void Log::trace(const std::string& msg) { log(LogLevel::Trace, msg); }
	void Log::debug(const std::string& msg) { log(LogLevel::Debug, msg); }
	void Log::info (const std::string& msg) { log(LogLevel::Info,  msg); }
	void Log::warn (const std::string& msg) { log(LogLevel::Warn,  msg); }
	void Log::error(const std::string& msg) { log(LogLevel::Error, msg); }
	void Log::fatal(const std::string& msg) { log(LogLevel::Fatal, msg); }

	void Log::write(LogLevel level, const SourceInfo& sourceInfo, std::string_view msg) {
		if (!enabled(level)) return;

		auto header = HeaderString(_name, level, sourceInfo);

		std::string out;
		out.reserve(header.size() + 1 + msg.size());
		out.append(header);
		out.push_back(' ');
		out.append(msg); // no '\n'

		dispatch(level, out);
	}

	void Log::write(LogLevel level, std::string_view msg) {
		if (!enabled(level)) return;

		auto header = HeaderString(_name, level);

		std::string out;
		out.reserve(header.size() + 1 + msg.size());
		out.append(header);
		out.push_back(' ');
		out.append(msg); // no '\n'

		dispatch(level, out);
	}

	void Log::log(LogLevel level, const SourceInfo& sourceInfo, const std::string& msg) {
		if (!enabled(level)) return;

		// exactly old semantics: header + space + msg + '\n'
		auto header = HeaderString(_name, level, sourceInfo);

		std::string line;
		line.reserve(header.size() + 1 + msg.size() + 1);
		line.append(header);
		line.push_back(' ');
		line.append(msg);
		line.push_back('\n');

		dispatch(level, line);
	}

	void Log::log(LogLevel level, const std::string& msg) {
		if (!enabled(level)) return;

		auto header = HeaderString(_name, level);

		std::string line;
		line.reserve(header.size() + 1 + msg.size() + 1);
		line.append(header);
		line.push_back(' ');
		line.append(msg);
		line.push_back('\n');

		dispatch(level, line);
	}

	void Log::dispatch(LogLevel level, std::string& output) {
		for (auto& sink : _sinks) sink->write(output, level);

		if (to_u(level) >= to_u(_flushLevel)) {
			flush();
		}
	}

	void Log::flush() {
		for (auto& s : _sinks) {
			if (s) s->flush();
		}
	}

	/// Source-location Entry overloads (out-of-line) ///

	Log::Entry Log::trace(std::source_location where) { return Entry{*this, LogLevel::Trace, MakeSourceInfo(where)}; }
	Log::Entry Log::debug(std::source_location where) { return Entry{*this, LogLevel::Debug, MakeSourceInfo(where)}; }
	Log::Entry Log::info (std::source_location where) { return Entry{*this, LogLevel::Info,  MakeSourceInfo(where)}; }
	Log::Entry Log::warn (std::source_location where) { return Entry{*this, LogLevel::Warn,  MakeSourceInfo(where)}; }
	Log::Entry Log::error(std::source_location where) { return Entry{*this, LogLevel::Error, MakeSourceInfo(where)}; }
	Log::Entry Log::fatal(std::source_location where) { return Entry{*this, LogLevel::Fatal, MakeSourceInfo(where)}; }

	/// Entry (non-template) out-of-line ///

	Log::Entry::Entry(Log& logger, LogLevel lvl, SourceInfo src)
			: _logger(&logger), _lvl(lvl), _src(std::move(src)) {}

	void Log::Entry::operator()(std::string_view msg) const {
		nl(msg);
	}

	void Log::Entry::nl(std::string_view msg) const {
		_logger->log(_lvl, _src, std::string(msg));
	}

	void Log::Entry::raw(std::string_view msg) const {
		_logger->write(_lvl, _src, msg);
	}

	/// MakeSourceInfo helpers (out-of-line) ///

	std::string_view Log::Basename(std::string_view p) {
		const size_t slash = p.find_last_of("/\\");
		return (slash == std::string_view::npos) ? p : p.substr(slash + 1);
	}

	std::string_view Log::ShortFunction(std::string_view s, int keepScopes) {
		// 1) Drop everything after the first '('
		if (const auto pos = s.find('('); pos != std::string_view::npos)
			s = s.substr(0, pos);

		// 2) Drop return type (everything before last space)
		if (const auto sp = s.find_last_of(' '); sp != std::string_view::npos)
			s = s.substr(sp + 1);

		// 3) Keep last N "::" scope components
		// (this matches your previous intent; keeps context but chops the insane template tail)
		for (int i = 0; i < keepScopes; ++i) {
			const auto pos = s.rfind("::");
			if (pos == std::string_view::npos) break;

			const auto prev = s.rfind("::", pos - 1);
			if (prev == std::string_view::npos) break;

			s = s.substr(prev + 2);
		}

		return s;
	}

	Log::SourceInfo Log::MakeSourceInfo(const std::source_location& where) {
		const auto file = Basename(where.file_name());
		const auto func = ShortFunction(where.function_name(), /*keepScopes=*/2);
		return SourceInfo{ std::string(file), static_cast<unsigned>(where.line()), std::string(func) };
	}

	/// Private Static Functions ///

	static std::string TimestampString() {
		constexpr size_t BUF_SIZE = 256;
		char buf[BUF_SIZE];

#ifdef A3D_WINDOWS
		time_t rawtime;
		struct tm* timeinfo;
		time(&rawtime);
		timeinfo = localtime(&rawtime);
		strftime(buf, sizeof(buf), "%Y-%m-%d %I:%M:%S", timeinfo);
		return std::string(buf);
#else
		timeval curTime;
		gettimeofday(&curTime, NULL); // gettimeofday() is POSIX
		const int milli = curTime.tv_usec / 1000;
		strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", localtime(&curTime.tv_sec));
		char msBuf[std::strlen(buf) + 5];
		snprintf(msBuf, sizeof(msBuf), "%s.%03d", buf, milli);
		return std::string(msBuf);
#endif
	}

	static std::string HeaderString(const std::string& logName, LogLevel level,
									const Log::SourceInfo& sourceInfo) {
		return std::format("{} [{}] [{}] [{}:{}] [{}()]",
						   TimestampString(),
						   logName,
						   magic_enum::enum_name(level),
						   std::get<0>(sourceInfo),
						   std::get<1>(sourceInfo),
						   std::get<2>(sourceInfo));
	}

	static std::string HeaderString(const std::string& logName, LogLevel level) {
		return std::format("{} [{}] [{}]",
						   TimestampString(),
						   logName,
						   magic_enum::enum_name(level));
	}

} // namespace a3d
