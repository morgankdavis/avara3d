//
//  Image.cpp
//	avara-engine
//
//  Created by Morgan Davis on 10/21/16.
//  Copyright © 2016 Morgan K Davis. All rights reserved.
//

#include "Image.h"

#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include "Exception.h"
#include "Logger.h"
#include "Utilities.h"


using namespace ae;
using namespace ae::utils;
using namespace std;


/***************************************************************************************
     Lifecycle
 ***************************************************************************************/

#ifndef ANDROID
Image::Image(const boost::filesystem::path& path, bool flipHorizontal):
	m_data(nullptr),
	m_width(0),
	m_height(0) {
		
		auto data = BinaryFile(path);
		loadBinary(data, flipHorizontal);
}
#endif

Image::Image(vector<unsigned char>& data, bool flipHorizontal):
	m_data(nullptr),
	m_width(0),
	m_height(0) {
		
		loadBinary(data, flipHorizontal);
}

Image::Image(unsigned char* data, unsigned width, unsigned height, bool flipHorizontal):
	m_data(nullptr),
	m_width(width),
	m_height(height) {
	
		m_data = (unsigned char*)malloc(width * height * 4);
		memcpy(m_data, data, width * height * 4);
		
		flip();
}

Image::Image(const Image& other) { // copy constructor

	// allotate our new memory and copy 'other' data into ours
	
	size_t dataSize = other.m_width * other.m_height * 4;
	m_data = (unsigned char *)malloc(dataSize);
	memcpy(m_data, other.m_data, dataSize);
	
	m_width = other.m_width;
	m_height = other.m_height;
}

Image& Image::operator=(const Image& other) { // copy assignment
	
	// make a copy of 'other's data, delete ours, and move their data into ours
	
	size_t dataSize = other.m_width * other.m_height * 4;
	
	unsigned char* tempData = (unsigned char *)malloc(dataSize);
	memcpy(tempData, other.m_data, dataSize);
	free(m_data);
	m_data = tempData;
	
	m_width = other.m_width;
	m_height = other.m_height;
	
	return *this;
}

Image::~Image() {
	if (m_data != nullptr) {
		free(m_data);
	}
}

/***************************************************************************************
     Public
 ***************************************************************************************/

unsigned Image::width() const {
	return m_width;
}

unsigned Image::height() const {
	return m_height;
}

bool Image::writePNG(boost::filesystem::path path) const {
	
	return !stbi_write_png(path.string().c_str(), m_width, m_height, 4, m_data, m_width*4);
}

/***************************************************************************************
     Internal
 ***************************************************************************************/

unsigned char* Image::data() const {
	return m_data;
}

/***************************************************************************************
     Private
 ***************************************************************************************/

void Image::loadBinary(vector<unsigned char>& data, bool flipHorizontal) {
	
	int width;
	int height;
	int num_byte_pix;
	
	m_data = stbi_load_from_memory(&data[0], data.size(), &width, &height, &num_byte_pix, 4);
	
	if (!m_data) {
//		char errMsg[1024];
//		sprintf(errMsg, "Couldn't load image at path: %s\n",  path.string().c_str());
//		throw Exception(errMsg);
		throw Exception("Failed to load image data.");
	}
	
	m_width = width;
	m_height = height;
	
	if (flipHorizontal) {
		flip();
	}
}

void Image::flip() {
	// this is not needed for cube maps (?)
	int width_in_bytes = m_width * 4;
	unsigned char *top = NULL;
	unsigned char *bottom = NULL;
	unsigned char temp = 0;
	int half_height = m_height / 2;
	for (int row = 0; row < half_height; ++row) {
		top = m_data + row * width_in_bytes;
		bottom = m_data + (m_height - row - 1) * width_in_bytes;
		for (int col = 0; col < width_in_bytes; col++) {
			temp = *top;
			*top = *bottom;
			*bottom = temp;
			++top;
			++bottom;
		}
	}
}
