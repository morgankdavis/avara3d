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
#include <set>
#include <vector>

#include <GL/glew.h>

#include "Camera.h"
#include "Color.h"
#include "Geometry.h"
#include "GeometryElement.h"
#include "Global.h"
#include "Image.h"
#include "Light.h"
#include "Logger.h"
#include "Material.h"
#include "MaterialProperty.h"
#include "Node.h"
#include "Program.h"
#include "RenderContext.h"
#include "Scene.h"


using namespace ae;
using namespace glm;
using namespace std;


/***************************************************************************************
     Types
 ***************************************************************************************/

enum class MATERIAL_MODE : int {
	NONE = 		0,
	COLOR = 	1,
	SAMPLER = 	2
};
	
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
	/* vec3 direction_world;
	float attenuationStart;
	float attenuationEnd;
	float attenuationExponent;
	float innerAngle;
	float outerAngle; */
} LightGLSLStruct;

typedef struct {
	float32_t 	startDistance;
	float32_t 	endDistance;
	float32_t 	densityExponent;
	float32_t 	PADDING1;
	vec4 		color;
	/* float32_t 	PADDING2; */
} FogGLSLStruct;

/**************************************************************************************
     Static
 **************************************************************************************/
	
static void CheckGLError() {
	GLenum err = glGetError();
	if (err != GL_NO_ERROR) {
		AE_LOG->warn("*** GL error: 0x{:X} ***", err);
	}
}

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
		case GL_LINEAR: 							return FILTER_MODE::LINEAR;
		case GL_NEAREST_MIPMAP_NEAREST:				return FILTER_MODE::NEAREST_MIPMAP_NEAREST;
		case GL_LINEAR_MIPMAP_NEAREST: 				return FILTER_MODE::LINEAR_MIPMAP_NEAREST;
		case GL_NEAREST_MIPMAP_LINEAR: 				return FILTER_MODE::NEAREST_MIPMAP_LINEAR;
		case GL_LINEAR_MIPMAP_LINEAR: 				return FILTER_MODE::LINEAR_MIPMAP_LINEAR;
		default: /* GL_NEAREST */					return FILTER_MODE::NEAREST; }
}

static GLenum GLWrapModeForWrapMode(WRAP_MODE mode) {
	switch (mode) {
		case WRAP_MODE::CLAMP_TO_EDGE:				return GL_CLAMP_TO_EDGE;
		case WRAP_MODE::CLAMP_TO_BORDER:			return GL_CLAMP_TO_BORDER;
		case WRAP_MODE::REPEAT:						return GL_REPEAT;
		case WRAP_MODE::MIRRORED_REPEAT: 			return GL_MIRRORED_REPEAT; }
}

static WRAP_MODE WrapModeForGLWrapMode(GLenum mode) {
	switch (mode) {
		case GL_CLAMP_TO_BORDER:					return WRAP_MODE::CLAMP_TO_EDGE;
		case GL_REPEAT:								return WRAP_MODE::REPEAT;
		case GL_MIRRORED_REPEAT: 					return WRAP_MODE::MIRRORED_REPEAT;
		default: /* GL_CLAMP_TO_EDGE */				return WRAP_MODE::CLAMP_TO_BORDER; }
}

static void SetTextureMinificationFilter(GLuint glTextureHandle, bool cube, FILTER_MODE mode) {

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
	
	glBindTexture(texType, glTextureHandle);
	glTexParameteri(texType, GL_TEXTURE_MIN_FILTER, GLFilterModeForFilterMode(mode));
}

static void SetTextureMagnificationFilter(GLuint glTextureHandle, bool cube, FILTER_MODE mode) {

	GLenum texType = (cube ? GL_TEXTURE_CUBE_MAP : GL_TEXTURE_2D);
	
	switch (mode) {
		case FILTER_MODE::NEAREST:
		case FILTER_MODE::LINEAR:
			glBindTexture(texType, glTextureHandle);
			glTexParameteri(texType, GL_TEXTURE_MAG_FILTER, GLFilterModeForFilterMode(mode));
			break;
		default:
			AE_LOG->warn("Unsupported magnification filter mode: {}", (unsigned)mode);
			break;
	}
}

