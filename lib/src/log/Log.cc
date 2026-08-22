//
//  Log.cc
//  avara3d
//
//  Created by Morgan Davis on 1/5/18.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#include "a3d/log/Log.h"

#include <cstring>
#include <ctime>
#include <type_traits>

#ifdef A3D_POSIX
    #include <sys/time.h>
#endif

#include "a3d/log/sink/LogSink.h"
#include "a3d/log/sink/StdOutLogSink.h"
#include "a3d/util/Enum.h"

#if defined(_MSC_VER)
static_assert(_MSVC_LANG >= 202002L, "C++20 (/std:c++20) is required for std::source_location");
#else
static_assert(__cplusplus >= 202002L, "C++20 is required for std::source_location");
#endif

using namespace a3d;
using namespace std;

// [Private Static Non-Member Prototypes]

static string TimestampString();
static string HeaderString(const string& logName, Log::Level level, const Log::SourceInfo& sourceInfo);
static string HeaderString(const string& logName, Log::Level level);

// [Public Static Member Functions]

Log& Log::AppLog() {
    if (_appLog) {
        return *_appLog;
    }

    return MainLog();
}

void Log::AppLog(unique_ptr<Log> log) {
    _appLog = std::move(log);
}

Log& Log::MainLog() {
    static Log main {"a3d", std::make_unique<a3d::log::StdOutLogSink>(), DEFAULT_LEVEL, DEFAULT_FLUSH_LEVEL};
    return main;
}

// [Public Types]

Log::Entry::Entry(Log& logger, Log::Level level, SourceInfo source):
    _logger(&logger),
    _level(level),
    _source(std::move(source)) {}

void Log::Entry::operator()(string_view msg) const {
    nl(msg);
}

void Log::Entry::nl(string_view msg) const {
    _logger->log(_level, _source, string(msg));
}

void Log::Entry::raw(string_view msg) const {
    _logger->write(_level, _source, msg);
}

// [Public Lifecycle Functions]

Log::Log():
    _name("unnamed"),
    _level(DEFAULT_LEVEL),
    _flushLevel(DEFAULT_FLUSH_LEVEL) {}

Log::Log(const string& name, unique_ptr<LogSink> sink, Log::Level level, Log::Level flushLevel):
    _name(name),
    _level(level),
    _flushLevel(flushLevel) {
    if (sink) {
        _sinks.emplace_back(std::move(sink));
    }
}

Log::Log(const string& name, vector<unique_ptr<LogSink>> sinks, Log::Level level, Log::Level flushLevel):
    _name(name),
    _sinks(std::move(sinks)),
    _level(level),
    _flushLevel(flushLevel) {}

Log::~Log() = default;

// [Public Member Functions]

const string& Log::name() const {
    return _name;
}

const vector<unique_ptr<LogSink>>& Log::sinks() const {
    return _sinks;
}

Log::Level Log::level() const {
    return _level;
}

void Log::level(Log::Level level) {
    _level = level;
}

Log::Level Log::flushLevel() const {
    return _flushLevel;
}

void Log::flushLevel(Log::Level flushLevel) {
    _flushLevel = flushLevel;
}

void Log::trace(const string& msg) {
    log(Log::Level::Trace, msg);
}

void Log::debug(const string& msg) {
    log(Log::Level::Debug, msg);
}

void Log::info(const string& msg) {
    log(Log::Level::Info, msg);
}

void Log::warn(const string& msg) {
    log(Log::Level::Warn, msg);
}

void Log::error(const string& msg) {
    log(Log::Level::Error, msg);
}

void Log::fatal(const string& msg) {
    log(Log::Level::Fatal, msg);
}

void Log::log(Log::Level level, const SourceInfo& sourceInfo, const string& msg) {
    if (!enabled(level)) {
        return;
    }

    // exactly old semantics: header + space + msg + '\n'
    auto header = HeaderString(_name, level, sourceInfo);

    string line;
    line.reserve(header.size() + 1 + msg.size() + 1);
    line.append(header);
    line.push_back(' ');
    line.append(msg);
    line.push_back('\n');

    dispatch(level, line);
}

void Log::flush() {
    for (auto& s : _sinks) {
        if (s) {
            s->flush();
        }
    }
}

// [Internal Member Functions]

Log::Entry Log::trace(std::source_location where) {
    return Entry {*this, Log::Level::Trace, MakeSourceInfo(where)};
}

Log::Entry Log::debug(std::source_location where) {
    return Entry {*this, Log::Level::Debug, MakeSourceInfo(where)};
}

Log::Entry Log::info(std::source_location where) {
    return Entry {*this, Log::Level::Info, MakeSourceInfo(where)};
}

Log::Entry Log::warn(std::source_location where) {
    return Entry {*this, Log::Level::Warn, MakeSourceInfo(where)};
}

