//
//  OpenGLRenderer.cpp
//	avara-engine
//
//  Created by Morgan Davis on 4/24/18.
//  Copyright © 2018 Morgan K Davis. All rights reserved.
//

#include "OpenGLRenderer.h"

#include <algorithm>
#include <functional>
#include <iostream>
#include <map>
#include <memory>
#include <set>
#include <vector>

#include <glm/glm.hpp>
//#include <glm/gtc/matrix_transform.hpp>
#include <GL/glew.h>

//#include "Camera.h"
#include "Color.h"
//#include "Geometry.h"
#include "GeometryElement.h"
#include "Global.h"
#include "Image.h"
#include "Light.h"
#include "Logger.h"
#include "Material.h"
#include "MaterialProperty.h"
#include "Node.h"
//#include "PhysicsWorld.h"
#include "Program.h"
#include "RenderContext.h"
#include "Scene.h"
//#include "SkyboxGeometry.h"

using namespace ae;
using namespace glm;
using namespace std;


/***************************************************************************************
     Types
 ***************************************************************************************/

typedef struct {
	int32_t 	type;
	float32_t 	PADDING1;
	float32_t 	PADDING2;
	float32_t 	PADDING3;
	vec3 		position_world;
	float32_t 	PADDING4;
	vec3 		color;
	float32_t 	PADDING5;
	float 		attenuationFactor;
	float32_t 	PADDING6;
	float32_t	PADDING7;
	float32_t 	PADDING8;
	//	vec3 direction_world;
	//	float attenuationStart;
	//	float attenuationEnd;
	//	float attenuationExponent;
	//	float innerAngle;
	//	float outerAngle;
} LightGLSLStruct;

typedef struct {
	float32_t 	startDistance;
	float32_t 	endDistance;
	float32_t 	densityExponent;
	float32_t 	PADDING1;
	vec4 		color;
	//float32_t 	PADDING2;
} FogGLSLStruct;

/**************************************************************************************
     Static
 **************************************************************************************/

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

static void SetTextureMinificationFilter(GLuint textureID, bool cube, FILTER_MODE mode) {

	GLenum texType = (cube ? GL_TEXTURE_CUBE_MAP : GL_TEXTURE_2D);
	
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
	
	glBindTexture(texType, textureID);
	glTexParameteri(texType, GL_TEXTURE_MIN_FILTER, GLFilterModeForFilterMode(mode));
}

static void SetTextureMagnificationFilter(GLuint textureID, bool cube, FILTER_MODE mode) {

	GLenum texType = (cube ? GL_TEXTURE_CUBE_MAP : GL_TEXTURE_2D);
	
	switch (mode) {
		case FILTER_MODE::NEAREST:
		case FILTER_MODE::LINEAR:
			glBindTexture(texType, textureID);
			glTexParameteri(texType, GL_TEXTURE_MAG_FILTER, GLFilterModeForFilterMode(mode));
			break;
		default:
			AE_LOG->warn("Unsupported magnification filter mode: {}", (unsigned)mode);
			break;
	}
}

static void SetTextureMaxAnisotropy(GLuint textureID, bool cube, float max) {

	GLenum texType = (cube ? GL_TEXTURE_CUBE_MAP : GL_TEXTURE_2D);
	
	float anisotropy = max;
	glBindTexture(texType, textureID);
	float largest;
	glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &largest);
	if (max > largest) anisotropy = largest;
	//anisotropy = MIN(anisotropy, largest);
	glTexParameterf(texType, GL_TEXTURE_MAX_ANISOTROPY_EXT, anisotropy);
}

static void SetTextureWrapS(GLuint textureID, bool cube, WRAP_MODE mode) {
	
	GLenum texType = (cube ? GL_TEXTURE_CUBE_MAP : GL_TEXTURE_2D);
	
	glBindTexture(texType, textureID);
	glTexParameteri(texType, GL_TEXTURE_WRAP_S, GLWrapModeForWrapMode(mode));
}

