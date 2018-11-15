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


constexpr size_t MAX_HEADER_STR_SIZE = 128;
constexpr size_t MAX_LOG_MSG_SIZE = 1024 * 256; // ~256,000 characters
constexpr size_t MAX_LOG_LINE_SIZE = MAX_HEADER_STR_SIZE + MAX_LOG_MSG_SIZE;


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
string HeaderString(const string& name, LOG_LEVEL level);
string StringFromLogLevel(LOG_LEVEL level);

/*********************************************************************************************
     Lifecycle
 *********************************************************************************************/

Logger::Logger(string name, shared_ptr<LoggerSink> sink,
			   LOG_LEVEL level, LOG_LEVEL flushLevel):
	m_name(name),
	m_sinks(vector<shared_ptr<LoggerSink>>()),
	m_level(level),
	m_flushLevel(flushLevel) {
	
		m_sinks.emplace_back(sink);
}

Logger::Logger(string name, vector<shared_ptr<LoggerSink>> sinks,
			   LOG_LEVEL level, LOG_LEVEL flushLevel):
	m_name(name),
	m_sinks(sinks),
	m_level(level),
	m_flushLevel(flushLevel) {
	
}

Logger::~Logger() {
	
}

/*********************************************************************************************
     Public
 *********************************************************************************************/

string Logger::name() const {
	return m_name;
}

vector<shared_ptr<LoggerSink>> Logger::sinks() const {
	return m_sinks;
}

LOG_LEVEL Logger::level() const {
	return m_level;
}

void Logger::level(LOG_LEVEL level) {
	m_level = level;
}

void Logger::log(LOG_LEVEL level, const char* message) {
	
	if (static_cast<underlying_type<LOG_LEVEL>::type>(level)
		>= static_cast<underlying_type<LOG_LEVEL>::type>(m_level)) {

		char lineStr[MAX_LOG_LINE_SIZE];
		snprintf(lineStr, MAX_LOG_LINE_SIZE, "%s %s",
				 HeaderString(m_name, m_level).c_str(), message);
		
		for (auto sink : m_sinks) {
			
#if defined(DESKTOP)
			if (dynamic_pointer_cast<STDLoggerSink>(sink)) {
				dynamic_pointer_cast<STDLoggerSink>(sink)->write(lineStr, level);
			}
#elif defined(ANDROID)
			if (dynamic_pointer_cast<AndroidLoggerSink>(sink)) {
				dynamic_pointer_cast<AndroidLoggerSink>(sink)->write(message, m_name.c_str(), level);
			}
#endif
			if (dynamic_pointer_cast<FileLoggerSink>(sink)) {
				dynamic_pointer_cast<FileLoggerSink>(sink)->write(lineStr);
			}
		}
		
		if (static_cast<underlying_type<LOG_LEVEL>::type>(level)
			>= static_cast<underlying_type<LOG_LEVEL>::type>(m_flushLevel)) {
			flush();
		}
	}
}

