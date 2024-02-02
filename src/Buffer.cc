//
//  Buffer.cc
//	avara-engine
//
//  Created by Morgan Davis on 9/5/18.
//  Copyright © 2018 Morgan K Davis. All rights reserved.
//

#include "ae/Buffer.h"

#include <cstring>

#include "ae/Utilities.h"
#include "ae/diagnostic/logging/Logger.h"


using namespace ae;
using namespace ae::utils;
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

Buffer::Buffer(const Buffer& other) { // copy constructor
	// allotate our new memory and copy 'other' data into ours

	size_t bufSize = other._size;
	// TODO: free old pointer?
	_data = (unsigned char*)malloc(bufSize);
	memcpy(_data, other._data, bufSize);
	_size = bufSize;
}

Buffer& Buffer::operator=(const Buffer& other) { // copy assignment
	// make a copy of 'other's data, delete ours, and move their data into ours

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

Buffer::~Buffer() {
	AE_LOG_D("Destroying Buffer {:p}", static_cast<void*>(this));

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
