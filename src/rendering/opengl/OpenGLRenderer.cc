//
//  OpenGLRenderer.cc
//	avara-engine
//
//  Created by Morgan Davis on 4/24/18.
//  Copyright © 2018 Morgan K Davis. All rights reserved.
//

#include "rendering/opengl/OpenGLRenderer.h"

#include <algorithm>
#include <iostream>
#include <locale.h>
#include <set>
#include <vector>

#ifdef OPENGL_ES
#include <EGL/egl.h>
#include <GLES3/gl3.h>
#else
#include "GL/glew.h"
#endif

#ifdef OPENGL_CORE
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "magic_enum.hpp"
#endif

#include "Global.h"
#include "diagnostic/logging/Logger.h"
#include "geometry/Geometry.h"
#include "geometry/GeometryElement.h"
#include "geometry/Line.h"
#include "geometry/Point.h"
#include "rendering/Light.h"
#include "rendering/VisualWorld.h"
#include "rendering/camera/Camera.h"
#include "rendering/context/RenderContext.h"
#include "rendering/context/platform/desktop/Window.h"
#include "rendering/materials/Material.h"
#include "rendering/materials/MaterialProperty.h"
#include "rendering/opengl/Program.h"
#include "scene/Node.h"
#include "scene/Scene.h"
#include "utilities/Buffer.h"
#include "utilities/Color.h"
#include "utilities/CubeImage.h"
#include "utilities/Font.h"
#include "utilities/Image.h"
#include "utilities/Utilities.h"


using namespace ae;
using namespace ae::utils;
using namespace glm;
using namespace std;


//#define DISABLE_RESOURCE_MANAGEMENT


/*********************************************************************************************
	Static Prototypes
 *********************************************************************************************/

static void 		RenderSkybox(shared_ptr<Geometry> skyboxGeometry,
								Node& pointOfView,
								const DEBUG_OPTIONS& debugOptions,
								Stats& stats,
								OpenGLRenderer::GeometryElementGLMapping& elementGLMapping,
								OpenGLRenderer::MaterialPropertyGLMapping& materialGLMapping,
								unordered_set<shared_ptr<MaterialProperty>>& activeProperties);
static void 		GetGeometryElementGLVertexDataHandles(shared_ptr<GeometryElement> element,
														 OpenGLRenderer::GeometryElementGLMapping& glMapping,
														 GLuint& glVBO, GLuint& glVAO, GLuint& glIBO);
static void 		GetSkyboxGLVertexDataHandles(shared_ptr<Geometry> skyboxGeometry,
												OpenGLRenderer::GeometryElementGLMapping& glMapping,
												GLuint& glVBO, GLuint& glVAO, GLuint& glIBO);
static void 		GetGeometryAABBLineSetVertexDataHandles(shared_ptr<Geometry> geometry,
														   OpenGLRenderer::GeometryAABBLineSetMapping& aabbLineSetMapping,
														   OpenGLRenderer::LineSetGLMapping lineSetGLMapping,
														   GLuint& glVBO, GLuint& glVAO);
static void 		GetLineSetVertexDataHandles(shared_ptr<LineSet> lineSet,
											   Program& program,
											   OpenGLRenderer::LineSetGLMapping& glMapping,
											   GLuint& glVBO, GLuint& glVAO);
static void 		GetPointSetVertexDataHandles(shared_ptr<PointSet> pointSet,
												Program& program,
												OpenGLRenderer::PointSetGLMapping& glMapping,
												GLuint& glVBO, GLuint& glVAO);
static void 		GetMaterialGLTextureHandles(Material& material,
											   OpenGLRenderer::MaterialPropertyGLMapping& glMapping,
											   unordered_set<shared_ptr<MaterialProperty>>& activeProperties,
											   map<MATERIAL_PROPERTY_TYPE, GLuint>& glTextureHandles);
static void 		BufferGeometryElementVertexData(const GeometryElement& element,
												   Program& program,
												   GLuint& glVBO, GLuint& glVAO, GLuint& glIBO);
static void 		BufferSkyboxVertexData(Geometry& skyboxGeometry,
										  Program& program,
										  GLuint& glVBO, GLuint& glVAO, GLuint& glIBO);
static void 		BufferLineSetVertexData(LineSet& lineSet,
										   Program& program,
										   GLuint& glVBO, GLuint& glVAO);
static void 		BufferPointSetVertexData(PointSet& pointSet,
											Program& program,
											GLuint& glVBO, GLuint& glVAO);
static void 		BufferMaterialPropertyTexture(const MaterialProperty& property,
												 MATERIAL_PROPERTY_TYPE type,
												 GLuint& glTextureHandle);
static void 		SendMaterialUniforms(const Material& material,
										Program& program,
										map<MATERIAL_PROPERTY_TYPE, GLuint>& glTextureHandles,
										const DEBUG_OPTIONS& debugOptions);
static void 		SendMaterialPropertyUniforms(MaterialProperty& property,
												MATERIAL_PROPERTY_TYPE type,
												GLuint glTextureHandle,
												const DEBUG_OPTIONS& debugOptions,
												Program& program);
static void 		SendEnvironmentUniforms(GLuint glEnvironmentUBO, const Scene& scene, Stats& stats);
static void 		SetMaterialPropertyFilteringOptions(MaterialProperty& property,
													   GLuint glTextureHandle);
static void 		SetMaterialFilteringOptions(const Material& material,
											   map<MATERIAL_PROPERTY_TYPE, GLuint>& glTextureHandles);
static void 		SetMaterialOpenGLState(const Material& material,
										  const DEBUG_OPTIONS& debugOptions);
static void	 		SetSkyboxOpenGLState();
static void 		SetLineSetGLState();
static void 		SetPointSetGLState();
static void 		DrawGeometryElement(GeometryElement& element,
									   Program& program,
									   mat4 modelMat, mat4 viewMat, mat4 projectionMat,
									   GLuint vao, GLuint ibo);
static void 		DrawSkyboxElement(GeometryElement& element,
									 Program& program,
									 Node& pointOfView,
									 GLuint vao, GLuint ibo);
static void 		DrawLineSet(LineSet& lineSet,
							   Program& program,
							   mat4 modelMat,
							   mat4 viewMat,
							   mat4 projectionMat,
							   GLuint glVBO, GLuint glVAO);
static void 		DrawPointSet(PointSet& pointSet,
								Program& program,
								mat4 modelMat,
								mat4 viewMat,
								mat4 projectionMat,
								GLuint glVBO, GLuint glVAO);
static void 		CleanupGeometryElementResources(unordered_set<shared_ptr<GeometryElement>>& active,
												   OpenGLRenderer::GeometryElementGLMapping& glMapping);
static void 		CleanupMaterialPropertyResources(unordered_set<shared_ptr<MaterialProperty>>& active,
													OpenGLRenderer::MaterialPropertyGLMapping& glMapping);
static void 		CleanupLineSetResources(unordered_set<shared_ptr<LineSet>>& active,
										   OpenGLRenderer::LineSetGLMapping& glMapping);
static void 		CleanupPointSetResources(unordered_set<shared_ptr<PointSet>>& active,
											OpenGLRenderer::PointSetGLMapping& glMapping);
static void 		DeleteGeometryElementGLResources(shared_ptr<GeometryElement> element,
													OpenGLRenderer::GeometryElementGLMapping& glMapping);
static void 		DeleteMaterialPropertyGLResources(shared_ptr<MaterialProperty> property,
													 OpenGLRenderer::MaterialPropertyGLMapping& glMapping);
static void 		DeleteLineSetGLResources(shared_ptr<LineSet> lineSet,
											OpenGLRenderer::LineSetGLMapping& glMapping);
static void 		DeletePointSetGLResources(shared_ptr<PointSet> pointSet,
											 OpenGLRenderer::PointSetGLMapping& glMapping);
static vector<shared_ptr<Node>> 	SortedLights(map<shared_ptr<Node>, float> lights);
static void 		DrawStatsOverlay(Stats& stats, float time, Scene& scene);
static void 		SetTextureMinificationFilter(GLuint glTextureHandle, bool cube, FILTER_MODE mode);
static void 		SetTextureMagnificationFilter(GLuint glTextureHandle, bool cube, FILTER_MODE mode);
static void 		SetTextureMaxAnisotropy(GLuint glTextureHandle, bool cube, float max);
static void 		SetTextureWrapS(GLuint glTextureHandle, bool cube, WRAP_MODE mode);
static void 		SetTextureWrapT(GLuint glTextureHandle, bool cube, WRAP_MODE mode);
static void 		SetTextureWrapR(GLuint glTextureHandle, WRAP_MODE mode);
static GLenum 		GLFilterModeForFilterMode(FILTER_MODE mode);
static FILTER_MODE 	FilterModeForGLFilterMode(GLenum mode);
static GLenum 		GLWrapModeForWrapMode(WRAP_MODE mode);
static WRAP_MODE 	WrapModeForGLWrapMode(GLenum mode);
static void 		CheckGLError();