static void SetTextureMaxAnisotropy(GLuint glTextureHandle, bool cube, float max) {

	GLenum texType = (cube ? GL_TEXTURE_CUBE_MAP : GL_TEXTURE_2D);
	
	float anisotropy = max;
	glBindTexture(texType, glTextureHandle);
	float largest;
	glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &largest);
	if (max > largest) anisotropy = largest;
	glTexParameterf(texType, GL_TEXTURE_MAX_ANISOTROPY_EXT, anisotropy);
}

static void SetTextureWrapS(GLuint glTextureHandle, bool cube, WRAP_MODE mode) {
	
	GLenum texType = (cube ? GL_TEXTURE_CUBE_MAP : GL_TEXTURE_2D);
	
	glBindTexture(texType, glTextureHandle);
	glTexParameteri(texType, GL_TEXTURE_WRAP_S, GLWrapModeForWrapMode(mode));
}

static void SetTextureWrapT(GLuint glTextureHandle, bool cube, WRAP_MODE mode) {

	GLenum texType = (cube ? GL_TEXTURE_CUBE_MAP : GL_TEXTURE_2D);
	
	glBindTexture(texType, glTextureHandle);
	glTexParameteri(texType, GL_TEXTURE_WRAP_T, GLWrapModeForWrapMode(mode));
}

static void SetTextureWrapR(GLuint glTextureHandle, WRAP_MODE mode) {

	glBindTexture(GL_TEXTURE_CUBE_MAP, glTextureHandle);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GLWrapModeForWrapMode(mode));
}

static void LoadGeometryElementVertexData(const GeometryElement& geometryElement,
						   Program& program,
						   GLuint& glVBO, GLuint& glVAO, GLuint& glIBO) {
	
	AE_LOG->info("Loading vertex data for geometry element {:p}...", (void*)&geometryElement);
	
	program.use();
	
	auto verticies = geometryElement.vertices();
	auto faces = geometryElement.faces();
	
	glGenBuffers(1, &glVBO);
	glBindBuffer(GL_ARRAY_BUFFER, glVBO);
	glBufferData(GL_ARRAY_BUFFER,
				 verticies.size() * sizeof(Vertex),
				 &(verticies[0]),
				 GL_STATIC_DRAW);
	
	glGenVertexArrays(1, &glVAO);
	glBindVertexArray(glVAO);
	
	glBindBuffer(GL_ARRAY_BUFFER, glVBO);
	
#warning figure out how to set layout only when needed
	
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
	
	glGenBuffers(1, &glIBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, glIBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,
				 faces.size() * sizeof(Face),
				 &(faces[0]),
				 GL_STATIC_DRAW);
	
	AE_LOG->info("Done.");
	
	//program.unuse();
}
	
static void LoadSkyboxVertexData(Geometry& skyboxGeometry,
								 Program& program,
								 GLuint& glVBO, GLuint& glVAO, GLuint& glIBO) {
	
	AE_LOG->info("Loading skybox vertex data...");
	
	program.use();
	
	auto element = skyboxGeometry.elements().front();
	auto verts = element->vertices();
	auto faces = element->faces();
	
	glGenBuffers(1, &glVBO);
	glBindBuffer(GL_ARRAY_BUFFER, glVBO);
	glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(Vertex), &(verts[0]), GL_STATIC_DRAW);
	
	glGenVertexArrays(1, &glVAO);
	glBindVertexArray(glVAO);
	
	glBindBuffer(GL_ARRAY_BUFFER, glVBO);
	
	GLuint positionIndex = program.getAttributeLocation("vertex_position");
	glVertexAttribPointer(positionIndex, // attrib index
						  3, // num components per attrib (3 float in vec3)
						  GL_FLOAT, // component type
						  GL_FALSE, // normalize
						  sizeof(Vertex), // stride
						  0); // start offset
	glEnableVertexAttribArray(positionIndex);
	
	glGenBuffers(1, &glIBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, glIBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,
				 faces.size() * sizeof(Face),
				 &(faces[0]),
				 GL_STATIC_DRAW);
	
	AE_LOG->info("Done.");
	
	//program.unuse();
}

