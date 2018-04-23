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

Image::Image(const boost::filesystem::path& path, bool flipHorizontal):
	m_data(nullptr),
	m_width(0),
	m_height(0) {
		
		loadFile(path, flipHorizontal);
}

Image::Image(unsigned char* data, unsigned width, unsigned height, bool flipHorizontal):
	m_data(nullptr),
	m_width(width),
	m_height(height) {
	
		m_data = (unsigned char*)malloc(width * height * 4);
		memcpy(m_data, data, width * height * 4);
		
		flip();
}

Image::~Image() {
	if (m_data != nullptr) {
		//stbi_image_free(m_data);
		//stbi_image_free() == free()
		free(m_data);
		m_data = nullptr;
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
	
	// int stbi_write_png(char const *filename, int x, int y, int comp, const void *data, int stride_bytes)
	
	stbi_write_png(path.string().c_str(), m_width, m_height, 4, m_data, m_width*4);
	
	// ret 0 == fail
	
	return true;
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

void Image::loadFile(boost::filesystem::path path, bool flipHorizontal) {
	
	//cout << "Loading image at path: " << m_path << endl;
	AE_LOG->info("Loading image at path {}...", path.string());
	
	int width, height, num_byte_pix;
	//const char *path_cstr = path.string().c_str();

    // work-around for path.string().c_str() encoding error in Win7
	auto fileBuf = LoadBinaryFile(path);

	//m_data = stbi_load(path_cstr, &width, &height, &num_byte_pix, 4);
    m_data = stbi_load_from_memory(&fileBuf[0], fileBuf.size(), &width, &height, &num_byte_pix, 4);
	
	if (!m_data) {
		//printf("Error loading image at path: %s\n", path_cstr);
		char errMsg[1024];
		sprintf(errMsg, "Couldn't load image at path: %s\n",  path.string().c_str());
		throw Exception(errMsg);
		//AE_LOG->error("Error loading image at path: {}", path_cstr);
		return;
	}
	
	m_width = width;
	m_height = height;
	
	if (flipHorizontal) {
		flip();
		
		//		// this is not needed for cube maps (?)
		//		int width_in_bytes = width * 4;
		//		unsigned char *top = NULL;
		//		unsigned char *bottom = NULL;
		//		unsigned char temp = 0;
		//		int half_height = height / 2;
		//		for (int row = 0; row < half_height; ++row) {
		//			top = m_data + row * width_in_bytes;
		//			bottom = m_data + (height - row - 1) * width_in_bytes;
		//			for (int col = 0; col < width_in_bytes; col++) {
		//				temp = *top;
		//				*top = *bottom;
		//				*bottom = temp;
		//				++top;
		//				++bottom;
		//			}
		//		}
	}
	
	AE_LOG->info("Done.");
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