/*********************************************************************************************
	Types
 *********************************************************************************************/

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
	/* vec3 	direction_world;
	float 		attenuationStart;
	float 		attenuationEnd;
	float 		attenuationExponent;
	float 		innerAngle;
	float 		outerAngle; */
} LightGLSLStruct;

typedef struct {
	float32_t 		startDistance;
	float32_t 		endDistance;
	float32_t 		densityExponent;
	float32_t 		PADDING1;
	vec4 			color;
	/* float32_t 	PADDING2; */
} FogGLSLStruct;

/*********************************************************************************************
	Lifecycle
 *********************************************************************************************/

OpenGLRenderer::OpenGLRenderer():
		Renderer(),
		_geometryElementGLMapping(GeometryElementGLMapping()),
		_materialPropertyGLMapping(MaterialPropertyGLMapping()),
		_lineSetGLMapping(LineSetGLMapping()),
		_pointSetGLMapping(PointSetGLMapping()),
		_activeGeometryElements(unordered_set<shared_ptr<GeometryElement>>()),
		_activeMaterialProperties(unordered_set<shared_ptr<MaterialProperty>>()),
		_activeLineSets(unordered_set<shared_ptr<LineSet>>()),
		_activePointSets(unordered_set<shared_ptr<PointSet>>()),
		_glEnvironmentUBO(0),
		_overlayFont(nullptr) { }

OpenGLRenderer::~OpenGLRenderer() {
	AE_LOG_D("Destroying OpenGLRenderer {:p}", (void*)this);
	
	_activeGeometryElements.clear();
	_activeMaterialProperties.clear();
	_activeLineSets.clear();
	_activePointSets.clear();
	
	glDeleteBuffers(1, &_glEnvironmentUBO);
	
	CleanupGeometryElementResources(_activeGeometryElements, _geometryElementGLMapping);
	CleanupMaterialPropertyResources(_activeMaterialProperties, _materialPropertyGLMapping);
	CleanupLineSetResources(_activeLineSets, _lineSetGLMapping);
	CleanupPointSetResources(_activePointSets, _pointSetGLMapping);
	
#ifdef OPENGL_CORE
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
#endif
}
	
/*********************************************************************************************
	Internal
 *********************************************************************************************/

bool OpenGLRenderer::initialize(const RenderContext& context) {
	
	AE_LOG_T("");
	
	// create environment UBO
	
	uint32 ubo;
	glGenBuffers(1, &ubo);
	_glEnvironmentUBO = ubo;

#ifdef OPENGL_CORE
	
	// setup Imgui for stats overlay
	
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.IniFilename = nullptr;

	GLFWwindow* glfwWindow = dynamic_cast<const Window*>(&context)->glfwWindow();
	
	ImGui_ImplGlfw_InitForOpenGL(glfwWindow, true);
	ImGui_ImplOpenGL3_Init();

	string fontName = "SourceCodePro-Semibold";
	string fontType = "otf";
	float fontSize = 14.0;
	
	_overlayFont = FontNamed(fontName, fontType);

	if (_overlayFont->buffer()->size()) {
		
		// by default Imgui transferrs font memory ownership to itself
		// this means Imgui eventually frees the font data, and then the Font/Buffer double-free it
		
		ImFontConfig config;
		config.FontDataOwnedByAtlas = false;
		
		ImFont* scp = io.Fonts->AddFontFromMemoryTTF(_overlayFont->buffer()->pointer(),
													 _overlayFont->buffer()->size(),
													 fontSize,
													 &config);

		if (!scp) {
			throw Exception("Unable to load font: " + fontName + "." + fontType);
		}
	}

#endif // OPENGL_CORE
	
	return true;
}
	
void OpenGLRenderer::beginFrame(const Scene& scene,
								const RenderContext& context,
								const DEBUG_OPTIONS& debugOptions,
								Stats& stats) {
	Renderer::beginFrame(scene, context, debugOptions, stats);

	_activeGeometryElements.clear();
	_activeMaterialProperties.clear();
	_activeLineSets.clear();
	_activePointSets.clear();
}

void OpenGLRenderer::endFrame(const Scene& scene,
							  const RenderContext& context,
							  const DEBUG_OPTIONS& debugOptions,
							  Stats& stats) {
	Renderer::endFrame(scene, context, debugOptions, stats);

	if (DEBUG_OPTIONS_CONTAINS(debugOptions, DEBUG_OPTIONS::SHOW_STATS_OVERLAY)) {
		auto scene = context.visualWorld()->scene();
		DrawStatsOverlay(stats,
						 scene->time(),
						 *scene);
	}
	
	CleanupGeometryElementResources(_activeGeometryElements, _geometryElementGLMapping);
	CleanupMaterialPropertyResources(_activeMaterialProperties, _materialPropertyGLMapping);
	CleanupLineSetResources(_activeLineSets, _lineSetGLMapping);
	CleanupPointSetResources(_activePointSets, _pointSetGLMapping);

	CheckGLError();
}

void OpenGLRenderer::render(Scene& scene,
							const DEBUG_OPTIONS& debugOptions,
							Stats& stats) {

	auto renderContext = scene.visualWorld()->renderContext();
	
	float framebufferWidth = renderContext->framebufferWidth();
	float framebufferHeight = renderContext->framebufferHeight();
	
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, framebufferWidth, framebufferHeight);

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	if (scene.visualWorld()->background()) {
		if (dynamic_pointer_cast<CubeImage>(scene.visualWorld()->background()->contents())) {
			auto skyboxGeometry = scene.visualWorld()->skyboxGeometry();
			auto pointOfView = scene.visualWorld()->pointOfView();

			RenderSkybox(skyboxGeometry,
						 *pointOfView,
						 debugOptions,
						 stats,
						 _geometryElementGLMapping,
						 _materialPropertyGLMapping,
						 _activeMaterialProperties);

			// save reference for housekeeping
			_activeGeometryElements.emplace(skyboxGeometry->elements().front());
		}
		else if (dynamic_pointer_cast<Color>(scene.visualWorld()->background()->contents())) {
			auto color = dynamic_pointer_cast<Color>(scene.visualWorld()->background()->contents());
			glClearColor(color->r, color->g, color->b, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		}
	}
	
	SendEnvironmentUniforms(_glEnvironmentUBO, scene, stats);
	
	Program::Default()->bindUniformBlock("EnvironmentBlock", _glEnvironmentUBO);
}

void OpenGLRenderer::render(shared_ptr<Geometry> geometry,
							const mat4& modelMat,
							const mat4& viewMat,
							const mat4& projectionMat,
							const DEBUG_OPTIONS& debugOptions,
							Stats& stats) {

	if (DEBUG_OPTIONS_CONTAINS(debugOptions, DEBUG_OPTIONS::SHOW_BOUNDING_BOXES)) {

		// will check dirty bit, and create an AABB lineset if necessary,
		// and insert into _geometryAABBLineSetMapping
		
		GLuint vbo, vao;
		GetGeometryAABBLineSetVertexDataHandles(geometry,
												_geometryAABBLineSetMapping,
												_lineSetGLMapping,
												vbo, vao);

		render(_geometryAABBLineSetMapping[geometry],
			   modelMat, viewMat, projectionMat);
		
	}
	else {
		// if there was an AABB lineset, just remove it
		_geometryAABBLineSetMapping.erase(geometry);
	}
}

void OpenGLRenderer::render(shared_ptr<GeometryElement> element,
							Material& material,
							const mat4& modelMat,
							const mat4& viewMat,
							const mat4& projectionMat,
							const DEBUG_OPTIONS& debugOptions,
							Stats& stats) {

	shared_ptr<Program> program = nullptr;

	// check and load vertex data if necessary

	GLuint vbo, vao, ibo;
	GetGeometryElementGLVertexDataHandles(element,
										  _geometryElementGLMapping,
										  vbo, vao, ibo);
	
	bool wireframe = DEBUG_OPTIONS_CONTAINS(debugOptions, DEBUG_OPTIONS::SHOW_WIREFRAMES);

	if (wireframe) {
		program = Program::Wireframe();
	}
	else {
		program = Program::Default();
	}
	
	// and load material contents if necessary

	auto glTextureHandles = map<MATERIAL_PROPERTY_TYPE, GLuint>();
	GetMaterialGLTextureHandles(material,
								_materialPropertyGLMapping,
								_activeMaterialProperties,
								glTextureHandles);

	if (!wireframe) {
		// send material and material property uniforms
		SendMaterialUniforms(material, *program, glTextureHandles, debugOptions);
		
		// update material property filtering options
		SetMaterialFilteringOptions(material, glTextureHandles);
	}
	
	// configure OpenGL state
	SetMaterialOpenGLState(material, debugOptions);

	// update

	DrawGeometryElement(*element, *program, modelMat, viewMat, projectionMat, vao, ibo);
	//stats.polygons += element->faces().size();

	// save reference for housekeeping
	_activeGeometryElements.emplace(element);
}
	
