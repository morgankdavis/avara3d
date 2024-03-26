//
//  Image.cc
//	avara3d
//
//  Created by Morgan Davis on 10/21/16.
//  Copyright © 2016 Morgan K Davis. All rights reserved.
//

#include "a3d/Image.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include "a3d/Buffer.h"
#include "a3d/diagnostic/exception/Exception.h"
#include "a3d/diagnostic/logging/Logger.h"


using namespace a3d;
using namespace std;


/*********************************************************************************************
	Lifecycle
 *********************************************************************************************/

#ifndef ANDROID
Image::Image(const filesystem::path& path,
			 bool flipVertical,
			 bool flipHorizontal):
		_buffer(nullptr),
		_width(0),
		_height(0),
		_bytesPerPixel(0) {

	auto buffer = Buffer(path);
	loadBuffer(buffer, flipVertical, flipHorizontal);
}
#endif

Image::Image(unique_ptr<Buffer> buffer,
			 bool flipVertical,
			 bool flipHorizontal):
		_buffer(nullptr),
		_width(0),
		_height(0),
		_bytesPerPixel(0) {

	loadBuffer(*buffer, flipVertical, flipHorizontal);
}

Image::Image(unique_ptr<Buffer> rawBuffer,
			 int width,
			 int height,
			 int bytesPerPixel,
			 bool flipVertical,
			 bool flipHorizontal):
		_buffer(std::move(rawBuffer)),
		_width(width),
		_height(height),
		_bytesPerPixel(bytesPerPixel) {

	if (flipVertical) {
		Image::flipVertical();
	}
	if (flipHorizontal) {
		Image::flipHorizontal();
	}
}

Image::~Image() {
	A3D_LOG_D("Destroying Image {:p}", static_cast<void*>(this));
	
//	if (_data) {
//		stbi_image_free(&_data);
//	}
}

/*********************************************************************************************
	Public
 *********************************************************************************************/

int Image::width() const {
	return _width;
}

int Image::height() const {
	return _height;
}

int Image::bytesPerPixel() const {
	return _bytesPerPixel;
}

unique_ptr<Image> Image::inverted() const {

	int widthInBytes = _width * _bytesPerPixel;
	int size = widthInBytes * _height;

	auto buf = (unsigned char*)malloc(size);

	auto existing = _buffer->data();
	for (int r=0; r<_height; ++r) {
		for (int c=0; c<widthInBytes; ++c) {
			buf[widthInBytes*r + c] = 255 - (int)existing[widthInBytes*r + c];
		}
	}

	auto inverted = make_unique<Image>(make_unique<Buffer>((byte*)buf, size),
									   _width,
									   _height,
									   _bytesPerPixel,
									   false);
	free(buf);

	return inverted;
}

bool Image::writePNG(filesystem::path path) const {
	
	return !stbi_write_png(path.string().c_str(),
						   _width,
						   _height,
						   _bytesPerPixel,
						   _buffer->data(),
						   _width*_bytesPerPixel);
}

/*********************************************************************************************
	Internal
 *********************************************************************************************/

const Buffer& Image::buffer() const {
	return *_buffer;
}

/*********************************************************************************************
	Private
 *********************************************************************************************/

void Image::loadBuffer(Buffer& inBuf,
					   bool flipVertical,
					   bool flipHorizontal) {
	
	int width;
	int height;
	int bytesPerPixel;
	
	stbi_uc* imgData = stbi_load_from_memory((unsigned char*)*inBuf,//inBuf.data(),
											 inBuf.size(),
											 &width,
											 &height,
											 &bytesPerPixel,
											 STBI_rgb_alpha);

	// force bytesPerPixel = 4 since we told STB to pad it
	// (STB fills this with the ACTUAL BPP in the file, but pads to what we ask)
	bytesPerPixel = 4;
	
	if (!imgData) {
		throw Exception("Failed to load image data.");
	}

	_buffer = make_unique<Buffer>(reinterpret_cast<const std::byte*>(imgData),//static_cast<const unsigned char*>(imgData),
								  static_cast<size_t>(width * height * bytesPerPixel));

	stbi_image_free(imgData);
	
	A3D_LOG_D("Loaded image data. width: {}, height: {}, bytesPerPixel: {}",
				  width, height, bytesPerPixel);
	
	_width = width;
	_height = height;
	_bytesPerPixel = bytesPerPixel;

	if (flipVertical) {
		Image::flipVertical();
	}
	if (flipHorizontal) {
		Image::flipHorizontal();
	}
}

void Image::flipVertical() { // "flip"

	int widthInBytes = _width * _bytesPerPixel;
	unsigned char* top = nullptr;
	unsigned char* bottom = nullptr;
	unsigned char temp = 0;
	int halfHeight = _height / 2;

	auto dPtr = reinterpret_cast<unsigned char*>(_buffer->data());

	for (int r=0; r<halfHeight; ++r) {

		top = dPtr + r * widthInBytes;
		bottom = dPtr + (_height - r - 1) * widthInBytes;

		for (int c=0; c<widthInBytes; ++c) {

			temp = *top;
			*top = *bottom;
			*bottom = temp;
			++top;
			++bottom;
		}
	}
}

// only works for 4-bytes-per-pixel images
void Image::flipHorizontal() { // "mirror"

	int widthInBytes = _width * _bytesPerPixel;
	uint32_t* row = nullptr;
	uint32_t* left = nullptr;
	uint32_t* right = nullptr;
	uint32_t temp = 0;
	int halfWidth = _width / 2;

	auto dPtr = reinterpret_cast<uint32_t*>(_buffer->data());

	for (int r=0; r<_height; ++r) {

		row = dPtr + (_width * r);

		for (int c=0; c<halfWidth; ++c) {

			left = row + c;
			right = row + _width - c - 1;

			temp = *left;
			*left = *right;
			*right = temp;
		}
	}
}
