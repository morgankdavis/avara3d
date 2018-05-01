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

#define FONTSTASH_IMPLEMENTATION
#include "fontstash.h"
#include <GL/glew.h>
#define GLFONTSTASH_IMPLEMENTATION
#include "gl3fontstash.h"

#include "Camera.h"
#include "Color.h"
#include "CubeImage.h"
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
#include "Utilities.h"


using namespace ae;
using namespace ae::utils;
using namespace glm;
using namespace std;


/**************************************************************************************
     Static Prorotypes
 **************************************************************************************/

static void RenderSkybox(Geometry& skyboxGeometry,
						 const Node& pointOfView,
						 const DEBUG_OPTIONS& debugOptions,
						 RenderStats& stats,
						 map<VERTEX_DATA_ID, tuple<unsigned, unsigned, unsigned>>& vertexDataIDMapping,
						 VERTEX_DATA_ID& vertexDataIDCounter,
						 map<TEXTURE_ID, unsigned>& textureIDMapping,
						 TEXTURE_ID& textureIDCounter);
static void RenderAABB(Geometry& geometry,
					   mat4 modelMat,
					   mat4 viewMat,
					   mat4 projectionMat,
					   map<VERTEX_DATA_ID, pair<unsigned, unsigned>>& idMapping,
					   VERTEX_DATA_ID& idCounter,
					   RenderStats& stats);
static void GetGeometryElementGLVertexDataHandles(GeometryElement& element,
												  map<VERTEX_DATA_ID, tuple<unsigned, unsigned, unsigned>>& idMapping,
												  VERTEX_DATA_ID& idCounter,
												  GLuint& glVBO, GLuint& glVAO, GLuint& glIBO);
static void GetSkyboxGLVertexDataHandles(Geometry& skyboxGeometry,
										 map<VERTEX_DATA_ID, tuple<unsigned, unsigned, unsigned>>& idMapping,
										 VERTEX_DATA_ID& idCounter,
										 GLuint& glVBO, GLuint& glVAO, GLuint& glIBO);
static void GetAABBGLVertexDataHandles(Geometry& geometry,
									   map<VERTEX_DATA_ID, pair<unsigned, unsigned>>& idMapping,
									   VERTEX_DATA_ID& idCounter,
									   GLuint& glVBO, GLuint& glVAO);
static void GetMaterialGLTextureHandles(Material& material,
										map<TEXTURE_ID, unsigned>& idMapping, TEXTURE_ID& idCounter,
										map<MATERIAL_PROPERTY_TYPE, GLuint>& glTextureHandles);
static void LoadGeometryElementVertexData(const GeometryElement& geometryElement,
										  Program& program,
										  GLuint& glVBO, GLuint& glVAO, GLuint& glIBO);
static void LoadSkyboxVertexData(Geometry& skyboxGeometry,
								 Program& program,
								 GLuint& glVBO, GLuint& glVAO, GLuint& glIBO);
static void LoadAABBVertexData(Geometry& geometry,
							   const Program& program,
							   GLuint& glVBO, GLuint& glVAO);
static void LoadMaterialPropertyTexture(const MaterialProperty& materialProperty, GLuint& glTextureHandle);	
static void SendMaterialUniforms(const Material& material,
								 Program& program,
								 map<MATERIAL_PROPERTY_TYPE, GLuint>& glTextureHandles,
								 const DEBUG_OPTIONS& debugOptions);
static void SendMaterialPropertyUniforms(MaterialProperty& property,
										 MATERIAL_PROPERTY_TYPE type,
										 GLuint glTextureHandle,
										 const DEBUG_OPTIONS& debugOptions,
										 Program& program);
static void SendEnvironmentUniforms(GLuint glEnvironmentUBO, const Scene& scene, RenderStats& stats);
static void SetMaterialPropertyFilteringOptions(MaterialProperty& property,
												GLuint glTextureHandle);
static void SetMaterialFilteringOptions(const Material& material,
										map<MATERIAL_PROPERTY_TYPE, GLuint>& glTextureHandles);
static void SetMaterialOpenGLState(const Material& material, 
								   const DEBUG_OPTIONS& debugOptions);
static void SetSkyboxOpenGLState();
static void SetAABBOpenGLState();
static void DrawGeometryElement(GeometryElement& element,
								Program& program,
								mat4 modelMat, mat4 viewMat, mat4 projectionMat,
								GLuint vao, GLuint ibo);	
static void DrawSkyboxElement(GeometryElement& element,
							  Program& program,
							  const Node& pointOfView,
							  GLuint vao, GLuint ibo);
