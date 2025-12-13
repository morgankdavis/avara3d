//
//  Log.cc
//  avara3d
//
//  Created by Morgan Davis on 1/5/18.
//  Copyright © 2024 Morgan K Davis. All rights reserved.
//

#include "a3d/diagnostic/log/Log.h"

#include <cstring>
#include <ctime>
#include <format>
#include <utility>

#ifdef POSIX
#include <sys/time.h>
#endif

#include "magic_enum.hpp"

#include "a3d/Utilities.h"
#include "a3d/diagnostic/exception/Exception.h"
#include "a3d/diagnostic/log/sink/LogSink.h"
#include "a3d/diagnostic/log/sink/FileLogSink.h"
#include "a3d/diagnostic/log/sink/StdOutLogSink.h"

using namespace a3d;
using namespace std;

/// Internal Static Member Functions ///

Log& Log::MainLog() {

	static Log log("a3d");
	static bool initialized = false;

	if (!initialized) {

		string executableName = *utils::ExecutableName();
		auto nativeSink = make_unique<StdOutLogSink>();
		auto fileSink = make_unique<FileLogSink>(*(utils::ExecutableDirectory()) / "a3d.log");

		auto sinks = vector<unique_ptr<LogSink>>();
		sinks.push_back(std::move(nativeSink));
		sinks.push_back(std::move(fileSink));

		log._sinks = std::move(sinks);
		//logger = make_unique<Log>("a3d", std::move(sinks));

		initialized = true;
	}

	return log;
}

/// Public Static Member Functions ///

Log& Log::AppLog() {
	return *_appLog;
}

void Log::AppLog(unique_ptr<Log> log) {
	_appLog = std::move(log);
}

//void Log::AppLog(const Log& log) {
//	_appLog = std::move(log);
//}

/// Private Static Prototypes ///

string TimestampString();
string HeaderString(const string& logName, LogLevel level,
					const Log::SourceInfo& sourceInfo);
string HeaderString(const string& logName, LogLevel level);

/// Public Lifecycle Functions ///

Log::Log(const string& name,
		 unique_ptr<LogSink> sink,
		 LogLevel level,
		 LogLevel flushLevel):
		_name{name},
		_sinks{},
		_level{level},
		_flushLevel{flushLevel} {

	_sinks.push_back(std::move(sink));
}

Log::Log(const string& name,
		 vector<unique_ptr<LogSink>> sinks,
		 LogLevel level,
		 LogLevel flushLevel):
		_name{name},
		_sinks{std::move(sinks)},
		_level{level},
		_flushLevel{flushLevel} { }

Log::Log() { }

/// Public Member Functions ///

const string& Log::name() const {
	return _name;
}

const vector<unique_ptr<LogSink>>& Log::sinks() const {
	return _sinks;
}

LogLevel Log::level() const {
	return _level;
}

void Log::level(LogLevel level) {
	_level = level;
}

LogLevel Log::flushLevel() const {
	return _flushLevel;
}

void Log::flushLevel(LogLevel level) {
	_flushLevel = level;
}

void Log::trace(const string& msg) {
	log(LogLevel::Trace, msg);
}

void Log::debug(const string& msg) {
	log(LogLevel::Debug, msg);
}

void Log::info(const string& msg) {
	log(LogLevel::Info, msg);
}

void Log::warn(const string& msg) {
	log(LogLevel::Warn, msg);
}

void Log::error(const string& msg) {
	log(LogLevel::Error, msg);
}

void Log::fatal(const string& msg) {
	log(LogLevel::Fatal, msg);
}

void Log::log(LogLevel level,
			  const SourceInfo& sourceInfo,
			  const std::string& msg) {

	if (static_cast<underlying_type<LogLevel>::type>(level)
		>= static_cast<underlying_type<LogLevel>::type>(_level)) {

		auto lineStr = std::format("{} {}\n",
								   HeaderString(_name, level, sourceInfo),
								   msg);
		dispatch(level, lineStr);
	}
}

void Log::flush() {

	for (auto& sink : _sinks) {
		sink->flush();
	}
}

/// Private Lifecycle ///

Log::Log(const std::string& name):
		_name{name},
		_sinks{},
		_level{DEFAULT_LEVEL},
		_flushLevel{DEFAULT_FLUSH_LEVEL} {}

/// Private  Member Functions ///

void Log::log(LogLevel level,
			  const std::string& msg) {

	if (static_cast<underlying_type<LogLevel>::type>(level)
		>= static_cast<underlying_type<LogLevel>::type>(_level)) {

		auto lineStr = std::format("{} {}\n",
								   HeaderString(_name, level),
								   msg);
		dispatch(level, lineStr);
	}
}

void Log::dispatch(LogLevel level, std::string& output) {

	for (auto& sink : _sinks) {

		if (auto stdOutSink = dynamic_cast<StdOutLogSink*>(sink.get())) {
			stdOutSink->write(output, level);
		}

		if (auto fileLogSink = dynamic_cast<FileLogSink*>(sink.get())) {
			fileLogSink->write(output);
		}
	}

	if (static_cast<underlying_type<LogLevel>::type>(level)
		>= static_cast<underlying_type<LogLevel>::type>(_flushLevel)) {
		flush();
	}
}

/// Private Static Functions ///

// TODO: move to utilities?
string TimestampString() {

	constexpr size_t BUF_SIZE = 256;
	char buf[BUF_SIZE];
#ifdef WINDOWS
	time_t rawtime;
	struct tm * timeinfo;
	time(&rawtime);
	timeinfo = localtime(&rawtime);
	strftime(buf, sizeof(buf), "%Y-%m-%d %I:%M:%S", timeinfo);
	return string(buf);
#else
	timeval curTime;
	gettimeofday(&curTime, NULL); // gettimeofday() is POSIX
	int milli = curTime.tv_usec / 1000;
	strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", localtime(&curTime.tv_sec));
	char msBuf[std::strlen(buf) + 5];
	snprintf(msBuf, sizeof(msBuf), "%s.%03d", buf, milli);
	return string(msBuf);
#endif
}

string HeaderString(const string& logName, LogLevel level,
					const Log::SourceInfo& sourceInfo) {

	return std::format("{} [{}] [{}] [{}:{}] [{}()]",
					   TimestampString(),
					   logName,
					   magic_enum::enum_name(level),
					   get<0>(sourceInfo),
					   get<1>(sourceInfo),
					   get<2>(sourceInfo));
}

string HeaderString(const string& logName, LogLevel level) {

	return std::format("{} [{}] [{}]",
					   TimestampString(),
					   logName,
					   magic_enum::enum_name(level));
}

/// Private Static Member Variables ///

std::unique_ptr<Log> Log::_appLog; // weird.