void OpenGLRenderer::render(shared_ptr<LineSet> lines,
							const glm::mat4& modelMat,
							const glm::mat4& viewMat,
							const glm::mat4& projectionMat) {
	
	shared_ptr<Program> program = Program::Lines();
	
	// check and load vertex data if necessary
	
	GLuint vbo, vao;
	GetLineSetVertexDataHandles(lines,
								*program,
								_lineSetGLMapping,
								vbo, vao);

	// configure OpenGL state
	
	SetLineSetGLState();
	
	// update
		
	DrawLineSet(*lines,
				*program,
				modelMat, viewMat, projectionMat,
				vbo, vao);
	//stats.polygons += element->faces().size();
	
	// save reference for housekeeping
	_activeLineSets.emplace(lines);
}

void OpenGLRenderer::render(shared_ptr<PointSet> points,
							const glm::mat4& modelMat,
							const glm::mat4& viewMat,
							const glm::mat4& projectionMat) {
	
}

shared_ptr<Image> OpenGLRenderer::snapshot(const RenderContext& context) const {
	
	unsigned framebufferWidth = context.framebufferWidth();
	unsigned framebufferHeight = context.framebufferHeight();
	unsigned char pixelBuf[framebufferWidth * framebufferHeight * 4];
	glReadPixels(0, 0, framebufferWidth, framebufferHeight, GL_RGBA, GL_UNSIGNED_BYTE, pixelBuf);
	auto buffer = make_shared<Buffer>((const unsigned char*)pixelBuf, framebufferWidth * framebufferHeight * 4);
	return make_shared<Image>(buffer, framebufferWidth, framebufferHeight, 4);
}
	
/*********************************************************************************************
	Static
 *********************************************************************************************/

static void RenderSkybox(shared_ptr<Geometry> skyboxGeometry,
						 Node& pointOfView,
						 const DEBUG_OPTIONS& debugOptions,
						 Stats& stats,
						 OpenGLRenderer::GeometryElementGLMapping& elementGLMapping,
						 OpenGLRenderer::MaterialPropertyGLMapping& materialGLMapping,
						 unordered_set<shared_ptr<MaterialProperty>>& activeProperties) {
	
	auto program = Program::Skybox();
	
	auto element = skyboxGeometry->elements().front();
	auto material = skyboxGeometry->materials().front();
	auto emissiveProperty = material->emissive();
	
	// check and load vertex data if necessary
	
	GLuint vbo, vao, ibo;
	GetSkyboxGLVertexDataHandles(skyboxGeometry,
								 elementGLMapping,
								 vbo, vao, ibo);
	
	// and load material contents if necessary
	
	auto glTextureHandles = map<MATERIAL_PROPERTY_TYPE, GLuint>();
	GetMaterialGLTextureHandles(*material,
								materialGLMapping,
								activeProperties,
								glTextureHandles);
	auto emissiveGLTextureHandle = glTextureHandles[MATERIAL_PROPERTY_TYPE::EMISSIVE];
	
	// send material property uniforms
	
	SendMaterialPropertyUniforms(*emissiveProperty,
								 MATERIAL_PROPERTY_TYPE::EMISSIVE,
								 emissiveGLTextureHandle,
								 debugOptions,
								 *program);
	
	// update material property filtering options
	
	SetMaterialPropertyFilteringOptions(*emissiveProperty, emissiveGLTextureHandle);
	
	// configure OpenGL state
	
	SetSkyboxOpenGLState();
	
	// update
	
	DrawSkyboxElement(*element, *program, pointOfView, vao, ibo);
	
	stats.geometries++;
	stats.polygons += element->faces().size();
	stats.meshes++;
}
	
static void GetGeometryElementGLVertexDataHandles(shared_ptr<GeometryElement> element,
												  OpenGLRenderer::GeometryElementGLMapping& glMapping,
												  GLuint& glVBO, GLuint& glVAO, GLuint& glIBO) {
	
	// looks up and populates glVBO, glVAO, and glIBO, loading the vertex data if needed

	if (GEOMETRY_ELEMENT_DIRTY_MASK_CONTAINS(element->dirtyMask(),
											 GEOMETRY_ELEMENT_DIRTY_MASK::VERTEX_DATA)) {
		
		DeleteGeometryElementGLResources(element, glMapping);
		
		BufferGeometryElementVertexData(*element, *Program::Default(), glVBO, glVAO, glIBO);
		
		glMapping[element] = make_tuple(glVBO, glVAO, glIBO);

		element->dirtyMask(GEOMETRY_ELEMENT_DIRTY_MASK_REMOVE(element->dirtyMask(),
															  GEOMETRY_ELEMENT_DIRTY_MASK::VERTEX_DATA));
	}
	else {
		auto mapping = glMapping[element];
		glVBO = get<0>(mapping);
		glVAO = get<1>(mapping);
		glIBO = get<2>(mapping);
	}
}
	
static void GetSkyboxGLVertexDataHandles(shared_ptr<Geometry> skyboxGeometry,
										 OpenGLRenderer::GeometryElementGLMapping& glMapping,
										 GLuint& glVBO, GLuint& glVAO, GLuint& glIBO) {
	
	// looks up and populates glVBO, glVAO, and glIBO, loading the vertex data if needed
	//
	// NOTE: this is essentially exactly the same as GetGeometryElementGLVertexDataHandles()
	// except if the data needs to be loaded, it uses BufferGeometryElementVertexData() as the
	// layout is different.  This will probaly need to be refacted in the future as more layouts are used
	
	auto element = skyboxGeometry->elements().front();
	
	if (GEOMETRY_ELEMENT_DIRTY_MASK_CONTAINS(element->dirtyMask(),
											 GEOMETRY_ELEMENT_DIRTY_MASK::VERTEX_DATA)) {
		
		DeleteGeometryElementGLResources(element, glMapping);
		
		BufferSkyboxVertexData(*skyboxGeometry, *Program::Skybox(), glVBO, glVAO, glIBO);
		
		glMapping[element] = make_tuple(glVBO, glVAO, glIBO);

		element->dirtyMask(GEOMETRY_ELEMENT_DIRTY_MASK_REMOVE(element->dirtyMask(),
															  GEOMETRY_ELEMENT_DIRTY_MASK::VERTEX_DATA));
	}
	else {
		auto mapping = glMapping[element];
		glVBO = get<0>(mapping);
		glVAO = get<1>(mapping);
		glIBO = get<2>(mapping);
	}
}
	