static void LoadMaterialPropertyTexture(const MaterialProperty& materialProperty, GLuint& glTextureHandle) {
	
	if (materialProperty.cube()) {
		AE_LOG->info("Buffering cube texture...");
		
		GLenum sides[] = {
			GL_TEXTURE_CUBE_MAP_POSITIVE_X,
			GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
			GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
			GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
			GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
			GL_TEXTURE_CUBE_MAP_NEGATIVE_Z };
		
		glGenTextures(1, &glTextureHandle);
		glBindTexture(GL_TEXTURE_CUBE_MAP, glTextureHandle);
		
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
	
		SetTextureMinificationFilter(glTextureHandle, true, materialProperty.minificationFilter());
		SetTextureMagnificationFilter(glTextureHandle, true, materialProperty.magnificationFilter());
		SetTextureMaxAnisotropy(glTextureHandle, true, materialProperty.maxAnisotropy());
		SetTextureWrapS(glTextureHandle, true, materialProperty.wrapS());
		SetTextureWrapT(glTextureHandle, true, materialProperty.wrapT());
		SetTextureWrapR(glTextureHandle, materialProperty.wrapR());
		
		AE_LOG->info("Done.");
	}
	else if (materialProperty.image()) {
		AE_LOG->info("Buffering 2D texture...");
		
		glGenTextures(1, &glTextureHandle);
		glBindTexture(GL_TEXTURE_2D, glTextureHandle);
		
		glTexImage2D(GL_TEXTURE_2D,
					 0,
					 GL_RGBA,//GL_SRGB_ALPHA,//GL_RGBA,
					 materialProperty.image()->width(),
					 materialProperty.image()->height(),
					 0,
					 GL_RGBA,
					 GL_UNSIGNED_BYTE,
					 materialProperty.image()->data());
		
		SetTextureMinificationFilter(glTextureHandle, false, materialProperty.minificationFilter());
		SetTextureMagnificationFilter(glTextureHandle, false, materialProperty.magnificationFilter());
		SetTextureMaxAnisotropy(glTextureHandle, false, materialProperty.maxAnisotropy());
		SetTextureWrapS(glTextureHandle, false, materialProperty.wrapS());
		SetTextureWrapT(glTextureHandle, false, materialProperty.wrapT());
		
		AE_LOG->info("Done.");
	}
}

static void SendMaterialUniforms(const Material& material,
								 const DEBUG_OPTIONS& debugOptions,
								 Program& program) {
	
	program.use();
	
	program.setUniform("specularExponent", material.specularExponent());
	program.setUniform("uvScale", material.uvScale());
	program.setUniform("locksAmbientWithDiffuse", material.locksAmbientWithDiffuse());
	program.setUniform("emissiveMode", 0); // 0 = MaterialMode_None -- why is this here?
	
	//program.unuse();
}

static void SendMaterialPropertyUniforms(const MaterialProperty& materialProperty,
										 MATERIAL_PROPERTY_TYPE type,
										 GLuint glTextureHandle,
										 const DEBUG_OPTIONS& debugOptions,
										 Program& program) {
	
#warning probably should be refactored
	
	program.use();
	
	if (materialProperty.cube()) {
		program.bindTexture("cubeSampler", GL_TEXTURE_CUBE_MAP, GL_TEXTURE0, glTextureHandle, 0);
	}
	else if (materialProperty.image()) { // texture
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
				cout << "Invalid MATERIAL_PROPERTY_TYPE: " << static_cast<int>(type) << endl;
				return;
		}
		
		program.setUniform(modeUniformName.c_str(), static_cast<int>(MATERIAL_MODE::SAMPLER));
		program.bindTexture(samplerUniformName.c_str(), GL_TEXTURE_2D, slot, glTextureHandle, index);
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
				cout << "Invalid MATERIAL_PROPERTY_TYPE: " << static_cast<int>(type) << endl;
				return;
		}
		
		program.setUniform(modeUniformName.c_str(), static_cast<int>(MATERIAL_MODE::COLOR));
		program.setUniform(colorUniformName.c_str(),
						   materialProperty.color()->r,
						   materialProperty.color()->g,
						   materialProperty.color()->b);
	}
	
	//program.unuse();
}

