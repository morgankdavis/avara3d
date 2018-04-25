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
#include "Logger.h"
#include "Program.h"
#include "Utilities.h"


using namespace ae;
using namespace std;


/***************************************************************************************
     Static
 ***************************************************************************************/

static GLenum GLFilterModeForFilterMode(FILTER_MODE mode) {
	switch (mode) {
		case FILTER_MODE::NEAREST: 					return GL_NEAREST;
		case FILTER_MODE::LINEAR: 					return GL_LINEAR;
		case FILTER_MODE::NEAREST_MIPMAP_NEAREST:	return GL_NEAREST_MIPMAP_NEAREST;
		case FILTER_MODE::LINEAR_MIPMAP_NEAREST: 	return GL_LINEAR_MIPMAP_NEAREST;
		case FILTER_MODE::NEAREST_MIPMAP_LINEAR: 	return GL_NEAREST_MIPMAP_LINEAR;
		case FILTER_MODE::LINEAR_MIPMAP_LINEAR: 	return GL_LINEAR_MIPMAP_LINEAR; }
}

static FILTER_MODE FilterModeForGLFilterMode(GLenum mode) {
	switch (mode) {
		case GL_LINEAR: 					return FILTER_MODE::LINEAR;
		case GL_NEAREST_MIPMAP_NEAREST:		return FILTER_MODE::NEAREST_MIPMAP_NEAREST;
		case GL_LINEAR_MIPMAP_NEAREST: 		return FILTER_MODE::LINEAR_MIPMAP_NEAREST;
		case GL_NEAREST_MIPMAP_LINEAR: 		return FILTER_MODE::NEAREST_MIPMAP_LINEAR;
		case GL_LINEAR_MIPMAP_LINEAR: 		return FILTER_MODE::LINEAR_MIPMAP_LINEAR;
		default: /* GL_NEAREST */			return FILTER_MODE::NEAREST; }
}

static GLenum GLWrapModeForWrapMode(WRAP_MODE mode) {
	switch (mode) {
		case WRAP_MODE::CLAMP_TO_EDGE:		return GL_CLAMP_TO_EDGE;
		case WRAP_MODE::CLAMP_TO_BORDER:	return GL_CLAMP_TO_BORDER;
		case WRAP_MODE::REPEAT:				return GL_REPEAT;
		case WRAP_MODE::MIRRORED_REPEAT: 	return GL_MIRRORED_REPEAT; }
}

static WRAP_MODE WrapModeForGLWrapMode(GLenum mode) {
	switch (mode) {
		case GL_CLAMP_TO_BORDER:			return WRAP_MODE::CLAMP_TO_EDGE;
		case GL_REPEAT:						return WRAP_MODE::REPEAT;
		case GL_MIRRORED_REPEAT: 			return WRAP_MODE::MIRRORED_REPEAT;
		default: /* GL_CLAMP_TO_EDGE */		return WRAP_MODE::CLAMP_TO_BORDER; }
}

/***************************************************************************************
     Types
 ***************************************************************************************/

typedef enum {
	MaterialMode_None = 0,
	MaterialMode_Color = 1,
	MaterialMode_Sampler = 2
} MaterialMode;

/***************************************************************************************
     Lifecycle
 ***************************************************************************************/

MaterialProperty::MaterialProperty(std::shared_ptr<Image> image):
	m_image(nullptr),
	m_color(nullptr),
	m_cube(nullptr),
m_wrapS(WRAP_MODE::REPEAT),
	m_wrapT(WRAP_MODE::REPEAT),
	m_minificationFilter(FILTER_MODE::LINEAR_MIPMAP_LINEAR),
	m_magnificationFilter(FILTER_MODE::LINEAR),
//	m_mipFilter(WrapMode_Linear),
	m_maxAnisotropy(16),
	m_glTextureID(0),
	m_dirtyBits(MATERIAL_PROPERTY_DIRTY_BITS::ALL) {

		this->image(image);
}