static void SetTextureWrapT(GLuint textureID, bool cube, WRAP_MODE mode) {

	GLenum texType = (cube ? GL_TEXTURE_CUBE_MAP : GL_TEXTURE_2D);
	
	glBindTexture(texType, textureID);
	glTexParameteri(texType, GL_TEXTURE_WRAP_T, GLWrapModeForWrapMode(mode));
}

static void SetTextureWrapR(GLuint textureID, WRAP_MODE mode) {

	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GLWrapModeForWrapMode(mode));
}

static void LoadVertexData(const GeometryElement& geometryElement, const Program& program,
						   GLuint& vbo, GLuint& vao, GLuint& ibo) {
	
	AE_LOG->info("Loading vertex data for geometry element {:p}...", (void*)&geometryElement);
	
	auto verticies = geometryElement.vertices();
	auto faces = geometryElement.faces();
	
	//GLuint vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER,
				 verticies.size() * sizeof(Vertex),
				 &(verticies[0]),
				 GL_STATIC_DRAW);
	//m_glVBO = vbo;
	
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	
	GLuint positionIndex = program.getAttributeLocation("vertex_position");
	glVertexAttribPointer(positionIndex, 			// attrib index
						  3, 						// num components per attrib (3 float in vec3)
						  GL_FLOAT, 				// component type
						  GL_FALSE, 				// normalize
						  sizeof(Vertex), 			// stride
						  0); 						// start offset
	glEnableVertexAttribArray(positionIndex);
	
	GLuint normalIndex = program.getAttributeLocation("vertex_normal");
	glVertexAttribPointer(normalIndex, 				// attrib index
						  3, 						// num components per attrib (3 float in vec3)
						  GL_FLOAT, 				// component type
						  GL_FALSE, 				// normalize
						  sizeof(Vertex), 			// stride
						  (void *)sizeof(vec3)); 	// start offset
	glEnableVertexAttribArray(normalIndex);
	
	GLuint texCoordIndex = program.getAttributeLocation("texture_coordinate");
	glVertexAttribPointer(texCoordIndex, 							// attrib index
						  2, 										// num components per attrib (2 float in vec2)
						  GL_FLOAT, 								// component type
						  GL_FALSE, 								// normalize
						  sizeof(Vertex), 							// stride
						  (void *)(sizeof(vec3) + sizeof(vec3))); 	// start offset
	glEnableVertexAttribArray(texCoordIndex);
	
	glGenBuffers(1, &ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,
				 faces.size() * sizeof(Face),
				 &(faces[0]),
				 GL_STATIC_DRAW);
	
	AE_LOG->info("Done.");
}

//void MaterialProperty::loadTexture() {
//	
//	if (m_cube) {
//		//cout << "Loading cube texture..." << endl;
//		AE_LOG->info("Buffering cube texture...");
//		
//		GLenum sides[] = {
//			GL_TEXTURE_CUBE_MAP_POSITIVE_X,
//			GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
//			GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
//			GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
//			GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
//			GL_TEXTURE_CUBE_MAP_NEGATIVE_Z };
//		
//		glGenTextures(1, &m_glTextureID);
//		glBindTexture(GL_TEXTURE_CUBE_MAP, m_glTextureID);
//		
//		for (int s=0; s<6; ++s) {
//			GLenum side = sides[s];
//			Image image = *(*m_cube)[s];
//			
//			glTexImage2D(side,
//						 0,
//						 GL_RGBA,//GL_SRGB_ALPHA,//GL_RGBA,
//						 image.width(),
//						 image.height(),
//						 0,
//						 GL_RGBA,
//						 GL_UNSIGNED_BYTE,
//						 image.data());
//		}
//		
//		minificationFilter(FILTER_MODE::LINEAR_MIPMAP_LINEAR);
//		magnificationFilter(FILTER_MODE::LINEAR);
//		maxAnisotropy(16);
//		wrapS(WRAP_MODE::CLAMP_TO_EDGE);
//		wrapT(WRAP_MODE::CLAMP_TO_EDGE);
//		
//		AE_LOG->info("Done.");
//	}
//	else if (m_image) {
//		//cout << "Loading 2D texture..." << endl;
//		AE_LOG->info("Buffering 2D texture...");
//		
//		glGenTextures(1, &m_glTextureID);
//		glBindTexture(GL_TEXTURE_2D, m_glTextureID);
//		
//		glTexImage2D(GL_TEXTURE_2D,
//					 0,
//					 GL_RGBA,//GL_SRGB_ALPHA,//GL_RGBA,
//					 m_image->width(),
//					 m_image->height(),
//					 0,
//					 GL_RGBA,
//					 GL_UNSIGNED_BYTE,
//					 m_image->data());
//		
//		minificationFilter(m_minificationFilter);
//		magnificationFilter(m_magnificationFilter);
//		maxAnisotropy(m_maxAnisotropy);
//		wrapS(m_wrapS);
//		wrapT(m_wrapT);
//		
//		AE_LOG->info("Done.");
//	}
//}

