//
//  Material.cc
//	avara-engine
//
//  Created by Morgan Davis on 10/21/16.
//  Copyright © 2016 Morgan K Davis. All rights reserved.
//

#include "Material.h"

#include <iostream>
#include <memory>

#include "Color.h"
#include "Exception.h"
#include "Image.h"
#include "Logger.h"
#include "MaterialProperty.h"
#include "Utilities.h"


using namespace ae;
using namespace glm;
using namespace std;


/*********************************************************************************************
	Public Static
 *********************************************************************************************/

shared_ptr<Material> Material::DefaultMaterial() {
	static shared_ptr<Material> material = nullptr;
	if (!material) {
		auto ambientProperty = make_shared<MaterialProperty>(make_shared<Color>(0.75, 0.75, 0.75, 1.0));
		auto diffuseProperty = make_shared<MaterialProperty>(make_shared<Color>(1.0, 1.0, 1.0, 1.0));
		material = make_shared<Material>(ambientProperty, diffuseProperty, nullptr);
	}
	return material;
}

/*********************************************************************************************
	Lifecycle
 *********************************************************************************************/

Material::Material():
	m_name(boost::none),
	m_ambient(nullptr),
	m_diffuse(nullptr),
	m_specular(nullptr),
	m_emissive(nullptr),
	m_specularExponent(150.0),
	m_locksAmbientWithDiffuse(true),
	m_doubleSided(false),
	m_fillMode(FILL_MODE::FILL),
	m_uvScale(1.0f),
	m_dirtyBits(MATERIAL_DIRTY_BITS::ALL) {
	
		AE_LOG_D("Creating Material {:p}", (void*)this);
}

Material::Material(shared_ptr<MaterialProperty> ambient,
				   shared_ptr<MaterialProperty> diffuse,
				   shared_ptr<MaterialProperty> specular):
	Material() {

		m_ambient = ambient;
		m_diffuse = diffuse;
		m_specular = specular;
}

Material::Material(shared_ptr<MaterialProperty> ambient,
				   shared_ptr<MaterialProperty> diffuse,
				   shared_ptr<MaterialProperty> specular,
				   shared_ptr<MaterialProperty> emissive):
	Material() {
	
		m_ambient = ambient;
		m_diffuse = diffuse;
		m_specular = specular;
		m_emissive = emissive;
}

Material::~Material() {
	AE_LOG_D("Destroying Material {:p}", (void*)this);
}

/*********************************************************************************************
	Public
 *********************************************************************************************/

boost::optional<std::string> Material::name() const {
	return m_name;
}

void Material::name(const string& name) {
	m_name = name;
}

shared_ptr<MaterialProperty> Material::ambient() const {
	return m_ambient;
}

void Material::ambient(const shared_ptr<MaterialProperty> property) {
	m_ambient = property;
}

shared_ptr<MaterialProperty> Material::diffuse() const {
	return m_diffuse;
}

void Material::diffuse(const shared_ptr<MaterialProperty> property) {
	m_diffuse = property;
}

shared_ptr<MaterialProperty> Material::specular() const {
	return m_specular;
}

void Material::specular(const shared_ptr<MaterialProperty> property) {
	m_specular = property;
}

shared_ptr<MaterialProperty> Material::emissive() const {
	return m_emissive;
}

void Material::emissive(const shared_ptr<MaterialProperty> property) {
	m_emissive = property;
}

float Material::specularExponent() const {
	return m_specularExponent;
}

void Material::specularExponent(float exponent) {
	m_specularExponent = exponent;
}

bool Material::locksAmbientWithDiffuse() const {
	return m_locksAmbientWithDiffuse;
}

void Material::locksAmbientWithDiffuse(bool flag) {
	m_locksAmbientWithDiffuse = flag;
}

bool Material::doubleSided() const {
	return m_doubleSided;
}

void Material::doubleSided(bool flag) {
	m_doubleSided = flag;
}

FILL_MODE Material::fillMode() const {
#ifdef ANDROID
	return FILL_MODE::FILL;
#else
	return m_fillMode;
#endif
}

void Material::fillMode(FILL_MODE mode) {
#ifdef ANDROID
	if (mode == FILL_MODE::LINES || mode == FILL_MODE::POINTS) {
		throw Exception("Fill mode not supported on this platform.");
	}
#endif
	
	m_fillMode = mode;
}

float Material::uvScale() const {
	return m_uvScale;
}

void Material::uvScale(float scale) {
	m_uvScale = scale;
}

/*********************************************************************************************
	Internal
 *********************************************************************************************/

MATERIAL_DIRTY_BITS Material::dirtyBits() const {
	return m_dirtyBits;
}

void Material::dirtyBits(MATERIAL_DIRTY_BITS bits) {
	m_dirtyBits = bits;
}
