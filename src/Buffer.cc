//
//  Buffer.cc
//	avara3d
//
//  Created by Morgan Davis on 9/5/18.
//  Copyright © 2018 Morgan K Davis. All rights reserved.
//

#include "a3d/Buffer.h"

#include <cstring>

#include "a3d/Utilities.h"
#include "a3d/diagnostic/logging/Logger.h"


using namespace a3d;
using namespace a3d::utils;
using namespace std;


/*********************************************************************************************
	 Lifecycle
 *********************************************************************************************/

Buffer::Buffer(const std::filesystem::path& path):
		_data(nullptr),
		_size(0) {

	// TODO: check path or exception
	ifstream inStream(path.string(), ios::binary | ios::ate); // ate == initial position at eof
	ifstream::pos_type pos = inStream.tellg();
	_data = (unsigned char*)malloc(pos);
	inStream.seekg(0, ios::beg);
	inStream.read((char*)_data, pos);
	_size = pos;
}

Buffer::Buffer(const unsigned char* buf, std::size_t size):
		_data(nullptr),
		_size(0) {

	_data = (unsigned char*)malloc(size);
	memcpy(_data, buf, size);
	_size = size;
}

Buffer::Buffer(const vector<unsigned char>& buf):
		Buffer(&buf[0], buf.size()) {
}

Buffer::Buffer(const Buffer& other) {

	size_t bufSize = other._size;
	_data = (unsigned char*)malloc(bufSize);
	memcpy(_data, other._data, bufSize);
	_size = bufSize;
}

Buffer& Buffer::operator=(const Buffer& other) {

	size_t bufSize = other._size;
	auto tempPointer = (unsigned char*)malloc(bufSize);
	memcpy(tempPointer, other._data, bufSize);
	if (_data) {
		free(_data);
	}
	_data = tempPointer;
	_size = bufSize;

	return *this;
}

Buffer::Buffer(Buffer&& other) noexcept {

	if (this != &other) {
		_size = other._size;
		_data = other._data;
		other._size = 0;
		other._data = nullptr;
	}
}

Buffer& Buffer::operator=(Buffer&& other) noexcept {

	_size = other._size;
	_data = other._data;
	other._size = 0;
	other._data = nullptr;
}

Buffer::~Buffer() {
	A3D_LOG_D("Destroying Buffer {:p}", static_cast<void*>(this));

	if (_data) {
		free(_data);
	}
}

/*********************************************************************************************
	Public
 *********************************************************************************************/

unsigned char* Buffer::data() const {
	return _data;
}

unsigned Buffer::size() const {
	return _size;
}

/*********************************************************************************************
	Operator Overloads
 *********************************************************************************************/

unsigned char* Buffer::operator*() const {
	return _data;
}

unsigned char Buffer::operator[](std::size_t idx) const {
	return _data[idx];
}