static void DrawAABB(Geometry& geometry,
					 mat4 modelMat,
					 mat4 viewMat,
					 mat4 projectionMat,
					 GLuint glVBO, GLuint glVAO);
static vector<shared_ptr<Node>> SortedLights(map<shared_ptr<Node>, float> lights);	
static void UpdateStatsOverlay(RenderStats& stats, float time, Scene& scene,
							   FONScontext* fonsContext, int fonsFont);
static float DrawString(string string, float size, float dx, float dy,
						FONScontext* fonsContext);
static void SetTextureMinificationFilter(GLuint glTextureHandle, bool cube, FILTER_MODE mode);
static void SetTextureMagnificationFilter(GLuint glTextureHandle, bool cube, FILTER_MODE mode);
static void SetTextureMaxAnisotropy(GLuint glTextureHandle, bool cube, float max);
static void SetTextureWrapS(GLuint glTextureHandle, bool cube, WRAP_MODE mode);
static void SetTextureWrapT(GLuint glTextureHandle, bool cube, WRAP_MODE mode);
static void SetTextureWrapR(GLuint glTextureHandle, WRAP_MODE mode);
static GLenum GLFilterModeForFilterMode(FILTER_MODE mode);
static FILTER_MODE FilterModeForGLFilterMode(GLenum mode);
static GLenum GLWrapModeForWrapMode(WRAP_MODE mode);
static WRAP_MODE WrapModeForGLWrapMode(GLenum mode);
static void CheckGLError();

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

/***************************************************************************************
     Lifecycle
 ***************************************************************************************/

OpenGLRenderer::OpenGLRenderer():
	Renderer(),
	m_vertexDataIDMapping(map<VERTEX_DATA_ID, tuple<unsigned, unsigned, unsigned>>()),
	m_textureIDMapping(map<TEXTURE_ID, unsigned>()),
	m_vertexDataIDCounter(0),
	m_textureIDCounter(0),
	m_aabbVertexDataIDMapping(map<VERTEX_DATA_ID, pair<unsigned, unsigned>>()),
	m_aabbVertexDataIDCounter(0),
	m_glEnvironmentUBO(0),
	m_fonsContext(nullptr),
	m_fonsFont(-1) {

}

OpenGLRenderer::~OpenGLRenderer() {
	
}

/**************************************************************************************
     Internal
 **************************************************************************************/

bool OpenGLRenderer::initialize() {
	
	AE_LOG->trace("OpenGLRenderer::initialize()");
	
	// initialize GLEW
	// NOTE: OpenGL context must be setup first
	
	static bool glewInitialized = false;
	if (!glewInitialized) {
		glewExperimental = GL_TRUE;
		glewInit();
		
		const GLubyte *renderer = glGetString(GL_RENDERER);
		const GLubyte *version = glGetString(GL_VERSION);
		AE_LOG->info("Renderer: {}", renderer);
		AE_LOG->info("Version: {}", version);
		
		glewInitialized = true;
	}
	
	// initialize FontStash
	
	m_fonsContext = gl3fonsCreate(512, 512, FONS_ZERO_TOPLEFT);
	if (m_fonsContext == NULL) {
		//AE_LOG->error("Error creating Font Stash context.");
		throw Exception("Error creating Font Stash context.");
	}
	
	string fontName = "SourceCodePro-Semibold";
	string fontType = "otf";
	auto fontPath = FontPath(fontName, fontType);
	
	if (fontPath) {
		m_fonsFont = fonsAddFont(m_fonsContext, fontName.c_str(), fontPath->string().c_str());
		if (m_fonsFont == FONS_INVALID) {
			char errStr[1024];
			sprintf(errStr, "Could not load font: %s\n", fontPath->string().c_str());
			throw Exception(errStr);
		}
	}
	else {
		char errStr[1024];
		sprintf(errStr, "Could not find font: %s\n", fontPath->string().c_str());
		throw Exception(errStr);
	}

	return true;
}
	
void OpenGLRenderer::beginFrame(const RenderContext& context) {
	
	Renderer::beginFrame(context);
}

void OpenGLRenderer::endFrame(const RenderContext& context) {
	auto debugOptions = context.debugOptions();
	
	if (DEBUG_OPTIONS_CONTAINS(debugOptions, DEBUG_OPTIONS::SHOW_STATS_OVERLAY)) {
		UpdateStatsOverlay(Renderer::renderStats(),
						   context.sceneTime(),
						   *context.scene(),
						   m_fonsContext, m_fonsFont);
	}
	
	CheckGLError();
}