MaterialProperty::MaterialProperty(std::shared_ptr<Color> color):
	m_image(nullptr),
	m_color(color),
	m_cube(nullptr),
	m_wrapS(WRAP_MODE::REPEAT),
	m_wrapT(WRAP_MODE::REPEAT),
	m_minificationFilter(FILTER_MODE::LINEAR_MIPMAP_LINEAR),
	m_magnificationFilter(FILTER_MODE::LINEAR),
	m_maxAnisotropy(16),
	m_glTextureID(0),
	m_dirtyBits(MATERIAL_PROPERTY_DIRTY_BITS::ALL) {
	
}

MaterialProperty::MaterialProperty(std::shared_ptr<std::vector<std::shared_ptr<Image>>> cube):
	m_image(nullptr),
	m_color(nullptr),
	m_cube(nullptr),
	m_wrapS(WRAP_MODE::REPEAT),
	m_wrapT(WRAP_MODE::REPEAT),
	m_minificationFilter(FILTER_MODE::LINEAR_MIPMAP_LINEAR),
	m_magnificationFilter(FILTER_MODE::LINEAR),
	m_maxAnisotropy(16),
	m_glTextureID(0),
	m_dirtyBits(MATERIAL_PROPERTY_DIRTY_BITS::ALL) {
	
		this->cube(cube);
}

MaterialProperty::~MaterialProperty() {
//	if (m_glTextureID > 0) {
//		AE_LOG->debug("Deleting GL texture {}...", m_glTextureID);
//		glDeleteTextures(1, &m_glTextureID);
//	}
}

/***************************************************************************************
     Public
 ***************************************************************************************/

shared_ptr<Image> MaterialProperty::image() const {
	return m_image;
}

void MaterialProperty::image(const shared_ptr<Image> image) {
	// EXPERIMENTAL
	MATERIAL_PROPERTY_DIRTY_BITS_ADD(m_dirtyBits,
									 MATERIAL_PROPERTY_DIRTY_BITS::CONTENTS);
	
	m_image = image;
	//m_image->load(true);
	loadTexture();
}

shared_ptr<Color> MaterialProperty::color() const {
	return m_color;
}

void MaterialProperty::color(const shared_ptr<Color> color) {
	// EXPERIMENTAL
	MATERIAL_PROPERTY_DIRTY_BITS_ADD(m_dirtyBits,
									 MATERIAL_PROPERTY_DIRTY_BITS::CONTENTS);
	
	m_color = color;
}

shared_ptr<vector<shared_ptr<Image>>> MaterialProperty::cube() const {
	return m_cube;
}

void MaterialProperty::cube(const shared_ptr<vector<shared_ptr<Image>>> cube) {
	// EXPERIMENTAL
	MATERIAL_PROPERTY_DIRTY_BITS_ADD(m_dirtyBits,
									 MATERIAL_PROPERTY_DIRTY_BITS::CONTENTS);
	
	m_cube = cube;
//	for (auto image : *cube) {
//		image->load(false);
//	}
	loadTexture();
}

FILTER_MODE MaterialProperty::minificationFilter() const {
	return m_minificationFilter;
}

void MaterialProperty::minificationFilter(FILTER_MODE mode) {
	// EXPERIMENTAL
	MATERIAL_PROPERTY_DIRTY_BITS_ADD(m_dirtyBits,
									 MATERIAL_PROPERTY_DIRTY_BITS::MINIFICATION_FILTER);
	
	m_minificationFilter = mode;
	
	GLenum texType = (m_cube ? GL_TEXTURE_CUBE_MAP : GL_TEXTURE_2D);
	
	switch (mode) {
		case FILTER_MODE::NEAREST_MIPMAP_NEAREST:
		case FILTER_MODE::NEAREST_MIPMAP_LINEAR:
		case FILTER_MODE::LINEAR_MIPMAP_NEAREST:
		case FILTER_MODE::LINEAR_MIPMAP_LINEAR:
			glGenerateMipmap(texType);
			break;
		default:
			break;
	}
	
	glBindTexture(texType, m_glTextureID);
	glTexParameteri(texType, GL_TEXTURE_MIN_FILTER, GLFilterModeForFilterMode(mode));
}

FILTER_MODE MaterialProperty::magnificationFilter() const {
	return m_magnificationFilter;
}

