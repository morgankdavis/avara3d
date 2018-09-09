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


/***************************************************************************************
   	 Lifecycle
 ***************************************************************************************/

Buffer::Buffer(const boost::filesystem::path& path):
	m_pointer(nullptr),
	m_size(0) {
	
		ifstream inStream(path.string(), ios::binary | ios::ate); // ate == initial position at eof
		ifstream::pos_type pos = inStream.tellg();
		m_pointer = (unsigned char*)malloc(pos);
		inStream.seekg(0, ios::beg);
		inStream.read((char*)m_pointer, pos);
		m_size = pos;
}

Buffer::Buffer(const unsigned char* buf, std::size_t size):
	m_pointer(nullptr),
	m_size(0) {

		m_pointer = (unsigned char*)malloc(size);
		memcpy(m_pointer, buf, size);
		m_size = size;
}

Buffer::Buffer(const vector<unsigned char>& buf):
	Buffer(&buf[0], buf.size()) {
}

Buffer::Buffer(const Buffer& other) { // copy constructor
	// allotate our new memory and copy 'other' data into ours
	
	size_t bufSize = other.m_size;
	m_pointer = (unsigned char *)malloc(bufSize);
	memcpy(m_pointer, other.m_pointer, bufSize);
	m_size = bufSize;
}

Buffer& Buffer::operator=(const Buffer& other) { // copy assignment
	// make a copy of 'other's data, delete ours, and move their data into ours
	
	size_t bufSize = other.m_size;
	unsigned char* tempPointer = (unsigned char *)malloc(bufSize);
	memcpy(tempPointer, other.m_pointer, bufSize);
	if (m_pointer) {
		free(m_pointer);
	}
	m_pointer = tempPointer;
	m_size = bufSize;
	
	return *this;
}

Buffer::~Buffer() {
	AE_LOG->debug("Destroying Buffer {:p}", (void*)this);
	
	if (m_pointer) {
		free(m_pointer);
	}
}

/***************************************************************************************
     Public
 ***************************************************************************************/

unsigned char* Buffer::pointer() const {
	return m_pointer;
}

unsigned Buffer::size() const {
	return m_size;
}