//static void GetGeometryAABBLineSetVertexDataHandles(shared_ptr<Geometry> geometry,
//													OpenGLRenderer::GeometryAABBLineSetMapping& aabbLineSetMapping,
//													OpenGLRenderer::LineSetGLMapping lineSetGLMapping,
//													GLuint& glVBO, GLuint& glVAO) {
//
//	auto program = Program::Lines();
//
//	// looks up and populates glVBO and glVAO, loading the vertex data if needed
//
//	if (GEOMETRY_DIRTY_MASK_CONTAINS(geometry->dirtyMask(),
//									 GEOMETRY_DIRTY_MASK::EXTENT)) {
//
//		DeleteLineSetGLResources(aabbLineSetMapping[geometry], lineSetGLMapping);
//
//		// construct a new lineset matching the geometry's extent
//
//		AE_LOG_T("Creating AABB LineSet for Geometry {:p}...", (void*)geometry.get());
//
//		map<string, vec3> bp = *(geometry->aabb(false));
//
//		float xMin = bp["xMin"].x;
//		float xMax = bp["xMax"].x;
//		float yMin = bp["yMin"].y;
//		float yMax = bp["yMax"].y;
//		float zMin = bp["zMin"].z;
//		float zMax = bp["zMax"].z;
//
//		vec3 one =      vec3(xMin, yMax, zMin);
//		vec3 two =      vec3(xMin, yMax, zMax);
//		vec3 three =    vec3(xMax, yMax, zMax);
//		vec3 four =     vec3(xMax, yMax, zMin);
//		vec3 five =     vec3(xMin, yMin, zMin);
//		vec3 six =      vec3(xMin, yMin, zMax);
//		vec3 seven =    vec3(xMax, yMin, zMax);
//		vec3 eight =    vec3(xMax, yMin, zMin);
//
//		auto aabbLineSet = make_shared<LineSet>();
//		auto red = Color::Red();
//
//		aabbLineSet->emplace(make_shared<Line>(one, two, red));
//		aabbLineSet->emplace(make_shared<Line>(two, three, red));
//		aabbLineSet->emplace(make_shared<Line>(three, four, red));
//		aabbLineSet->emplace(make_shared<Line>(four, one, red));
//		aabbLineSet->emplace(make_shared<Line>(five, six, red));
//		aabbLineSet->emplace(make_shared<Line>(six, seven, red));
//		aabbLineSet->emplace(make_shared<Line>(seven, eight, red));
//		aabbLineSet->emplace(make_shared<Line>(eight, five, red));
//		aabbLineSet->emplace(make_shared<Line>(one, five, red));
//		aabbLineSet->emplace(make_shared<Line>(two, six, red));
//		aabbLineSet->emplace(make_shared<Line>(three, seven, red));
//		aabbLineSet->emplace(make_shared<Line>(four, eight, red));
//
//		aabbLineSetMapping[geometry] = aabbLineSet;
//
//		geometry->dirtyMask(GEOMETRY_DIRTY_MASK_REMOVE(geometry->dirtyMask(),
//													   GEOMETRY_DIRTY_MASK::EXTENT));
//	}
//
//	GetLineSetVertexDataHandles(aabbLineSetMapping[geometry],
//								*program,
//								lineSetGLMapping,
//								glVBO, glVAO);
//}

static void GetGeometryAABBLineSetVertexDataHandles(shared_ptr<Geometry> geometry,
													OpenGLRenderer::GeometryAABBLineSetMapping& aabbLineSetMapping,
													OpenGLRenderer::LineSetGLMapping lineSetGLMapping,
													GLuint& glVBO, GLuint& glVAO) {

	auto program = Program::Lines();

	// looks up and populates glVBO and glVAO, loading the vertex data if needed

	if (GEOMETRY_DIRTY_MASK_CONTAINS(geometry->dirtyMask(),
									 GEOMETRY_DIRTY_MASK::EXTENT)) {

		DeleteLineSetGLResources(aabbLineSetMapping[geometry], lineSetGLMapping);

		// construct a new lineset matching the geometry's extent

		AE_LOG_T("Creating AABB LineSet for Geometry {:p}...", (void*)geometry.get());

		auto aabb = geometry->aabb();

		float xMin = aabb.min.x;
		float xMax = aabb.max.x;
		float yMin = aabb.min.y;
		float yMax = aabb.max.y;
		float zMin = aabb.min.z;
		float zMax = aabb.max.z;

		vec3 one =      vec3(xMin, yMax, zMin);
		vec3 two =      vec3(xMin, yMax, zMax);
		vec3 three =    vec3(xMax, yMax, zMax);
		vec3 four =     vec3(xMax, yMax, zMin);
		vec3 five =     vec3(xMin, yMin, zMin);
		vec3 six =      vec3(xMin, yMin, zMax);
		vec3 seven =    vec3(xMax, yMin, zMax);
		vec3 eight =    vec3(xMax, yMin, zMin);

		auto aabbLineSet = make_shared<LineSet>();
		auto red = Color::Red();

		aabbLineSet->emplace(make_shared<Line>(one, two, red));
		aabbLineSet->emplace(make_shared<Line>(two, three, red));
		aabbLineSet->emplace(make_shared<Line>(three, four, red));
		aabbLineSet->emplace(make_shared<Line>(four, one, red));
		aabbLineSet->emplace(make_shared<Line>(five, six, red));
		aabbLineSet->emplace(make_shared<Line>(six, seven, red));
		aabbLineSet->emplace(make_shared<Line>(seven, eight, red));
		aabbLineSet->emplace(make_shared<Line>(eight, five, red));
		aabbLineSet->emplace(make_shared<Line>(one, five, red));
		aabbLineSet->emplace(make_shared<Line>(two, six, red));
		aabbLineSet->emplace(make_shared<Line>(three, seven, red));
		aabbLineSet->emplace(make_shared<Line>(four, eight, red));

		aabbLineSetMapping[geometry] = aabbLineSet;

		geometry->dirtyMask(GEOMETRY_DIRTY_MASK_REMOVE(geometry->dirtyMask(),
													   GEOMETRY_DIRTY_MASK::EXTENT));
	}

	GetLineSetVertexDataHandles(aabbLineSetMapping[geometry],
								*program,
								lineSetGLMapping,
								glVBO, glVAO);
}

static void GetLineSetVertexDataHandles(shared_ptr<LineSet> lineSet,
										Program& program,
										OpenGLRenderer::LineSetGLMapping& glMapping,
										GLuint& glVBO, GLuint& glVAO) {
	
	if (!glMapping.count(lineSet)) {
		BufferLineSetVertexData(*lineSet, program, glVBO, glVAO);
		
		glMapping[lineSet] = make_pair(glVBO, glVAO);
	}
	else {
		auto mapping = glMapping[lineSet];
		glVBO = get<0>(mapping);
		glVAO = get<1>(mapping);
	}
}

static void GetPointSetVertexDataHandles(shared_ptr<PointSet> pointSet,
										 Program& program,
										 OpenGLRenderer::PointSetGLMapping& glMapping,
										 GLuint& glVBO, GLuint& glVAO) {
	
	if (!glMapping.count(pointSet)) {
		BufferPointSetVertexData(*pointSet, program, glVBO, glVAO);
		
		glMapping[pointSet] = make_pair(glVBO, glVAO);
	}
	else {
		auto mapping = glMapping[pointSet];
		glVBO = get<0>(mapping);
		glVAO = get<1>(mapping);
	}
}
	
static void GetMaterialGLTextureHandles(Material& material,
										OpenGLRenderer::MaterialPropertyGLMapping& glMapping,
										unordered_set<shared_ptr<MaterialProperty>>& activeProperties,
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
			
			if (MATERIAL_PROPERTY_DIRTY_MASK_CONTAINS(property->dirtyMask(),
													  MATERIAL_PROPERTY_DIRTY_MASK::CONTENTS)) {
				
				AE_LOG_D("MaterialProperty {:p} CONTENTS dirty.", (void*)property.get());
				
				DeleteMaterialPropertyGLResources(property, glMapping);
				
				GLuint textureID = 0;
				BufferMaterialPropertyTexture(*property, type, textureID);
				if (textureID > 0) {
					glTextureHandles[type] = textureID;
					glMapping[property] = textureID;
				}

				property->dirtyMask(MATERIAL_PROPERTY_DIRTY_MASK_REMOVE(property->dirtyMask(),
																		MATERIAL_PROPERTY_DIRTY_MASK::CONTENTS));
			}
			else {
				auto textureHandle = glMapping[property];
				glTextureHandles[type] = textureHandle;
			}
			
			// save reference for housekeeping
			activeProperties.emplace(property);
		}
	}
}
	
static void BufferGeometryElementVertexData(const GeometryElement& element,
										  Program& program,
										  GLuint& glVBO, GLuint& glVAO, GLuint& glIBO) {
	
	AE_LOG_D("Buffering vertex data for geometry element {:p}...", (void*)&element);
	
	program.use();
	
	const auto& verticies = element.vertices();
	const auto& faces = element.faces();
	
	glGenBuffers(1, &glVBO);
	glBindBuffer(GL_ARRAY_BUFFER, glVBO);
	glBufferData(GL_ARRAY_BUFFER,
				 verticies.size() * sizeof(Vertex),
				 &(verticies[0]),
				 GL_STATIC_DRAW);
	
	glGenVertexArrays(1, &glVAO);
	glBindVertexArray(glVAO);
	
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
	
	//program.unuse();
}

static void BufferSkyboxVertexData(Geometry& skyboxGeometry,
								 Program& program,
								 GLuint& glVBO, GLuint& glVAO, GLuint& glIBO) {
	
	AE_LOG_D("Buffering skybox vertex data...");
	
	program.use();
	
	auto element = skyboxGeometry.elements().front();
	const auto& verts = element->vertices();
	const auto& faces = element->faces();
	
	glGenBuffers(1, &glVBO);
	glBindBuffer(GL_ARRAY_BUFFER, glVBO);
	glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(Vertex), &(verts[0]), GL_STATIC_DRAW);
	
	glGenVertexArrays(1, &glVAO);
	glBindVertexArray(glVAO);

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
	
	//program.unuse();
}