void MaterialProperty::magnificationFilter(FILTER_MODE mode) {
	// EXPERIMENTAL
	MATERIAL_PROPERTY_DIRTY_BITS_ADD(m_dirtyBits,
									 MATERIAL_PROPERTY_DIRTY_BITS::MAGNIFICATION_FILTER);
	
	m_magnificationFilter = mode;
	
	GLenum texType = (m_cube ? GL_TEXTURE_CUBE_MAP : GL_TEXTURE_2D);
	
	switch (mode) {
		case FILTER_MODE::NEAREST:
		case FILTER_MODE::LINEAR:
			glBindTexture(texType, m_glTextureID);
			glTexParameteri(texType, GL_TEXTURE_MAG_FILTER, GLFilterModeForFilterMode(mode));
			break;
		default:
			//cout << "Error: unsupported magnification filter mode: " << mode << endl;
			//cout << "Error: unsupported magnification filter mode: " << mode << endl;
			AE_LOG->warn("Unsupported magnification filter mode: {}", (unsigned)mode);
			break;
	}
}

float MaterialProperty::maxAnisotropy() const {
	return m_maxAnisotropy;
}

void MaterialProperty::maxAnisotropy(float max) {
	// EXPERIMENTAL
	MATERIAL_PROPERTY_DIRTY_BITS_ADD(m_dirtyBits,
									 MATERIAL_PROPERTY_DIRTY_BITS::MAX_ANISTROPY);
	
	float anisotropy = max;
	
	GLenum texType = (m_cube ? GL_TEXTURE_CUBE_MAP : GL_TEXTURE_2D);

	glBindTexture(texType, m_glTextureID);
	float largest;
	glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &largest);
	if (anisotropy > largest) anisotropy = largest;
	glTexParameterf(texType, GL_TEXTURE_MAX_ANISOTROPY_EXT, anisotropy);
	
	m_maxAnisotropy = anisotropy;
}

WRAP_MODE MaterialProperty::wrapS() const {
	return m_wrapS;
}

void MaterialProperty::wrapS(WRAP_MODE mode) {
	// EXPERIMENTAL
	MATERIAL_PROPERTY_DIRTY_BITS_ADD(m_dirtyBits,
									 MATERIAL_PROPERTY_DIRTY_BITS::WRAP_S);
	
	m_wrapS = mode;
	
	GLenum texType = (m_cube ? GL_TEXTURE_CUBE_MAP : GL_TEXTURE_2D);
	
	glBindTexture(texType, m_glTextureID);
	glTexParameteri(texType, GL_TEXTURE_WRAP_S, GLWrapModeForWrapMode(mode));
}

WRAP_MODE MaterialProperty::wrapT() const {
	return m_wrapT;
}

void MaterialProperty::wrapT(WRAP_MODE mode) {
	// EXPERIMENTAL
	MATERIAL_PROPERTY_DIRTY_BITS_ADD(m_dirtyBits,
									 MATERIAL_PROPERTY_DIRTY_BITS::WRAP_T);
	
	m_wrapT = mode;
	
	GLenum texType = (m_cube ? GL_TEXTURE_CUBE_MAP : GL_TEXTURE_2D);
	
	glBindTexture(texType, m_glTextureID);
	glTexParameteri(texType, GL_TEXTURE_WRAP_T, GLWrapModeForWrapMode(mode));
}

/***************************************************************************************
     Internal
 ***************************************************************************************/

