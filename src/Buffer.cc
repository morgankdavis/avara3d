//
//  Buffer.cc
//  avara3d
//
//  Created by Morgan Davis on 9/5/18.
//  Copyright © 2024 Morgan K Davis. All rights reserved.
//

#include "a3d/Buffer.h"

#include <cstring>

#include "a3d/Utilities.h"
#include "a3d/diagnostic/log/Log.h"


using namespace a3d;
using namespace std;


/*********************************************************************************************
	 Public Lifecycle Functions
 *********************************************************************************************/

Buffer::Buffer(const std::filesystem::path& path):
		_data{},
		_size{0} {

	// TODO: check path or exception
	ifstream inStream(path.string(), ios::binary | ios::ate);
	ifstream::pos_type pos = inStream.tellg();
	_data = make_unique<byte*>(new byte[pos]);
	inStream.seekg(0, ios::beg);
	inStream.read((char*)&((*_data)[0]), pos);
	_size = pos;
}

Buffer::Buffer(const vector<byte>& buf):
		Buffer(&buf[0], buf.size()) {
}

Buffer::Buffer(const byte* buf, std::size_t size):
		_data(nullptr),
		_size(0) {

	_data = make_unique<byte*>(new byte[size]);
	memcpy(&((*_data)[0]), buf, size);
	_size = size;
}

Buffer::~Buffer() {
	A3D_LOG_D("Destroying Buffer {:p}", static_cast<void*>(this));
}

/*********************************************************************************************
	Public Member Functions
 *********************************************************************************************/

byte* Buffer::data() const {
	return &((*_data)[0]);
}

size_t Buffer::size() const {
	return _size;
}

byte* Buffer::operator*() const {
	return &((*_data)[0]);
}

byte Buffer::operator[](size_t idx) const {
	return (*_data)[idx];
}