static void SetMaterialOpenGLState(const Material& material, 
								   const DEBUG_OPTIONS& debugOptions) {
	
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glDepthMask(GL_TRUE);
	
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	//AE_LOG->trace("SetOpenGLState({:p}, {}, {:p})", material, debugOptions, program);
	AE_LOG->trace("SetMaterialOpenGLState()");

	if (DEBUG_OPTIONS_CONTAINS(debugOptions, DEBUG_OPTIONS::SHOW_PHYSICS_WIREFRAMES)) {
		// can create zbuffer problems
		// https://www.opengl.org/archives/resources/faq/technical/polygonoffset.htm
		//glDepthRange(0.1, 1.0);
		//		glEnable(GL_POLYGON_OFFSET_FILL);
		//		glPolygonOffset(20.0, 0.0);
	}
	
	if (DEBUG_OPTIONS_CONTAINS(debugOptions, DEBUG_OPTIONS::SHOW_WIREFRAMES)) {
		//m_program = Program::Wireframe();
		
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
	else {
		//m_program = Program::Default();
		
		if (material.fillMode() == FILL_MODE::LINES) {
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		}
		else if (material.fillMode() == FILL_MODE::POINTS) {
			glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
		}
		else {
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}
		
		if (material.doubleSided()) {
			glDisable(GL_CULL_FACE);
		}
		else {
			glEnable(GL_CULL_FACE);
			glCullFace(GL_BACK);
		}
	}
}

static vector<shared_ptr<Node>> SortedLights(map<shared_ptr<Node>, float> lights) {
	// map: <node, distance from camera>
	
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
		sortedVector.emplace_back(element.first);
	}
	
	return sortedVector;
}
	
static void SendEnvironmentUniforms(GLuint glEnvironmentUBO, const Scene& scene, RenderStats& stats) {
	
	// program "Default" must be active
	
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
		vec3 cameraPos_world = scene.renderContext().lock()->pointOfView()->worldPosition();
		for (auto lightNode: lights) {
			auto lightPos_world = lightNode->worldPosition();
			auto lightToCamera = lightPos_world - cameraPos_world;
			auto lightToCameraDistance = length(lightToCamera);
			lightsUnsorted[lightNode] = lightToCameraDistance;
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
	
	glBindBuffer(GL_UNIFORM_BUFFER, glEnvironmentUBO);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(environmentBlock), &environmentBlock, GL_DYNAMIC_DRAW);
}
	
static void GetGeometryElementGLVertexDataHandles(GeometryElement& element,
								   map<VERTEX_DATA_ID, tuple<unsigned, unsigned, unsigned>>& idMapping,
								   VERTEX_DATA_ID& idCounter,
								   GLuint& glVBO, GLuint& glVAO, GLuint& glIBO) {
		
	// looks up and populates glVBO, glVAO, and glIBO, loading the vertex data if needed
	
	auto vertexDataID = element.vertexDataID();
	
	if (GEOMETRY_ELEMENT_DIRTY_BITS_CONTAINS(element.dirtyBits(),
											 GEOMETRY_ELEMENT_DIRTY_BITS::VERTEX_DATA)) {
		
#warning check remove any old data (hard transformed?)
		
		GLuint vbo = 0;
		LoadGeometryElementVertexData(element, *Program::Default(), glVBO, glVAO, glIBO);
		
		idMapping[++idCounter] = make_tuple(glVBO, glVAO, glIBO);
		element.vertexDataID(idCounter);
		
		element.dirtyBits(GEOMETRY_ELEMENT_DIRTY_BITS_REMOVE(element.dirtyBits(),
																	 GEOMETRY_ELEMENT_DIRTY_BITS::VERTEX_DATA));
	}
	else {
		auto mapping = idMapping[vertexDataID];
		glVBO = get<0>(mapping);
		glVAO = get<1>(mapping);
		glIBO = get<2>(mapping);
	}
}
	
