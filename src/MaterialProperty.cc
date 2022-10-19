//
//  MaterialProperty.cc
//	avara-engine
//
//  Created by Morgan Davis on 10/21/16.
//  Copyright © 2016 Morgan K Davis. All rights reserved.
//

#include "MaterialProperty.h"

#include <iostream>

#include "Color.h"
#include "CubeImage.h"
#include "Exception.h"
#include "Image.h"
#include "Logger.h"
#include "Utilities.h"


using namespace ae;
using namespace std;


/*********************************************************************************************
	Lifecycle
 *********************************************************************************************/

MaterialProperty::MaterialProperty():
	_contents(nullptr),
	_wrapS(WRAP_MODE::REPEAT),
	_wrapT(WRAP_MODE::REPEAT),
	_wrapR(WRAP_MODE::REPEAT),
	_minificationFilter(FILTER_MODE::LINEAR_MIPMAP_LINEAR),
	_magnificationFilter(FILTER_MODE::LINEAR),
	_maxAnisotropy(16),
	_dirtyBits(MATERIAL_PROPERTY_DIRTY_BITS::ALL) {
	
		AE_LOG_D("Creating MaterialProperty {:p}", (void*)this);
}

MaterialProperty::MaterialProperty(shared_ptr<MaterialPropertyContents> contents):
	MaterialProperty() {
		
		_contents = contents;
}

MaterialProperty::~MaterialProperty() {
	AE_LOG_D("Destroying MaterialProperty {:p}", (void*)this);
}

/*********************************************************************************************
	Public
 *********************************************************************************************/

shared_ptr<MaterialPropertyContents> MaterialProperty::contents() const {
	return _contents;
}

void MaterialProperty::contents(const shared_ptr<MaterialPropertyContents> contents) {
	
	_contents = contents;
	
	_dirtyBits = MATERIAL_PROPERTY_DIRTY_BITS_ADD(_dirtyBits,
												   MATERIAL_PROPERTY_DIRTY_BITS::CONTENTS);
}

FILTER_MODE MaterialProperty::minificationFilter() const {
	return _minificationFilter;
}

void MaterialProperty::minificationFilter(FILTER_MODE mode) {
	_minificationFilter = mode;
	
	_dirtyBits = MATERIAL_PROPERTY_DIRTY_BITS_ADD(_dirtyBits,
												   MATERIAL_PROPERTY_DIRTY_BITS::MINIFICATION_FILTER);
}

FILTER_MODE MaterialProperty::magnificationFilter() const {
	return _magnificationFilter;
}

void MaterialProperty::magnificationFilter(FILTER_MODE mode) {
	_magnificationFilter = mode;

	_dirtyBits = MATERIAL_PROPERTY_DIRTY_BITS_ADD(_dirtyBits,
												   MATERIAL_PROPERTY_DIRTY_BITS::MAGNIFICATION_FILTER);
}

float MaterialProperty::maxAnisotropy() const {
#ifdef ANDROID
	return 0;
#else
	return _maxAnisotropy;
#endif
}

void MaterialProperty::maxAnisotropy(float max) {
#ifdef ANDROID
	throw Exception("Anisotropy is not supported on this platform.");
#endif
	
	_maxAnisotropy = max;

	_dirtyBits = MATERIAL_PROPERTY_DIRTY_BITS_ADD(_dirtyBits,
												   MATERIAL_PROPERTY_DIRTY_BITS::MAX_ANISTROPY);
}

WRAP_MODE MaterialProperty::wrapS() const {
	return _wrapS;
}

void MaterialProperty::wrapS(WRAP_MODE mode) {
	_wrapS = mode;

	_dirtyBits = MATERIAL_PROPERTY_DIRTY_BITS_ADD(_dirtyBits,
												   MATERIAL_PROPERTY_DIRTY_BITS::WRAP_S);
}

WRAP_MODE MaterialProperty::wrapT() const {
	return _wrapT;
}

void MaterialProperty::wrapT(WRAP_MODE mode) {
	_wrapT = mode;

	_dirtyBits = MATERIAL_PROPERTY_DIRTY_BITS_ADD(_dirtyBits,
												   MATERIAL_PROPERTY_DIRTY_BITS::WRAP_T);
}

WRAP_MODE MaterialProperty::wrapR() const {
	return _wrapR;
}

void MaterialProperty::wrapR(WRAP_MODE mode) {
	_wrapR = mode;
	
	_dirtyBits = MATERIAL_PROPERTY_DIRTY_BITS_ADD(_dirtyBits,
												   MATERIAL_PROPERTY_DIRTY_BITS::WRAP_R);
}

/*********************************************************************************************
	Internal
 *********************************************************************************************/

MATERIAL_PROPERTY_DIRTY_BITS MaterialProperty::dirtyBits() const {
	return _dirtyBits;
}

void MaterialProperty::dirtyBits(MATERIAL_PROPERTY_DIRTY_BITS bits) {
	_dirtyBits = bits;
}
