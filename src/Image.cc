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
Image::Image(const std::filesystem::path& path, bool flipVertical):
		_data(nullptr),
		_width(0),
		_height(0),
		_bytesPerPixel(0) {

	auto buffer = Buffer(path);
	loadBuffer(buffer, flipVertical);
}
#endif

Image::Image(std::shared_ptr<Buffer> buffer, bool flipVertical):
		_data(nullptr),
		_width(0),
		_height(0),
		_bytesPerPixel(0) {

	loadBuffer(*buffer, flipVertical);
}

Image::Image(shared_ptr<Buffer> rawBuffer, unsigned width, unsigned height,
			 unsigned bytesPerPixel, bool flip):
		_data(rawBuffer),
		_width(width),
		_height(height),
		_bytesPerPixel(bytesPerPixel) {

	if (flip) {
		flipVertical();
	}
}

Image::~Image() {
	AE_LOG_D("Destroying Image {:p}", (void*)this);
	
//	if (_data) {
//		stbi_image_free(&_data);
//	}
}

/*********************************************************************************************
	Public
 *********************************************************************************************/

unsigned Image::width() const {
	return _width;
}

unsigned Image::height() const {
	return _height;
}

unsigned Image::bytesPerPixel() const {
	return _bytesPerPixel;
}

bool Image::writePNG(std::filesystem::path path) const {
	
	return !stbi_write_png(path.string().c_str(),
						   _width, _height, _bytesPerPixel,
						   _data->pointer(),
						   _width*_bytesPerPixel);
}

/*********************************************************************************************
	Internal
 *********************************************************************************************/

shared_ptr<Buffer> Image::data() const {
	return _data;
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

	_data = make_shared<Buffer>(static_cast<const unsigned char*>(imgData),
								static_cast<size_t>(width * height * bytesPerPixel));
	
	stbi_image_free(imgData);
	
	AE_LOG_D("Loaded image data. width: {}, height: {}, bytesPerPixel: {}",
				  width, height, bytesPerPixel);
	
	_width = width;
	_height = height;
	_bytesPerPixel = bytesPerPixel;

	if (flip) {
		flipVertical();
	}
}

void Image::flipVertical() { // "flip"

	int widthInBytes = _width * _bytesPerPixel;
	unsigned char* top = NULL;
	unsigned char* bottom = NULL;
	unsigned char temp = 0;
	int halfHeight = _height / 2;
	for (int row = 0; row < halfHeight; ++row) {
		top = _data->pointer() + row * widthInBytes;
		bottom = _data->pointer() + (_height - row - 1) * widthInBytes;
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
//	int widthInBytes = _width * _bytesPerPixel;
//	unsigned char* top = NULL;
//	unsigned char* bottom = NULL;
//	unsigned char temp = 0;
//	int halfHeight = _height / 2;
//	for (int row = 0; row < halfHeight; ++row) {
//		top = _data->pointer() + row * widthInBytes;
//		bottom = _data->pointer() + (_height - row - 1) * widthInBytes;
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
