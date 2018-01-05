//
//  Image.cpp
//	avara-engine
//
//  Created by Morgan Davis on 10/21/16.
//  Copyright © 2016 Morgan K Davis. All rights reserved.
//

#include "Image.h"

#include <iostream>

#include "stb/stb_image.h"

#include "Global.h"



using namespace ae;
using namespace std;


/***************************************************************************************
     MARK:   Lifecycle
 **************************************************************************************/

Image::Image(const string path):
	m_path(path),
	m_loaded(false),
	m_data(nullptr) {
		
}

Image::~Image() {
	if (m_data) {
		stbi_image_free(m_data);
	}
}

/***************************************************************************************
     MARK:   Internal
 **************************************************************************************/

bool Image::load(bool flipHorizontal) {
	
	if (!m_loaded) {
		
		//cout << "Loading image at path: " << m_path << endl;
		INFO_F("Loading image at path: {}", m_path);
		
		int width, height, num_byte_pix;
		const char *path_cstr = m_path.c_str();
		m_data = stbi_load(path_cstr, &width, &height, &num_byte_pix, 4);

		if (!m_data) {
			//printf("Error loading image at path: %s\n", path_cstr);
			ERROR_F("Error loading image at path: {}", path_cstr);
			return false;
		}

		m_width = width;
		m_height = height;

		if (flipHorizontal) {
			// this is not needed for cube maps (?)
			int width_in_bytes = width * 4;
			unsigned char *top = NULL;
			unsigned char *bottom = NULL;
			unsigned char temp = 0;
			int half_height = height / 2;
			for (int row = 0; row < half_height; ++row) {
				top = m_data + row * width_in_bytes;
				bottom = m_data + (height - row - 1) * width_in_bytes;
				for (int col = 0; col < width_in_bytes; col++) {
					temp = *top;
					*top = *bottom;
					*bottom = temp;
					++top;
					++bottom;
				}
			}
		}
		
		m_loaded = true;
	}
	
	INFO("Done.");
	
	return true;
}

unsigned Image::width() const {
	return m_width;
}

unsigned Image::height() const {
	return m_height;
}

unsigned char* Image::data() const {
	return m_data;
}

string Image::path() const {
	return m_path;
}

bool Image::loaded() const {
	return m_loaded;
}
