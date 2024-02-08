//
//  MaterialProperty.cc
//	avara-engine
//
//  Created by Morgan Davis on 10/21/16.
//  Copyright © 2016 Morgan K Davis. All rights reserved.
//

#include "ae/rendering/materials/MaterialProperty.h"

#include "ae/CubeImage.h"
#include "ae/diagnostic/logging/Logger.h"


using namespace ae;
using namespace std;


/*********************************************************************************************
	Pulic Static
 *********************************************************************************************/

//shared_ptr<MaterialProperty> MaterialProperty::ColorProperty(shared_ptr<Color> color) {
//	return make_shared<MaterialProperty>(color);
//}

/*********************************************************************************************
	Lifecycle
 *********************************************************************************************/

MaterialProperty::MaterialProperty():
		_contents(nullptr),
		_wrapS(WRAP_MODE::Repeat),
		_wrapT(WRAP_MODE::Repeat),
		_wrapR(WRAP_MODE::Repeat),
		_minificationFilter(FilterMode::LinearMipmapLinear),
		_magnificationFilter(FilterMode::Linear),
		_maxAnisotropy(16),
		_dirtyMask(MaterialPropertyDirtyMask::All) {
	
		AE_LOG_D("Creating MaterialProperty {:p}", static_cast<void*>(this));
}

//MaterialProperty::MaterialProperty(shared_ptr<Color> contents):
//	MaterialProperty() {
//
//		_contents = contents;
//}

MaterialProperty::MaterialProperty(std::shared_ptr<MaterialPropertyContents> contents):
		MaterialProperty() {

	_contents = contents;
}

MaterialProperty::~MaterialProperty() {
	AE_LOG_D("Destroying MaterialProperty {:p}", static_cast<void*>(this));
}

/*********************************************************************************************
	Public
 *********************************************************************************************/

shared_ptr<MaterialPropertyContents> MaterialProperty::contents() const {
	return _contents;
}

void MaterialProperty::contents(const shared_ptr<MaterialPropertyContents> contents) {
	
	_contents = contents;

	_dirtyMask = MATERIAL_PROPERTY_DIRTY_MASK_ADD(_dirtyMask,
												  MaterialPropertyDirtyMask::Contents);
}

FilterMode MaterialProperty::minificationFilter() const {
	return _minificationFilter;
}

void MaterialProperty::minificationFilter(FilterMode mode) {
	_minificationFilter = mode;

	_dirtyMask = MATERIAL_PROPERTY_DIRTY_MASK_ADD(_dirtyMask,
												  MaterialPropertyDirtyMask::MinificationFilter);
}

FilterMode MaterialProperty::magnificationFilter() const {
	return _magnificationFilter;
}

void MaterialProperty::magnificationFilter(FilterMode mode) {
	_magnificationFilter = mode;

	_dirtyMask = MATERIAL_PROPERTY_DIRTY_MASK_ADD(_dirtyMask,
												  MaterialPropertyDirtyMask::MagnificationFilter);
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

	_dirtyMask = MATERIAL_PROPERTY_DIRTY_MASK_ADD(_dirtyMask,
												  MaterialPropertyDirtyMask::MaxAnisotropy);
}

WRAP_MODE MaterialProperty::wrapS() const {
	return _wrapS;
}

void MaterialProperty::wrapS(WRAP_MODE mode) {
	_wrapS = mode;

	_dirtyMask = MATERIAL_PROPERTY_DIRTY_MASK_ADD(_dirtyMask,
												  MaterialPropertyDirtyMask::WrapS);
}

WRAP_MODE MaterialProperty::wrapT() const {
	return _wrapT;
}

void MaterialProperty::wrapT(WRAP_MODE mode) {
	_wrapT = mode;

	_dirtyMask = MATERIAL_PROPERTY_DIRTY_MASK_ADD(_dirtyMask,
												  MaterialPropertyDirtyMask::WrapT);
}

WRAP_MODE MaterialProperty::wrapR() const {
	return _wrapR;
}

void MaterialProperty::wrapR(WRAP_MODE mode) {
	_wrapR = mode;

	_dirtyMask = MATERIAL_PROPERTY_DIRTY_MASK_ADD(_dirtyMask,
												  MaterialPropertyDirtyMask::WrapR);
}

/*********************************************************************************************
	Internal
 *********************************************************************************************/

MaterialPropertyDirtyMask MaterialProperty::dirtyMask() const {
	return _dirtyMask;
}

void MaterialProperty::dirtyMask(MaterialPropertyDirtyMask mask) {
	_dirtyMask = mask;
}