static void LoadTexture(const MaterialProperty& materialProperty, GLuint& textureID) {
	
	if (materialProperty.cube()) {
		AE_LOG->info("Buffering cube texture...");
		
		GLenum sides[] = {
			GL_TEXTURE_CUBE_MAP_POSITIVE_X,
			GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
			GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
			GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
			GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
			GL_TEXTURE_CUBE_MAP_NEGATIVE_Z };
		
		glGenTextures(1, &textureID);
		glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
		
		for (int s=0; s<6; ++s) {
			GLenum side = sides[s];
			Image image = *((*materialProperty.cube())[s]);
			
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
		
//		SetTextureMinificationFilter(textureID, true, FILTER_MODE::LINEAR_MIPMAP_LINEAR);
//		SetTextureMagnificationFilter(textureID, true, FILTER_MODE::LINEAR);
//		SetTextureMaxAnisotropy(textureID, true, 16);
//		SetTextureWrapS(textureID, true, WRAP_MODE::CLAMP_TO_EDGE);
//		SetTextureWrapT(textureID, true, WRAP_MODE::CLAMP_TO_EDGE);
//		SetTextureWrapR(textureID, WRAP_MODE::CLAMP_TO_EDGE);
		
		SetTextureMinificationFilter(textureID, false, materialProperty.minificationFilter());
		SetTextureMagnificationFilter(textureID, false, materialProperty.magnificationFilter());
		SetTextureMaxAnisotropy(textureID, false, materialProperty.maxAnisotropy());
		SetTextureWrapS(textureID, false, materialProperty.wrapS());
		SetTextureWrapT(textureID, false, materialProperty.wrapT());
		SetTextureWrapR(textureID, materialProperty.wrapR());
		
		AE_LOG->info("Done.");
	}
	else if (materialProperty.image()) {
		AE_LOG->info("Buffering 2D texture...");
		
		glGenTextures(1, &textureID);
		glBindTexture(GL_TEXTURE_2D, textureID);
		
		glTexImage2D(GL_TEXTURE_2D,
					 0,
					 GL_RGBA,//GL_SRGB_ALPHA,//GL_RGBA,
					 materialProperty.image()->width(),
					 materialProperty.image()->height(),
					 0,
					 GL_RGBA,
					 GL_UNSIGNED_BYTE,
					 materialProperty.image()->data());
		
		SetTextureMinificationFilter(textureID, false, materialProperty.minificationFilter());
		SetTextureMagnificationFilter(textureID, false, materialProperty.magnificationFilter());
		SetTextureMaxAnisotropy(textureID, false, materialProperty.maxAnisotropy());
		SetTextureWrapS(textureID, false, materialProperty.wrapS());
		SetTextureWrapT(textureID, false, materialProperty.wrapT());
		
		AE_LOG->info("Done.");
	}
}

static vector<shared_ptr<Node>> SortedLights(map<shared_ptr<Node>, float> lights) {
	
	// http://thispointer.com/how-to-sort-a-map-by-value-in-c/
	
	typedef function<bool(pair<shared_ptr<Node>, float>, pair<shared_ptr<Node>, float>)> Comparator;
	
	Comparator compFunctor = [](pair<shared_ptr<Node>, float> elem1, pair<shared_ptr<Node>, float> elem2) {
		return elem1.second < elem2.second;
	};
	
	set<pair<shared_ptr<Node>, float>, Comparator> lightsSorted(lights.begin(),
																lights.end(),
																compFunctor);
	
	auto sortedVector = vector<shared_ptr<Node>>();
	for (pair<shared_ptr<Node>, float> element : lightsSorted) {
		//cout << element.first << " :: " << element.second << endl;
		sortedVector.emplace_back(element.first);
	}
	
	return sortedVector;
}

// * TEMPORARY *
//static void PrepareMaterialForRender(const Material& material, Program& program, DEBUG_OPTIONS debugOptions) {
//	AE_LOG->trace("PrepareMaterialForRender()");
//	
//	//if ((unsigned)debugOptions | (unsigned)DEBUG_OPTIONS::SHOW_PHYSICS_WIREFRAMES) {
//	if (DEBUG_OPTIONS_CONTAINS(debugOptions, DEBUG_OPTIONS::SHOW_PHYSICS_WIREFRAMES)) {
//		// can create zbuffer problems
//		// https://www.opengl.org/archives/resources/faq/technical/polygonoffset.htm
//		//glDepthRange(0.1, 1.0);
//		//		glEnable(GL_POLYGON_OFFSET_FILL);
//		//		glPolygonOffset(20.0, 0.0);
//	}
//	
//	//if ((unsigned)debugOptions & (unsigned)DEBUG_OPTIONS::SHOW_WIREFRAMES) {
//	if (DEBUG_OPTIONS_CONTAINS(debugOptions, DEBUG_OPTIONS::SHOW_WIREFRAMES)) {
//		//m_program = Program::Wireframe();
//		
//		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
//	}
//	else {
//		//m_program = Program::Default();
//		
//		//if ((debugOptions & DebugOption_ShowWireframes) || (m_fillMode == FillMode_Lines)) {
//		if (material.fillMode() == FILL_MODE::LINES) {
//			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
//		}
//		else if (material.fillMode() == FILL_MODE::POINTS) {
//			glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
//		}
//		else {
//			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
//		}
//		
//		if (material.doubleSided()) {
//			glDisable(GL_CULL_FACE);
//		}
//		else {
//			glEnable(GL_CULL_FACE);
//			glCullFace(GL_BACK);
//		}
//		
//		program.setUniform("specularExponent", material.specularExponent());
//		
//		program.setUniform("uvScale", material.uvScale());
//		//		m_program->setUniform("specularExponent", m_uvScale);
//		
//		// this is a bit of a hack, but since we're sharing programs now this needs to be reset...
//		//MaterialPropertyType_Emissive
//		program.setUniform("emissiveMode", 0); // 0 = MaterialMode_None
//		
//		// only lock for diffuse textures, not colors
//		if (material.locksAmbientWithDiffuse() && material.diffuse() && (material.diffuse()->color() || material.diffuse()->image())) {
//			material.diffuse()->bind(MATERIAL_PROPERTY_TYPE::AMBIENT, program);
//		}
//		else {
//			if (material.ambient()) {
//				material.ambient()->bind(MATERIAL_PROPERTY_TYPE::AMBIENT, program);
//			}
//		}
//		if (material.diffuse()) {
//			material.diffuse()->bind(MATERIAL_PROPERTY_TYPE::DIFFUSE, program);
//		}
//		if (material.specular()) {
//			material.specular()->bind(MATERIAL_PROPERTY_TYPE::SPECULAR, program);
//		}
//		if (material.emissive()) {
//			material.emissive()->bind(MATERIAL_PROPERTY_TYPE::EMISSIVE, program);
//		}
//	}
//}

/***************************************************************************************
     Lifecycle
 ***************************************************************************************/

OpenGLRenderer::OpenGLRenderer():
	Renderer(),
	m_vertexDataHandleGLMapping(map<VERTEX_DATA_ID, tuple<unsigned, unsigned, unsigned>>()),
	m_textureHandleGLMapping(map<TEXTURE_ID, unsigned>()),
	m_vertexDataHandleCounter(0),
	m_textureHandleCounter(0) {

}

OpenGLRenderer::~OpenGLRenderer() {
	
}

/**************************************************************************************
     Internal
 **************************************************************************************/

void OpenGLRenderer::render(Scene& scene,
							unsigned framebufferWidth,
							unsigned framebufferHeight,
							const DEBUG_OPTIONS& debugOptions) {
	
	Renderer::render(scene, framebufferWidth, framebufferHeight, debugOptions); // initializes m_renderStats
	
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, framebufferWidth, framebufferHeight);
	
	//	auto viewMat = m_pointOfView->worldTransform();
	//	auto projectionMat = m_pointOfView->camera()->projection();
	
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	if (scene.background()) {
		/*if (scene.background()->cube()) {
			mat4 skyboxViewMat = lookAt(vec3(0.0f, 0.0f, 0.0f), // eye - location
										m_pointOfView->worldForward(), // center - look at
										m_pointOfView->worldUp()); // up
			
			SkyboxGeometry skyboxGeometry = *(*(scene.skyboxGeometry()));
			SkyboxGeometryElement& element = static_cast<SkyboxGeometryElement&>(geoElem);
			
			m_skyboxGeometry->draw(skyboxViewMat, projectionMat, stats);
			render(*(scene.skyboxGeometry()));
		}
		else */if (scene.background()->color()) {
			auto color = *(scene.background()->color());
			glClearColor(color.r, color.g, color.b, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		}
	}
	
	if (m_glEnvironmentUBO < 0) {
		uint32 ubo;
		glGenBuffers(1, &ubo);
		m_glEnvironmentUBO = ubo;
	}
	bindEnvironment(scene, renderStats());
}

void OpenGLRenderer::render(Geometry& geometry,
							const mat4& modelMat,
							const mat4& viewMat,
							const mat4& projectionMat,
							const DEBUG_OPTIONS& debugOptions) {
	
}

void OpenGLRenderer::render(GeometryElement& geometryElement,
							Material& material,
							const mat4& modelMat,
							const mat4& viewMat,
							const mat4& projectionMat,
							const DEBUG_OPTIONS& debugOptions) {
	
	// check and load vertex data if necessary
	
	// refactor to GetVertexDataHandles();
	
	GLuint vao = 0;
	GLuint ibo = 0;
	
	if (GEOMETRY_ELEMENT_DIRTY_BITS_CONTAINS(geometryElement.dirtyBits(),
											 GEOMETRY_ELEMENT_DIRTY_BITS::VERTEX_DATA)) {
		
#warning check remove any old data (hard transformed?)
		
		GLuint vbo = 0;
		LoadVertexData(geometryElement, *Program::Default(), vbo, vao, ibo);
		
		m_vertexDataHandleGLMapping[++m_vertexDataHandleCounter] = make_tuple(vbo, vao, ibo);
		geometryElement.vertexDataID(m_vertexDataHandleCounter);
		
		geometryElement.dirtyBits(GEOMETRY_ELEMENT_DIRTY_BITS_REMOVE(geometryElement.dirtyBits(),
																	 GEOMETRY_ELEMENT_DIRTY_BITS::VERTEX_DATA));
	}
	else {
		auto mapping = m_vertexDataHandleGLMapping[geometryElement.vertexDataID()];
		vao = get<1>(mapping);
		ibo = get<2>(mapping);
	}
	
	// load material contents if necessary
	
	// refactor to GetTextureHandle();
	
	int textureIDs[] = {-1, -1, -1, -1};

	auto materialProperties = vector<shared_ptr<MaterialProperty>>();
	materialProperties.reserve(4);
	if(material.ambient()) materialProperties.emplace_back(material.ambient());
	if(material.diffuse()) materialProperties.emplace_back(material.diffuse());
	if(material.specular()) materialProperties.emplace_back(material.specular());
	if(material.emissive()) materialProperties.emplace_back(material.emissive());
	
	unsigned textureIndex = 0;
	for (auto& property : materialProperties) {
		if (MATERIAL_PROPERTY_DIRTY_BITS_CONTAINS(property->dirtyBits(),
												  MATERIAL_PROPERTY_DIRTY_BITS::CONTENTS)) {
			
			GLuint tempTextureID;
			LoadTexture(*property, tempTextureID);
			textureIDs[textureIndex] = tempTextureID;
			
			property->dirtyBits(MATERIAL_PROPERTY_DIRTY_BITS_REMOVE(property->dirtyBits(),
																	MATERIAL_PROPERTY_DIRTY_BITS::CONTENTS));
			
			m_textureHandleGLMapping[++m_textureHandleCounter] = textureIDs[textureIndex];
			property->textureID(m_textureHandleCounter);
		}
		else {
			textureIDs[textureIndex] = m_textureHandleGLMapping[property->textureID()];
		}
		
		++textureIndex;
	}
		
	// bind uniforms
	
	for (auto& property : materialProperties) {

		if (MATERIAL_PROPERTY_DIRTY_BITS_CONTAINS(property->dirtyBits(),
												  MATERIAL_PROPERTY_DIRTY_BITS::CONTENTS)) {
			
			geometryElement.dirtyBits(GEOMETRY_ELEMENT_DIRTY_BITS_REMOVE(geometryElement.dirtyBits(),
																		 MATERIAL_PROPERTY_DIRTY_BITS::CONTENTS));
		}
		
		if (MATERIAL_PROPERTY_DIRTY_BITS_CONTAINS(property->dirtyBits(),
												  MATERIAL_PROPERTY_DIRTY_BITS::MINIFICATION_FILTER)) {

			geometryElement.dirtyBits(GEOMETRY_ELEMENT_DIRTY_BITS_REMOVE(geometryElement.dirtyBits(),
																		 MATERIAL_PROPERTY_DIRTY_BITS::MINIFICATION_FILTER));
		}
		
		if (MATERIAL_PROPERTY_DIRTY_BITS_CONTAINS(property->dirtyBits(),
												  MATERIAL_PROPERTY_DIRTY_BITS::MAGNIFICATION_FILTER)) {
			
			geometryElement.dirtyBits(GEOMETRY_ELEMENT_DIRTY_BITS_REMOVE(geometryElement.dirtyBits(),
																		 MATERIAL_PROPERTY_DIRTY_BITS::MAGNIFICATION_FILTER));
		}
		
		if (MATERIAL_PROPERTY_DIRTY_BITS_CONTAINS(property->dirtyBits(),
												  MATERIAL_PROPERTY_DIRTY_BITS::WRAP_S)) {
			
			geometryElement.dirtyBits(GEOMETRY_ELEMENT_DIRTY_BITS_REMOVE(geometryElement.dirtyBits(),
																		 MATERIAL_PROPERTY_DIRTY_BITS::WRAP_S));
		}
		
		if (MATERIAL_PROPERTY_DIRTY_BITS_CONTAINS(property->dirtyBits(),
												  MATERIAL_PROPERTY_DIRTY_BITS::WRAP_T)) {
			
			geometryElement.dirtyBits(GEOMETRY_ELEMENT_DIRTY_BITS_REMOVE(geometryElement.dirtyBits(),
																		 MATERIAL_PROPERTY_DIRTY_BITS::WRAP_T));
		}
		
		if (MATERIAL_PROPERTY_DIRTY_BITS_CONTAINS(property->dirtyBits(),
												  MATERIAL_PROPERTY_DIRTY_BITS::WRAP_R)) {
			
			geometryElement.dirtyBits(GEOMETRY_ELEMENT_DIRTY_BITS_REMOVE(geometryElement.dirtyBits(),
																		 MATERIAL_PROPERTY_DIRTY_BITS::WRAP_R));
		}
		
		if (MATERIAL_PROPERTY_DIRTY_BITS_CONTAINS(property->dirtyBits(),
												  MATERIAL_PROPERTY_DIRTY_BITS::MAX_ANISTROPY)) {
			
			geometryElement.dirtyBits(GEOMETRY_ELEMENT_DIRTY_BITS_REMOVE(geometryElement.dirtyBits(),
																		 MATERIAL_PROPERTY_DIRTY_BITS::MAX_ANISTROPY));
		}
	}
	

	
	//auto program = material.program();
	//auto program = material.selectProgram(m_debugOptions);
	
	auto program = Program::Default();
//	if (DEBUG_OPTIONS_CONTAINS(debugOptions, DEBUG_OPTIONS::SHOW_WIREFRAMES)) {
//		program = Program::Wireframe();
//		glEnable(GL_LINE_SMOOTH);
//	}
	
	// gl config
	
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glDepthMask(GL_TRUE);
	
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	// use shader program
	
	program->use();
	
	// uniforms
	
	program->setUniform("model", modelMat);
	program->setUniform("view", inverse(viewMat));
	program->setUniform("projection", projectionMat);
	
	// * TEMPORARY *
	//material.prepareToRender(m_debugOptions);
	//PrepareMaterialForRender(material, *program, debugOptions);
	
	//	if (!((unsigned)debugOptions & (unsigned)DEBUG_OPTIONS::SHOW_WIREFRAMES)) {
	//		program->bindUniformBlock("EnvironmentBlock", glEnvironmentUBO);
	//	}
	
	// draw
	
	glBindVertexArray(vao);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	unsigned int numFaces = geometryElement.faces().size();
	renderStats().polygons += numFaces;
	glDrawElements(GL_TRIANGLES, numFaces * 3, GL_UNSIGNED_INT, (void*)0);
	
	program->unuse();
}

shared_ptr<Image> OpenGLRenderer::snapshot(unsigned framebufferWidth,
										   unsigned framebufferHeight) const {
	
	unsigned char *buf = (unsigned char*)malloc(framebufferWidth * framebufferHeight * 4);
	glReadPixels(0, 0, framebufferWidth, framebufferHeight, GL_RGBA, GL_UNSIGNED_BYTE, buf);
	auto image = make_shared<Image>(buf, framebufferWidth, framebufferHeight);
	free(buf);
	return image;
}

/**************************************************************************************
     Private
 **************************************************************************************/

void OpenGLRenderer::bindEnvironment(const Scene& scene, RenderStats& stats) const {
	
	// lights
	
	auto lights = vector<shared_ptr<Node>>();
	shared_ptr<Node> ambientLight = nullptr;
	
	// find all lights in the scene
	for (auto node: scene.rootNode()->childNodes(true)) {
		if (!node->hidden()) {
			auto light = node->light();
			if (light != nullptr) {
				if (light->type() == LIGHT_TYPE::POINT) {
					lights.emplace_back(node);
				}
				else if (light->type() == LIGHT_TYPE::AMBIENT) {
					ambientLight = node;
				}
			}
		}
	}
	
	if (lights.size() > MAX_DYNAMIC_LIGHTS) {
		
		// find all light distances from the camera
		
		auto lightsUnsorted = map<shared_ptr<Node>, float>();
		//vec3 cameraPos_world = m_pointOfView->worldPosition();
		vec3 cameraPos_world = scene.renderContext().lock()->pointOfView()->worldPosition();
		for (auto lightNode: lights) {
			auto lightPos_world = lightNode->worldPosition();
			auto lightToCamera = lightPos_world - cameraPos_world;
			auto lightToCameraDistance = length(lightToCamera);
			lightsUnsorted[lightNode] = lightToCameraDistance;
			//cout << "lightToCameraDistance: " << lightToCameraDistance << endl;
		}
		
		lights = SortedLights(lightsUnsorted);
		
		unsigned endIndex = std::min((unsigned)lights.size(), (unsigned)(MAX_DYNAMIC_LIGHTS));
		vector<shared_ptr<Node>>::const_iterator first = lights.begin() + 0;
		vector<shared_ptr<Node>>::const_iterator last = lights.begin() + endIndex;
		vector<shared_ptr<Node>> lightsSlice(first, last);
		
		lights = lightsSlice;
	}
	
	// check for default lighting
	
	if (lights.size() == 0) {
		auto detaultPoint = Light::DefaultPointNode();
		// set position based on scene extent...
		static vec3 sceneExtent = scene.extent(); // only doing this once or it runs reallll slow
		detaultPoint->position({sceneExtent.x + sceneExtent.x/4.0,
			sceneExtent.y + sceneExtent.y/4.0,
			sceneExtent.z + sceneExtent.z/4.0});
		lights.emplace_back(detaultPoint);
	}
	if (!ambientLight) ambientLight = Light::DefaultAmbientNode();
	
	lights.emplace_back(ambientLight);
	
	unsigned numLights = lights.size();
	LightGLSLStruct lightStruct[numLights];
	
	stats.lights = numLights - 1; // not counting ambient
	
	for (int l=0; l<numLights; ++l) {
		auto node = lights[l];
		auto light = node->light();
		
		lightStruct[l].type = (unsigned)(light->type());
		lightStruct[l].position_world = node->worldPosition();
		lightStruct[l].attenuationFactor = light->attenuationFactor();
		
		auto color = *light->color();
		lightStruct[l].color = vec3(color.r, color.g, color.b);
	}
	
	// fog
	
	//	FogGLSLStruct fogStruct;
	//	fogStruct.startDistance = m_fogStartDistance;
	//	fogStruct.endDistance = m_fogEndDistance;
	//	fogStruct.densityExponent = m_fogDensityExponent;
	//	fogStruct.startDistance = m_fogStartDistance;
	//	if (m_fogColor) fogStruct.color = vec4(m_fogColor->r, m_fogColor->g, m_fogColor->b, m_fogColor->a);
	//	else fogStruct.color = vec4(0.0, 0.0, 0.0, 0.0);
	
	FogGLSLStruct fogStruct;
	fogStruct.startDistance = scene.fogStartDistance();
	fogStruct.endDistance = scene.fogEndDistance();
	fogStruct.densityExponent = scene.fogDensityExponent();
	fogStruct.startDistance = scene.fogStartDistance();
	if (scene.fogColor()) fogStruct.color = vec4(scene.fogColor()->r, scene.fogColor()->g, scene.fogColor()->b,
												 scene.fogColor()->a);
	else fogStruct.color = vec4(0.0, 0.0, 0.0, 0.0);
	
	// block
	
	typedef struct {
		int32_t 			numLights;
		float32_t 			PADDING1;
		float32_t 			PADDING2;
		float32_t 			PADDING3;
		LightGLSLStruct 	lights[MAX_DYNAMIC_LIGHTS+1]; // +1 ambient
		FogGLSLStruct		fog;
	} EnvironmentBlock;
	
	EnvironmentBlock environmentBlock;
	environmentBlock.numLights = numLights;
	memcpy(&environmentBlock.lights, &lightStruct, sizeof(lightStruct));
	memcpy(&environmentBlock.fog, &fogStruct, sizeof(fogStruct));
	
	glBindBuffer(GL_UNIFORM_BUFFER, m_glEnvironmentUBO);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(environmentBlock), &environmentBlock, GL_DYNAMIC_DRAW);
}