static void BufferAABBVertexData(Geometry& geometry,
							   const Program& program,
							   GLuint& glVBO, GLuint& glVAO) {
	
	AE_LOG_I("Buffering vertex data for AABB {:p}...", (void*)&geometry);

	auto aabb = geometry.aabb();

	float xMin = aabb.min.x;
	float xMax = aabb.max.x;
	float yMin = aabb.min.y;
	float yMax = aabb.max.y;
	float zMin = aabb.min.z;
	float zMax = aabb.max.z;

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

	GLuint positionIndex = program.getAttributeLocation("vertex_position");
	glVertexAttribPointer(positionIndex, 	// attrib index
						  3, 				// num components per attrib (3 float in vec3)
						  GL_FLOAT, 		// component type
						  GL_FALSE, 		// normalize
						  sizeof(vec3), 	// stride
						  0); 				// start offset
	glEnableVertexAttribArray(positionIndex);
}
	
static void BufferLineSetVertexData(LineSet& lineSet,
									Program& program,
									GLuint& glVBO, GLuint& glVAO) {
	
	// back each Line into a vector with format <fromLocation, fromColor, toLocation, toColor>
	
	auto massagedBuffer = vector<vec3>();
	massagedBuffer.reserve(lineSet.size()*4);

	for (auto l : lineSet) {
		auto fromLocation = l->fromLocation();
		auto fromColor = l->fromColor();
		auto toLocation = l->toLocation();
		auto toColor = l->toColor();
		massagedBuffer.emplace_back(fromLocation);
		massagedBuffer.emplace_back(vec3(fromColor->r, fromColor->g, fromColor->b));
		massagedBuffer.emplace_back(toLocation);
		massagedBuffer.emplace_back(vec3(toColor->r, toColor->g, toColor->b));
	}
	
	glGenBuffers(1, &glVBO);
	glBindBuffer(GL_ARRAY_BUFFER, glVBO);
	glBufferData(GL_ARRAY_BUFFER, massagedBuffer.size() * sizeof(vec3), &(massagedBuffer[0]), GL_STATIC_DRAW);
	
	glGenVertexArrays(1, &glVAO);
	glBindVertexArray(glVAO);

	GLuint positionIndex = program.getAttributeLocation("vertex_position");
	glVertexAttribPointer(positionIndex, 		// attrib index
						  3, 					// num components per attrib (3 float in vec3)
						  GL_FLOAT, 			// component type
						  GL_FALSE, 			// normalize
						  sizeof(vec3)*2, 		// stride
						  0); 					// start offset
	glEnableVertexAttribArray(positionIndex);
	
	GLuint colorIndex = program.getAttributeLocation("vertex_color");
	glVertexAttribPointer(colorIndex, 			// attrib index
						  3, 					// num components per attrib (3 float in vec3)
						  GL_FLOAT, 			// component type
						  GL_FALSE, 			// normalize
						  sizeof(vec3)*2, 		// stride
						  (void*)sizeof(vec3));	// start offset
	glEnableVertexAttribArray(colorIndex);
}

static void BufferPointSetVertexData(PointSet& pointSet,
									 Program& program,
									 GLuint& glVBO, GLuint& glVAO) {
	
}
	
static void BufferMaterialPropertyTexture(const MaterialProperty& property,
										  MATERIAL_PROPERTY_TYPE type,
										  GLuint& glTextureHandle) {
	
	if (dynamic_pointer_cast<CubeImage>(property.contents())) {
		AE_LOG_D("Buffering cube texture {:p}...", (void*)&property);
		
		auto cubeImage = dynamic_pointer_cast<CubeImage>(property.contents());
		
		shared_ptr<Image> images[] = {
			cubeImage->posX(),
			cubeImage->negX(),
			cubeImage->posY(),
			cubeImage->negY(),
			cubeImage->posZ(),
			cubeImage->negZ() };
		
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
			auto image = images[s];
			
			unsigned bytesPerPixel = image->bytesPerPixel();
			GLint glInternalFormat = GL_RGBA;
			if (bytesPerPixel == 3) glInternalFormat = GL_RGB;
			else if (bytesPerPixel == 1) glInternalFormat = GL_RED;
			
			glTexImage2D(side,
						 0,
						 glInternalFormat,//GL_RGB, //GL_SRGB_ALPHA,
						 image->width(),
						 image->height(),
						 0,
						 GL_RGBA,//(image->bytesPerPixel() == 3 ? GL_RGB : GL_RGBA),
						 GL_UNSIGNED_BYTE,
						 image->data()->pointer());
		}
		
		SetTextureMinificationFilter(glTextureHandle, true, property.minificationFilter());
		SetTextureMagnificationFilter(glTextureHandle, true, property.magnificationFilter());
		SetTextureMaxAnisotropy(glTextureHandle, true, property.maxAnisotropy());
		SetTextureWrapS(glTextureHandle, true, property.wrapS());
		SetTextureWrapT(glTextureHandle, true, property.wrapT());
		SetTextureWrapR(glTextureHandle, property.wrapR());
	}
	else if (dynamic_pointer_cast<Image>(property.contents())) {
		
		
//		switch (type) {
//			case MATERIAL_PROPERTY_TYPE::AMBIENT:
//				glActiveTexture(GL_TEXTURE0);
//				break;
//			case MATERIAL_PROPERTY_TYPE::DIFFUSE:
//				glActiveTexture(GL_TEXTURE1);
//				break;
//			case MATERIAL_PROPERTY_TYPE::SPECULAR:
//				glActiveTexture(GL_TEXTURE2);
//				break;
//			case MATERIAL_PROPERTY_TYPE::EMISSIVE:
//				glActiveTexture(GL_TEXTURE3);
//				break;
//			default:
//				cout << "Invalid MATERIAL_PROPERTY_TYPE: " << static_cast<int>(type) << endl;
//				return;
//		}
		
		
		
		
		AE_LOG_D("Buffering 2D texture {:p}...", (void*)&property);
		
		auto image = dynamic_pointer_cast<Image>(property.contents());
		
		glGenTextures(1, &glTextureHandle);
		AE_LOG_D("Binding new texture handle: {}", glTextureHandle);
		glBindTexture(GL_TEXTURE_2D, glTextureHandle);

		unsigned bytesPerPixel = image->bytesPerPixel();
		GLint glInternalFormat = GL_RGBA;
		if (bytesPerPixel == 3) glInternalFormat = GL_RGB;
		else if (bytesPerPixel == 1) glInternalFormat = GL_RED;
		
		AE_LOG_D("Buffering image {:p}: width: {}, height: {}, bytesPerPixel: {}, data size: {}",
					  (void*)image.get(), image->width(), image->height(), image->bytesPerPixel(),
					  image->width() * image->height() * image->bytesPerPixel());
		
	
		
		
		
//		// DEBUG: write texture data to file
//		
//		AE_LOG_D("Saving property {:p}...", (void*)&property);
//	
//		unsigned imageDataSize = image->width() * image->height() * image->bytesPerPixel();
//		vector<unsigned char> imageBuf = Buffer(image->data(), imageDataSize);
//		
//		static unsigned index = 0;
//		string filePath = "./" + to_string(index) + ".buf			";
//		unsigned written = BinaryFile(filesystem::path(filePath), imageBuf);
//		AE_LOG_D("### WROTE {} BYTES OF TEXTURE IMAGE TO: {}", written, filePath);
//		++index;
		
		
//		unsigned char* thing = (unsigned char*)malloc(1024 * sizeof(unsigned char));
//		AE_LOG_D("THING SIZE: {}", sizeof(thing));
//		
//		AE_LOG_D("DATA SIZE: {}", sizeof(image->data()));
		
		//glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glTexImage2D(GL_TEXTURE_2D,
					 0,
					 GL_RGBA,//glInternalFormat,//GL_RGBA,//GL_SRGB_ALPHA,
					 image->width(),
					 image->height(),
					 0,
					 GL_RGBA,//(image->bytesPerPixel() == 3 ? GL_RGB : GL_RGBA),
					 GL_UNSIGNED_BYTE,
					 image->data()->pointer());
		
		SetTextureMinificationFilter(glTextureHandle, false, property.minificationFilter());
		SetTextureMagnificationFilter(glTextureHandle, false, property.magnificationFilter());
		SetTextureMaxAnisotropy(glTextureHandle, false, property.maxAnisotropy());
		SetTextureWrapS(glTextureHandle, false, property.wrapS());
		SetTextureWrapT(glTextureHandle, false, property.wrapT());
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
				cout << "Invalid MATERIAL_PROPERTY_TYPE: "
				<< static_cast<underlying_type<MATERIAL_PROPERTY_TYPE>::type>(type) << endl;
				return;
		}
		
