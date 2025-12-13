//
//  Material.cc
//  avara3d
//
//  Created by Morgan Davis on 10/21/16.
//  Copyright © 2024 Morgan K Davis. All rights reserved.
//

#include "a3d/rendering/material/Material.h"

#include "a3d/Color.h"
#include "a3d/diagnostic/exception/Exception.h"
#include "a3d/diagnostic/log/Log.h"

using namespace a3d;
using namespace a3d::math;
using namespace std;

/// Public Static Member Functions ///

shared_ptr<Material> Material::DefaultMaterial() {
	static shared_ptr<Material> material = nullptr;
	if (!material) {
		auto ambient = make_shared<Color>(0.75f);
		auto diffuse = make_shared<Color>(0.75f);
		auto specular = make_shared<Color>(0.85f);
		material = make_shared<Material>(ambient, diffuse, specular);
		material->name("Default material");
		material->doubleSided(true);
		material->specularExponent(75);
	}
	return material;
}

shared_ptr<Material> Material::EmissionMaterial(MaterialProperty property) {
	return make_shared<Material>(monostate{}, monostate{}, monostate{}, property);
}

/// Public Lifecycle Functions ///

Material::Material():
		_name{},
		_ambient{},
		_diffuse{},
		_specular{},
		_emission{},
		_specularExponent{75},
		_locksAmbientWithDiffuse{true},
		_doubleSided{false},
		_fillMode{FillMode::Fill},
		_uvScale{1.0f},
		_blendFunction{BlendFunction::Disabled},
		_dirtyMask{MaterialDirtyMask::All} {

	A3D_LOG_D("Creating Material {:p}", static_cast<void*>(this));
}

Material::Material(const MaterialProperty& ambient,
				   const MaterialProperty& diffuse,
				   const MaterialProperty& specular):
		Material() {

	_ambient = ambient;
	_diffuse = diffuse;
	_specular = specular;
}

Material::Material(const MaterialProperty& ambient,
				   const MaterialProperty& diffuse,
				   const MaterialProperty& specular,
				   const MaterialProperty& emission):
		Material() {

	_ambient = ambient;
	_diffuse = diffuse;
	_specular = specular;
	_emission = emission;
}

Material::~Material() {
	A3D_LOG_D("Destroying Material {:p}", static_cast<void*>(this));
}

/// Public Member Functions ///

const optional<string>& Material::name() const {
	return _name;
}

void Material::name(const string& name) {
	_name = name;
}

const MaterialProperty& Material::ambient() const {
	return _ambient;
}

void Material::ambient(const MaterialProperty& property) {
	_ambient = property;
}

const MaterialProperty& Material::diffuse() const {
	return _diffuse;
}

void Material::diffuse(const MaterialProperty& property) {
	_diffuse = property;
}

const MaterialProperty& Material::specular() const {
	return _specular;
}

void Material::specular(const MaterialProperty& property) {
	_specular = property;
}

const MaterialProperty& Material::emission() const {
	return _emission;
}

void Material::emission(const MaterialProperty& property) {
	_emission = property;
}

MaterialPropertyList Material::properties() const {
	return MaterialPropertyList {
			{ &_ambient, MaterialPropertyType::Ambient },
			{ &_diffuse, MaterialPropertyType::Diffuse },
			{ &_specular, MaterialPropertyType::Specular },
			{ &_emission, MaterialPropertyType::Emission }
	};
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
#ifdef OPENGL_ES
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

/// Internal Static Member Functions ///

shared_ptr<Material> Material::MissingTextureMaterial() {
	static shared_ptr<Material> material = nullptr;
	if (!material) {
		material = Material::EmissionMaterial(Color::Magenta());
		material->name("Missing texture material");
		material->doubleSided(true);
	}
	return material;
}

MaterialProperty Material::MissingTextureProperty() {
	return {Color::Magenta()};
}

/// Internal Member Functions ///

MaterialDirtyMask Material::dirtyMask() const {
	return _dirtyMask;
}

void Material::dirtyMask(MaterialDirtyMask mask) {
	_dirtyMask = mask;
}
