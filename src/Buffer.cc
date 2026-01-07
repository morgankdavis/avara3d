//
//  Buffer.cc
//  avara3d
//
//  Created by Morgan Davis on 9/5/18.
//  Copyright © 2024 Morgan K Davis. All rights reserved.
//

#include "a3d/Buffer.h"

#include <cstring>
#include <format>
#include <fstream>
#include <stdexcept>

#include "a3d/log/Log.h"

using namespace a3d;
using namespace std;

///  Public Lifecycle Functions ///

Buffer::Buffer(const std::filesystem::path& path):
		_data{},
		_size{0} {

	ifstream in(path, ios::binary | ios::ate);
	if (!in) {
		throw runtime_error(std::format("Failed to open file: {}", string(path)));
	}

	const auto endPos = in.tellg();
	if (endPos < 0) {
		throw runtime_error("tellg() failed");
	}

	_size = static_cast<size_t>(endPos);
	_data = (_size > 0) ? make_unique<byte[]>(_size) : nullptr;

	in.seekg(0, ios::beg);
	if (_size > 0) {
		in.read(reinterpret_cast<char*>(_data.get()),
				static_cast<std::streamsize>(_size));
		if (!in) {
			throw runtime_error(std::format("Failed to read file: {}", string(path)));
		}
	}
}

Buffer::Buffer(const vector<byte>& buf):
		Buffer(buf.data(), buf.size()) {}

Buffer::Buffer(const byte* buf, size_t size):
		_data(size ? make_unique<byte[]>(size) : nullptr),
		  _size(size) {

	if (_size > 0 && buf) {
		memcpy(_data.get(), buf, _size);
	}
}

Buffer::Buffer(size_t size):
		_data(size ? make_unique<byte[]>(size) : nullptr),
		  _size(size) {}

Buffer::~Buffer() {
	log::d()("Destroying Buffer {:p}", static_cast<void*>(this));
}

/// Public Member Functions ///

byte* Buffer::data() const {
	return _data.get();
}

size_t Buffer::size() const {
	return _size;
}

byte* Buffer::operator*() const {
	return _data.get();
}

byte Buffer::operator[](size_t idx) const {
	return _data[idx];
}