//		glActiveTexture(slot);

		program.setUniform(modeUniformName.c_str(),
						   static_cast<underlying_type<MATERIAL_MODE>::type>(MATERIAL_MODE::SAMPLER));
		program.bindTexture(samplerUniformName.c_str(), GL_TEXTURE_2D, slot, glTextureHandle, index);
	}
	else if (dynamic_pointer_cast<Color>(property.contents()) ){ // color
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
				cout << "Invalid MATERIAL_PROPERTY_TYPE: "
				<< static_cast<underlying_type<MATERIAL_PROPERTY_TYPE>::type>(type) << endl;
				return;
		}

		program.setUniform(modeUniformName.c_str(),
						   static_cast<underlying_type<MATERIAL_MODE>::type>(MATERIAL_MODE::COLOR));
		program.setUniform(colorUniformName.c_str(), color->r, color->g, color->b);
	}
	else {
		AE_LOG_W("NULL material property contents.");
	}
	
	//program.unuse();
}
	
static void SendEnvironmentUniforms(GLuint glEnvironmentUBO, const Scene& scene, Stats& stats) {
	
	// program "Default" must be active
	
	// lights
	
	auto lights = vector<shared_ptr<Node>>();
	shared_ptr<Node> ambientLightNode = nullptr;
	
	// find all lights in the scene
	for (auto node: scene.rootNode()->children(true)) {
		if (!node->hidden()) {
			auto light = node->light();
			if (light != nullptr) {
				if (light->type() == LIGHT_TYPE::POINT) {
					lights.emplace_back(node);
				}
				else if (light->type() == LIGHT_TYPE::AMBIENT) {
					ambientLightNode = node;
				}
			}
		}
	}
	
	if (lights.size() > MAX_DYNAMIC_LIGHTS) {
		
		// find all light distances from the camera
		
		auto lightsUnsorted = map<shared_ptr<Node>, float>();
		vec3 cameraPos_world = scene.visualWorld()->pointOfView()->worldPosition();
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
//		auto detaultPoint = Light::PointNode();
		auto detaultPointNode = Node::LightNode(Light::DefaultPoint());
		// set position based on scene extent...
		static vec3 sceneExtent = scene.rootNode()->extent(); // only doing this once or it runs reallll slow
		detaultPointNode->position({sceneExtent.x + sceneExtent.x/4.0,
			sceneExtent.y + sceneExtent.y/4.0,
			sceneExtent.z + sceneExtent.z/4.0});
		lights.emplace_back(detaultPointNode);
	}
	if (!ambientLightNode) ambientLightNode = Node::LightNode(Light::DefaultAmbient());
	
	lights.emplace_back(ambientLightNode);
	
	unsigned numLights = lights.size();
	LightGLSLStruct lightStruct[numLights];
	
	stats.lights = numLights;// - 1; // not counting ambient
	
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

	auto visualWorld = scene.visualWorld();
	FogGLSLStruct fogStruct;
	fogStruct.startDistance = visualWorld->fogStartDistance();
	fogStruct.endDistance = visualWorld->fogEndDistance();
	fogStruct.densityExponent = visualWorld->fogDensityExponent();
	fogStruct.startDistance = visualWorld->fogStartDistance();
	auto fogColor = visualWorld->fogColor();
	if (visualWorld->fogColor()) fogStruct.color = vec4(fogColor->r,
														fogColor->g,
														fogColor->b,
														fogColor->a);
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
	
	if (MATERIAL_PROPERTY_DIRTY_MASK_CONTAINS(property.dirtyMask(),
											  MATERIAL_PROPERTY_DIRTY_MASK::MINIFICATION_FILTER)) {
		SetTextureMinificationFilter(glTextureHandle, cube, property.minificationFilter());
		property.dirtyMask(MATERIAL_PROPERTY_DIRTY_MASK_REMOVE(property.dirtyMask(),
															   MATERIAL_PROPERTY_DIRTY_MASK::MINIFICATION_FILTER));
	}
	
	if (MATERIAL_PROPERTY_DIRTY_MASK_CONTAINS(property.dirtyMask(),
											  MATERIAL_PROPERTY_DIRTY_MASK::MAGNIFICATION_FILTER)) {
		SetTextureMagnificationFilter(glTextureHandle, cube, property.magnificationFilter());
		property.dirtyMask(MATERIAL_PROPERTY_DIRTY_MASK_REMOVE(property.dirtyMask(),
															   MATERIAL_PROPERTY_DIRTY_MASK::MAGNIFICATION_FILTER));
	}
	
	if (MATERIAL_PROPERTY_DIRTY_MASK_CONTAINS(property.dirtyMask(),
											  MATERIAL_PROPERTY_DIRTY_MASK::WRAP_S)) {
		SetTextureWrapS(glTextureHandle, cube, property.wrapS());
		property.dirtyMask(MATERIAL_PROPERTY_DIRTY_MASK_REMOVE(property.dirtyMask(),
															   MATERIAL_PROPERTY_DIRTY_MASK::WRAP_S));
	}
	
	if (MATERIAL_PROPERTY_DIRTY_MASK_CONTAINS(property.dirtyMask(),
											  MATERIAL_PROPERTY_DIRTY_MASK::WRAP_T)) {
		SetTextureWrapT(glTextureHandle, cube, property.wrapT());
		property.dirtyMask(MATERIAL_PROPERTY_DIRTY_MASK_REMOVE(property.dirtyMask(),
															   MATERIAL_PROPERTY_DIRTY_MASK::WRAP_T));
	}
	
	if (cube) {
		if (MATERIAL_PROPERTY_DIRTY_MASK_CONTAINS(property.dirtyMask(),
												  MATERIAL_PROPERTY_DIRTY_MASK::WRAP_R)) {
			SetTextureWrapR(glTextureHandle, property.wrapR());
			property.dirtyMask(MATERIAL_PROPERTY_DIRTY_MASK_REMOVE(property.dirtyMask(),
																   MATERIAL_PROPERTY_DIRTY_MASK::WRAP_R));
		}
	}
	
	if (MATERIAL_PROPERTY_DIRTY_MASK_CONTAINS(property.dirtyMask(),
											  MATERIAL_PROPERTY_DIRTY_MASK::MAX_ANISTROPY)) {
		SetTextureMaxAnisotropy(glTextureHandle, cube, property.maxAnisotropy());
		property.dirtyMask(MATERIAL_PROPERTY_DIRTY_MASK_REMOVE(property.dirtyMask(),
															   MATERIAL_PROPERTY_DIRTY_MASK::MAX_ANISTROPY));
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

//	if (material.blendFunction() == BLEND_FUNCTION::THING) {
//	https://developer.apple.com/documentation/scenekit/scnblendmode
//	https://registry.khronos.org/OpenGL-Refpages/gl4/html/glBlendFunc.xhtml
//		glEnable(GL_BLEND);
//		glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA); // param 0 was "GL_SRC_ALPHA" (?), GL_ONE_MINUS_SRC_ALPHA
//	}
//	else {
//		glDisable(GL_BLEND);
//	}

	if (DEBUG_OPTIONS_CONTAINS(debugOptions, DEBUG_OPTIONS::SHOW_PHYSICS_WIREFRAMES)) {
		// can create zbuffer problems
		// https://www.opengl.org/archives/resources/faq/technical/polygonoffset.htm
		//glDepthRange(0.1, 1.0);
		//		glEnable(GL_POLYGON_OFFSET_FILL);
		//		glPolygonOffset(20.0, 0.0);
	}
	
	if (DEBUG_OPTIONS_CONTAINS(debugOptions, DEBUG_OPTIONS::SHOW_WIREFRAMES)) {
		//_program = Program::Wireframe();
		
#ifdef OPENGL_CORE
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glEnable(GL_LINE_SMOOTH);
#endif
	}
	else {
#ifdef OPENGL_CORE
		if (material.fillMode() == FILL_MODE::LINES) {
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		}
		else if (material.fillMode() == FILL_MODE::POINTS) {
			glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
		}
		else {
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}
#endif
		
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
#ifdef OPENGL_CORE
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
#endif
	glDisable(GL_CULL_FACE);
//	glDisable(GL_BLEND);
}

static void SetAABBOpenGLState() {
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glDepthMask(GL_TRUE);
#ifdef OPENGL_CORE
	glEnable(GL_LINE_SMOOTH);
#endif
}

static void SetLineSetGLState() {
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glDepthMask(GL_TRUE);
#ifdef OPENGL_CORE
	glEnable(GL_LINE_SMOOTH);
#endif
	
	// https://www.opengl.org/archives/resources/faq/technical/polygonoffset.htm
	//glDepthRange(0.0, 0.9);
//	glDisable(GL_POLYGON_OFFSET_FILL);
//	glPolygonOffset(0.0, 0.0);
}
	
static void SetPointSetGLState() {
	
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
	
	// update
	
	glBindVertexArray(vao);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	unsigned int numFaces = element.faces().size();
	glDrawElements(GL_TRIANGLES, numFaces * 3, GL_UNSIGNED_INT, (void*)0);
}

static void DrawSkyboxElement(GeometryElement& element,
							  Program& program,
							  Node& pointOfView,
							  GLuint vao, GLuint ibo) {
	
	program.use();
	
	mat4 viewMat = lookAt(vec3(0.0f, 0.0f, 0.0f), // eye - location
						  pointOfView.worldForward(), // center - look at
						  pointOfView.worldUp()); // up
	
	auto projectionMat = pointOfView.camera()->projection();
	
	program.setUniform("view", viewMat);
	program.setUniform("projection", projectionMat);
	
	// update
	
	const auto& faces = element.faces();
	
	glBindVertexArray(vao);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	unsigned int numFaces = faces.size();
	glDrawElements(GL_TRIANGLES, numFaces * sizeof(Face), GL_UNSIGNED_INT, (void*)0);
}
	
static void DrawAABB(Geometry& geometry,
					 Program& program,
					 mat4 modelMat,
					 mat4 viewMat,
					 mat4 projectionMat,
					 GLuint glVBO, GLuint glVAO) {
	
	//auto program = Program::AABB();
	program.use();
	
	// uniforms
	
	program.setUniform("model", modelMat);
	program.setUniform("view", inverse(viewMat));
	program.setUniform("projection", projectionMat);
	
	// update
	
	glBindVertexArray(glVAO);
	glDrawArrays(GL_LINES, 0, 24);
}

static void DrawLineSet(LineSet& lineSet,
						Program& program,
						mat4 modelMat,
						mat4 viewMat,
						mat4 projectionMat,
						GLuint glVBO, GLuint glVAO) {
	
	program.use();
	
	// uniforms
	
	program.setUniform("model", modelMat);
	program.setUniform("view", inverse(viewMat));
	program.setUniform("projection", projectionMat);
	
	// update
	
	glBindVertexArray(glVAO);
	glDrawArrays(GL_LINES, 0, lineSet.size() * 4);
}

static void DrawPointSet(PointSet& pointSet,
						 Program& program,
						 mat4 modelMat,
						 mat4 viewMat,
						 mat4 projectionMat,
						 GLuint glVBO, GLuint glVAO) {
	
}

static void CleanupGeometryElementResources(unordered_set<shared_ptr<GeometryElement>>& active,
											OpenGLRenderer::GeometryElementGLMapping& glMapping) {
#ifndef DISABLE_RESOURCE_MANAGEMENT
	
	// gather sorted vector of elements used this frame
	auto activeElementsSorted = vector<shared_ptr<GeometryElement>>();
	activeElementsSorted.reserve(active.size());
	copy(active.begin(), active.end(), back_inserter(activeElementsSorted));
	sort(activeElementsSorted.begin(), activeElementsSorted.end());
	
	// gather sorted vector of elements in the mapping
	auto storedElementsSorted = vector<shared_ptr<GeometryElement>>();
	storedElementsSorted.reserve(glMapping.size());
	for (auto it = glMapping.begin(); it != glMapping.end(); ++it) {
		storedElementsSorted.emplace_back(it->first);
	}
	sort(storedElementsSorted.begin(), storedElementsSorted.end());
	
	// find unused elements
	auto unused = vector<shared_ptr<GeometryElement>>(storedElementsSorted.size());
	vector<shared_ptr<GeometryElement>>::iterator it;
	it = set_difference(storedElementsSorted.begin(), storedElementsSorted.end(),
						activeElementsSorted.begin(), activeElementsSorted.end(),
						unused.begin());
	unused.resize(it - unused.begin());
	
	// deallocate unused elements
	if (unused.size()) {
		//AE_LOG_D("Deleting GL resources for {} geometry elements...", unused.size());
		
		for (it=unused.begin(); it!=unused.end(); ++it) {
			shared_ptr<GeometryElement> element = *it;
			DeleteGeometryElementGLResources(element, glMapping);
		}
	}
	
#endif
}
	
static void CleanupMaterialPropertyResources(unordered_set<shared_ptr<MaterialProperty>>& active,
											 OpenGLRenderer::MaterialPropertyGLMapping& glMapping) {
#ifndef DISABLE_RESOURCE_MANAGEMENT
	
	// gather sorted vector of properties used this frame
	auto activePropertiesSorted = vector<shared_ptr<MaterialProperty>>();
	activePropertiesSorted.reserve(glMapping.size());
	copy(active.begin(), active.end(), back_inserter(activePropertiesSorted));
	sort(activePropertiesSorted.begin(), activePropertiesSorted.end());
	
	// gather sorted vector of properties in the mapping
	auto storedPropertiesSorted = vector<shared_ptr<MaterialProperty>>();
	storedPropertiesSorted.reserve(glMapping.size());
	for (auto it = glMapping.begin(); it != glMapping.end(); ++it) {
		storedPropertiesSorted.emplace_back(it->first);
	}
	sort(storedPropertiesSorted.begin(), storedPropertiesSorted.end());
	
	// find unused properties
	auto unused = vector<shared_ptr<MaterialProperty>>(storedPropertiesSorted.size());
	vector<shared_ptr<MaterialProperty>>::iterator it;
	it = set_difference(storedPropertiesSorted.begin(), storedPropertiesSorted.end(),
						activePropertiesSorted.begin(), activePropertiesSorted.end(),
						unused.begin());
	unused.resize(it - unused.begin());
	
	// deallocate unused properties
	if (unused.size()) {
		//AE_LOG_D("Deleting GL resources for {} textures...", unused.size());
		
		for (it=unused.begin(); it!=unused.end(); ++it) {
			shared_ptr<MaterialProperty> property = *it;
			DeleteMaterialPropertyGLResources(property, glMapping);
		}
	}
	
#endif
}
	
static void CleanupLineSetResources(unordered_set<shared_ptr<LineSet>>& active,
									OpenGLRenderer::LineSetGLMapping& glMapping) {
#ifndef DISABLE_RESOURCE_MANAGEMENT
	
	// gather sorted vector of LineSets used this frame
	auto activeLineSetsSorted = vector<shared_ptr<LineSet>>();
	activeLineSetsSorted.reserve(glMapping.size());
	copy(active.begin(), active.end(), back_inserter(activeLineSetsSorted));
	sort(activeLineSetsSorted.begin(), activeLineSetsSorted.end());
	
	// gather sorted vector of LineSets in the mapping
	auto storedLineSetsSorted = vector<shared_ptr<LineSet>>();
	storedLineSetsSorted.reserve(glMapping.size());
	for (auto it = glMapping.begin(); it != glMapping.end(); ++it) {
		storedLineSetsSorted.emplace_back(it->first);
	}
	sort(storedLineSetsSorted.begin(), storedLineSetsSorted.end());
	
	// find unused LineSets
	auto unused = vector<shared_ptr<LineSet>>(storedLineSetsSorted.size());
	vector<shared_ptr<LineSet>>::iterator it;
	it = set_difference(storedLineSetsSorted.begin(), storedLineSetsSorted.end(),
						activeLineSetsSorted.begin(), activeLineSetsSorted.end(),
						unused.begin());
	unused.resize(it - unused.begin());
	
	// deallocate unused LineSets
	if (unused.size()) {
		//AE_LOG_D("Deleting GL resources for {} line sets...", unused.size());
		
		for (it=unused.begin(); it!=unused.end(); ++it) {
			shared_ptr<LineSet> lineSet = *it;
			DeleteLineSetGLResources(lineSet, glMapping);
		}
	}
	
#endif
}

static void CleanupPointSetResources(unordered_set<shared_ptr<PointSet>>& active,
									 OpenGLRenderer::PointSetGLMapping& glMapping) {
#ifndef DISABLE_RESOURCE_MANAGEMENT
	
#endif
}

static void DeleteGeometryElementGLResources(shared_ptr<GeometryElement> element,
											 OpenGLRenderer::GeometryElementGLMapping& glMapping) {
#ifndef DISABLE_RESOURCE_MANAGEMENT
	
	if (glMapping.count(element)) {
		
		AE_LOG_D("Deleting GL resources for GeometryElement {:p}...", (void*)element.get());
		
		auto glHandles = glMapping[element];
		
		GLuint vbo = get<0>(glHandles);
		GLuint vao = get<1>(glHandles);
		GLuint ibo = get<2>(glHandles);
		
		glDeleteBuffers(1, &vbo);
		glDeleteVertexArrays(1, &vao);
		glDeleteBuffers(1, &ibo);
		
		glMapping.erase(element);

		element->dirtyMask(GEOMETRY_ELEMENT_DIRTY_MASK_REMOVE(element->dirtyMask(),
															  GEOMETRY_ELEMENT_DIRTY_MASK::VERTEX_DATA));
	}
	
#endif
}

static void DeleteMaterialPropertyGLResources(shared_ptr<MaterialProperty> property,
											  OpenGLRenderer::MaterialPropertyGLMapping& glMapping) {
#ifndef DISABLE_RESOURCE_MANAGEMENT
	
	if (glMapping.count(property)) {
		
		AE_LOG_D("Deleting GL resources for MaterialProperty {:p}...", (void*)property.get());
		
		GLuint handle = glMapping[property];
		
		glDeleteTextures(1, &handle);
		
		glMapping.erase(property);

		property->dirtyMask(MATERIAL_PROPERTY_DIRTY_MASK_REMOVE(property->dirtyMask(),
																MATERIAL_PROPERTY_DIRTY_MASK::ALL));
	}
	
#endif
}

static void DeleteLineSetGLResources(shared_ptr<LineSet> lineSet,
									 OpenGLRenderer::LineSetGLMapping& glMapping) {
#ifndef DISABLE_RESOURCE_MANAGEMENT
	
	if (glMapping.count(lineSet)) {
		
		AE_LOG_T("Deleting GL resources for LineSet {:p}..", (void*)lineSet.get());
		
		auto glHandles = glMapping[lineSet];
		
		GLuint vbo = get<0>(glHandles);
		GLuint vao = get<1>(glHandles);
		
		glDeleteBuffers(1, &vbo);
		glDeleteVertexArrays(1, &vao);
		
		glMapping.erase(lineSet);
	}
	
#endif
}

static void DeletePointSetGLResources(shared_ptr<PointSet> pointSet,
									  OpenGLRenderer::PointSetGLMapping& glMapping) {
#ifndef DISABLE_RESOURCE_MANAGEMENT
	
#endif
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
	
void DrawStatsOverlay(Stats& stats, float time, Scene& scene) {

#ifdef OPENGL_CORE

	auto renderContext = scene.visualWorld()->renderContext();

	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	ImGuiWindowFlags window_flags = 0;
	window_flags |= ImGuiWindowFlags_NoTitleBar;
	window_flags |= ImGuiWindowFlags_NoScrollbar;
//	window_flags |= ImGuiWindowFlags_NoMove;
	window_flags |= ImGuiWindowFlags_NoResize;
	window_flags |= ImGuiWindowFlags_NoCollapse;
	window_flags |= ImGuiWindowFlags_NoNav;
	window_flags |= ImGuiWindowFlags_AlwaysAutoResize;

	ImGui::SetNextWindowBgAlpha(0);
	bool* p_open = nullptr;
	ImGui::Begin("Stats", p_open, window_flags);
	ImGuiStyle& style = ImGui::GetStyle();
	style.WindowBorderSize = 0;

	static bool setInitialPosition = false;
	if (!setInitialPosition) {
		ImGui::SetWindowPos((ImVec2){10.0, 10.0});
		setInitialPosition = true;
	}

	if (renderContext->recordingGIF()) {
		auto numFrames = renderContext->recordedGIFFrames();
		ImGui::Text("%-14s %.1f fps %s\n" \
					"%-14s %.1f ms\n" \
					"\n" \
					"%-14s %d\n" \
					"%-14s %d\n" \
					"%-14s %d\n" \
					"%-14s %d\n" \
					"%-14s %d\n" \
					"\n" \
					"%-14s %d\n" \
					"%-14s %d\n" \
					"%-14s %d\n" \
					"%-14s %d\n" \
					"%-14s %d\n" \
					"%-14s %d\n" \
					"%-14s %d\n" \
					"%-14s %d\n" \
					"\n" \
					"%-14s (%.1f %.1f %.1f)\n" \
					"\n" \
					"%-14s %d %s\n",

					"framerate", stats.averageFramerate, (renderContext->vSyncEnabled() ? "[vsync]" : ""),
					"frametime", stats.averageFrametime,
					"nodes", stats.nodes,
					"geometries", stats.geometries,
					"meshes", stats.meshes,
					"polygons", stats.polygons,
					"lights", stats.lights,

					"physics bodies", stats.dynamicBodies + stats.kinematicBodies + stats.staticBodies,
					" static", stats.staticBodies,
					" dynamic", stats.dynamicBodies,
					" kinematic", stats.kinematicBodies,
					"physics shapes", stats.concavePolyhedronShapes + stats.boundingBoxShapes + stats.convexHullShapes,
					" bounding box", stats.boundingBoxShapes,
					" convex hull", stats.convexHullShapes,
					" concave polyhedron", stats.concavePolyhedronShapes,

					"camera pos", stats.cameraPosition.x, stats.cameraPosition.y, stats.cameraPosition.z,
					"RECORDING", numFrames, (numFrames==1 ? "frame" : "frames"));
	}
	else {
		ImGui::Text("%-14s %.1f fps %s\n" \
					"%-14s %.1f ms\n" \
					"\n" \
					"%-14s %d\n" \
					"%-14s %d\n" \
					"%-14s %d\n" \
					"%-14s %d\n" \
					"%-14s %d\n" \
					"\n" \
					"%-14s %d\n" \
					"%-14s %d\n" \
					"%-14s %d\n" \
					"%-14s %d\n" \
					"%-14s %d\n" \
					"%-14s %d\n" \
					"%-14s %d\n" \
					"%-14s %d\n" \
					"\n" \
					"%-14s (%.1f, %.1f, %.1f)\n",

					"framerate", stats.averageFramerate, (renderContext->vSyncEnabled() ? "[vsync]" : ""),
					"frametime", stats.averageFrametime,
					"nodes", stats.nodes,
					"geometries", stats.geometries,
					"meshes", stats.meshes,
					"polygons", stats.polygons,
					"lights", stats.lights,

					"physics bodies", stats.dynamicBodies + stats.kinematicBodies + stats.staticBodies,
					" static", stats.staticBodies,
					" dynamic", stats.dynamicBodies,
					" kinematic", stats.kinematicBodies,
					"physics shapes", stats.concavePolyhedronShapes + stats.boundingBoxShapes + stats.convexHullShapes,
					" bounding box", stats.boundingBoxShapes,
					" convex hull", stats.convexHullShapes,
					" concave polyhedron", stats.concavePolyhedronShapes,

					"camera pos", stats.cameraPosition.x, stats.cameraPosition.y, stats.cameraPosition.z);
	}

	ImGui::End();

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

#endif // OPENGL_CORE
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
		AE_LOG_W("Unsupported magnification filter mode: {}", (unsigned)mode);
		break;
	}
}

static void SetTextureMaxAnisotropy(GLuint glTextureHandle, bool cube, float max) {

#ifdef OPENGL_CORE
	GLenum texType = (cube ? GL_TEXTURE_CUBE_MAP : GL_TEXTURE_2D);
	
	float anisotropy = max;
	glBindTexture(texType, glTextureHandle);
	float largest;
	glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &largest);
	if (max > largest) anisotropy = largest;
	glTexParameterf(texType, GL_TEXTURE_MAX_ANISOTROPY_EXT, anisotropy);
#endif
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
#ifdef OPENGL_CORE
		case WRAP_MODE::CLAMP_TO_BORDER:			return GL_CLAMP_TO_BORDER;
#endif
		case WRAP_MODE::REPEAT:						return GL_REPEAT;
        default: /* MIRRORED_REPEAT */   			return GL_MIRRORED_REPEAT; }
}

static WRAP_MODE WrapModeForGLWrapMode(GLenum mode) {
	switch (mode) {
#ifdef OPENGL_CORE
		case GL_CLAMP_TO_BORDER:					return WRAP_MODE::CLAMP_TO_EDGE;
#endif
		case GL_REPEAT:								return WRAP_MODE::REPEAT;
		case GL_MIRRORED_REPEAT: 					return WRAP_MODE::MIRRORED_REPEAT;
		default: /* GL_CLAMP_TO_EDGE */				return WRAP_MODE::CLAMP_TO_BORDER; }
}

static void CheckGLError() {
	GLenum err = glGetError();
	if (err != GL_NO_ERROR) {
		AE_LOG_E("*** GL error: 0x{:X} ***", err);
	}
}
