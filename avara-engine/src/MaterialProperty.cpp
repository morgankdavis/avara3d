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
     MARK:   Static
 **************************************************************************************/

static GLenum GLFilterModeForFilterMode(FilterMode mode) {
	switch (mode) {
		case FilterMode_Nearest: 				return GL_NEAREST;
		case FilterMode_Linear: 				return GL_LINEAR;
		case FilterMode_NearestMipmapNearest:	return GL_NEAREST_MIPMAP_NEAREST;
		case FilterMode_LinearMipmapNearest: 	return GL_LINEAR_MIPMAP_NEAREST;
		case FilterMode_NearestMipmapLinear: 	return GL_NEAREST_MIPMAP_LINEAR;
		case FilterMode_LinearMipmapLinear: 	return GL_LINEAR_MIPMAP_LINEAR; }
}

static FilterMode FilterModeForGLFilterMode(GLenum mode) {
	switch (mode) {
		case GL_LINEAR: 					return FilterMode_Linear;
		case GL_NEAREST_MIPMAP_NEAREST:		return FilterMode_NearestMipmapNearest;
		case GL_LINEAR_MIPMAP_NEAREST: 		return FilterMode_LinearMipmapNearest;
		case GL_NEAREST_MIPMAP_LINEAR: 		return FilterMode_NearestMipmapLinear;
		case GL_LINEAR_MIPMAP_LINEAR: 		return FilterMode_LinearMipmapLinear;
		default: /* GL_NEAREST */			return FilterMode_Nearest; }
}

static GLenum GLWrapModeForWrapMode(WrapMode mode) {
	switch (mode) {
		case WrapMode_ClampToEdge:		return GL_CLAMP_TO_EDGE;
		case WrapMode_ClampToBorder:	return GL_CLAMP_TO_BORDER;
		case WrapMode_Repeat:			return GL_REPEAT;
		case WrapMode_MirroredRepeat: 	return GL_MIRRORED_REPEAT; }
}

static WrapMode WrapModeForGLWrapMode(GLenum mode) {
	switch (mode) {
		case GL_CLAMP_TO_BORDER:			return WrapMode_ClampToEdge;
		case GL_REPEAT:						return WrapMode_Repeat;
		case GL_MIRRORED_REPEAT: 			return WrapMode_MirroredRepeat;
		default: /* GL_CLAMP_TO_EDGE */		return WrapMode_ClampToBorder; }
}

/***************************************************************************************
     MARK:   Types
 **************************************************************************************/

typedef enum {
	MaterialMode_None = 0,
	MaterialMode_Color = 1,
	MaterialMode_Sampler = 2
} MaterialMode;

/***************************************************************************************
     MARK:   Lifecycle
 **************************************************************************************/

MaterialProperty::MaterialProperty(std::shared_ptr<Image> image):
	m_image(nullptr),
	m_color(nullptr),
	m_cube(nullptr),
	m_wrapS(WrapMode_ClampToEdge),
	m_wrapT(WrapMode_ClampToEdge),
	m_minificationFilter(FilterMode_LinearMipmapLinear),
	m_magnificationFilter(FilterMode_Linear),
//	m_mipFilter(WrapMode_Linear),
	m_maxAnisotropy(16),
	m_glTextureID(0) {

		this->image(image);
}

MaterialProperty::MaterialProperty(std::shared_ptr<Color> color):
	m_image(nullptr),
	m_color(color),
	m_cube(nullptr),
	m_wrapS(WrapMode_ClampToEdge),
	m_wrapT(WrapMode_ClampToEdge),
	m_minificationFilter(FilterMode_LinearMipmapLinear),
	m_magnificationFilter(FilterMode_Linear),
	m_maxAnisotropy(16),
	m_glTextureID(0) {
	
}

MaterialProperty::MaterialProperty(std::shared_ptr<std::vector<std::shared_ptr<Image>>> cube):
	m_image(nullptr),
	m_color(nullptr),
	m_cube(nullptr),
	m_wrapS(WrapMode_ClampToEdge),
	m_wrapT(WrapMode_ClampToEdge),
	m_minificationFilter(FilterMode_LinearMipmapLinear),
	m_magnificationFilter(FilterMode_Linear),
	m_maxAnisotropy(16),
	m_glTextureID(0) {
	
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
	//m_image->load(true);
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
//	for (auto image : *cube) {
//		image->load(false);
//	}
	loadTexture();
}

FilterMode MaterialProperty::minificationFilter() const {
	return m_minificationFilter;
}

void MaterialProperty::minificationFilter(FilterMode mode) {
	m_minificationFilter = mode;
	
	GLenum texType = (m_cube ? GL_TEXTURE_CUBE_MAP : GL_TEXTURE_2D);
	
	switch (mode) {
		case FilterMode_NearestMipmapNearest:
		case FilterMode_NearestMipmapLinear:
		case FilterMode_LinearMipmapNearest:
		case FilterMode_LinearMipmapLinear:
			glGenerateMipmap(texType);
			break;
		default:
			break;
	}
	
	glBindTexture(texType, m_glTextureID);
	glTexParameteri(texType, GL_TEXTURE_MIN_FILTER, GLFilterModeForFilterMode(mode));
}

