//
//  Logger.cc
//	avara-engine
//
//  Created by Morgan Davis on 1/5/18.
//  Copyright © 2018 Morgan K Davis. All rights reserved.
//

#include "Logger.h"

#include <ctime>
#include <iostream>

#if defined(MACOS) || defined(LINUX)
#include <sys/time.h>
#endif

#ifdef ANDROID
#include <android/log.h>
#include <NDKHelper.h>
#endif

#include "Exception.h"
#include "Global.h"
#include "Utilities.h"


using namespace ae;
using namespace ae::utils;
using namespace std;


/*######################################################################################
 #######################################################################################
	Logger
 #######################################################################################
 ######################################################################################*/


constexpr size_t MAX_HEADER_STR_SIZE = 256;
constexpr size_t MAX_LOG_BODY_SIZE = 1024 * 256; // ~256,000 characters
constexpr size_t MAX_LOG_LINE_SIZE = MAX_HEADER_STR_SIZE + MAX_LOG_BODY_SIZE;


/*********************************************************************************************
	Public Static
 *********************************************************************************************/

std::shared_ptr<Logger> Logger::MainLogger() {
	
	static shared_ptr<Logger> logger = nullptr;
	if (!logger) {

#if defined(DESKTOP)
		string executableName = utils::ExecutableName()->string();
		auto nativeSink = make_shared<STDLoggerSink>();
		auto fileSink = make_shared<FileLoggerSink>(*(utils::ExecutableDirectory())
													/ (executableName + string(".log")));
#elif defined(ANDROID)
		string executableName = ndk_helper::JNIHelper::GetInstance()->GetAppName();
		auto nativeSink = make_shared<AndroidLoggerSink>();
//		auto fileSink = make_shared<FileLoggerSink>(*(utils::InternalFilesDirectory())
//													/ (executableName + string(".log")));
		auto fileSink = make_shared<FileLoggerSink>(executableName + string(".log"));
#endif
		
		auto sinks = vector<shared_ptr<LoggerSink>>();
		sinks.emplace_back(static_pointer_cast<LoggerSink>(nativeSink));
		sinks.emplace_back(static_pointer_cast<LoggerSink>(fileSink));
		
		logger = make_shared<Logger>("ae", sinks);
	}
	return logger;
}

/*********************************************************************************************
	Static Prototypes
 *********************************************************************************************/

string DateString();
string HeaderString(const string& logName, LOG_LEVEL level);
string HeaderString(const string& logName, LOG_LEVEL level,
					const char* filename, int line, const char* function);
string StringFromLogLevel(LOG_LEVEL level);

/*********************************************************************************************
	Lifecycle
 *********************************************************************************************/

Logger::Logger(string name, shared_ptr<LoggerSink> sink,
			   LOG_LEVEL level, LOG_LEVEL flushLevel):
	_name(name),
	_sinks(vector<shared_ptr<LoggerSink>>()),
	_level(level),
	_flushLevel(flushLevel) {
	
		_sinks.emplace_back(sink);
}

Logger::Logger(string name, vector<shared_ptr<LoggerSink>> sinks,
			   LOG_LEVEL level, LOG_LEVEL flushLevel):
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

vector<shared_ptr<LoggerSink>> Logger::sinks() const {
	return _sinks;
}

LOG_LEVEL Logger::level() const {
	return _level;
}

void Logger::level(LOG_LEVEL level) {
	_level = level;
}

void Logger::trace(const char* format, ...) {

	va_list args;
	va_start(args, format);
	log(LOG_LEVEL::TRACE_, format, args);
	va_end(args);
}

void Logger::debug(const char* format, ...) {

	va_list args;
	va_start(args, format);
	log(LOG_LEVEL::DEBUG_, format, args);
	va_end(args);
}

void Logger::info(const char* format, ...) {

	va_list args;
	va_start(args, format);
	log(LOG_LEVEL::INFO_, format, args);
	va_end(args);
}

void Logger::warn(const char* format, ...) {

	va_list args;
	va_start(args, format);
	log(LOG_LEVEL::WARN_, format, args);
	va_end(args);
}

void Logger::error(const char* format, ...) {

	va_list args;
	va_start(args, format);
	log(LOG_LEVEL::ERROR_, format, args);
	va_end(args);
}

void Logger::critical(const char* format, ...) {

	va_list args;
	va_start(args, format);
	log(LOG_LEVEL::CRITICAL_, format, args);
	va_end(args);
}