static void GetSkyboxGLVertexDataHandles(Geometry& skyboxGeometry,
										 map<VERTEX_DATA_ID, tuple<unsigned, unsigned, unsigned>>& idMapping,
										 VERTEX_DATA_ID& idCounter,
										 GLuint& glVBO, GLuint& glVAO, GLuint& glIBO) {
	
	// looks up and populates glVBO, glVAO, and glIBO, loading the vertex data if needed
	//
	// NOTE: this is essentially exactly the same as GetGeometryElementGLVertexDataHandles()
	// except if the data needs to be loaded, it uses LoadGeometryElementVertexData() as the
	// layout is different.  This will probaly need to be refacted in the future as more layouts are used

	auto element = skyboxGeometry.elements().front();
	
	if (GEOMETRY_ELEMENT_DIRTY_BITS_CONTAINS(element->dirtyBits(),
											 GEOMETRY_ELEMENT_DIRTY_BITS::VERTEX_DATA)) {

		GLuint vbo = 0;
		LoadSkyboxVertexData(skyboxGeometry, *Program::Skybox(), glVBO, glVAO, glIBO);
		
		idMapping[++idCounter] = make_tuple(glVBO, glVAO, glIBO);
		element->vertexDataID(idCounter);
		
		element->dirtyBits(GEOMETRY_ELEMENT_DIRTY_BITS_REMOVE(element->dirtyBits(),
															 GEOMETRY_ELEMENT_DIRTY_BITS::VERTEX_DATA));
	}
	else {
		auto mapping = idMapping[element->vertexDataID()];
		glVBO = get<0>(mapping);
		glVAO = get<1>(mapping);
		glIBO = get<2>(mapping);
	}
}

static void GetMaterialGLTextureHandles(Material& material,
								map<TEXTURE_ID, unsigned>& idMapping,
								TEXTURE_ID& idCounter,
								GLuint* glTextureHandles) {
	
	// looks up and populates glTextureHandle, loading the texture data if needed

	shared_ptr<MaterialProperty> materialProperties[] = {material.ambient(),
		material.diffuse(), material.specular(), material.emissive()};
	
	unsigned propertyIndex = 0;
	for (propertyIndex = 0; propertyIndex<4; ++propertyIndex) {
		auto property = materialProperties[propertyIndex];
		if (property) {
			
			if (MATERIAL_PROPERTY_DIRTY_BITS_CONTAINS(property->dirtyBits(),
													  MATERIAL_PROPERTY_DIRTY_BITS::CONTENTS)) {
				
				GLuint tempTextureID = 0;
				LoadMaterialPropertyTexture(*property, tempTextureID);
				AE_LOG->debug("tempTextureID: {}", tempTextureID);
				if (tempTextureID > 0) {
					glTextureHandles[propertyIndex] = tempTextureID;
					
					idMapping[++idCounter] = glTextureHandles[propertyIndex];
					property->textureID(idCounter);
				}
				
				property->dirtyBits(MATERIAL_PROPERTY_DIRTY_BITS_REMOVE(property->dirtyBits(),
																		MATERIAL_PROPERTY_DIRTY_BITS::CONTENTS));
			}
			else {
				if (property->textureID() > 0) {
					glTextureHandles[propertyIndex] = idMapping[property->textureID()];
				}
			}
		}
	}
}
	
static void RenderSkybox(Geometry& skyboxGeometry,
						 const Node& pointOfView,
						 const DEBUG_OPTIONS& debugOptions,
						 map<VERTEX_DATA_ID, tuple<unsigned, unsigned, unsigned>>& vertexDataIDMapping,
						 VERTEX_DATA_ID& vertexDataIDCounter,
						 map<TEXTURE_ID, unsigned>& textureIDMapping,
						 TEXTURE_ID& textureIDCounter) {

	auto element = *(skyboxGeometry.elements().front());
	auto material = *(skyboxGeometry.materials().front());
	
	GLuint vbo, vao, ibo;
	GetSkyboxGLVertexDataHandles(skyboxGeometry, vertexDataIDMapping, vertexDataIDCounter, vbo, vao, ibo);
	
	GLuint glTextureHandles[] = {0, 0, 0, 0};
	GetMaterialGLTextureHandles(material, textureIDMapping, textureIDCounter, glTextureHandles);

	mat4 viewMat = lookAt(vec3(0.0f, 0.0f, 0.0f), // eye - location
						  pointOfView.worldForward(), // center - look at
						  pointOfView.worldUp()); // up
	
	auto projectionMat = pointOfView.camera()->projection();
	
	auto program = Program::Skybox();
	program->use();

	glDepthMask(GL_FALSE);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glDisable(GL_CULL_FACE);

	auto property = material.emissive();
	SendMaterialPropertyUniforms(*property,
								 // EMISSIVE is currently ignored by SendMaterialPropertyUniforms()
								 MATERIAL_PROPERTY_TYPE::EMISSIVE,
								 glTextureHandles[3], // 3 = emissive
								 debugOptions,
								 *program);
	
	program->setUniform("view", viewMat);
	program->setUniform("projection", projectionMat);

	// draw
	
	auto faces = element.faces();
	
	glBindVertexArray(vao);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	unsigned int numFaces = faces.size();
	//stats.polygons += numFaces;
	glDrawElements(GL_TRIANGLES, numFaces * sizeof(Face), GL_UNSIGNED_INT, (void*)0);
	
	//program->unuse();
}