FilterMode MaterialProperty::magnificationFilter() const {
	return m_magnificationFilter;
}

void MaterialProperty::magnificationFilter(FilterMode mode) {
	m_magnificationFilter = mode;
	
	GLenum texType = (m_cube ? GL_TEXTURE_CUBE_MAP : GL_TEXTURE_2D);
	
	switch (mode) {
		case FilterMode_Nearest:
		case FilterMode_Linear:
			glBindTexture(texType, m_glTextureID);
			glTexParameteri(texType, GL_TEXTURE_MAG_FILTER, GLFilterModeForFilterMode(mode));
			break;
		default:
			//cout << "Error: unsupported magnification filter mode: " << mode << endl;
			//cout << "Error: unsupported magnification filter mode: " << mode << endl;
			AE_LOG->warn("Unsupported magnification filter mode: {}", mode);
			break;
	}
}

float MaterialProperty::maxAnisotropy() const {
	return m_maxAnisotropy;
}

void MaterialProperty::maxAnisotropy(float max) {
	float anisotropy = max;
	
	GLenum texType = (m_cube ? GL_TEXTURE_CUBE_MAP : GL_TEXTURE_2D);

	glBindTexture(texType, m_glTextureID);
	float largest;
	glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &largest);
	if (anisotropy > largest) anisotropy = largest;
	glTexParameterf(texType, GL_TEXTURE_MAX_ANISOTROPY_EXT, anisotropy);
	
	m_maxAnisotropy = anisotropy;
}

WrapMode MaterialProperty::wrapS() const {
	return m_wrapS;
}

void MaterialProperty::wrapS(WrapMode mode) {
	m_wrapS = mode;
	
	GLenum texType = (m_cube ? GL_TEXTURE_CUBE_MAP : GL_TEXTURE_2D);
	
	glBindTexture(texType, m_glTextureID);
	glTexParameteri(texType, GL_TEXTURE_WRAP_S, GLWrapModeForWrapMode(mode));
}

WrapMode MaterialProperty::wrapT() const {
	return m_wrapT;
}

void MaterialProperty::wrapT(WrapMode mode) {
	m_wrapT = mode;
	
	GLenum texType = (m_cube ? GL_TEXTURE_CUBE_MAP : GL_TEXTURE_2D);
	
	glBindTexture(texType, m_glTextureID);
	glTexParameteri(texType, GL_TEXTURE_WRAP_T, GLWrapModeForWrapMode(mode));
}

/***************************************************************************************
     MARK:   Internal
 **************************************************************************************/

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
		
		minificationFilter(FilterMode_LinearMipmapLinear);
		magnificationFilter(FilterMode_Linear);
		maxAnisotropy(16);
		wrapS(WrapMode_ClampToEdge);
		wrapT(WrapMode_ClampToEdge);
		
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

void MaterialProperty::bind(MaterialPropertyType type, Program& program) {
	
	if (m_cube) { // currently only used for skybox
		program.bindTexture("cubeSampler", GL_TEXTURE_CUBE_MAP, GL_TEXTURE0, m_glTextureID, 0);
	}
	else if (m_image) { // texture
		string modeUniformName = "";
		string samplerUniformName = "";
		GLenum slot;
		GLint index;
		
		switch (type) {
			case MaterialPropertyType_Ambient:
				modeUniformName = "ambientMode";
				samplerUniformName = "samplers.ambient";
				slot = GL_TEXTURE0; index = 0;
				break;
			case MaterialPropertyType_Diffuse:
				modeUniformName = "diffuseMode";
				samplerUniformName = "samplers.diffuse";
				slot = GL_TEXTURE1; index = 1;
				break;
			case MaterialPropertyType_Specular:
				modeUniformName = "specularMode";
				samplerUniformName = "samplers.specular";
				slot = GL_TEXTURE2; index = 2;
				break;
			case MaterialPropertyType_Emissive:
				modeUniformName = "emissiveMode";
				samplerUniformName = "samplers.emissive";
				slot = GL_TEXTURE3; index = 3;
				break;
			default:
				cout << "Invalid MaterialPropertyType: " << type << endl;
				return;
		}

		program.setUniform(modeUniformName.c_str(), MaterialMode_Sampler);
		program.bindTexture(samplerUniformName.c_str(), GL_TEXTURE_2D, slot, m_glTextureID, index);
	}
	else { // color
		string modeUniformName = "";
		string colorUniformName = "";
		
		switch (type) {
			case MaterialPropertyType_Ambient:
				modeUniformName = "ambientMode";
				colorUniformName = "colors.ambient";
				break;
			case MaterialPropertyType_Diffuse:
				modeUniformName = "diffuseMode";
				colorUniformName = "colors.diffuse";
				break;
			case MaterialPropertyType_Specular:
				modeUniformName = "specularMode";
				colorUniformName = "colors.specular";
				break;
			case MaterialPropertyType_Emissive:
				modeUniformName = "emissiveMode";
				colorUniformName = "colors.emissive";
				break;
			default:
				cout << "Invalid MaterialPropertyType: " << type << endl;
				return;
		}
		
		program.setUniform(modeUniformName.c_str(), MaterialMode_Color);
		program.setUniform(colorUniformName.c_str(), m_color->r, m_color->g, m_color->b);
	}
}