void MaterialProperty::loadTexture() {
	
	if (m_cube) {
		//cout << "Loading cube texture..." << endl;
		AE_LOG->info("Buffering cube texture...");
		
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
						 GL_RGBA,//GL_SRGB_ALPHA,//GL_RGBA,
						 image.width(),
						 image.height(),
						 0,
						 GL_RGBA,
						 GL_UNSIGNED_BYTE,
						 image.data());
		}
		
		minificationFilter(FILTER_MODE::LINEAR_MIPMAP_LINEAR);
		magnificationFilter(FILTER_MODE::LINEAR);
		maxAnisotropy(16);
		wrapS(WRAP_MODE::CLAMP_TO_EDGE);
		wrapT(WRAP_MODE::CLAMP_TO_EDGE);
		
		AE_LOG->info("Done.");
	}
	else if (m_image) {
		//cout << "Loading 2D texture..." << endl;
		AE_LOG->info("Buffering 2D texture...");
		
		glGenTextures(1, &m_glTextureID);
		glBindTexture(GL_TEXTURE_2D, m_glTextureID);
		
		glTexImage2D(GL_TEXTURE_2D,
					 0,
					 GL_RGBA,//GL_SRGB_ALPHA,//GL_RGBA,
					 m_image->width(),
					 m_image->height(),
					 0,
					 GL_RGBA,
					 GL_UNSIGNED_BYTE,
					 m_image->data());

		minificationFilter(m_minificationFilter);
		magnificationFilter(m_magnificationFilter);
		maxAnisotropy(m_maxAnisotropy);
		wrapS(m_wrapS);
		wrapT(m_wrapT);
		
		AE_LOG->info("Done.");
	}
}

void MaterialProperty::bind(MATERIAL_PROPERTY_TYPE type, Program& program) {
	
	if (m_cube) { // currently only used for skybox
		program.bindTexture("cubeSampler", GL_TEXTURE_CUBE_MAP, GL_TEXTURE0, m_glTextureID, 0);
	}
	else if (m_image) { // texture
		string modeUniformName = "";
		string samplerUniformName = "";
		GLenum slot;
		GLint index;
		
		switch (type) {
			case MATERIAL_PROPERTY_TYPE::AMBIENT:
				modeUniformName = "ambientMode";
				samplerUniformName = "samplers.ambient";
				slot = GL_TEXTURE0; index = 0;
				break;
			case MATERIAL_PROPERTY_TYPE::DIFFUSE:
				modeUniformName = "diffuseMode";
				samplerUniformName = "samplers.diffuse";
				slot = GL_TEXTURE1; index = 1;
				break;
			case MATERIAL_PROPERTY_TYPE::SPECULAR:
				modeUniformName = "specularMode";
				samplerUniformName = "samplers.specular";
				slot = GL_TEXTURE2; index = 2;
				break;
			case MATERIAL_PROPERTY_TYPE::EMISSIVE:
				modeUniformName = "emissiveMode";
				samplerUniformName = "samplers.emissive";
				slot = GL_TEXTURE3; index = 3;
				break;
			default:
				cout << "Invalid MaterialPropertyType: " << static_cast<int>(type) << endl;
				return;
		}

		program.setUniform(modeUniformName.c_str(), MaterialMode_Sampler);
		program.bindTexture(samplerUniformName.c_str(), GL_TEXTURE_2D, slot, m_glTextureID, index);
	}
	else { // color
		string modeUniformName = "";
		string colorUniformName = "";
		
		switch (type) {
			case MATERIAL_PROPERTY_TYPE::AMBIENT:
				modeUniformName = "ambientMode";
				colorUniformName = "colors.ambient";
				break;
			case MATERIAL_PROPERTY_TYPE::DIFFUSE:
				modeUniformName = "diffuseMode";
				colorUniformName = "colors.diffuse";
				break;
			case MATERIAL_PROPERTY_TYPE::SPECULAR:
				modeUniformName = "specularMode";
				colorUniformName = "colors.specular";
				break;
			case MATERIAL_PROPERTY_TYPE::EMISSIVE:
				modeUniformName = "emissiveMode";
				colorUniformName = "colors.emissive";
				break;
			default:
				cout << "Invalid MaterialPropertyType: " << static_cast<int>(type) << endl;
				return;
		}
		
		program.setUniform(modeUniformName.c_str(), MaterialMode_Color);
		program.setUniform(colorUniformName.c_str(), m_color->r, m_color->g, m_color->b);
	}
}

// EXPERIMENTAL

MATERIAL_PROPERTY_DIRTY_BITS MaterialProperty::dirtyBits() const {
	return m_dirtyBits;
}

void MaterialProperty::dirtyBits(MATERIAL_PROPERTY_DIRTY_BITS bits) {
	m_dirtyBits = bits;
}