/***************************************************************************************
     Lifecycle
 ***************************************************************************************/

OpenGLRenderer::OpenGLRenderer():
	Renderer(),
	m_vertexDataIDMapping(map<VERTEX_DATA_ID, tuple<unsigned, unsigned, unsigned>>()),
	m_textureIDMapping(map<TEXTURE_ID, unsigned>()),
	m_vertexDataIDCounter(0),
	m_textureIDCounter(0),
	m_glEnvironmentUBO(0) {

}

OpenGLRenderer::~OpenGLRenderer() {
	
}

/**************************************************************************************
     Internal
 **************************************************************************************/

bool OpenGLRenderer::initialize() {
	
	AE_LOG->trace("OpenGLRenderer::initialize()");
	
	// NOTE: must setup OpenGL context first
	
	static bool initialized = false;
	if (!initialized) {
		glewExperimental = GL_TRUE;
		glewInit();
		
		const GLubyte *renderer = glGetString(GL_RENDERER);
		const GLubyte *version = glGetString(GL_VERSION);
		AE_LOG->info("Renderer: {}", renderer);
		AE_LOG->info("Version: {}", version);
		
		initialized = true;
	}
	
	return true;
}
	
void OpenGLRenderer::beginFrame(const RenderContext& context) {
	
}

void OpenGLRenderer::endFrame(const RenderContext& context) {
	CheckGLError();
}

void OpenGLRenderer::render(Scene& scene,
							const DEBUG_OPTIONS& debugOptions) {
	
	Renderer::render(scene, debugOptions); // initializes m_renderStats
	
	auto renderContext = scene.renderContext().lock();
	
	float framebufferWidth = renderContext->framebufferWidth();
	float framebufferHeight = renderContext->framebufferHeight();
	
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, framebufferWidth, framebufferHeight);
	
	//	auto viewMat = m_pointOfView->worldTransform();
	//	auto projectionMat = m_pointOfView->camera()->projection();
	
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	if (scene.background()) {
		if (scene.background()->cube()) {
			auto renderContext = scene.renderContext().lock();
			auto pointOfView = renderContext->pointOfView();
			
			RenderSkybox(*scene.skyboxGeometry(),
						 *pointOfView,
						 debugOptions,
						 m_vertexDataIDMapping, m_vertexDataIDCounter,
						 m_textureIDMapping, m_textureIDCounter);
		}
		else if (scene.background()->color()) {
			auto color = *(scene.background()->color());
			glClearColor(color.r, color.g, color.b, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		}
	}
	
	if (m_glEnvironmentUBO == 0) {
		uint32 ubo;
		glGenBuffers(1, &ubo);
		m_glEnvironmentUBO = ubo;
	}
	
	SendEnvironmentUniforms(m_glEnvironmentUBO, scene, renderStats());
	
	//if (!DEBUG_OPTIONS_CONTAINS(debugOptions, DEBUG_OPTIONS::SHOW_BOUNDING_BOXES)) {
		Program::Default()->bindUniformBlock("EnvironmentBlock", m_glEnvironmentUBO);
	//}
}

