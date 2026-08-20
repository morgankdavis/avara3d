//
//  FileLogSink.cc
//  avara3d
//
//  Created by Morgan Davis on 11/9/23.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#include "a3d/log/sink/FileLogSink.h"

#include <format>
#include <stdexcept>

#include "a3d/log/Log.h"

using namespace a3d;
using namespace std;

// [Public Lifecycle Functions]

FileLogSink::FileLogSink(const filesystem::path& relPath, int maxFiles, int maxFilesize):
    _filepath {relPath},
    _maxFiles {maxFiles},
    _maxFilesize {maxFilesize} {

    error_code errorCode;
    filesystem::create_directories(_filepath.parent_path(), errorCode);
    // this bugs me.
    // errc::success or anything evaluating to 0 does not exist, apparently.
    auto code = errorCode.value();
    if (code != 0) {
        // TODO: esception sublass
        throw std::runtime_error(std::format("Error creating intermediate directories for log: '{}', code: {}.",
                                             _filepath.string(), to_string(code)));
    }

    openStream();
}

FileLogSink::~FileLogSink() {

    flush();

    if (_fileStream && _fileStream->is_open()) {
        _fileStream->close();
    }
}

// [Public Member Functions]

const filesystem::path& FileLogSink::filepath() const {
    return _filepath;
}

int FileLogSink::maxFiles() const {
    return _maxFiles;
}

int FileLogSink::maxFilesize() const {
    return _maxFilesize;
}

// [Public LogSink Member Functions]

void FileLogSink::write(const string& output, Log::Level level) {

    *_fileStream << output;

    checkRotate();
}

void FileLogSink::flush() {

    if (_fileStream && _fileStream->is_open()) {
        _fileStream->flush();
    }
}

// [Private Member Functions]

void FileLogSink::openStream() {

    if (_fileStream && _fileStream->is_open()) {
        _fileStream->close();
    }

    _fileStream = make_shared<ofstream>(_filepath.string(), fstream::out | fstream::app);
}

void FileLogSink::checkRotate() {

    if (filesystem::exists(_filepath)) {
        if (filesystem::file_size(_filepath) >= _maxFilesize) {
            rotate();
        }
    }
}

void FileLogSink::rotate() {

    // find list of existing files
    // start at index 0, count down until the next isn't found

    auto stem = _filepath.stem();
    auto extension = _filepath.extension();

    auto existing = vector<filesystem::path>();

    existing.push_back(_filepath);

    int i = 0;
    while (true) {

        auto path =
            _filepath.parent_path() / filesystem::path(stem.string() + to_string(i) + extension.string());

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

    for (auto e = existing.rbegin(); e != existing.rend(); ++e) {
        auto path = *e;

        if (index >= _maxFiles) {

            // ! creates recursion with A3D log !
            //log::i()("Removing log file '{}...'", path.string());

            error_code errorCode;

            // Linux (and maybe macOS?) will let us rename/remove a file while it's open, Windows will crash and burn.
            if (_fileStream && _fileStream->is_open()) {
                _fileStream->close();
            }

            filesystem::remove(path, errorCode);

            auto code = errorCode.value();
            if (code != 0) {
                // TODO: exception subclass
                throw std::runtime_error(std::format("Error removing log file: '{}', code: {}.", path.string(),
                                                     to_string(code)));
            }
        }
        else {

            auto existStem = path.stem();
            auto newPath = path.parent_path()
                           / filesystem::path(existStem.string().substr(0, stem.string().length())
                                              + to_string(index - 1) + extension.string());

            // Linux (and maybe macOS?) will let us rename/remove a file while it's open, Windows will crash and burn.
            if (_fileStream && _fileStream->is_open()) {
                _fileStream->close();
            }

            filesystem::rename(path, newPath);
        }

        --index;
    }

    // move the last file

    auto newPath = _filepath.parent_path() / filesystem::path(stem.string() + string("0") + extension.string());

    openStream();
}