void OpenGLRenderer::render(Scene& scene,
							const DEBUG_OPTIONS& debugOptions,
							RenderStats& stats) {

	auto renderContext = scene.renderContext().lock();
	
	float framebufferWidth = renderContext->framebufferWidth();
	float framebufferHeight = renderContext->framebufferHeight();
	
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, framebufferWidth, framebufferHeight);

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	if (scene.background()) {
		if (dynamic_pointer_cast<CubeImage>(scene.background()->contents())) {
			auto skyboxGeometry = scene.skyboxGeometry();
			auto pointOfView = renderContext->pointOfView();
			
			RenderSkybox(*skyboxGeometry,
						 *pointOfView,
						 debugOptions,
						 stats,
						 m_vertexDataIDMapping, m_vertexDataIDCounter,
						 m_textureIDMapping, m_textureIDCounter);
		}
		else if (dynamic_pointer_cast<Color>(scene.background()->contents())) {
			auto color = dynamic_pointer_cast<Color>(scene.background()->contents());
			glClearColor(color->r, color->g, color->b, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		}
	}
	
	if (m_glEnvironmentUBO == 0) {
		uint32 ubo;
		glGenBuffers(1, &ubo);
		m_glEnvironmentUBO = ubo;
	}
	
	SendEnvironmentUniforms(m_glEnvironmentUBO, scene, stats);
	
	Program::Default()->bindUniformBlock("EnvironmentBlock", m_glEnvironmentUBO);
}

void OpenGLRenderer::render(Geometry& geometry,
							const mat4& modelMat,
							const mat4& viewMat,
							const mat4& projectionMat,
							const DEBUG_OPTIONS& debugOptions,
							RenderStats& stats) {
	
	if (DEBUG_OPTIONS_CONTAINS(debugOptions, DEBUG_OPTIONS::SHOW_BOUNDING_BOXES)) {
		RenderAABB(geometry,
				   modelMat, viewMat, projectionMat,
				   m_aabbVertexDataIDMapping,
				   m_aabbVertexDataIDCounter,
				   stats);
	}
}

