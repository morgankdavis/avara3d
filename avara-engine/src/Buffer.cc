//
//  Buffer.cc
//	avara-engine
//
//  Created by Morgan Davis on 9/5/18.
//  Copyright © 2018 Morgan K Davis. All rights reserved.
//

#include "Buffer.h"

#include "Logger.h"


using namespace ae;
using namespace std;


/***************************************************************************************
   	 Lifecycle
 ***************************************************************************************/

Buffer::Buffer(boost::filesystem::path& path) {
	
	// read file then use Buffer(const unsigned char* buf, unsigned len);
}

Buffer::Buffer(const unsigned char* buf, unsigned len):
	m_pointer(nullptr),
	m_length(0) {
	
		
}

Buffer::Buffer(const vector<unsigned char>& buf):
	Buffer(&buf[0], buf.size()) {
	
}

Buffer::Buffer(const Buffer& other) { // copy constructor
	// allotate our new memory and copy 'other' data into ours
	
//	size_t dataSize = other.m_width * other.m_height * other.m_bytesPerPixel;
//	m_data = (unsigned char *)malloc(dataSize);
//	memcpy(m_data, other.m_data, dataSize);
//	
//	m_width = other.m_width;
//	m_height = other.m_height;
}

Buffer& Buffer::operator=(const Buffer& other) { // copy assignment
	// make a copy of 'other's data, delete ours, and move their data into ours
	
//	size_t dataSize = other.m_width * other.m_height * other.m_bytesPerPixel;
//	
//	unsigned char* tempData = (unsigned char *)malloc(dataSize);
//	memcpy(tempData, other.m_data, dataSize);
//	free(m_data);
//	m_data = tempData;
//	
//	m_width = other.m_width;
//	m_height = other.m_height;
	
	return *this;
}

Buffer::~Buffer() {
	AE_LOG->debug("Destroying Buffer {:p}", (void*)this);
	
//	if (m_data) {
//		stbi_image_free(m_data);
//	}
}

/***************************************************************************************
     Public
 ***************************************************************************************/

unsigned char* Buffer::pointer() const {
	
	return m_pointer;
}

unsigned Buffer::length() const {
	
	return m_length;
}