Log::Entry Log::error(std::source_location where) {
    return Entry {*this, Log::Level::Error, MakeSourceInfo(where)};
}

Log::Entry Log::fatal(std::source_location where) {
    return Entry {*this, Log::Level::Fatal, MakeSourceInfo(where)};
}

void Log::write(Log::Level level, const SourceInfo& sourceInfo, string_view msg) {
    if (!enabled(level)) {
        return;
    }

    auto header = HeaderString(_name, level, sourceInfo);

    string out;
    out.reserve(header.size() + 1 + msg.size());
    out.append(header);
    out.push_back(' ');
    out.append(msg); // no '\n'

    dispatch(level, out);
}

void Log::write(Log::Level level, string_view msg) {
    if (!enabled(level)) {
        return;
    }

    auto header = HeaderString(_name, level);

    string out;
    out.reserve(header.size() + 1 + msg.size());
    out.append(header);
    out.push_back(' ');
    out.append(msg); // no '\n'

    dispatch(level, out);
}

// [Private Static Member Functions]

string_view Log::Basename(string_view p) {
    const size_t slash = p.find_last_of("/\\");
    return (slash == string_view::npos) ? p : p.substr(slash + 1);
}

string_view Log::ShortFunction(string_view s, int keepScopes) {

    if (const auto pos = s.find('('); pos != string_view::npos) {
        s = s.substr(0, pos);
    }

    if (const auto sp = s.find_last_of(' '); sp != string_view::npos) {
        s = s.substr(sp + 1);
    }

    for (int i = 0; i < keepScopes; ++i) {
        const auto pos = s.rfind("::");
        if (pos == string_view::npos) {
            break;
        }

        const auto prev = s.rfind("::", pos - 1);
        if (prev == string_view::npos) {
            break;
        }

        s = s.substr(prev + 2);
    }

    return s;
}

Log::SourceInfo Log::MakeSourceInfo(const std::source_location& where) {
    const auto file = Basename(where.file_name());
    const auto func = ShortFunction(where.function_name(), /*keepScopes=*/2);
    return SourceInfo {file, static_cast<unsigned>(where.line()), func};
}

// [Private Lifecycle]

Log::Log(const string& name):
    _name(name),
    _level(DEFAULT_LEVEL),
    _flushLevel(DEFAULT_FLUSH_LEVEL) {}

// [Private Member Functions]

void Log::log(Log::Level level, const string& msg) {
    if (!enabled(level)) {
        return;
    }

    auto header = HeaderString(_name, level);

    string line;
    line.reserve(header.size() + 1 + msg.size() + 1);
    line.append(header);
    line.push_back(' ');
    line.append(msg);
    line.push_back('\n');

    dispatch(level, line);
}

void Log::dispatch(Log::Level level, string& output) {
    for (auto& sink : _sinks) {
        sink->write(output, level);
    }

    if (static_cast<std::underlying_type_t<Log::Level>>(level)
        >= static_cast<std::underlying_type_t<Log::Level>>(_flushLevel)) {
        flush();
    }
}

bool Log::enabled(Log::Level level) const {
    if (level == Log::Level::Off) {
        return false;
    }
    if (_level == Log::Level::Off) {
        return false;
    }
    return static_cast<std::underlying_type_t<Log::Level>>(level)
           >= static_cast<std::underlying_type_t<Log::Level>>(_level);
}

// [Private Static Non-Member Functions]

static string TimestampString() {
    constexpr size_t BUF_SIZE = 256;
    char             buf[BUF_SIZE];

#ifdef A3D_WINDOWS
    time_t     rawtime;
    struct tm* timeinfo;
    time(&rawtime);
    timeinfo = localtime(&rawtime);
    strftime(buf, sizeof(buf), "%Y-%m-%d %I:%M:%S", timeinfo);
    return string(buf);
#else
    timeval curTime;
    gettimeofday(&curTime, NULL); // gettimeofday() is POSIX
    const int milli = curTime.tv_usec / 1000;
    strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", localtime(&curTime.tv_sec));
    char msBuf[std::strlen(buf) + 5];
    snprintf(msBuf, sizeof(msBuf), "%s.%03d", buf, milli);
    return string(msBuf);
#endif
}

static string HeaderString(const string& logName, Log::Level level, const Log::SourceInfo& sourceInfo) {
    return std::format("{} [{}] [{}] [{}:{}] [{}()]", TimestampString(), logName, util::enums::enum_name(level),
                       sourceInfo.filename, sourceInfo.line, sourceInfo.function);
}

// [Private Static Member Variables]

unique_ptr<Log> Log::_appLog {};

static string HeaderString(const string& logName, Log::Level level) {
    return std::format("{} [{}] [{}]", TimestampString(), logName, util::enums::enum_name(level));
}
