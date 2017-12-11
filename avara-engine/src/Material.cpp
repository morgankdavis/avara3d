//
//  Material.cpp
//	avara-engine
//
//  Created by Morgan Davis on 10/21/16.
//  Copyright © 2016 Morgan K Davis. All rights reserved.
//

#include "Material.h"

#include <iostream>

#include "Color.h"
#include "Image.h"
#include "MaterialProperty.h"


using namespace ae;
using namespace std;


/***************************************************************************************
     MARK:   Lifecycle
 **************************************************************************************/

Material::Material():
	//	m_name(name),
	m_ambient(nullptr),
	m_diffuse(nullptr),
	m_specular(nullptr),
	m_specularExponent(150.0),
	m_litPerPixel(true),
	m_locksAmbientWithDiffuse(true),
	m_doubleSided(false),
	m_readFromDepthBuffer(true),
	m_fillMode(MaterialFillMode_Fill) {
	
}

Material::Material(const string imagePath):
//	m_name(name),
//	m_ambient(nullptr),
//	m_diffuse(nullptr),
//	m_specular(nullptr),
	m_specularExponent(150.0),
	m_litPerPixel(true),
	m_locksAmbientWithDiffuse(true),
	m_doubleSided(false),
	m_readFromDepthBuffer(true),
	m_fillMode(MaterialFillMode_Fill) {
		
		// makes a "best guess" for a material based on a referenced image file
		// (locks ambient and diffuse and sets a medium specular color)
	
		cout << "Making material with image: " << imagePath << endl;
		
		auto textureImage = make_shared<Image>(imagePath);
		auto ambientDiffuseProperty = make_shared<MaterialProperty>(textureImage);
		auto specularProperty = make_shared<MaterialProperty>(make_shared<Color>(Color::Gray()));

		m_ambient = ambientDiffuseProperty;
		m_diffuse = ambientDiffuseProperty;
		m_specular = specularProperty;
}

Material::Material(shared_ptr<MaterialProperty> ambient,
				   shared_ptr<MaterialProperty> diffuse,
				   shared_ptr<MaterialProperty> specular):
//	m_name(name),
	m_ambient(ambient),
	m_diffuse(diffuse),
	m_specular(specular),
	m_specularExponent(150.0),
	m_litPerPixel(true),
	m_locksAmbientWithDiffuse(true),
	m_doubleSided(false),
	m_readFromDepthBuffer(true),
	m_fillMode(MaterialFillMode_Fill) {
	
}

Material::~Material() {
	cout << "[Material deallocating]" << endl;
}

/***************************************************************************************
     MARK:   Public
 **************************************************************************************/

//string Material::name() const {
//	return m_name;
//}
//
//void Material::name(const string name) {
//	m_name = name;
//}

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

float Material::specularExponent() const {
	return m_specularExponent;
}

void Material::specularExponent(const float exponent) {
	m_specularExponent = exponent;
}

bool Material::litPerPixel() const {
	return m_litPerPixel;
}

void Material::litPerPixel(const bool flag) {
	m_litPerPixel = flag;
}

bool Material::locksAmbientWithDiffuse() const {
	return m_locksAmbientWithDiffuse;
}

void Material::locksAmbientWithDiffuse(const bool flag) {
	m_locksAmbientWithDiffuse = flag;
}

bool Material::doubleSided() const {
	return m_doubleSided;
}

void Material::doubleSided(const bool flag) {
	m_doubleSided = flag;
}

MaterialFillMode Material::fillMode() const {
	return m_fillMode;
}

void Material::fillMode(const MaterialFillMode mode) {
	m_fillMode = mode;
}
