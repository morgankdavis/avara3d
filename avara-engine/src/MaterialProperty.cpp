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
#include "Program.h"


using namespace ae;
using namespace std;


/***************************************************************************************
     MARK:   Lifecycle
 **************************************************************************************/

MaterialProperty::MaterialProperty(const std::shared_ptr<Image> image):
	m_image(nullptr),
	m_color(nullptr),
	m_cube(nullptr),
	m_wrapS(WrapMode_Clamp),
	m_wrapT(WrapMode_Clamp),
	m_minificationFilter(WrapMode_Linear),
	m_magnificationFilter(WrapMode_Linear),
	m_mipFilter(WrapMode_Linear),
	m_maxAnisotropy(0),
	m_glTextureID(-1) {

		this->image(image);
}

MaterialProperty::MaterialProperty(const std::shared_ptr<Color> color):
	m_image(nullptr),
	m_color(color),
	m_cube(nullptr),
	m_wrapS(WrapMode_Clamp),
	m_wrapT(WrapMode_Clamp),
	m_minificationFilter(WrapMode_Linear),
	m_magnificationFilter(WrapMode_Linear),
	m_mipFilter(WrapMode_Linear),
	m_maxAnisotropy(0),
	m_glTextureID(-1) {
	
}

MaterialProperty::MaterialProperty(const std::shared_ptr<std::vector<std::shared_ptr<Image>>> cube):
	m_image(nullptr),
	m_color(nullptr),
	m_cube(nullptr),
	m_wrapS(WrapMode_Clamp),
	m_wrapT(WrapMode_Clamp),
	m_minificationFilter(WrapMode_Linear),
	m_magnificationFilter(WrapMode_Linear),
	m_mipFilter(WrapMode_Linear),
	m_maxAnisotropy(0),
	m_glTextureID(-1) {
	
		this->cube(cube);
}

/***************************************************************************************
     MARK:   Public
 **************************************************************************************/

shared_ptr<Image> MaterialProperty::image() const {
	return m_image;
}

void MaterialProperty::image(const shared_ptr<Image> image) {
	m_image = image;
	m_image->load(true);
	loadTexture();
}

shared_ptr<Color> MaterialProperty::color() const {
	return m_color;
}

void MaterialProperty::color(const shared_ptr<Color> color) {
	m_color = color;
}

shared_ptr<vector<shared_ptr<Image>>> MaterialProperty::cube() const {
	return m_cube;
}

void MaterialProperty::cube(const shared_ptr<vector<shared_ptr<Image>>> cube) {
	m_cube = cube;
	for (auto image : *cube) {
		image->load(false);
	}
	loadTexture();
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

FilterMode MaterialProperty::magnificationFilter() const {
	return m_magnificationFilter;
}

void MaterialProperty::magnificationFilter(const FilterMode mode) {
	m_magnificationFilter = mode;
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

void MaterialProperty::loadTexture() {
	
	if (m_cube) {
		cout << "Loading cube texture..." << endl;
		
		GLenum sides[] = {
			GL_TEXTURE_CUBE_MAP_POSITIVE_X,
			GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
			GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
			GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
			GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
			GL_TEXTURE_CUBE_MAP_NEGATIVE_Z };
		
		glGenTextures(1, &m_glTextureID);
		glBindTexture(GL_TEXTURE_CUBE_MAP, m_glTextureID);
		
		for (int s=0; s<6; ++s) {
			GLenum side = sides[s];
			Image image = *(*m_cube)[s];
			
			glTexImage2D(side,
						 0,
						 GL_RGBA,
						 image.width(),
						 image.height(),
						 0,
						 GL_RGBA,
						 GL_UNSIGNED_BYTE,
						 image.data());
		}
		
		// format cube map texture
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	}
	else if (m_image) {
		cout << "Loading 2D texture..." << endl;
		
		glGenTextures(1, &m_glTextureID);
		glBindTexture(GL_TEXTURE_2D, m_glTextureID);
		
		glTexImage2D(GL_TEXTURE_2D,
					 0,
					 GL_RGBA,
					 m_image->width(),
					 m_image->height(),
					 0,
					 GL_RGBA,
					 GL_UNSIGNED_BYTE,
					 m_image->data());
		
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // GL_CLAMP_TO_EDGE
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT); // GL_CLAMP_TO_EDGE
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	}
}

void MaterialProperty::bind(MaterialPropertyType type, Program& program) {
	
	if (m_cube) { // currently only used for skybox
		program.bindTexture("cubeSampler", GL_TEXTURE0, m_glTextureID, 0);
	}
	else if (m_image) { // texture
		string useUniformName = "";
		string samplerUniformName = "";
		GLenum slot;
		GLint index;
		
		switch (type) {
			case MaterialPropertyType_Ambient:
				useUniformName = "useAmbientSampler";
				samplerUniformName = "samplers.ambient";
				slot = GL_TEXTURE0; index = 0;
				break;
			case MaterialPropertyType_Diffuse:
				useUniformName = "useDiffuseSampler";
				samplerUniformName = "samplers.diffuse";
				slot = GL_TEXTURE1; index = 1;
				break;
			case MaterialPropertyType_Specular:
				useUniformName = "useSpecularSampler";
				samplerUniformName = "samplers.specular";
				slot = GL_TEXTURE2; index = 2;
				break;
				
			default:
				cout << "Invalid MaterialPropertyType: " << type << endl;
				return;
		}

		program.setUniform(useUniformName.c_str(), true);
		program.bindTexture(samplerUniformName.c_str(), slot, m_glTextureID, index);
	}
	else { // color
		
		string useUniformName = "";
		string colorUniformName = "";

		switch (type) {
			case MaterialPropertyType_Ambient:
				useUniformName = "useAmbientSampler";
				colorUniformName = "colors.ambient";
				break;
			case MaterialPropertyType_Diffuse:
				useUniformName = "useDiffuseSampler";
				colorUniformName = "colors.diffuse";
				break;
			case MaterialPropertyType_Specular:
				useUniformName = "useSpecularSampler";
				colorUniformName = "colors.specular";
				break;
			default:
				cout << "Invalid MaterialPropertyType: " << type << endl;
				return;
		}

		program.setUniform(useUniformName.c_str(), false);
		program.setUniform(colorUniformName.c_str(), m_color->r, m_color->g, m_color->b);
	}
}

GLuint MaterialProperty::glTextureID() {
//	if (m_glTextureID == -1) {
//		loadTexture();
//	}
	return m_glTextureID;
}

