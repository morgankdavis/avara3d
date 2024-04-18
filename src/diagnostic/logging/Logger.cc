//
//  Logger.cc
//  avara3d
//
//  Created by Morgan Davis on 1/5/18.
//  Copyright © 2024 Morgan K Davis. All rights reserved.
//

#include "a3d/diagnostic/logging/Logger.h"

#include <cstdarg>
#include <ctime>
#include <utility>

#ifdef POSIX
#include <sys/time.h>
#endif

#ifdef ANDROID
#include <android/log.h>
#include <NDKHelper.h>
#endif

#include "magic_enum.hpp"

#include "a3d/Utilities.h"
#include "a3d/diagnostic/exception/Exception.h"
#include "a3d/diagnostic/logging/sink/LoggerSink.h"
#include "a3d/diagnostic/logging/sink/FileLoggerSink.h"
#include "a3d/diagnostic/logging/sink/StdOutLoggerSink.h"


using namespace a3d;
using namespace a3d::utils;
using namespace std;


constexpr size_t MAX_HEADER_STR_SIZE = 256;
constexpr size_t MAX_LOG_BODY_SIZE = 1024 * 256; // ~256,000 characters
constexpr size_t MAX_LOG_LINE_SIZE = MAX_HEADER_STR_SIZE + MAX_LOG_BODY_SIZE;


/*********************************************************************************************
	Public Static Members
 *********************************************************************************************/

