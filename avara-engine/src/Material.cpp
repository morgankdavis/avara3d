//
//  Material.cpp
//	avara-engine
//
//  Created by Morgan Davis on 10/21/16.
//  Copyright © 2016 Morgan K Davis. All rights reserved.
//

#include "Material.h"

#include <iostream>
#include <memory>

#include <GL/glew.h>

#include "Color.h"
#include "Image.h"
#include "Logger.h"
#include "MaterialProperty.h"
#include "Program.h"
#include "Utilities.h"


using namespace ae;
using namespace glm;
using namespace std;


/***************************************************************************************
     Static
 ***************************************************************************************/

shared_ptr<Material> Material::DefaultMaterial() {
	static shared_ptr<Material> material = nullptr;
	if (!material) {
		auto ambientProperty = make_shared<MaterialProperty>(make_shared<Color>(0.75, 0.75, 0.75, 1.0));
		auto diffuseProperty = make_shared<MaterialProperty>(make_shared<Color>(1.0, 1.0, 1.0, 1.0));
		material = make_shared<Material>(ambientProperty, diffuseProperty, nullptr);
	}
	return material;
}

/***************************************************************************************
     Lifecycle
 ***************************************************************************************/

Material::Material():
	Material(nullptr, nullptr, nullptr, Program::Default()) {
	
}

Material::Material(shared_ptr<Program> program):
	Material(nullptr, nullptr, nullptr, program) {
	
}

Material::Material(shared_ptr<MaterialProperty> ambient,
				   shared_ptr<MaterialProperty> diffuse,
				   shared_ptr<MaterialProperty> specular):
	Material(ambient, diffuse, specular, Program::Default()) {

}

Material::Material(shared_ptr<MaterialProperty> ambient,
				   shared_ptr<MaterialProperty> diffuse,
				   shared_ptr<MaterialProperty> specular,
				   shared_ptr<Program> program):
	m_name(boost::none),
	m_ambient(ambient),
	m_diffuse(diffuse),
	m_specular(specular),
	m_emissive(nullptr),
	m_specularExponent(150.0),
	m_locksAmbientWithDiffuse(true),
	m_doubleSided(false),
	m_fillMode(FILL_MODE::FILL),
	m_uvScale(1.0f),
	m_program(program),
	m_dirtyBits(MATERIAL_DIRTY_BITS::ALL) {
	
		//loadShaderProgram(programName);
}

Material::Material(std::shared_ptr<MaterialProperty> emissive):
	Material() {
	
		m_emissive = emissive;
}

/***************************************************************************************
     Public
 ***************************************************************************************/

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
	return m_fillMode;
}

void Material::fillMode(FILL_MODE mode) {
	m_fillMode = mode;
}

float Material::uvScale() const {
	return m_uvScale;
}

void Material::uvScale(float scale) {
	m_uvScale = scale;
}

/***************************************************************************************
     Internal
 ***************************************************************************************/

shared_ptr<Program> Material::program() const {
	return m_program;
}

void Material::program(shared_ptr<Program> program) {
	m_program = program;
	
	//loadShaderProgram(program);
}

//void Material::loadShaderProgram(const string& shaderName) {
//
//	m_program = make_shared<Program>(shaderName);
//
//	if (m_program->compile()) {
//		//cout << "Shader program '" << shaderName << "' compiled." << endl;
//		AE_LOG->info("Program '{}' compiled.", shaderName);
//
//		if (m_program->link()) {
//			//cout << "Shader program '" << shaderName << "' linked." << endl;
//			AE_LOG->info("Program '{}' linked.", shaderName);
//		}
////		else {
////			cout << "Couldn't link '" << shaderName << "' shader:\n" << *(m_program->logString()) << endl;
////		}
//	}
////	else {
////		cout << "Couldn't compile '" << shaderName << "' shader:\n" << *(m_program->logString()) << endl;
////	}
//}

shared_ptr<Program> Material::selectProgram(DEBUG_OPTIONS debugOptions) {
	//if ((unsigned)debugOptions & (unsigned)DEBUG_OPTIONS::SHOW_WIREFRAMES) {
	if (DEBUG_OPTIONS_CONTAINS(debugOptions, DEBUG_OPTIONS::SHOW_WIREFRAMES)) {
		m_program = Program::Wireframe();
		glEnable(GL_LINE_SMOOTH);
	}
	else {
		m_program = Program::Default();
		//glDisable(GL_LINE_SMOOTH);
	}
	return m_program;
}

void Material::prepareToRender(DEBUG_OPTIONS debugOptions) {
	AE_LOG->trace("prepareToRender()");
	
	//if ((unsigned)debugOptions | (unsigned)DEBUG_OPTIONS::SHOW_PHYSICS_WIREFRAMES) {
	if (DEBUG_OPTIONS_CONTAINS(debugOptions, DEBUG_OPTIONS::SHOW_PHYSICS_WIREFRAMES)) {
		// can create zbuffer problems
		// https://www.opengl.org/archives/resources/faq/technical/polygonoffset.htm
		//glDepthRange(0.1, 1.0);
//		glEnable(GL_POLYGON_OFFSET_FILL);
//		glPolygonOffset(20.0, 0.0);
	}

	//if ((unsigned)debugOptions & (unsigned)DEBUG_OPTIONS::SHOW_WIREFRAMES) {
	if (DEBUG_OPTIONS_CONTAINS(debugOptions, DEBUG_OPTIONS::SHOW_WIREFRAMES)) {
		//m_program = Program::Wireframe();
		
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
	else {
		//m_program = Program::Default();
		
		//if ((debugOptions & DebugOption_ShowWireframes) || (m_fillMode == FillMode_Lines)) {
		if (m_fillMode == FILL_MODE::LINES) {
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		}
		else if (m_fillMode == FILL_MODE::POINTS) {
			glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
		}
		else {
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}
		
		if (m_doubleSided) {
			glDisable(GL_CULL_FACE);
		}
		else {
			glEnable(GL_CULL_FACE);
			glCullFace(GL_BACK);
		}
		
		m_program->setUniform("specularExponent", m_specularExponent);
		
		m_program->setUniform("uvScale", m_uvScale);
//		m_program->setUniform("specularExponent", m_uvScale);
		
		// this is a bit of a hack, but since we're sharing programs now this needs to be reset...
		//MaterialPropertyType_Emissive
		m_program->setUniform("emissiveMode", 0); // 0 = MaterialMode_None
		
		// only lock for diffuse textures, not colors
		if (m_locksAmbientWithDiffuse && m_diffuse && (m_diffuse->color() || m_diffuse->image())) {
			m_diffuse->bind(MATERIAL_PROPERTY_TYPE::AMBIENT, *m_program);
		}
		else {
			if (m_ambient) {
				m_ambient->bind(MATERIAL_PROPERTY_TYPE::AMBIENT, *m_program);
			}
		}
		if (m_diffuse) {
			m_diffuse->bind(MATERIAL_PROPERTY_TYPE::DIFFUSE, *m_program);
		}
		if (m_specular) {
			m_specular->bind(MATERIAL_PROPERTY_TYPE::SPECULAR, *m_program);
		}
		if (m_emissive) {
			m_emissive->bind(MATERIAL_PROPERTY_TYPE::EMISSIVE, *m_program);
		}
	}
}

// EXPERIMENTAL

MATERIAL_DIRTY_BITS Material::dirtyBits() const {
	return m_dirtyBits;
}

void Material::dirtyBits(MATERIAL_DIRTY_BITS bits) {
	m_dirtyBits = bits;
}
