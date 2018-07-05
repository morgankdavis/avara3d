//
//  Image.cc
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
	m_height(0),
	m_bytesPerPixel(0) {
		
		auto data = BinaryFile(path);
		loadBinary(data, flipHorizontal);
}
#endif

Image::Image(vector<unsigned char>& data, bool flipHorizontal):
	m_data(nullptr),
	m_width(0),
	m_height(0),
	m_bytesPerPixel(0) {
		
		loadBinary(data, flipHorizontal);
}

Image::Image(unsigned char* data, unsigned width, unsigned height,
			 unsigned bytesPerPixel, bool flipHorizontal):
	m_data(nullptr),
	m_width(width),
	m_height(height),
	m_bytesPerPixel(bytesPerPixel) {
	
		m_data = (unsigned char*)malloc(width * height * bytesPerPixel);
		memcpy(m_data, data, width * height * bytesPerPixel);
		
		flip();
}

Image::Image(const Image& other) { // copy constructor
	// allotate our new memory and copy 'other' data into ours
	
	AE_LOG->debug("### IMAGE {:p} COPY CONSTRUCTOR - OTHER: {:p}", (void*)this, (void*)&other);
	
	size_t dataSize = other.m_width * other.m_height * other.m_bytesPerPixel;
	m_data = (unsigned char *)malloc(dataSize);
	memcpy(m_data, other.m_data, dataSize);
	
	m_width = other.m_width;
	m_height = other.m_height;
}

Image& Image::operator=(const Image& other) { // copy assignment
	// make a copy of 'other's data, delete ours, and move their data into ours
	
	AE_LOG->debug("### IMAGE {:p} COPY ASSIGNMENT - OTHER: {:p}", (void*)this, (void*)&other);
	
	size_t dataSize = other.m_width * other.m_height * other.m_bytesPerPixel;
	
	unsigned char* tempData = (unsigned char *)malloc(dataSize);
	memcpy(tempData, other.m_data, dataSize);
	free(m_data);
	m_data = tempData;
	
	m_width = other.m_width;
	m_height = other.m_height;
	
	return *this;
}

Image::~Image() {
	AE_LOG->debug("Destroying Image {:p}", (void*)this);
	
	if (m_data) {
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

unsigned Image::bytesPerPixel() const {
	return m_bytesPerPixel;
}

bool Image::writePNG(boost::filesystem::path path) const {
	
	return !stbi_write_png(path.string().c_str(), m_width, m_height,
						   m_bytesPerPixel, m_data, m_width*m_bytesPerPixel);
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
	int bytesPerPixel;
	
	m_data = stbi_load_from_memory(&data[0], data.size(), &width, &height, &bytesPerPixel, 4);
	// force bytesPerPixel = 4 since we told STB to pad it
	// (STB fills this with the ACTUAL BPP in the file, but pads to what we ask)
	bytesPerPixel = 4;
	
	if (!m_data) {
		throw Exception("Failed to load image data.");
	}
	
	AE_LOG->debug("Loaded image data. width: {}, height: {}, bytesPerPixel: {}",
				  width, height, bytesPerPixel);
	
	m_width = width;
	m_height = height;
	m_bytesPerPixel = bytesPerPixel;
	
	AE_LOG->debug("image {:p}: width: {}, height: {}, bytesPerPixel: {}, data size: {}",
				  (void*)this, m_width, m_height, m_bytesPerPixel,
				  m_width * m_height * m_bytesPerPixel);
	
	if (flipHorizontal) {
		flip();
	}
}

void Image::flip() {
	// this is not needed for cube maps (?)
	int widthInBytes = m_width * m_bytesPerPixel;
	unsigned char* top = NULL;
	unsigned char* bottom = NULL;
	unsigned char temp = 0;
	int halfHeight = m_height / 2;
	for (int row = 0; row < halfHeight; ++row) {
		top = m_data + row * widthInBytes;
		bottom = m_data + (m_height - row - 1) * widthInBytes;
		for (int col = 0; col < widthInBytes; col++) {
			temp = *top;
			*top = *bottom;
			*bottom = temp;
			++top;
			++bottom;
		}
	}
}
