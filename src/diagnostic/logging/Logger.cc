//
//  Logger.cc
//	avara3d
//
//  Created by Morgan Davis on 1/5/18.
//  Copyright © 2018 Morgan K Davis. All rights reserved.
//

#include "a3d/diagnostic/logging/Logger.h"

#include <cstdarg>
#include <ctime>

#ifdef POSIX
#include <sys/time.h>
#endif

#ifdef ANDROID
#include <android/log.h>
#include <NDKHelper.h>
#endif

#include "a3d/Utilities.h"
#include "a3d/diagnostic/exception/Exception.h"
#include "a3d/diagnostic/logging/sink/LoggerSink.h"
#include "a3d/diagnostic/logging/sink/FileLoggerSink.h"
#ifdef DESKTOP
#include "a3d/diagnostic/logging/sink/platform/desktop/StdOutLoggerSink.h"
#endif


using namespace a3d;
using namespace a3d::utils;
using namespace std;


constexpr size_t MAX_HEADER_STR_SIZE = 256;
constexpr size_t MAX_LOG_BODY_SIZE = 1024 * 256; // ~256,000 characters
constexpr size_t MAX_LOG_LINE_SIZE = MAX_HEADER_STR_SIZE + MAX_LOG_BODY_SIZE;


/*********************************************************************************************
	Public Static
 *********************************************************************************************/

shared_ptr<Logger> Logger::MainLogger() {
	
	static shared_ptr<Logger> logger = nullptr;

	if (!logger) {
#ifdef ANDROID
		string executableName = ndk_helper::JNIHelper::GetInstance()->GetAppName();
		auto nativeSink = make_shared<AndroidLoggerSink>();
//		auto fileSink = make_shared<FileLoggerSink>(*(utils::InternalFilesDirectory())
//													/ (executableName + string(".log")));
		auto fileSink = make_shared<FileLoggerSink>(executableName + string(".log"));
#else
		string executableName = utils::ExecutableName()->string();
		auto nativeSink = make_shared<StdOutLoggerSink>();
		auto fileSink = make_shared<FileLoggerSink>(*(utils::ExecutableDirectory())
													/ (executableName + string(".log")));
#endif

		auto sinks = unordered_set<shared_ptr<LoggerSink>>();
		sinks.insert(static_pointer_cast<LoggerSink>(nativeSink));
		sinks.insert(static_pointer_cast<LoggerSink>(fileSink));

		logger = make_shared<Logger>("a3d", sinks);
	}
	return logger;
}

/*********************************************************************************************
	Static Prototypes
 *********************************************************************************************/

string DateString();
string HeaderString(const string& logName, LogLevel level);
string HeaderString(const string& logName, LogLevel level,
					const char* filename, int line, const char* function);
string StringFromLogLevel(LogLevel level);

/*********************************************************************************************
	Lifecycle
 *********************************************************************************************/

Logger::Logger(string name, shared_ptr<LoggerSink> sink,
			   LogLevel level, LogLevel flushLevel):
	_name(name),
	_sinks(unordered_set<shared_ptr<LoggerSink>>()),
	_level(level),
	_flushLevel(flushLevel) {
	
		_sinks.insert(sink);
}

Logger::Logger(string name, unordered_set<shared_ptr<LoggerSink>> sinks,
			   LogLevel level, LogLevel flushLevel):
	_name(name),
	_sinks(sinks),
	_level(level),
	_flushLevel(flushLevel) {
	
}

Logger::~Logger() {
	
}

/*********************************************************************************************
	Public
 *********************************************************************************************/

string Logger::name() const {
	return _name;
}

unordered_set<shared_ptr<LoggerSink>> Logger::sinks() const {
	return _sinks;
}

LogLevel Logger::level() const {
	return _level;
}

void Logger::level(LogLevel level) {
	_level = level;
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


//template <typename... Args>
//void Logger::f1(const char* format, Args&&... args) {
//
//	auto together = fmt::vformat(format,
//								 fmt::make_format_args(std::forward<Args>(args))...);
//
//}


//template <typename... Args>
//void Logger::f2(const char* format, Args&&...args) {
//
//
//}


//void Logger::f2(bool useHeader,
//				const char* filename, int line, const char* function,
//				const char* fthing) {
//
//}




//template <typename... Args>
//void Logger::f3(std::string_view fmt, Args&&... args) {
//	return fmt::vformat(fmt, fmt::make_format_args(std::forward<Args>(args)...));
//}
//
//
//template <typename F, typename... Args>
//void Logger::f4(F, Args&&... args) {
//	return fmt::format(F::string, std::forward<Args>(args)...);
//}



//void vlog(const char* file, int line, fmt::string_view format,
//		  fmt::format_args args) {
//	fmt::print("{}: {}: ", file, line);
//	fmt::vprint(format, args);
//}

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

	for (auto sink : _sinks) {

#if defined(DESKTOP)
		if (dynamic_pointer_cast<StdOutLoggerSink>(sink)) {
			dynamic_pointer_cast<StdOutLoggerSink>(sink)->write(line, level);
		}
#elif defined(ANDROID)
		if (dynamic_pointer_cast<AndroidLoggerSink>(sink)) {
			dynamic_pointer_cast<AndroidLoggerSink>(sink)->write(message, _name.c_str(), level);
		}
#endif
		if (dynamic_pointer_cast<FileLoggerSink>(sink)) {
			dynamic_pointer_cast<FileLoggerSink>(sink)->write(line);
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
	Static
 *********************************************************************************************/

string DateString() {
	
	char buffer[256];
#ifdef WINDOWS
	time_t rawtime;
	struct tm * timeinfo;
	time(&rawtime);
	timeinfo = localtime(&rawtime);
	strftime(buffer, sizeof(buffer), "%Y-%m-%d %I:%M:%S", timeinfo);
#else
	timeval curTime;
	gettimeofday(&curTime, NULL); // gettimeofday() is POSIX
	int milli = curTime.tv_usec / 1000;
	strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", localtime(&curTime.tv_sec));
	sprintf(buffer, "%s.%03d", buffer, milli);
#endif
	return string(buffer);
}

string HeaderString(const string& logName, LogLevel level) {

	char headerStr[MAX_HEADER_STR_SIZE];
	snprintf(headerStr, MAX_HEADER_STR_SIZE, "%s [%s] [%s]",
			 DateString().c_str(),
			 logName.c_str(),
			 StringFromLogLevel(level).c_str());

	//return string((const char*)headerStr);
	return {(const char*)headerStr};
}

string HeaderString(const string& logName, LogLevel level,
					const char* filename, int line, const char* function) {

	char headerStr[MAX_HEADER_STR_SIZE];
	snprintf(headerStr, MAX_HEADER_STR_SIZE, "%s [%s] [%s] [%s:%d] [%s()]",
			 DateString().c_str(),
			 logName.c_str(),
			 StringFromLogLevel(level).c_str(),
			 filename,
			 line,
			 function);

	//return string((const char*)headerStr);
	return {(const char*)headerStr};
}

string StringFromLogLevel(LogLevel level) {
	
	switch (level) {
		case LogLevel::Trace: 	return "trace";
		case LogLevel::Debug: 	return "debug";
		case LogLevel::Info: 		return "info";
		case LogLevel::Warn: 		return "WARN";
		case LogLevel::Error: 	return "ERROR";
		case LogLevel::Critical: 	return "CRITICAL";
		case LogLevel::Off: 		return "off";
	}
}