Logger& Logger::MainLogger() {
	
	static unique_ptr<Logger> logger = nullptr;

	if (!logger) {
#ifdef ANDROID
		string executableName = ndk_helper::JNIHelper::GetInstance()->GetAppName();
		auto nativeSink = make_shared<AndroidLoggerSink>();
//		auto fileSink = make_shared<FileLoggerSink>(*(utils::InternalFilesDirectory())
//													/ (executableName + string(".log")));
		auto fileSink = make_shared<FileLoggerSink>(executableName + string(".log"));
#else
//		string executableName = *utils::ExecutableName();
//		auto nativeSink = make_unique<StdOutLoggerSink>();
//		auto fileSink = make_unique<FileLoggerSink>(*(utils::ExecutableDirectory())
//													/ (executableName + string(".log")));
		string executableName = *utils::ExecutableName();
		auto nativeSink = make_unique<StdOutLoggerSink>();
		auto fileSink = make_unique<FileLoggerSink>(*(utils::ExecutableDirectory()) / "a3d.log");
#endif

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
string HeaderString(const string& logName, LogLevel level);
string HeaderString(const string& logName, LogLevel level,
					const char* filename, int line, const char* function);

/*********************************************************************************************
	Public Lifecycle
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
	Public Members
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

void Logger::trace(const char* format, ...) {

	va_list args;
	va_start(args, format);
	log(LogLevel::Trace, format, args);
	va_end(args);
}

void Logger::debug(const char* format, ...) {

	va_list args;
	va_start(args, format);
	log(LogLevel::Debug, format, args);
	va_end(args);
}

void Logger::info(const char* format, ...) {

	va_list args;
	va_start(args, format);
	log(LogLevel::Info, format, args);
	va_end(args);
}

void Logger::warn(const char* format, ...) {

	va_list args;
	va_start(args, format);
	log(LogLevel::Warn, format, args);
	va_end(args);
}

void Logger::error(const char* format, ...) {

	va_list args;
	va_start(args, format);
	log(LogLevel::Error, format, args);
	va_end(args);
}

void Logger::critical(const char* format, ...) {

	va_list args;
	va_start(args, format);
	log(LogLevel::Critical, format, args);
	va_end(args);
}

void Logger::crumb(const char* filename, int line, const char* function) {

	log_crumb(filename, line, function);
}

void Logger::trace(bool useHeader,
				   const char* filename, int line, const char* function,
				   const char* format, ...) {

	va_list args;
	va_start(args, format);
	log(LogLevel::Trace, useHeader, filename, line, function, format, args);
	va_end(args);
}

void Logger::debug(bool useHeader,
				   const char* filename, int line, const char* function,
				   const char* format, ...) {

	va_list args;
	va_start(args, format);
	log(LogLevel::Debug, useHeader, filename, line, function, format, args);
	va_end(args);
}

void Logger::info(bool useHeader,
				  const char* filename, int line, const char* function,
				  const char* format, ...) {

	va_list args;
	va_start(args, format);
	log(LogLevel::Info, useHeader, filename, line, function, format, args);
	va_end(args);
}

void Logger::warn(bool useHeader,
				  const char* filename, int line, const char* function,
				  const char* format, ...) {

	va_list args;
	va_start(args, format);
	log(LogLevel::Warn, useHeader, filename, line, function, format, args);
	va_end(args);
}

void Logger::error(bool useHeader,
				   const char* filename, int line, const char* function,
				   const char* format, ...) {

	va_list args;
	va_start(args, format);
	log(LogLevel::Error, useHeader, filename, line, function, format, args);
	va_end(args);
}

void Logger::critical(bool useHeader,
					  const char* filename, int line, const char* function,
					  const char* format, ...) {

	va_list args;
	va_start(args, format);
	log(LogLevel::Critical, useHeader, filename, line, function, format, args);
	va_end(args);
}

// constructs body with variable args list
void Logger::log(LogLevel level,
				 const char* format, va_list args) {

	char body[MAX_LOG_BODY_SIZE];

	// https://en.cppreference.com/w/c/io/vfprintf
	vsnprintf(body, MAX_LOG_BODY_SIZE, format, args);

	dispatch(level, body);
}

// constructs body with variable args list
void Logger::log(LogLevel level,
				 bool useHeader,
				 const char* filename, int line, const char* function,
				 const char* format, va_list args) {

	char body[MAX_LOG_BODY_SIZE];

	// https://en.cppreference.com/w/c/io/vfprintf
	vsnprintf(body, MAX_LOG_BODY_SIZE, format, args);

	if (useHeader) {
		construct(level, filename, line, function, body);
	}
	else {
		dispatch(level, body);
	}
}

void Logger::log_crumb(const char* filename, int line, const char* function) {

	dispatch(LogLevel::Trace,
			 HeaderString(_name, LogLevel::Trace, filename, line, function).c_str());
}

void Logger::construct(LogLevel level, const char* body) {

	if (static_cast<underlying_type<LogLevel>::type>(level)
		>= static_cast<underlying_type<LogLevel>::type>(_level)) {

		char lineStr[MAX_LOG_LINE_SIZE];
		snprintf(lineStr, MAX_LOG_LINE_SIZE, "%s %s",
				 HeaderString(_name, level).c_str(),
				 body);

		dispatch(level, lineStr);
	}
}

void Logger::construct(LogLevel level,
					   const char* filename, int line, const char* function,
					   const char* body) {

	if (static_cast<underlying_type<LogLevel>::type>(level)
		>= static_cast<underlying_type<LogLevel>::type>(_level)) {

		char lineStr[MAX_LOG_LINE_SIZE];
		snprintf(lineStr, MAX_LOG_LINE_SIZE, "%s %s",
				 HeaderString(_name, level, filename, line, function).c_str(),
				 body);

		dispatch(level, lineStr);
	}
}

void Logger::dispatch(LogLevel level, const char* line) {

	for (auto& sink : _sinks) {

#if defined(DESKTOP)
		if (auto stdOutSink = dynamic_cast<StdOutLoggerSink*>(sink.get())) {
			stdOutSink->write(line, level);
		}
#elif defined(ANDROID)
		if (dynamic_pointer_cast<AndroidLoggerSink>(sink)) {
			dynamic_pointer_cast<AndroidLoggerSink>(sink)->write(message, _name.c_str(), level);
		}
#endif
		if (auto fileLoggerSink = dynamic_cast<FileLoggerSink*>(sink.get())) {
			fileLoggerSink->write(line);
		}
	}

	if (static_cast<underlying_type<LogLevel>::type>(level)
		>= static_cast<underlying_type<LogLevel>::type>(_flushLevel)) {
		flush();
	}
}


void Logger::flush() {

	for (auto& sink : _sinks) {
		sink->flush();
	}	
}

/*********************************************************************************************
	Private Static Members
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

string HeaderString(const string& logName, LogLevel level) {

	char headerStr[MAX_HEADER_STR_SIZE];
	snprintf(headerStr, MAX_HEADER_STR_SIZE, "%s [%s] [%s]",
			 DateString().c_str(),
			 logName.c_str(),
			 string(magic_enum::enum_name(level)).c_str());

	//return string((const char*)headerStr);
	return {(const char*)headerStr};
}

string HeaderString(const string& logName, LogLevel level,
					const char* filename, int line, const char* function) {

	char headerStr[MAX_HEADER_STR_SIZE];
	snprintf(headerStr, MAX_HEADER_STR_SIZE, "%s [%s] [%s] [%s:%d] [%s()]",
			 DateString().c_str(),
			 logName.c_str(),
			 string(magic_enum::enum_name(level)).c_str(),
			 filename,
			 line,
			 function);

	//return string((const char*)headerStr);
	return {(const char*)headerStr};
}
