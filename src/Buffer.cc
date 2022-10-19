//
//  Buffer.cc
//	avara-engine
//
//  Created by Morgan Davis on 9/5/18.
//  Copyright © 2018 Morgan K Davis. All rights reserved.
//

#include "Buffer.h"

#include "Logger.h"
#include "Utilities.h"


using namespace ae;
using namespace ae::utils;
using namespace std;


/*********************************************************************************************
	 Lifecycle
 *********************************************************************************************/

Buffer::Buffer(const boost::filesystem::path& path):
	_pointer(nullptr),
	_size(0) {
	
		ifstream inStream(path.string(), ios::binary | ios::ate); // ate == initial position at eof
		ifstream::pos_type pos = inStream.tellg();
		_pointer = (unsigned char*)malloc(pos);
		inStream.seekg(0, ios::beg);
		inStream.read((char*)_pointer, pos);
		_size = pos;
}

Buffer::Buffer(const unsigned char* buf, std::size_t size):
	_pointer(nullptr),
	_size(0) {

		_pointer = (unsigned char*)malloc(size);
		memcpy(_pointer, buf, size);
		_size = size;
}

Buffer::Buffer(const vector<unsigned char>& buf):
		Buffer(&buf[0], buf.size()) {
}

Buffer::Buffer(const Buffer& other) { // copy constructor
	// allotate our new memory and copy 'other' data into ours
	
	size_t bufSize = other._size;
	_pointer = (unsigned char *)malloc(bufSize);
	memcpy(_pointer, other._pointer, bufSize);
	_size = bufSize;
}

Buffer& Buffer::operator=(const Buffer& other) { // copy assignment
	// make a copy of 'other's data, delete ours, and move their data into ours
	
	size_t bufSize = other._size;
	unsigned char* tempPointer = (unsigned char *)malloc(bufSize);
	memcpy(tempPointer, other._pointer, bufSize);
	if (_pointer) {
		free(_pointer);
	}
	_pointer = tempPointer;
	_size = bufSize;
	
	return *this;
}

Buffer::~Buffer() {
	AE_LOG_D("Destroying Buffer {:p}", (void*)this);
	
	if (_pointer) {
		free(_pointer);
	}
}

/*********************************************************************************************
	Public
 *********************************************************************************************/

unsigned char* Buffer::pointer() const {
	return _pointer;
}

unsigned Buffer::size() const {
	return _size;
}
