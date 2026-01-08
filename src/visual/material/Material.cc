//
//  Material.cc
//  avara3d
//
//  Created by Morgan Davis on 10/21/16.
//  Copyright © 2024 Morgan K Davis. All rights reserved.
//

#include "a3d/visual/material/Material.h"

#include "a3d/Color.h"
#include "a3d/IdGenerator.h"
#include "a3d/log/Log.h"

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

shared_ptr<Material> Material::EmissionMaterial(Property property) {
	return make_shared<Material>(monostate{}, monostate{}, monostate{}, property);
}

/// Public Lifecycle Functions ///

Material::Material():
		_id{IdGenerator<MaterialId>::next()},
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
		_alphaMode{AlphaMode::Opaque},
		_alphaCutoff{0.5f},
		_blendFunction{BlendFunction::Disabled},
		_dirtyMask{DirtyMask::All} {

	log::d()("Creating Material {:p}", static_cast<void*>(this));
}

Material::Material(const Property& ambient,
				   const Property& diffuse,
				   const Property& specular):
		Material() {

	_ambient = ambient;
	_diffuse = diffuse;
	_specular = specular;
}

Material::Material(const Property& ambient,
				   const Property& diffuse,
				   const Property& specular,
				   const Property& emission):
		Material() {

	_ambient = ambient;
	_diffuse = diffuse;
	_specular = specular;
	_emission = emission;
}

Material::~Material() {
	log::d()("Destroying Material {:p}", static_cast<void*>(this));
}

/// Public Member Functions ///

const optional<string>& Material::name() const {
	return _name;
}

void Material::name(const string& name) {
	_name = name;
}

const Material::Property& Material::ambient() const {
	return _ambient;
}

void Material::ambient(const Property& property) {
	_ambient = property;
}

const Material::Property& Material::diffuse() const {
	return _diffuse;
}

void Material::diffuse(const Property& property) {
	_diffuse = property;
}

const Material::Property& Material::specular() const {
	return _specular;
}

void Material::specular(const Property& property) {
	_specular = property;
}

const Material::Property& Material::emission() const {
	return _emission;
}

void Material::emission(const Property& property) {
	_emission = property;
}

Material::PropertyList Material::properties() const {
	return PropertyList {
			{ &_ambient, PropertyType::Ambient },
			{ &_diffuse, PropertyType::Diffuse },
			{ &_specular, PropertyType::Specular },
			{ &_emission, PropertyType::Emission }
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

Material::FillMode Material::fillMode() const {
#ifdef A3D_GL_ES
	return FILL_MODE::FILL;
#else
	return _fillMode;
#endif
}

void Material::fillMode(FillMode mode) {
#ifdef A3D_GL_ES
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

Material::AlphaMode Material::alphaMode() const {
	return _alphaMode;
}

void Material::alphaMode(AlphaMode mode) {
	_alphaMode = mode;
}

float Material::alphaCutoff() const {
	return _alphaCutoff;
}

void Material::alphaCutoff(float cutoff) {
	_alphaCutoff = cutoff;
}

Material::BlendFunction Material::blendFunction() const {
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

Material::Property Material::MissingTextureProperty() {
	return {Color::Magenta()};
}

/// Internal Member Functions ///

MaterialId Material::id() const noexcept {
	return _id;
}

Material::DirtyMask Material::dirtyMask() const {
	return _dirtyMask;
}

void Material::dirtyMask(DirtyMask mask) {
	_dirtyMask = mask;
}