//void Logger::trace(const char* filename, int line, const char* function) {
//
//	log_trace(filename, line, function);
//}

void Logger::trace(bool useHeader,
				   const char* filename, int line, const char* function,
				   const char* format, ...) {

	va_list args;
	va_start(args, format);
	log(LOG_LEVEL::TRACE_, useHeader, filename, line, function, format, args);
	va_end(args);
}

void Logger::debug(bool useHeader,
				   const char* filename, int line, const char* function,
				   const char* format, ...) {

	va_list args;
	va_start(args, format);
	log(LOG_LEVEL::DEBUG_, useHeader, filename, line, function, format, args);
	va_end(args);
}

void Logger::info(bool useHeader,
				  const char* filename, int line, const char* function,
				  const char* format, ...) {

	va_list args;
	va_start(args, format);
	log(LOG_LEVEL::INFO_, useHeader, filename, line, function, format, args);
	va_end(args);
}

void Logger::warn(bool useHeader,
				  const char* filename, int line, const char* function,
				  const char* format, ...) {

	va_list args;
	va_start(args, format);
	log(LOG_LEVEL::WARN_, useHeader, filename, line, function, format, args);
	va_end(args);
}

void Logger::error(bool useHeader,
				   const char* filename, int line, const char* function,
				   const char* format, ...) {

	va_list args;
	va_start(args, format);
	log(LOG_LEVEL::ERROR_, useHeader, filename, line, function, format, args);
	va_end(args);
}

void Logger::critical(bool useHeader,
					  const char* filename, int line, const char* function,
					  const char* format, ...) {

	va_list args;
	va_start(args, format);
	log(LOG_LEVEL::CRITICAL_, useHeader, filename, line, function, format, args);
	va_end(args);
}

// constructs body with variable args list
void Logger::log(LOG_LEVEL level,
				 const char* format, va_list args) {

	char body[MAX_LOG_BODY_SIZE];

	// https://en.cppreference.com/w/c/io/vfprintf
	vsnprintf(body, MAX_LOG_BODY_SIZE, format, args);

	dispatch(level, body);
}

// constructs body with variable args list
void Logger::log(LOG_LEVEL level,
				 bool useHeader,
				 const char* filename, int line, const char* function,
				 const char* format, va_list args) {

	char body[MAX_LOG_BODY_SIZE];

	// https://en.cppreference.com/w/c/io/vfprintf
	vsnprintf(body, MAX_LOG_BODY_SIZE, format, args);

	construct(level, filename, line, function, body);
}

//void Logger::log_trace(const char* filename, int line, const char* function) {
//
//	dispatch(LOG_LEVEL::TRACE_, HeaderString(_name, LOG_LEVEL::TRACE_, filename, line, function).c_str());
//}

void Logger::construct(LOG_LEVEL level, const char* body) {

	if (static_cast<underlying_type<LOG_LEVEL>::type>(level)
		>= static_cast<underlying_type<LOG_LEVEL>::type>(_level)) {

		char lineStr[MAX_LOG_LINE_SIZE];
		snprintf(lineStr, MAX_LOG_LINE_SIZE, "%s %s",
				 HeaderString(_name, level).c_str(),
				 body);

		dispatch(level, lineStr);
	}
}

void Logger::construct(LOG_LEVEL level,
					   const char* filename, int line, const char* function,
					   const char* body) {

	if (static_cast<underlying_type<LOG_LEVEL>::type>(level)
		>= static_cast<underlying_type<LOG_LEVEL>::type>(_level)) {

		char lineStr[MAX_LOG_LINE_SIZE];
		snprintf(lineStr, MAX_LOG_LINE_SIZE, "%s %s",
				 HeaderString(_name, level, filename, line, function).c_str(),
				 body);

		dispatch(level, lineStr);
	}
}

