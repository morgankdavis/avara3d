//
//  MaterialProperty.cpp
//	avara-engine
//
//  Created by Morgan Davis on 10/21/16.
//  Copyright © 2016 Morgan K Davis. All rights reserved.
//

#include "MaterialProperty.h"

#include <iostream>

#include "Color.h"
#include "CubeImage.h"
#include "Image.h"
#include "Logger.h"
#include "Utilities.h"


using namespace ae;
using namespace std;


/***************************************************************************************
     Lifecycle
 ***************************************************************************************/

MaterialProperty::MaterialProperty():
	m_contents(nullptr),
	m_wrapS(WRAP_MODE::REPEAT),
	m_wrapT(WRAP_MODE::REPEAT),
	m_wrapR(WRAP_MODE::REPEAT),
	m_minificationFilter(FILTER_MODE::LINEAR_MIPMAP_LINEAR),
	m_magnificationFilter(FILTER_MODE::LINEAR),
	m_maxAnisotropy(16),
	m_dirtyBits(MATERIAL_PROPERTY_DIRTY_BITS::ALL),
	m_textureID(0),
	m_replacedTextureIDs(vector<TEXTURE_ID>()) {
	
}

MaterialProperty::MaterialProperty(shared_ptr<MaterialPropertyContents> contents):
	MaterialProperty() {
		
		m_contents = contents;
}

MaterialProperty::~MaterialProperty() {

}

/***************************************************************************************
     Public
 ***************************************************************************************/

shared_ptr<MaterialPropertyContents> MaterialProperty::contents() const {
	return m_contents;
}

void MaterialProperty::contents(const shared_ptr<MaterialPropertyContents> contents) {
	
	m_contents = contents;
	
	if (dynamic_pointer_cast<Color>(contents)) {
		
	}
	
	if (dynamic_pointer_cast<Image>(contents)) {
		if (m_textureID != 0) {
			m_replacedTextureIDs.emplace_back(m_textureID);
			m_textureID = 0;
		}
	}
	
	if (dynamic_pointer_cast<CubeImage>(contents)) {
		if (m_textureID != 0) {
			m_replacedTextureIDs.emplace_back(m_textureID);
			m_textureID = 0;
		}
	}
	
	m_dirtyBits = MATERIAL_PROPERTY_DIRTY_BITS_ADD(m_dirtyBits,
												   MATERIAL_PROPERTY_DIRTY_BITS::CONTENTS);
}

//dynamic_pointer_cast

//shared_ptr<Image> MaterialProperty::image() const {
//	return m_image;
//}
//
//void MaterialProperty::image(const shared_ptr<Image> image) {
//	m_image = image;
//	
//	if (m_textureID != 0) {
//		m_replacedTextureIDs.emplace_back(m_textureID);
//		m_textureID = 0;
//	}
//	
//	m_dirtyBits = MATERIAL_PROPERTY_DIRTY_BITS_ADD(m_dirtyBits,
//												   MATERIAL_PROPERTY_DIRTY_BITS::CONTENTS);
//}
//
//shared_ptr<Color> MaterialProperty::color() const {
//	return m_color;
//}
//
//void MaterialProperty::color(const shared_ptr<Color> color) {
//	m_color = color;
//	
//	m_dirtyBits = MATERIAL_PROPERTY_DIRTY_BITS_ADD(m_dirtyBits,
//												   MATERIAL_PROPERTY_DIRTY_BITS::CONTENTS);
//}
//
//shared_ptr<vector<shared_ptr<Image>>> MaterialProperty::cube() const {
//	return m_cube;
//}
//
//void MaterialProperty::cube(const shared_ptr<vector<shared_ptr<Image>>> cube) {
//	m_cube = cube;
//	
//	if (m_textureID != 0) {
//		m_replacedTextureIDs.emplace_back(m_textureID);
//		m_textureID = 0;
//	}
//	
//	m_dirtyBits = MATERIAL_PROPERTY_DIRTY_BITS_ADD(m_dirtyBits,
//												   MATERIAL_PROPERTY_DIRTY_BITS::CONTENTS);
//}

FILTER_MODE MaterialProperty::minificationFilter() const {
	return m_minificationFilter;
}

void MaterialProperty::minificationFilter(FILTER_MODE mode) {
	m_minificationFilter = mode;
	
	m_dirtyBits = MATERIAL_PROPERTY_DIRTY_BITS_ADD(m_dirtyBits,
												   MATERIAL_PROPERTY_DIRTY_BITS::MINIFICATION_FILTER);
}

FILTER_MODE MaterialProperty::magnificationFilter() const {
	return m_magnificationFilter;
}

void MaterialProperty::magnificationFilter(FILTER_MODE mode) {
	m_magnificationFilter = mode;

	m_dirtyBits = MATERIAL_PROPERTY_DIRTY_BITS_ADD(m_dirtyBits,
												   MATERIAL_PROPERTY_DIRTY_BITS::MAGNIFICATION_FILTER);
}

float MaterialProperty::maxAnisotropy() const {
	return m_maxAnisotropy;
}

void MaterialProperty::maxAnisotropy(float max) {
	m_maxAnisotropy = max;

	m_dirtyBits = MATERIAL_PROPERTY_DIRTY_BITS_ADD(m_dirtyBits,
												   MATERIAL_PROPERTY_DIRTY_BITS::MAX_ANISTROPY);
}

WRAP_MODE MaterialProperty::wrapS() const {
	return m_wrapS;
}

void MaterialProperty::wrapS(WRAP_MODE mode) {
	m_wrapS = mode;

	m_dirtyBits = MATERIAL_PROPERTY_DIRTY_BITS_ADD(m_dirtyBits,
												   MATERIAL_PROPERTY_DIRTY_BITS::WRAP_S);
}

WRAP_MODE MaterialProperty::wrapT() const {
	return m_wrapT;
}

void MaterialProperty::wrapT(WRAP_MODE mode) {
	m_wrapT = mode;

	m_dirtyBits = MATERIAL_PROPERTY_DIRTY_BITS_ADD(m_dirtyBits,
												   MATERIAL_PROPERTY_DIRTY_BITS::WRAP_T);
}

WRAP_MODE MaterialProperty::wrapR() const {
	return m_wrapR;
}

void MaterialProperty::wrapR(WRAP_MODE mode) {
	m_wrapR = mode;
	
	m_dirtyBits = MATERIAL_PROPERTY_DIRTY_BITS_ADD(m_dirtyBits,
												   MATERIAL_PROPERTY_DIRTY_BITS::WRAP_R);
}

/***************************************************************************************
     Internal
 ***************************************************************************************/

MATERIAL_PROPERTY_DIRTY_BITS MaterialProperty::dirtyBits() const {
	return m_dirtyBits;
}

void MaterialProperty::dirtyBits(MATERIAL_PROPERTY_DIRTY_BITS bits) {
	m_dirtyBits = bits;
}

TEXTURE_ID MaterialProperty::textureID() const {
	return m_textureID;
}

void MaterialProperty::textureID(TEXTURE_ID textureID) {
	m_textureID = textureID;
}

std::vector<TEXTURE_ID> MaterialProperty::replacedTextureIDs() const {
	return m_replacedTextureIDs;
}

void MaterialProperty::replacedTextureIDs(std::vector<TEXTURE_ID> textureIDs) {
	m_replacedTextureIDs = textureIDs;
}