void OpenGLRenderer::render(Geometry& geometry,
							const mat4& modelMat,
							const mat4& viewMat,
							const mat4& projectionMat,
							const DEBUG_OPTIONS& debugOptions) {
	
//	if (!DEBUG_OPTIONS_CONTAINS(debugOptions, DEBUG_OPTIONS::SHOW_BOUNDING_BOXES)) {
//		drawAABB(modelMat, viewMat, projectionMat);
//	}
}

void OpenGLRenderer::render(GeometryElement& geometryElement,
							Material& material,
							const mat4& modelMat,
							const mat4& viewMat,
							const mat4& projectionMat,
							const DEBUG_OPTIONS& debugOptions) {
	
	auto program = Program::Default();
	
	// check and load vertex data if necessary
	
	GLuint vbo, vao, ibo;
	GetGeometryElementGLVertexDataHandles(geometryElement,
										  m_vertexDataIDMapping, m_vertexDataIDCounter,
										  vbo, vao, ibo);

	// load material contents if necessary
	
	#warning use a map with MATERIAL_PROPERTY_TYPE instead of indicies
	
	GLuint glTextureHandles[] = {0, 0, 0, 0};
	GetMaterialGLTextureHandles(material,
								m_textureIDMapping, m_textureIDCounter,
								glTextureHandles);

		
	// send uniforms
#warning REFACTOR
	
	SendMaterialUniforms(material, debugOptions, *program);
	
	shared_ptr<MaterialProperty> materialProperties[] = {material.ambient(),
		material.diffuse(), material.specular(), material.emissive()};
	
	static const MATERIAL_PROPERTY_TYPE propertyTypes[] = {MATERIAL_PROPERTY_TYPE::AMBIENT, MATERIAL_PROPERTY_TYPE::DIFFUSE,
		MATERIAL_PROPERTY_TYPE::SPECULAR, MATERIAL_PROPERTY_TYPE::EMISSIVE};
	
	unsigned propertyIndex = 0;
	for (propertyIndex = 0; propertyIndex<4; ++propertyIndex) {
		auto property = materialProperties[propertyIndex];
		if (property) {
			
//			if (MATERIAL_PROPERTY_DIRTY_BITS_CONTAINS(property->dirtyBits(),
//													  MATERIAL_PROPERTY_DIRTY_BITS::CONTENTS)) {
//				
//				geometryElement.dirtyBits(GEOMETRY_ELEMENT_DIRTY_BITS_REMOVE(geometryElement.dirtyBits(),
//																			 MATERIAL_PROPERTY_DIRTY_BITS::CONTENTS));
//			}
			
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
			
			SendMaterialPropertyUniforms(*property,
										 propertyTypes[propertyIndex],
										 glTextureHandles[propertyIndex],
										 debugOptions,
										 *program);
		}
	}
	
	//SetOpenGLState(material, debugOptions, *program);
	SetMaterialOpenGLState(material, debugOptions);
	
//	if (DEBUG_OPTIONS_CONTAINS(debugOptions, DEBUG_OPTIONS::SHOW_WIREFRAMES)) {
//		program = Program::Wireframe();
//		glEnable(GL_LINE_SMOOTH);
//	}
	

	// refactor to RenderGeometryElement()
	
	program->use();

	// uniforms
	
	program->setUniform("model", modelMat);
	program->setUniform("view", inverse(viewMat));
	program->setUniform("projection", projectionMat);
	
	// draw
	
	glBindVertexArray(vao);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	unsigned int numFaces = geometryElement.faces().size();
	renderStats().polygons += numFaces;
	glDrawElements(GL_TRIANGLES, numFaces * 3, GL_UNSIGNED_INT, (void*)0);
	
	//program->unuse();
}

shared_ptr<Image> OpenGLRenderer::snapshot(const RenderContext& context) const {
	
	unsigned framebufferWidth = context.framebufferWidth();
	unsigned framebufferHeight = context.framebufferHeight();
	unsigned char *buf = (unsigned char*)malloc(framebufferWidth * framebufferHeight * 4);
	glReadPixels(0, 0, framebufferWidth, framebufferHeight, GL_RGBA, GL_UNSIGNED_BYTE, buf);
	auto image = make_shared<Image>(buf, framebufferWidth, framebufferHeight);
	free(buf);
	return image;
}
