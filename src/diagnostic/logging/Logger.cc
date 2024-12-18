//
//  Logger.cc
//  avara3d
//
//  Created by Morgan Davis on 1/5/18.
//  Copyright © 2024 Morgan K Davis. All rights reserved.
//

#include "a3d/diagnostic/logging/Logger.h"

#include <ctime>
#include <utility>

#ifdef POSIX
#include <sys/time.h>
#endif

#include "magic_enum.hpp"

#include "a3d/Utilities.h"
#include "a3d/diagnostic/exception/Exception.h"
#include "a3d/diagnostic/logging/sink/LoggerSink.h"
#include "a3d/diagnostic/logging/sink/FileLoggerSink.h"
#include "a3d/diagnostic/logging/sink/StdOutLoggerSink.h"


using namespace a3d;
using namespace std;


constexpr size_t MAX_HEADER_STR_SIZE = 256;
constexpr size_t MAX_LOG_BODY_SIZE = 1024 * 256; // ~256,000 characters
constexpr size_t MAX_LOG_LINE_SIZE = MAX_HEADER_STR_SIZE + MAX_LOG_BODY_SIZE;


/*********************************************************************************************
	Public Static Member Functions
 *********************************************************************************************/

Logger& Logger::MainLogger() {
	
	static unique_ptr<Logger> logger = nullptr;

	if (!logger) {
//		string executableName = *utils::ExecutableName();
//		auto nativeSink = make_unique<StdOutLoggerSink>();
//		auto fileSink = make_unique<FileLoggerSink>(*(utils::ExecutableDirectory())
//													/ (executableName + string(".log")));
		string executableName = *utils::ExecutableName();
		auto nativeSink = make_unique<StdOutLoggerSink>();
		auto fileSink = make_unique<FileLoggerSink>(*(utils::ExecutableDirectory()) / "a3d.log");

		auto sinks = unordered_set<unique_ptr<LoggerSink>>();
//		sinks.insert(static_pointer_cast<LoggerSink>(nativeSink));
//		sinks.insert(static_pointer_cast<LoggerSink>(fileSink));
		sinks.insert(std::move(nativeSink));
		sinks.insert(std::move(fileSink));

		logger = make_unique<Logger>("a3d", std::move(sinks));
	}

	return *logger;
}

/*********************************************************************************************
	Private Static Member Prototypes
 *********************************************************************************************/

string DateString();
//string HeaderString(const string& logName, LogLevel level);
string HeaderString(const string& logName, LogLevel level,
					const Logger::SourceInfo& sourceInfo);

/*********************************************************************************************
	Public Lifecycle Functions
 *********************************************************************************************/

Logger::Logger(const string& name,
			   unique_ptr<LoggerSink> sink,
			   LogLevel level,
			   LogLevel flushLevel):
		_name{name},
		_sinks{},
		_level{level},
		_flushLevel{flushLevel} {

	_sinks.insert(std::move(sink));
}

Logger::Logger(const string& name,
			   unordered_set<unique_ptr<LoggerSink>> sinks,
			   LogLevel level,
			   LogLevel flushLevel):
		_name(name),
		_sinks(std::move(sinks)),
		_level(level),
		_flushLevel(flushLevel) { }

Logger::~Logger() { }

/*********************************************************************************************
	Public Member Functions
 *********************************************************************************************/

const string& Logger::name() const {
	return _name;
}

const unordered_set<unique_ptr<LoggerSink>>& Logger::sinks() const {
	return _sinks;
}

LogLevel Logger::level() const {
	return _level;
}

void Logger::level(LogLevel level) {
	_level = level;
}

LogLevel Logger::flushLevel() const {
	return _flushLevel;
}

void Logger::flushLevel(LogLevel level) {
	_flushLevel = level;
}

void Logger::flush() {

	for (auto& sink : _sinks) {
		sink->flush();
	}
}

void Logger::log(LogLevel level,
				 const SourceInfo& sourceInfo,
				 const std::string& msg) {

	if (static_cast<underlying_type<LogLevel>::type>(level)
		>= static_cast<underlying_type<LogLevel>::type>(_level)) {

		construct(level, sourceInfo, msg);
	}
}

void Logger::construct(LogLevel level,
					   const SourceInfo& sourceInfo,
					   const std::string& msg) {

	auto lineStr = fmt::format("{} {}",
							   HeaderString(_name, level, sourceInfo),
							   msg);
	dispatch(level, lineStr);
}

void Logger::dispatch(LogLevel level, std::string& line) {

	for (auto& sink : _sinks) {

		if (auto stdOutSink = dynamic_cast<StdOutLoggerSink*>(sink.get())) {
			stdOutSink->write(line, level);
		}

		if (auto fileLoggerSink = dynamic_cast<FileLoggerSink*>(sink.get())) {
			fileLoggerSink->write(line);
		}
	}

	if (static_cast<underlying_type<LogLevel>::type>(level)
		>= static_cast<underlying_type<LogLevel>::type>(_flushLevel)) {
		flush();
	}
}

/*********************************************************************************************
	Private Static Member Functions
 *********************************************************************************************/

string DateString() {

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
	char msBuf[strlen(buf) + 5];
	snprintf(msBuf, sizeof(msBuf), "%s.%03d", buf, milli);
	return string(msBuf);
#endif
}

string HeaderString(const string& logName, LogLevel level,
					const Logger::SourceInfo& sourceInfo) {

	auto filename = get<0>(sourceInfo);
	auto line = get<1>(sourceInfo);
	auto function = get<2>(sourceInfo);

	return fmt::format("{} [{}] [{}] [{}:{}] [{}()]",
					   DateString(),
					   logName,
					   string(magic_enum::enum_name(level)),
					   filename,
					   line,
					   function);
}