void OpenGLRenderer::render(GeometryElement& element,
							Material& material,
							const mat4& modelMat,
							const mat4& viewMat,
							const mat4& projectionMat,
							const DEBUG_OPTIONS& debugOptions,
							RenderStats& stats) {
	
	shared_ptr<Program> program = nullptr;
	
	// check and load vertex data if necessary
	
	GLuint vbo, vao, ibo;
	GetGeometryElementGLVertexDataHandles(element,
										  m_vertexDataIDMapping, m_vertexDataIDCounter,
										  vbo, vao, ibo);

	if (DEBUG_OPTIONS_CONTAINS(debugOptions, DEBUG_OPTIONS::SHOW_WIREFRAMES)) {
		program = Program::Wireframe();
	}
	else {
		program = Program::Default();
		
		// and load material contents if necessary
		
		auto glTextureHandles = map<MATERIAL_PROPERTY_TYPE, GLuint>();
		GetMaterialGLTextureHandles(material,
									m_textureIDMapping, m_textureIDCounter,
									glTextureHandles);
		
		// send material and material property uniforms
		
		SendMaterialUniforms(material, *program, glTextureHandles, debugOptions);
		
		// update material property filtering options
		
		SetMaterialFilteringOptions(material, glTextureHandles);
	}
	
	// configure OpenGL state
	
	SetMaterialOpenGLState(material, debugOptions);
	
	// draw
	
	DrawGeometryElement(element, *program, modelMat, viewMat, projectionMat, vao, ibo);
	stats.polygons += element.faces().size();
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
	
/**************************************************************************************
     Static
 **************************************************************************************/

static void RenderSkybox(Geometry& skyboxGeometry,
						 const Node& pointOfView,
						 const DEBUG_OPTIONS& debugOptions,
						 RenderStats& stats,
						 map<VERTEX_DATA_ID, tuple<unsigned, unsigned, unsigned>>& vertexDataIDMapping,
						 VERTEX_DATA_ID& vertexDataIDCounter,
						 map<TEXTURE_ID, unsigned>& textureIDMapping,
						 TEXTURE_ID& textureIDCounter) {
	
	auto program = Program::Skybox();
	
	auto element = *(skyboxGeometry.elements().front());
	auto material = *(skyboxGeometry.materials().front());
	auto emissiveProperty = *(material.emissive());
	
	// check and load vertex data if necessary
	
	GLuint vbo, vao, ibo;
	GetSkyboxGLVertexDataHandles(skyboxGeometry,
								 vertexDataIDMapping, vertexDataIDCounter,
								 vbo, vao, ibo);
	
	// and load material contents if necessary
	
	auto glTextureHandles = map<MATERIAL_PROPERTY_TYPE, GLuint>();
	GetMaterialGLTextureHandles(material,
								textureIDMapping, textureIDCounter,
								glTextureHandles);
	auto emissiveGLTextureHandle = glTextureHandles[MATERIAL_PROPERTY_TYPE::EMISSIVE];
	
	// send material property uniforms
	
	SendMaterialPropertyUniforms(emissiveProperty,
								 MATERIAL_PROPERTY_TYPE::EMISSIVE,
								 emissiveGLTextureHandle,
								 debugOptions,
								 *program);
	
	// update material property filtering options
	
	SetMaterialPropertyFilteringOptions(emissiveProperty, emissiveGLTextureHandle);
	
	// configure OpenGL state
	
	SetSkyboxOpenGLState();
	
	// draw
	
	DrawSkyboxElement(element, *program, pointOfView, vao, ibo);
	
	stats.geometries++;
	stats.polygons += element.faces().size();
	stats.meshes++;
}
	
static void RenderAABB(Geometry& geometry,
					   mat4 modelMat,
					   mat4 viewMat,
					   mat4 projectionMat,
					   map<VERTEX_DATA_ID, pair<unsigned, unsigned>>& idMapping,
					   VERTEX_DATA_ID& idCounter,
					   RenderStats& stats) {
	GLuint vbo = 0;
	GLuint vao = 0;
	GetAABBGLVertexDataHandles(geometry,
							   idMapping, idCounter,
							   vbo, vao);
	
	SetAABBOpenGLState();
	
	DrawAABB(geometry, modelMat, viewMat, projectionMat, vbo, vao);
}
	
static void GetGeometryElementGLVertexDataHandles(GeometryElement& element,
												  map<VERTEX_DATA_ID, tuple<unsigned, unsigned, unsigned>>& idMapping,
												  VERTEX_DATA_ID& idCounter,
												  GLuint& glVBO, GLuint& glVAO, GLuint& glIBO) {
	
	// looks up and populates glVBO, glVAO, and glIBO, loading the vertex data if needed
	
	auto vertexDataID = element.vertexDataID();
	
	if (GEOMETRY_ELEMENT_DIRTY_BITS_CONTAINS(element.dirtyBits(),
											 GEOMETRY_ELEMENT_DIRTY_BITS::VERTEX_DATA)) {
		
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
	
static void GetAABBGLVertexDataHandles(Geometry& geometry,
									   map<VERTEX_DATA_ID, pair<unsigned, unsigned>>& idMapping,
									   VERTEX_DATA_ID& idCounter,
									   GLuint& glVBO, GLuint& glVAO) {
	
	// looks up and populates glVBO and glVAO, loading the vertex data if needed
	
	auto vertexDataID = geometry.aabbVertexDataID();
	
	if (GEOMETRY_DIRTY_BITS_CONTAINS(geometry.dirtyBits(),
									 GEOMETRY_DIRTY_BITS::EXTENT)) {
		
		LoadAABBVertexData(geometry, *Program::AABB(), glVBO, glVAO);
		
		idMapping[++idCounter] = make_pair(glVBO, glVAO);
		geometry.aabbVertexDataID(idCounter);
		
		geometry.dirtyBits(GEOMETRY_DIRTY_BITS_REMOVE(geometry.dirtyBits(),
													  GEOMETRY_DIRTY_BITS::EXTENT));
	}
	else {
		auto mapping = idMapping[vertexDataID];
		glVBO = get<0>(mapping);
		glVAO = get<1>(mapping);
	}
}
	
static void GetMaterialGLTextureHandles(Material& material,
										map<TEXTURE_ID, unsigned>& idMapping, TEXTURE_ID& idCounter,
										map<MATERIAL_PROPERTY_TYPE, GLuint>& glTextureHandles) {
	
	// looks up and populates glTextureHandle, loading the texture data if needed
	
	shared_ptr<MaterialProperty> properties[] = {material.ambient(),
		material.diffuse(), material.specular(), material.emissive()};
	
	MATERIAL_PROPERTY_TYPE types[] = {MATERIAL_PROPERTY_TYPE::AMBIENT, MATERIAL_PROPERTY_TYPE::DIFFUSE,
		MATERIAL_PROPERTY_TYPE::SPECULAR, MATERIAL_PROPERTY_TYPE::EMISSIVE};
	
	for (unsigned p = 0; p<4; ++p) {
		auto property = properties[p];
		
		if (property) {
			auto type = types[p];
			
			if (MATERIAL_PROPERTY_DIRTY_BITS_CONTAINS(property->dirtyBits(),
													  MATERIAL_PROPERTY_DIRTY_BITS::CONTENTS)) {
				
				GLuint textureID = 0;
				LoadMaterialPropertyTexture(*property, textureID);
				if (textureID > 0) {
					glTextureHandles[type] = textureID;
					
					idMapping[++idCounter] = textureID;
					property->textureID(idCounter);
				}
				
				property->dirtyBits(MATERIAL_PROPERTY_DIRTY_BITS_REMOVE(property->dirtyBits(),
																		MATERIAL_PROPERTY_DIRTY_BITS::CONTENTS));
			}
			else {
				if (property->textureID() > 0) {
					glTextureHandles[type] = idMapping[property->textureID()];
				}
			}
		}
	}
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

static void LoadAABBVertexData(Geometry& geometry,
							   const Program& program,
							   GLuint& glVBO, GLuint& glVAO) {
	
	map<string, vec3> bp = *(geometry.boundingPoints(false));
	
	float xMin = bp["xMin"].x;
	float xMax = bp["xMax"].x;
	float yMin = bp["yMin"].y;
	float yMax = bp["yMax"].y;
	float zMin = bp["zMin"].z;
	float zMax = bp["zMax"].z;
	
	vec3 one =      vec3(xMin, yMax, zMin);
	vec3 two =      vec3(xMin, yMax, zMax);
	vec3 three =    vec3(xMax, yMax, zMax);
	vec3 four =     vec3(xMax, yMax, zMin);
	vec3 five =     vec3(xMin, yMin, zMin);
	vec3 six =      vec3(xMin, yMin, zMax);
	vec3 seven =    vec3(xMax, yMin, zMax);
	vec3 eight =    vec3(xMax, yMin, zMin);
	
	vec3 verts[] = {
		one, 	two,
		two, 	three,
		three,	four,
		four, 	one,
		five, 	six,
		six, 	seven,
		seven, 	eight,
		eight, 	five,
		one, 	five,
		two, 	six,
		three, 	seven,
		four, 	eight};
	
	glGenBuffers(1, &glVBO);
	glBindBuffer(GL_ARRAY_BUFFER, glVBO);
	glBufferData(GL_ARRAY_BUFFER, 24 * sizeof(vec3), &(verts[0]), GL_STATIC_DRAW);
	
	glGenVertexArrays(1, &glVAO);
	glBindVertexArray(glVAO);
	
	glBindBuffer(GL_ARRAY_BUFFER, glVBO);
	
	GLuint positionIndex = program.getAttributeLocation("vertex_position");
	glVertexAttribPointer(positionIndex, 	// attrib index
						  3, 				// num components per attrib (3 float in vec3)
						  GL_FLOAT, 		// component type
						  GL_FALSE, 		// normalize
						  sizeof(vec3), 	// stride
						  0); 				// start offset
	glEnableVertexAttribArray(positionIndex);
}
	
static void LoadMaterialPropertyTexture(const MaterialProperty& materialProperty, GLuint& glTextureHandle) {
	
	if (dynamic_pointer_cast<CubeImage>(materialProperty.contents())) {
		AE_LOG->info("Buffering cube texture...");
		
		auto cubeImage = dynamic_pointer_cast<CubeImage>(materialProperty.contents());
		
		Image images[] = {
			*(cubeImage->posX()),
			*(cubeImage->negX()),
			*(cubeImage->posY()),
			*(cubeImage->negY()),
			*(cubeImage->posZ()),
			*(cubeImage->negZ()) };
		
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
			Image image = images[s];
			
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
	else if (dynamic_pointer_cast<Image>(materialProperty.contents())) {
		AE_LOG->info("Buffering 2D texture...");
		
		auto image = dynamic_pointer_cast<Image>(materialProperty.contents());
		
		glGenTextures(1, &glTextureHandle);
		glBindTexture(GL_TEXTURE_2D, glTextureHandle);
		
		glTexImage2D(GL_TEXTURE_2D,
					 0,
					 GL_RGBA,//GL_SRGB_ALPHA,//GL_RGBA,
					 image->width(),
					 image->height(),
					 0,
					 GL_RGBA,
					 GL_UNSIGNED_BYTE,
					 image->data());
		
		SetTextureMinificationFilter(glTextureHandle, false, materialProperty.minificationFilter());
		SetTextureMagnificationFilter(glTextureHandle, false, materialProperty.magnificationFilter());
		SetTextureMaxAnisotropy(glTextureHandle, false, materialProperty.maxAnisotropy());
		SetTextureWrapS(glTextureHandle, false, materialProperty.wrapS());
		SetTextureWrapT(glTextureHandle, false, materialProperty.wrapT());
		
		AE_LOG->info("Done.");
	}
}
	
static void SendMaterialUniforms(const Material& material,
								 Program& program,
								 map<MATERIAL_PROPERTY_TYPE, GLuint>& glTextureHandles,
								 const DEBUG_OPTIONS& debugOptions) {
	
	// sends uniforms for the Material, and and MaterialProperties it has
	
	program.use();
	
	program.setUniform("specularExponent", material.specularExponent());
	program.setUniform("uvScale", material.uvScale());
	program.setUniform("locksAmbientWithDiffuse", material.locksAmbientWithDiffuse());
	program.setUniform("emissiveMode", 0); // 0 = MaterialMode_None -- why is this here?
	
	shared_ptr<MaterialProperty> properties[] = {material.ambient(),
		material.diffuse(), material.specular(), material.emissive()};
	
	MATERIAL_PROPERTY_TYPE types[] = {MATERIAL_PROPERTY_TYPE::AMBIENT, MATERIAL_PROPERTY_TYPE::DIFFUSE,
		MATERIAL_PROPERTY_TYPE::SPECULAR, MATERIAL_PROPERTY_TYPE::EMISSIVE};
	
	for (unsigned p = 0; p<4; ++p) {
		auto property = properties[p];
		
		if (property) {
			auto type = types[p];
			
			SendMaterialPropertyUniforms(*property,
										 type,
										 glTextureHandles[type],
										 debugOptions,
										 program);
		}
	}
	
	//program.unuse();
}
	
static void SendMaterialPropertyUniforms(MaterialProperty& property,
										 MATERIAL_PROPERTY_TYPE type,
										 GLuint glTextureHandle,
										 const DEBUG_OPTIONS& debugOptions,
										 Program& program) {
	
	program.use();
	
	if (dynamic_pointer_cast<CubeImage>(property.contents())) { // cubemap
		program.bindTexture("cubeSampler", GL_TEXTURE_CUBE_MAP, GL_TEXTURE0, glTextureHandle, 0);
	}
	else if (dynamic_pointer_cast<Image>(property.contents())) { // 2d texture
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
		auto color = dynamic_pointer_cast<Color>(property.contents());
		
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
		program.setUniform(colorUniformName.c_str(), color->r, color->g, color->b);
	}
	
	//program.unuse();
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
	
static void SetMaterialPropertyFilteringOptions(MaterialProperty& property,
												GLuint glTextureHandle) {
	
	bool cube = dynamic_pointer_cast<CubeImage>(property.contents()) != nullptr;
	
	if (MATERIAL_PROPERTY_DIRTY_BITS_CONTAINS(property.dirtyBits(),
											  MATERIAL_PROPERTY_DIRTY_BITS::MINIFICATION_FILTER)) {
		SetTextureMinificationFilter(glTextureHandle, cube, property.minificationFilter());
		property.dirtyBits(MATERIAL_PROPERTY_DIRTY_BITS_REMOVE(property.dirtyBits(),
															   MATERIAL_PROPERTY_DIRTY_BITS::MINIFICATION_FILTER));
	}
	
	if (MATERIAL_PROPERTY_DIRTY_BITS_CONTAINS(property.dirtyBits(),
											  MATERIAL_PROPERTY_DIRTY_BITS::MAGNIFICATION_FILTER)) {
		SetTextureMagnificationFilter(glTextureHandle, cube, property.magnificationFilter());
		property.dirtyBits(MATERIAL_PROPERTY_DIRTY_BITS_REMOVE(property.dirtyBits(),
															   MATERIAL_PROPERTY_DIRTY_BITS::MAGNIFICATION_FILTER));
	}
	
	if (MATERIAL_PROPERTY_DIRTY_BITS_CONTAINS(property.dirtyBits(),
											  MATERIAL_PROPERTY_DIRTY_BITS::WRAP_S)) {
		SetTextureWrapS(glTextureHandle, cube, property.wrapS());
		property.dirtyBits(MATERIAL_PROPERTY_DIRTY_BITS_REMOVE(property.dirtyBits(),
															   MATERIAL_PROPERTY_DIRTY_BITS::WRAP_S));
	}
	
	if (MATERIAL_PROPERTY_DIRTY_BITS_CONTAINS(property.dirtyBits(),
											  MATERIAL_PROPERTY_DIRTY_BITS::WRAP_T)) {
		SetTextureWrapT(glTextureHandle, cube, property.wrapT());
		property.dirtyBits(MATERIAL_PROPERTY_DIRTY_BITS_REMOVE(property.dirtyBits(),
															   MATERIAL_PROPERTY_DIRTY_BITS::WRAP_T));
	}
	
	if (cube) {
		if (MATERIAL_PROPERTY_DIRTY_BITS_CONTAINS(property.dirtyBits(),
												  MATERIAL_PROPERTY_DIRTY_BITS::WRAP_R)) {
			SetTextureWrapR(glTextureHandle, property.wrapR());
			property.dirtyBits(MATERIAL_PROPERTY_DIRTY_BITS_REMOVE(property.dirtyBits(),
																   MATERIAL_PROPERTY_DIRTY_BITS::WRAP_R));
		}
	}
	
	if (MATERIAL_PROPERTY_DIRTY_BITS_CONTAINS(property.dirtyBits(),
											  MATERIAL_PROPERTY_DIRTY_BITS::MAX_ANISTROPY)) {
		SetTextureMaxAnisotropy(glTextureHandle, cube, property.maxAnisotropy());
		property.dirtyBits(MATERIAL_PROPERTY_DIRTY_BITS_REMOVE(property.dirtyBits(),
															   MATERIAL_PROPERTY_DIRTY_BITS::MAX_ANISTROPY));
	}
}
	
static void SetMaterialFilteringOptions(const Material& material,
										map<MATERIAL_PROPERTY_TYPE, GLuint>& glTextureHandles) {
	
	shared_ptr<MaterialProperty> properties[] = {material.ambient(),
		material.diffuse(), material.specular(), material.emissive()};
	
	MATERIAL_PROPERTY_TYPE types[] = {MATERIAL_PROPERTY_TYPE::AMBIENT, MATERIAL_PROPERTY_TYPE::DIFFUSE,
		MATERIAL_PROPERTY_TYPE::SPECULAR, MATERIAL_PROPERTY_TYPE::EMISSIVE};
	
	for (unsigned p = 0; p<4; ++p) {
		auto property = properties[p];
		
		if (property) {
			auto type = types[p];
			
			SetMaterialPropertyFilteringOptions(*property, glTextureHandles[type]);
		}
	}
}
	
static void SetMaterialOpenGLState(const Material& material, 
								   const DEBUG_OPTIONS& debugOptions) {
	
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glDepthMask(GL_TRUE);
	
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
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
		glEnable(GL_LINE_SMOOTH);
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
	
static void SetSkyboxOpenGLState() {
	
	glDepthMask(GL_FALSE);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glDisable(GL_CULL_FACE);
}

static void SetAABBOpenGLState() {
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glDepthMask(GL_TRUE);
	glEnable(GL_LINE_SMOOTH);
}

static void DrawGeometryElement(GeometryElement& element,
								Program& program,
								mat4 modelMat, mat4 viewMat, mat4 projectionMat,
								GLuint vao, GLuint ibo) {
	
	program.use();
	
	// uniforms
	
	program.setUniform("model", modelMat);
	program.setUniform("view", inverse(viewMat));
	program.setUniform("projection", projectionMat);
	
	// draw
	
	glBindVertexArray(vao);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	unsigned int numFaces = element.faces().size();
	glDrawElements(GL_TRIANGLES, numFaces * 3, GL_UNSIGNED_INT, (void*)0);
}

static void DrawSkyboxElement(GeometryElement& element,
							  Program& program,
							  const Node& pointOfView,
							  GLuint vao, GLuint ibo) {
	
	program.use();
	
	mat4 viewMat = lookAt(vec3(0.0f, 0.0f, 0.0f), // eye - location
						  pointOfView.worldForward(), // center - look at
						  pointOfView.worldUp()); // up
	
	auto projectionMat = pointOfView.camera()->projection();
	
	program.setUniform("view", viewMat);
	program.setUniform("projection", projectionMat);
	
	// draw
	
	auto faces = element.faces();
	
	glBindVertexArray(vao);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	unsigned int numFaces = faces.size();
	glDrawElements(GL_TRIANGLES, numFaces * sizeof(Face), GL_UNSIGNED_INT, (void*)0);
}
	
static void DrawAABB(Geometry& geometry,
					 mat4 modelMat,
					 mat4 viewMat,
					 mat4 projectionMat,
					 GLuint glVBO, GLuint glVAO) {
	
	auto program = Program::AABB();
	program->use();
	
	// uniforms
	
	program->setUniform("model", modelMat);
	program->setUniform("view", inverse(viewMat));
	program->setUniform("projection", projectionMat);
	
	// draw
	
	glBindVertexArray(glVAO);
	glDrawArrays(GL_LINES, 0, 24);
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

static void UpdateStatsOverlay(RenderStats& stats, float time, Scene& scene,
							   FONScontext* fonsContext, int fonsFont) {
	
	auto renderContext = scene.renderContext().lock();
	
	float framebufferWidth = renderContext->framebufferWidth();
	float framebufferHeight = renderContext->framebufferHeight();
	float framebufferScale = renderContext->framebufferScale();
	
	static float fps = 0.0;
	static float ms = 0.0;
	static float percent = 0.0;
	
	const float GOAL_TIME = 16.6666667f;
	
	static unsigned elapsedFrames = 0; ++elapsedFrames;
	static float previousSeconds = time;
	float currentSeconds = time;
	float elapsedSeconds = currentSeconds - previousSeconds;
	
	if (elapsedSeconds > 0.5) {
		// only update the framerate stats every so often so they're readable
		
		ms = ((elapsedSeconds*1000.0) / elapsedFrames);
		fps = elapsedFrames/elapsedSeconds;
		percent = (ms / GOAL_TIME) * 100.0f;
		
		// reset framerate stats
		previousSeconds = currentSeconds;
		elapsedFrames = 0;
	}
	
	gl3fonsProjectionSize(fonsContext, framebufferWidth, framebufferHeight);
	
	glDisable(GL_DEPTH_TEST);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	float dx = 12.0 * framebufferScale;
	float dy = 20.0 * framebufferScale;
	
	fonsClearState(fonsContext);
	
	fonsSetFont(fonsContext, fonsFont);
	
	static float textSize = 14.0 * framebufferScale;
	static float hPadding = 0.0 * framebufferScale;
	
	char tmpStr[256];
	
	sprintf(tmpStr, "%-14s %.1f%s", "framerate", fps, (renderContext->vSyncEnabled() ? " [vsync]" : ""));
	DrawString(tmpStr, textSize, dx, dy, fonsContext);
	dy += (textSize + hPadding);
	
	sprintf(tmpStr, "%-14s %.1f", "frametime", ms);
	DrawString(tmpStr, textSize, dx, dy, fonsContext);
	dy += (textSize + hPadding);
	
	sprintf(tmpStr, "%-14s %.1f", "percent", percent);
	DrawString(tmpStr, textSize, dx, dy, fonsContext);
	dy += (textSize + hPadding);
	
	dy += textSize; // skip a line
	
	sprintf(tmpStr, "%-14s %d", "nodes", stats.nodes);
	DrawString(tmpStr, textSize, dx, dy, fonsContext);
	dy += (textSize + hPadding);
	
	sprintf(tmpStr, "%-14s %d", "geometries", stats.geometries);
	DrawString(tmpStr, textSize, dx, dy, fonsContext);
	dy += (textSize + hPadding);
	
	sprintf(tmpStr, "%-14s %d", "meshes", stats.meshes);
	DrawString(tmpStr, textSize, dx, dy, fonsContext);
	dy += (textSize + hPadding);
	
	sprintf(tmpStr, "%-14s %d", "polygons", stats.polygons);
	DrawString(tmpStr, textSize, dx, dy, fonsContext);
	dy += (textSize + hPadding);
	
	sprintf(tmpStr, "%-14s %d", "lights", stats.lights);
	DrawString(tmpStr, textSize, dx, dy, fonsContext);
	dy += (textSize + hPadding);
	
	dy += textSize; // skip a line
	
	sprintf(tmpStr, "%-14s %.1f, %.1f, %.1f", "camera pos",
			stats.cameraPosition.x, stats.cameraPosition.y, stats.cameraPosition.z);
	DrawString(tmpStr, textSize, dx, dy, fonsContext);
	dy += (textSize + hPadding);
	
	if (renderContext->recordingGIF()) {
		dy += textSize; // skip a line
		
		sprintf(tmpStr, "%-14s %d" , "RECORDING", renderContext->recordedGIFFrames());
		DrawString(tmpStr, textSize, dx, dy, fonsContext);
		dy += (textSize + hPadding);
	}
}
	
static float DrawString(string string, float size, float dx, float dy,
						FONScontext* fonsContext) {
	// must setup fons GL state first
	
	static unsigned black = gl3fonsRGBA(0, 0, 0, 255);
	static unsigned white = gl3fonsRGBA(255, 255, 255, 255);
	
	fonsSetSize(fonsContext, size);
	
	fonsSetColor(fonsContext, black);
	fonsSetBlur(fonsContext, 1);
	fonsDrawText(fonsContext, dx, dy, string.c_str(), NULL);
	
	fonsSetColor(fonsContext, white);
	fonsSetBlur(fonsContext, 0);
	return fonsDrawText(fonsContext, dx, dy, string.c_str(), NULL);
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

static void CheckGLError() {
	GLenum err = glGetError();
	if (err != GL_NO_ERROR) {
		AE_LOG->warn("*** GL error: 0x{:X} ***", err);
	}
}