void Logger::dispatch(LOG_LEVEL level, const char* line) {

	for (auto sink : _sinks) {

#if defined(DESKTOP)
		if (dynamic_pointer_cast<STDLoggerSink>(sink)) {
			dynamic_pointer_cast<STDLoggerSink>(sink)->write(line, level);
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

	if (static_cast<underlying_type<LOG_LEVEL>::type>(level)
		>= static_cast<underlying_type<LOG_LEVEL>::type>(_flushLevel)) {
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

string HeaderString(const string& logName, LOG_LEVEL level) {

	char headerStr[MAX_HEADER_STR_SIZE];
	snprintf(headerStr, MAX_HEADER_STR_SIZE, "%s [%s] [%s]",
			 DateString().c_str(),
			 logName.c_str(),
			 StringFromLogLevel(level).c_str());

	//return string((const char*)headerStr);
	return {(const char*)headerStr};
}

string HeaderString(const string& logName, LOG_LEVEL level,
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

string StringFromLogLevel(LOG_LEVEL level) {
	
	switch (level) {
		case LOG_LEVEL::TRACE_: 	return "trace";
		case LOG_LEVEL::DEBUG_: 	return "debug";
		case LOG_LEVEL::INFO_: 		return "info";
		case LOG_LEVEL::WARN_: 		return "WARN";
		case LOG_LEVEL::ERROR_: 	return "ERROR";
		case LOG_LEVEL::CRITICAL_: 	return "CRITICAL";
		case LOG_LEVEL::OFF_: 		return "off";
	}
}


/*######################################################################################
 #######################################################################################
	LoggerSink
 #######################################################################################
 ######################################################################################*/


/*********************************************************************************************
	Lifecycle
 *********************************************************************************************/

LoggerSink::~LoggerSink() {
	
}

/*********************************************************************************************
	Public
 *********************************************************************************************/

void LoggerSink::flush() {
	
}


/*######################################################################################
 #######################################################################################
	STDLoggerSink
 #######################################################################################
 ######################################################################################*/


#ifdef DESKTOP

/*********************************************************************************************
	Lifecycle
 *********************************************************************************************/

STDLoggerSink::STDLoggerSink() {
	
}

STDLoggerSink::~STDLoggerSink() {
	
	flush();
}

/*********************************************************************************************
	Public
 *********************************************************************************************/

void STDLoggerSink::flush() {

	fflush(stdout);
	fflush(stderr);
}

/*********************************************************************************************
	Internal
 **************************************************************************************/

void STDLoggerSink::write(const char* message, LOG_LEVEL level) {

	if (static_cast<underlying_type<LOG_LEVEL>::type>(level)
		>= static_cast<underlying_type<LOG_LEVEL>::type>(LOG_LEVEL::ERROR_)) {
		fprintf(stderr, "%s\n", message);

	}
	else {
		fprintf(stdout, "%s\n", message);
	}

#ifdef WINDOWS
	auto newLined = (char*)malloc(strlen(message) + 2);
	//sprintf(newLined, "%s\n", message);
	strcpy(newLined, message);
	strcat(newLined, "\n");
	OutputDebugStringA((const char*)newLined);
	free(newLined);
#endif
}

#endif // DESKTOP


/*######################################################################################
 #######################################################################################
	AndroidLoggerSink
 #######################################################################################
 ######################################################################################*/


#ifdef ANDROID

/*********************************************************************************************
	Static Prototypes
 *********************************************************************************************/

unsigned AndroidPriorityFromLogLevel(LOG_LEVEL level);

/*********************************************************************************************
	Lifecycle
 *********************************************************************************************/

AndroidLoggerSink::AndroidLoggerSink() {
	
}

AndroidLoggerSink::~AndroidLoggerSink() {
	
	flush();
}

/*********************************************************************************************
	Public
 *********************************************************************************************/

void AndroidLoggerSink::flush() {

	// no flush in Android.
}

/*********************************************************************************************
	Internal
 *********************************************************************************************/

void AndroidLoggerSink::write(const char* message, const char* tag, LOG_LEVEL level) {
	
	// https://developer.android.com/ndk/reference/group/logging	
	__android_log_write(AndroidPriorityFromLogLevel(level), tag, message);
}

/*********************************************************************************************
	Static
 *********************************************************************************************/

unsigned AndroidPriorityFromLogLevel(LOG_LEVEL level) {

	switch (level) {
		case LOG_LEVEL::TRACE_: 	return ANDROID_LOG_VERBOSE;
		case LOG_LEVEL::DEBUG_: 	return ANDROID_LOG_DEBUG;
		case LOG_LEVEL::INFO_: 		return ANDROID_LOG_INFO;
		case LOG_LEVEL::WARN_: 		return ANDROID_LOG_WARN;
		case LOG_LEVEL::ERROR_: 	return ANDROID_LOG_ERROR;
		case LOG_LEVEL::CRITICAL_: 	return ANDROID_LOG_FATAL;
		case LOG_LEVEL::OFF_: 		return ANDROID_LOG_SILENT;
	}
}

#endif


/*######################################################################################
 #######################################################################################
	FileLoggerSink
 #######################################################################################
 ######################################################################################*/


/*********************************************************************************************
	Lifecycle
 *********************************************************************************************/

FileLoggerSink::FileLoggerSink(std::filesystem::path relPath,
							   unsigned maxFiles,
							   unsigned maxFilesize):
		_filepath(relPath),
		_maxFiles(maxFiles),
		_maxFilesize(maxFilesize) {

#if defined(ANDROID)
	_filepath = (*(utils::InternalFilesDirectory())) / relPath;
#endif

	std::error_code errorCode;
	std::filesystem::create_directories(_filepath.parent_path(), errorCode);
	// this bugs me.
	// std::errc::success -> used to be boost:errc::success
	// std::errc::success or anything evaluating to 0 does not exist, apparently.
	auto code = errorCode.value();
	if (code != 0) {
		throw Exception("Couldn't create intermediate directories for log: " + _filepath.string() + " [code " + to_string(code) + "]");
	}

	openStream();
}

FileLoggerSink::~FileLoggerSink() {
	
	flush();
	
	if (_fileStream && _fileStream->is_open()) {
		_fileStream->close();
	}
}

/*********************************************************************************************
	Public
 *********************************************************************************************/

std::filesystem::path FileLoggerSink::filepath() const {
	return _filepath;
}

unsigned FileLoggerSink::maxFiles() const {
	return _maxFiles;
}

unsigned FileLoggerSink::maxFilesize() const {
	return _maxFilesize;
}

void FileLoggerSink::flush() {
	
	if (_fileStream && _fileStream->is_open()) {
		_fileStream->flush();
	}
}

/*********************************************************************************************
	Internal
 *********************************************************************************************/

void FileLoggerSink::write(const char* message) {
	
	*_fileStream << message << endl;

	checkRotate();
}

/*********************************************************************************************
	Private
 *********************************************************************************************/

void FileLoggerSink::openStream() {
	
	if (_fileStream && _fileStream->is_open()) {
		_fileStream->close();
	}

#warning check file writable
	_fileStream = make_shared<ofstream>(_filepath.string(), fstream::out | fstream::app);
}

void FileLoggerSink::checkRotate() {
	
	if (std::filesystem::exists(_filepath)) {
		if (std::filesystem::file_size(_filepath) > _maxFilesize) {
			rotate();
		}
	}
}

void FileLoggerSink::rotate() {

	// find list of existing files
	// start at index 0, count down until the next isn't found

	auto stem = _filepath.stem();
	auto extension = _filepath.extension();
	
	auto existing = vector<std::filesystem::path>();

	existing.emplace_back(_filepath);
	
	unsigned i = 0;
	while (true) {
		
		auto path = _filepath.parent_path() / std::filesystem::path(stem.string() + to_string(i) + extension.string());
		
		if (std::filesystem::exists(path)) {
			existing.emplace_back(path);
			++i;
		}
		else {
			break;
		}
	}
	
	// go through from the end and move each file down an index
	
	unsigned index = existing.size();
	
	for (vector<std::filesystem::path>::reverse_iterator i = existing.rbegin(); i != existing.rend(); ++i ) {
		auto path = *i;
		
		if (index > _maxFiles) {
			
			std::error_code errorCode;
			std::filesystem::remove(path, errorCode);
			// still bugs me. see note above about std:errc
			auto code = errorCode.value();
			if (code != 0) {
				throw Exception("Cannot remove log file: " + path.string() + " [code " + to_string(code) + "]");
			}
		}
		else {
			
			auto existStem = path.stem();
//			auto oldExtension = path.extension();

			//std::filesystem::path newPath;
//			if (path.string() == _filepath) {
//				newPath = std::filesystem::path(oldStem.string() + to_string(index-1) + oldExtension.string());
//			}
//			else {
				//newPath = std::filesystem::path(stem.string().substr(0, stem.string().length()-1) + to_string(index-1) + extension.string());
				std::filesystem::path newPath = path.parent_path() / std::filesystem::path(existStem.string().substr(0, stem.string().length()) + to_string(index-1) + extension.string());
//			}

			std::filesystem::rename(path, newPath);
		}
		
		--index;
	}
	
	// move the last file
	
	auto newPath = _filepath.parent_path() / std::filesystem::path(stem.string() + string("0") + extension.string());
	
	openStream();
}
