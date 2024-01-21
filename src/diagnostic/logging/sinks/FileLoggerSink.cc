//
// Created by mkd on 11/9/23.
//

#include "ae/diagnostic/logging/sinks/FileLoggerSink.h"


#include "ae/diagnostic/Exception.h"


using namespace ae;
using namespace std;


/*********************************************************************************************
	Lifecycle
 *********************************************************************************************/

FileLoggerSink::FileLoggerSink(filesystem::path relPath,
							   unsigned maxFiles,
							   unsigned maxFilesize):
		_filepath(relPath),
		_maxFiles(maxFiles),
		_maxFilesize(maxFilesize) {

#if defined(ANDROID)
	_filepath = (*(utils::InternalFilesDirectory())) / relPath;
#endif

	error_code errorCode;
	filesystem::create_directories(_filepath.parent_path(), errorCode);
	// this bugs me.
	// errc::success -> used to be boost:errc::success
	// errc::success or anything evaluating to 0 does not exist, apparently.
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

filesystem::path FileLoggerSink::filepath() const {
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

	_fileStream = make_shared<ofstream>(_filepath.string(), fstream::out | fstream::app);
}

void FileLoggerSink::checkRotate() {

	if (filesystem::exists(_filepath)) {
		if (filesystem::file_size(_filepath) > _maxFilesize) {
			rotate();
		}
	}
}

void FileLoggerSink::rotate() {

	// find list of existing filesw
	// start at index 0, count down until the next isn't found

	auto stem = _filepath.stem();
	auto extension = _filepath.extension();

	auto existing = vector<filesystem::path>();

	existing.emplace_back(_filepath);

	unsigned i = 0;
	while (true) {

		auto path = _filepath.parent_path() / filesystem::path(stem.string() + to_string(i) + extension.string());

		if (filesystem::exists(path)) {
			existing.emplace_back(path);
			++i;
		}
		else {
			break;
		}
	}

	// go through from the end and move each file down an index

	unsigned index = existing.size();

	for (vector<filesystem::path>::reverse_iterator i = existing.rbegin(); i != existing.rend(); ++i ) {
		auto path = *i;

		if (index > _maxFiles) {

			error_code errorCode;
			filesystem::remove(path, errorCode);
			// still bugs me. see note above about std:errc
			auto code = errorCode.value();
			if (code != 0) {
				throw Exception("Cannot remove log file: " + path.string() + " [code " + to_string(code) + "]");
			}
		}
		else {

			auto existStem = path.stem();
//			auto oldExtension = path.extension();

			//filesystem::path newPath;
//			if (path.string() == _filepath) {
//				newPath = filesystem::path(oldStem.string() + to_string(index-1) + oldExtension.string());
//			}
//			else {
			//newPath = filesystem::path(stem.string().substr(0, stem.string().length()-1) + to_string(index-1) + extension.string());
			filesystem::path newPath = path.parent_path() / filesystem::path(existStem.string().substr(0, stem.string().length()) + to_string(index-1) + extension.string());
//			}

			filesystem::rename(path, newPath);
		}

		--index;
	}

	// move the last file

	auto newPath = _filepath.parent_path() / filesystem::path(stem.string() + string("0") + extension.string());

	openStream();
}
