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

#include "Buffer.h"
#include "Exception.h"
#include "Logger.h"
#include "Utilities.h"


using namespace ae;
using namespace ae::utils;
using namespace std;


/*********************************************************************************************
     Lifecycle
 *********************************************************************************************/

#ifndef ANDROID
Image::Image(const boost::filesystem::path& path, bool flipVertical):
	m_data(nullptr),
	m_width(0),
	m_height(0),
	m_bytesPerPixel(0) {
	
		//auto data = BinaryFile(path);
		auto buffer = Buffer(path);
		loadBuffer(buffer, flipVertical);
}
#endif

Image::Image(std::shared_ptr<Buffer> buffer, bool flipVertical):
	m_data(nullptr),
	m_width(0),
	m_height(0),
	m_bytesPerPixel(0) {
		
		loadBuffer(*buffer, flipVertical);
}

Image::Image(shared_ptr<Buffer> rawBuffer, unsigned width, unsigned height,
			 unsigned bytesPerPixel, bool flip):
	m_data(rawBuffer),
	m_width(width),
	m_height(height),
	m_bytesPerPixel(bytesPerPixel) {

	if (flip) {
		flipVertical();
	}
}

Image::~Image() {
	AE_LOG_D("Destroying Image {:p}", (void*)this);
	
//	if (m_data) {
//		stbi_image_free(&m_data);
//	}
}

/*********************************************************************************************
     Public
 *********************************************************************************************/

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
						   m_bytesPerPixel, m_data->pointer(), m_width*m_bytesPerPixel);
}

/*********************************************************************************************
     Internal
 *********************************************************************************************/

shared_ptr<Buffer> Image::data() const {
	return m_data;
}

/*********************************************************************************************
     Private
 *********************************************************************************************/

void Image::loadBuffer(Buffer& inBuf, bool flip) {
	
	int width;
	int height;
	int bytesPerPixel;
	
	stbi_uc* imgData = stbi_load_from_memory(inBuf.pointer(), inBuf.size(),
											 &width, &height, &bytesPerPixel,
											 STBI_rgb_alpha);

	// force bytesPerPixel = 4 since we told STB to pad it
	// (STB fills this with the ACTUAL BPP in the file, but pads to what we ask)
	bytesPerPixel = 4;
	
	if (!imgData) {
		throw Exception("Failed to load image data.");
	}

	m_data = make_shared<Buffer>(static_cast<const unsigned char*>(imgData),
								 static_cast<size_t>(width * height * bytesPerPixel));
	
	stbi_image_free(imgData);
	
	AE_LOG_D("Loaded image data. width: {}, height: {}, bytesPerPixel: {}",
				  width, height, bytesPerPixel);
	
	m_width = width;
	m_height = height;
	m_bytesPerPixel = bytesPerPixel;

	if (flip) {
		flipVertical();
	}
}

void Image::flipVertical() { // "flip"

	int widthInBytes = m_width * m_bytesPerPixel;
	unsigned char* top = NULL;
	unsigned char* bottom = NULL;
	unsigned char temp = 0;
	int halfHeight = m_height / 2;
	for (int row = 0; row < halfHeight; ++row) {
		top = m_data->pointer() + row * widthInBytes;
		bottom = m_data->pointer() + (m_height - row - 1) * widthInBytes;
		for (int col = 0; col < widthInBytes; col++) {
			temp = *top;
			*top = *bottom;
			*bottom = temp;
			++top;
			++bottom;
		}
	}
}

void Image::flipHorizontal() { // "mirror"
	// this is not needed for cube maps (?)
//	int widthInBytes = m_width * m_bytesPerPixel;
//	unsigned char* top = NULL;
//	unsigned char* bottom = NULL;
//	unsigned char temp = 0;
//	int halfHeight = m_height / 2;
//	for (int row = 0; row < halfHeight; ++row) {
//		top = m_data->pointer() + row * widthInBytes;
//		bottom = m_data->pointer() + (m_height - row - 1) * widthInBytes;
//		for (int col = 0; col < widthInBytes; col++) {
//			temp = *top;
//			*top = *bottom;
//			*bottom = temp;
//			++top;
//			++bottom;
//		}
//	}
}

//const int width = 100;
//const int height = width;
//const int components = 3;
//unsigned char pixels[width * height * components];
//glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, pixels);
//unsigned char flipPixels[width * height * components];
//for (int i = 0; i < width; ++i) {
//	for (int j = 0; j < height; ++j) {
//		for (int k = 0; k < components; ++k) {
//			flipPixels[i + j * width + k] = pixels[(height) * (width) - ((j+1) * width) + i + k];
//		}
//	}
//}