void Logger::log(LOG_LEVEL level, const char* format, va_list args) {
	
	char msg[MAX_LOG_MSG_SIZE];

	// https://en.cppreference.com/w/c/io/vfprintf
	vsnprintf(msg, MAX_LOG_MSG_SIZE, format, args);
	
	log(level, msg);
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

void Logger::flush() {

	for (auto& sink : m_sinks) {
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

string HeaderString(const string& name, LOG_LEVEL level) {
	
	char headerStr[MAX_HEADER_STR_SIZE];
	snprintf(headerStr, MAX_HEADER_STR_SIZE, "%s [%s] [%s]",
			 DateString().c_str(), name.c_str(), StringFromLogLevel(level).c_str());
	
	return string((const char*)headerStr);
}

string StringFromLogLevel(LOG_LEVEL level) {
	
	switch (level) {
		case LOG_LEVEL::TRACE_: 	return "trace";
		case LOG_LEVEL::DEBUG_: 	return "debug";
		case LOG_LEVEL::INFO_: 		return "info";
		case LOG_LEVEL::WARN_: 		return "warn";
		case LOG_LEVEL::ERROR_: 	return "error";
		case LOG_LEVEL::CRITICAL_: 	return "critical";
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
		<= static_cast<underlying_type<LOG_LEVEL>::type>(LOG_LEVEL::WARN_)) {
		fprintf(stdout, "%s\n", message);
	}
	else {
		fprintf(stderr, "%s\n", message);
	}
}

#endif


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

FileLoggerSink::FileLoggerSink(boost::filesystem::path relPath,
							   unsigned maxFiles,
							   unsigned maxFilesize):
		m_filepath(relPath),
		m_maxFiles(maxFiles),
		m_maxFilesize(maxFilesize) {

#if defined(ANDROID)
	m_filepath = (*(utils::InternalFilesDirectory())) / relPath;
#endif

	boost::system::error_code errorCode;
	boost::filesystem::create_directories(m_filepath.parent_path(), errorCode);
	if (errorCode.value() != boost::system::errc::success) {
		throw Exception("Couldn't create intermediate directories for log: " + m_filepath.string());
	}

	openStream();
}

FileLoggerSink::~FileLoggerSink() {
	
	flush();
	
	if (m_fileStream && m_fileStream->is_open()) {
		m_fileStream->close();
	}
}

/*********************************************************************************************
     Public
 *********************************************************************************************/

boost::filesystem::path FileLoggerSink::filepath() const {
	return m_filepath;
}

unsigned FileLoggerSink::maxFiles() const {
	return m_maxFiles;
}

unsigned FileLoggerSink::maxFilesize() const {
	return m_maxFilesize;
}

void FileLoggerSink::flush() {
	
	if (m_fileStream && m_fileStream->is_open()) {
		m_fileStream->flush();
	}
}

/*********************************************************************************************
     Internal
 *********************************************************************************************/

void FileLoggerSink::write(const char* message) {
	
	*m_fileStream << message << endl;

	checkRotate();
}

/*********************************************************************************************
     Private
 *********************************************************************************************/

void FileLoggerSink::openStream() {
	
	if (m_fileStream && m_fileStream->is_open()) {
		m_fileStream->close();
	}

#warning check file writable
	m_fileStream = make_shared<ofstream>(m_filepath.string(), fstream::out | fstream::app);
}

void FileLoggerSink::checkRotate() {
	
	if (boost::filesystem::exists(m_filepath)) {
		if (boost::filesystem::file_size(m_filepath) > m_maxFilesize) {
			rotate();
		}
	}
}

void FileLoggerSink::rotate() {

	// find list of existing files
	// start at index 0, count down until the next isn't found

	auto stem = m_filepath.stem();
	auto extension = m_filepath.extension();
	
	auto existing = vector<boost::filesystem::path>();

	existing.emplace_back(m_filepath);
	
	unsigned i = 0;
	while (true) {
		
		auto path = m_filepath.parent_path() / boost::filesystem::path(stem.string() + to_string(i) + extension.string());
		
		if (boost::filesystem::exists(path)) {
			existing.emplace_back(path);
			++i;
		}
		else {
			break;
		}
	}
	
	// go through from the end and move each file down an index
	
	unsigned index = existing.size();
	
	for (vector<boost::filesystem::path>::reverse_iterator i = existing.rbegin(); i != existing.rend(); ++i ) {
		auto path = *i;
		
		if (index > m_maxFiles) {
			
			boost::system::error_code errorCode;
			boost::filesystem::remove(path, errorCode);
			if (errorCode.value() != boost::system::errc::success) {
				throw Exception("Cannot remove log file: " + path.string());
			}
		}
		else {
			
			auto existStem = path.stem();
//			auto oldExtension = path.extension();

			//boost::filesystem::path newPath;
//			if (path.string() == m_filepath) {
//				newPath = boost::filesystem::path(oldStem.string() + to_string(index-1) + oldExtension.string());
//			}
//			else {
				//newPath = boost::filesystem::path(stem.string().substr(0, stem.string().length()-1) + to_string(index-1) + extension.string());
				boost::filesystem::path newPath = path.parent_path() / boost::filesystem::path(existStem.string().substr(0, stem.string().length()) + to_string(index-1) + extension.string());
//			}

			boost::filesystem::rename(path, newPath);
		}
		
		--index;
	}
	
	// move the last file
	
	auto newPath = m_filepath.parent_path() / boost::filesystem::path(stem.string() + string("0") + extension.string());
	
	openStream();
}
