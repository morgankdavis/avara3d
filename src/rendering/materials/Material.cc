//
//  Material.cc
//	avara-engine
//
//  Created by Morgan Davis on 10/21/16.
//  Copyright © 2016 Morgan K Davis. All rights reserved.
//

#include "ae/rendering/materials/Material.h"

#include <memory>

#include "ae/Color.h"
#include "ae/diagnostic/exceptions/Exception.h"
#include "ae/diagnostic/logging/Logger.h"
#include "ae/rendering/materials/MaterialProperty.h"
#include "ae/rendering/materials/MaterialPropertyContents.h"


using namespace ae;
using namespace glm;
using namespace std;


/*********************************************************************************************
	Public Static
 *********************************************************************************************/

shared_ptr<Material> Material::DefaultMaterial() {
	static shared_ptr<Material> material = nullptr;
	if (!material) {
		auto ambientProperty = make_shared<MaterialProperty>(make_shared<Color>(0.75f));
		auto diffuseProperty = make_shared<MaterialProperty>(make_shared<Color>(0.75f));
		auto specularProperty = make_shared<MaterialProperty>(make_shared<Color>(0.85f));
		material = make_shared<Material>(ambientProperty, diffuseProperty, specularProperty);
		material->doubleSided(true);
		material->specularExponent(75);
	}
	return material;
}

shared_ptr<Material> Material::MissingTextureMaterial() {
	static shared_ptr<Material> material = nullptr;
	if (!material) {
		material = Material::EmissiveMaterial(Color::Magenta());
		material->doubleSided(true);
	}
	return material;
}

shared_ptr<Material> Material::EmissiveMaterial(shared_ptr<MaterialPropertyContents> contents) {
	auto property = make_shared<MaterialProperty>(contents);
	return make_shared<Material>(nullptr, nullptr, nullptr, property);
}

/*********************************************************************************************
	Lifecycle
 *********************************************************************************************/

Material::Material():
		_name(nullopt),
		_ambient(nullptr),
		_diffuse(nullptr),
		_specular(nullptr),
		_emission(nullptr),
		_specularExponent(150.0),
		_locksAmbientWithDiffuse(true),
		_doubleSided(false),
		_fillMode(FillMode::Fill),
		_uvScale(1.0f),
		_blendFunction(BlendFunction::Disabled),
		_dirtyMask(MaterialDirtyMask::All) {

	AE_LOG_D("Creating Material {:p}", static_cast<void*>(this));
}

Material::Material(shared_ptr<MaterialProperty> ambient,
				   shared_ptr<MaterialProperty> diffuse,
				   shared_ptr<MaterialProperty> specular):
		Material() {

	_ambient = ambient;
	_diffuse = diffuse;
	_specular = specular;
}

Material::Material(shared_ptr<MaterialProperty> ambient,
				   shared_ptr<MaterialProperty> diffuse,
				   shared_ptr<MaterialProperty> specular,
				   shared_ptr<MaterialProperty> emission):
		Material() {

	_ambient = ambient;
	_diffuse = diffuse;
	_specular = specular;
	_emission = emission;
}

Material::~Material() {
	AE_LOG_D("Destroying Material {:p}", static_cast<void*>(this));
}

/*********************************************************************************************
	Public
 *********************************************************************************************/

optional<string> Material::name() const {
	return _name;
}

void Material::name(const string& name) {
	_name = name;
}

shared_ptr<MaterialProperty> Material::ambient() const {
	return _ambient;
}

void Material::ambient(const shared_ptr<MaterialProperty> property) {
	_ambient = property;
}

shared_ptr<MaterialProperty> Material::diffuse() const {
	return _diffuse;
}

void Material::diffuse(const shared_ptr<MaterialProperty> property) {
	_diffuse = property;
}

shared_ptr<MaterialProperty> Material::specular() const {
	return _specular;
}

void Material::specular(const shared_ptr<MaterialProperty> property) {
	_specular = property;
}

shared_ptr<MaterialProperty> Material::emission() const {
	return _emission;
}

void Material::emission(const std::shared_ptr<MaterialProperty> property) {
	_emission = property;
}

float Material::specularExponent() const {
	return _specularExponent;
}

void Material::specularExponent(float exponent) {
	_specularExponent = exponent;
}

bool Material::locksAmbientWithDiffuse() const {
	return _locksAmbientWithDiffuse;
}

void Material::locksAmbientWithDiffuse(bool flag) {
	_locksAmbientWithDiffuse = flag;
}

bool Material::doubleSided() const {
	return _doubleSided;
}

void Material::doubleSided(bool flag) {
	_doubleSided = flag;
}

FillMode Material::fillMode() const {
#ifdef ANDROID
	return FILL_MODE::FILL;
#else
	return _fillMode;
#endif
}

void Material::fillMode(FillMode mode) {
#ifdef OPENGL_ES
	if (mode == FILL_MODE::LINES || mode == FILL_MODE::POINTS) {
		throw Exception("Fill mode not supported with this rendering API.");
	}
#endif
	
	_fillMode = mode;
}

float Material::uvScale() const {
	return _uvScale;
}

void Material::uvScale(float scale) {
	_uvScale = scale;
}

BlendFunction Material::blendFunction() const {
	return _blendFunction;
}

void Material::blendFunction(BlendFunction function) {
	_blendFunction = function;
}

/*********************************************************************************************
	Internal
 *********************************************************************************************/

MaterialDirtyMask Material::dirtyMask() const {
	return _dirtyMask;
}

void Material::dirtyMask(MaterialDirtyMask mask) {
	_dirtyMask = mask;
}
