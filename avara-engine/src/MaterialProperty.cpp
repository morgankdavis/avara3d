//
//  MaterialProperty.cpp
//	avara-engine
//
//  Created by Morgan Davis on 10/21/16.
//  Copyright © 2016 Morgan K Davis. All rights reserved.
//

#include "MaterialProperty.h"

#include <iostream>

#include <GL/glew.h>

#include "Color.h"
#include "Image.h"


using namespace ae;
using namespace std;


/***************************************************************************************
     MARK:   Lifecycle
 **************************************************************************************/

MaterialProperty::MaterialProperty(const std::string imagePath):
		m_image(make_shared<Image>(imagePath)),
		m_color(nullptr),
		m_wrapS(WrapModeClamp),
		m_wrapT(WrapModeClamp),
		m_minificationFilter(WrapModeLinear),
		m_magnificationFilter(WrapModeLinear),
		m_mipFilter(WrapModeLinear),
		m_maxAnisotropy(0),
		m_glTex(-1) {

		// TODO: temporary
			m_image->load();
}


MaterialProperty::MaterialProperty(const std::shared_ptr<Image> image):
	m_image(image),
	m_color(nullptr),
	m_wrapS(WrapModeClamp),
	m_wrapT(WrapModeClamp),
	m_minificationFilter(WrapModeLinear),
	m_magnificationFilter(WrapModeLinear),
	m_mipFilter(WrapModeLinear),
	m_maxAnisotropy(0),
	m_glTex(-1) {

		// TODO: temporary
		m_image->load();
}

MaterialProperty::MaterialProperty(const std::shared_ptr<Color> color):
	m_image(nullptr),
	m_color(color),
	m_wrapS(WrapModeClamp),
	m_wrapT(WrapModeClamp),
	m_minificationFilter(WrapModeLinear),
	m_magnificationFilter(WrapModeLinear),
	m_mipFilter(WrapModeLinear),
	m_maxAnisotropy(0),
	m_glTex(-1) {
	
}

/***************************************************************************************
     MARK:   Public
 **************************************************************************************/

shared_ptr<Image> MaterialProperty::image() {
	return m_image;
}

void MaterialProperty::image(const shared_ptr<Image> image) {
	m_image = image;
}

shared_ptr<Color> MaterialProperty::color() {
	return m_color;
}

void MaterialProperty::color(const shared_ptr<Color> color) {
	m_color = color;
}

WrapMode MaterialProperty::wrapS() const {
	return m_wrapS;
}

void MaterialProperty::wrapS(const WrapMode mode) {
	m_wrapS = mode;
}

WrapMode MaterialProperty::wrapT() const {
	return m_wrapT;
}

void MaterialProperty::wrapT(const WrapMode mode) {
	m_wrapT = mode;
}

FilterMode MaterialProperty::minificationFilter() const {
	return m_minificationFilter;
}

void MaterialProperty::minificationFilter(const FilterMode mode) {
	m_minificationFilter = mode;
}

FilterMode MaterialProperty::mipFilter() const {
	return m_mipFilter;
}

void MaterialProperty::mipFilter(const FilterMode mode) {
	m_mipFilter = mode;
}

float MaterialProperty::maxAnisotropy() const {
	return m_maxAnisotropy;
}

void MaterialProperty::maxAnisotropy(const float max) {
	m_maxAnisotropy = max;
}

/***************************************************************************************
     MARK:   Internal
 **************************************************************************************/

int MaterialProperty::glTex() {
	if (m_glTex == -1) {
		load();
	}
	return m_glTex;
}

void MaterialProperty::load() {
	
	cout << "Loading texture..." << endl;
	
	// generate, activate and bind texture
	GLuint tex;
	glGenTextures(1, &tex);
	m_glTex = tex; // m_glTex is signed (change!)
	glBindTexture(GL_TEXTURE_2D, tex);
	glTexImage2D(GL_TEXTURE_2D,
				 0,
				 GL_RGBA,
				 image()->width(),
				 image()->height(),
				 0,
				 GL_RGBA,
				 GL_UNSIGNED_BYTE,
				 image()->data());

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
}

