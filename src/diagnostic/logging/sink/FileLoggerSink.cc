//
//  FileLoggerSink.cc
//  avara3d
//
//  Created by Morgan Davis on 11/9/23.
//  Copyright © 2024 Morgan K Davis. All rights reserved.
//


#include "a3d/diagnostic/logging/sink/FileLoggerSink.h"


#include "fmt/format.h"

#include "a3d/diagnostic/exception/Exception.h"
#include "a3d/diagnostic/logging/Logger.h"


using namespace a3d;
using namespace std;


/*********************************************************************************************
	Public Lifecycle Functions
 *********************************************************************************************/

FileLoggerSink::FileLoggerSink(const filesystem::path& relPath,
							   int maxFiles,
							   int maxFilesize):
		_filepath{relPath},
		_maxFiles{maxFiles},
		_maxFilesize{maxFilesize} {

	error_code errorCode;
	filesystem::create_directories(_filepath.parent_path(), errorCode);
	// this bugs me.
	// errc::success or anything evaluating to 0 does not exist, apparently.
	auto code = errorCode.value();
	if (code != 0) {
		// TODO: esception sublass
		throw Exception(fmt::format("Error creating intermediate directories for log: '{}', code: {}.",
									_filepath.string(), to_string(code)));
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
	Public Member Functions
 *********************************************************************************************/

const filesystem::path& FileLoggerSink::filepath() const {
	return _filepath;
}

int FileLoggerSink::maxFiles() const {
	return _maxFiles;
}

int FileLoggerSink::maxFilesize() const {
	return _maxFilesize;
}

void FileLoggerSink::flush() {

	if (_fileStream && _fileStream->is_open()) {
		_fileStream->flush();
	}
}

/*********************************************************************************************
	Internal Member Functions
 *********************************************************************************************/

void FileLoggerSink::write(const string& output) {

	*_fileStream << output;

	checkRotate();
}

/*********************************************************************************************
	Private Member Functions
 *********************************************************************************************/

void FileLoggerSink::openStream() {

	if (_fileStream && _fileStream->is_open()) {
		_fileStream->close();
	}

	_fileStream = make_shared<ofstream>(_filepath.string(), fstream::out | fstream::app);
}

void FileLoggerSink::checkRotate() {

	if (filesystem::exists(_filepath)) {
		if (filesystem::file_size(_filepath) >= _maxFilesize) {
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

	existing.push_back(_filepath);

	int i = 0;
	while (true) {

		auto path = _filepath.parent_path() / filesystem::path(stem.string() + to_string(i) + extension.string());

		if (filesystem::exists(path)) {
			existing.push_back(path);
			++i;
		}
		else {
			break;
		}
	}

	// go through from the end and move each file down an index

	auto index = existing.size();

	for (auto e = existing.rbegin(); e != existing.rend(); ++e ) {
		auto path = *e;

		if (index >= _maxFiles) {

			// ! creates recursion with A3D log !
			//A3D_LOG_I("Removing log file '{}...'", path.string());

			error_code errorCode;
			filesystem::remove(path, errorCode);

			auto code = errorCode.value();
			if (code != 0) {
				// TODO: exception subclass
				throw Exception(fmt::format("Error removing log file: '', code: {}.",
											path.string(), to_string(code)));
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
