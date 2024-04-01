//
//  OpenGLRenderer.cc
//	avara3d
//
//  Created by Morgan Davis on 4/24/18.
//  Copyright © 2018 Morgan K Davis. All rights reserved.
//

#include "a3d/rendering/opengl/OpenGLRenderer.h"

#include <algorithm>
#include <iostream>
#include <set>
#include <vector>

#ifdef OPENGL_ES
#include <EGL/egl.h>
#include <GLES3/gl3.h>
#else
#include "GL/glew.h"
#endif

//#ifdef OPENGL_DESKTOP
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
//#endif

#include "magic_enum.hpp"

#include "a3d/Buffer.h"
#include "a3d/Color.h"
#include "a3d/Configuration.h"
#include "a3d/CubeImage.h"
#include "a3d/Font.h"
#include "a3d/Image.h"
#include "a3d/Utilities.h"
#include "a3d/diagnostic/exception/Exception.h"
#include "a3d/diagnostic/logging/Logger.h"
#include "a3d/mesh/Mesh.h"
#include "a3d/mesh/MeshElement.h"
#include "a3d/mesh/Line.h"
#include "a3d/mesh/Point.h"
#include "a3d/rendering/Light.h"
#include "a3d/rendering/VisualWorld.h"
#include "a3d/rendering/camera/Camera.h"
#include "a3d/rendering/context/RenderContext.h"
#include "a3d/rendering/context/platform/desktop/Window.h"
#include "a3d/rendering/material/Material.h"
#include "a3d/rendering/material/Sampler.h"
#include "a3d/rendering/material/Texture.h"
#include "a3d/rendering/opengl/Program.h"
#include "a3d/scene/Node.h"
#include "a3d/scene/Scene.h"


using namespace a3d;
using namespace a3d::utils;
using namespace glm;
using namespace std;


//#define DISABLE_RESOURCE_MANAGEMENT


/*********************************************************************************************
	Static Prototypes
 *********************************************************************************************/

static void 		RenderSkybox(Mesh& skyboxMesh,
								Node& pointOfView,
								OpenGLRenderer::MeshElementGLMapping& elementGLMapping,
								OpenGLRenderer::TextureGLMapping& textureGLMapping,
								unordered_set<Texture*>& activeTextures);
static void 		GetMeshElementGLVertexDataHandles(MeshElement& element,
													 OpenGLRenderer::MeshElementGLMapping& glMapping,
													 GLuint& glVBO, GLuint& glVAO, GLuint& glIBO);
static void 		GetSkyboxGLVertexDataHandles(Mesh& skyboxMesh,
												OpenGLRenderer::MeshElementGLMapping& glMapping,
												GLuint& glVBO, GLuint& glVAO, GLuint& glIBO);
//static void 		GetMeshAABBLinesVertexDataHandles(Mesh& mesh,
//													 OpenGLRenderer::LinesGLMapping& linesGLMapping,
//													 GLuint& glVBO, GLuint& glVAO);
static void 		GetLinesVertexDataHandles(const vector<Line>& lines,
											 Program& program,
											 OpenGLRenderer::LinesGLMapping& glMapping,
											 GLuint& glVBO, GLuint& glVAO);
static void 		GetTextureGLTextureHandles(Material& material,
											  OpenGLRenderer::TextureGLMapping& glMapping,
											  unordered_set<Texture*>& activeTextures,
											  map<MaterialPropertyType, GLuint>& glTextureHandles);
static void 		BufferMeshElementVertexData(const MeshElement& element,
											   Program& program,
											   GLuint& glVBO, GLuint& glVAO, GLuint& glIBO);
static void 		BufferSkyboxVertexData(Mesh& skyboxMesh,
										  Program& program,
										  GLuint& glVBO, GLuint& glVAO, GLuint& glIBO);
static void 		BufferLinesVertexData(const vector<Line>& lines,
										 Program& program,
										 GLuint& glVBO, GLuint& glVAO);
static void 		BufferTexture(const Texture &texture,
								 GLuint& glTextureHandle);
static void 		SendMaterialUniforms(const Material& material,
										Program& program,
										map<MaterialPropertyType, GLuint>& glTextureHandles);
static void 		SendMaterialPropertyUniforms(const MaterialProperty& property,
												MaterialPropertyType type,
												GLuint glTextureHandle,
												Program& program);
static void 		SendEnvironmentUniforms(GLuint glEnvironmentUBO, const Scene& scene, Stats& stats);
static void 		SetTextureSamplingOptions(Texture& texture,
											 GLuint glTextureHandle);
static void 		SetMaterialFilteringOptions(const Material& material,
											   map<MaterialPropertyType, GLuint>& glTextureHandles);
static void 		SetMaterialOpenGLState(const Material& material,
										  const DebugOptions& debugOptions);
static void	 		SetSkyboxOpenGLState();
static void 		SetLinesGLState();
static void 		DrawMeshElement(MeshElement& element,
								   Program& program,
								   mat4 modelMat, mat4 viewMat, mat4 projectionMat,
								   GLuint vao, GLuint ibo);
static void 		DrawSkyboxElement(MeshElement& element,
									 Program& program,
									 Node& pointOfView,
									 GLuint vao, GLuint ibo);
static void 		DrawLines(const vector<Line>& lines,
							 Program& program,
							 mat4 modelMat,
							 mat4 viewMat,
							 mat4 projectionMat,
							 GLuint glVAO);
static void 		CleanupMeshElementResources(unordered_set<MeshElement*>& active,
											   OpenGLRenderer::MeshElementGLMapping& glMapping);
static void 		CleanupTextureResources(unordered_set<Texture*>& active,
										   OpenGLRenderer::TextureGLMapping& glMapping);
static void 		CleanupLinesResources(unordered_set<const vector<Line>*>& active,
										 OpenGLRenderer::LinesGLMapping& glMapping);
static void 		DeleteMeshElementGLResources(MeshElement* element,
												OpenGLRenderer::MeshElementGLMapping& glMapping);
static void 		DeleteTextureGLResources(Texture* texture,
											OpenGLRenderer::TextureGLMapping& glMapping);
static void 		DeleteLinesGLResources(const vector<Line>& lines,
										  OpenGLRenderer::LinesGLMapping& glMapping);
static vector<Node*> 	SortedLights(map<Node*, float> lights);
static void 		DrawStatsOverlay(Stats& stats, const Scene& scene);
static void 		SetTextureMinificationFilter(GLuint glTextureHandle, bool cube, FilterMode mode);
static void 		SetTextureMagnificationFilter(GLuint glTextureHandle, bool cube, FilterMode mode);
static void 		SetTextureMaxAnisotropy(GLuint glTextureHandle, bool cube, float max);
static void 		SetTextureWrapS(GLuint glTextureHandle, bool cube, WrapMode mode);
static void 		SetTextureWrapT(GLuint glTextureHandle, bool cube, WrapMode mode);
static void 		SetTextureWrapR(GLuint glTextureHandle, WrapMode mode);
static GLenum 		GLFilterModeForFilterMode(FilterMode mode);
static GLenum 		GLWrapModeForWrapMode(WrapMode mode);
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
		Renderer{},
		_meshElementGLMapping{},
		_textureGLMapping{},
		_linesGLMapping{},
		_activeMeshElements{},
		_activeTextures{},
		_activeLines{},
		_glEnvironmentUBO{0},
		_overlayFont{} { }

OpenGLRenderer::~OpenGLRenderer() {
	A3D_LOG_D("Destroying OpenGLRenderer {:p}", static_cast<void*>(this));
	
	_activeMeshElements.clear();
	_activeTextures.clear();
	_activeLines.clear();
	
	glDeleteBuffers(1, &_glEnvironmentUBO);

	CleanupMeshElementResources(_activeMeshElements, _meshElementGLMapping);
	CleanupTextureResources(_activeTextures, _textureGLMapping);
	CleanupLinesResources(_activeLines, _linesGLMapping);
	
//#ifdef OPENGL_DESKTOP
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
//#endif
}
	
/*********************************************************************************************
	Renderer
 *********************************************************************************************/

RenderingApi OpenGLRenderer::renderingApi() const {
	return RenderingApi::OpenGL;
}

bool OpenGLRenderer::initialize(const RenderContext& context) {
	
	A3D_LOG_T("");
	
	// create environment UBO
	
	uint32 ubo;
	glGenBuffers(1, &ubo);
	_glEnvironmentUBO = ubo;

#ifdef OPENGL_DESKTOP
	
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
		
		ImFont* scp = io.Fonts->AddFontFromMemoryTTF(_overlayFont->buffer()->data(),
													 (int)_overlayFont->buffer()->size(),
													 fontSize,
													 &config);

		if (!scp) {
			throw Exception("Unable to load font: " + fontName + "." + fontType);
		}
	}

#endif // OPENGL_DESKTOP
	
	return true;
}
	
void OpenGLRenderer::beginFrame(const Scene& scene,
								const RenderContext& context,
								const DebugOptions& debugOptions,
								Stats& stats) {

	_activeMeshElements.clear();
	_activeTextures.clear();
	_activeLines.clear();
}

void OpenGLRenderer::endFrame(const Scene& scene,
							  const RenderContext& context,
							  const DebugOptions& debugOptions,
							  Stats& stats) {

	if (A3D_MASK_CONTAINS(debugOptions, DebugOptions::ShowStatsOverlay)) {
		DrawStatsOverlay(stats, scene);
	}

	CleanupMeshElementResources(_activeMeshElements, _meshElementGLMapping);
	CleanupTextureResources(_activeTextures, _textureGLMapping);
	CleanupLinesResources(_activeLines, _linesGLMapping);

	CheckGLError();
}

void OpenGLRenderer::render(const Scene& scene,
							const DebugOptions& debugOptions,
							Stats& stats) {

	auto renderContext = scene.visualWorld()->renderContext();

	auto framebufferWidth = renderContext->framebufferWidth();
	auto framebufferHeight = renderContext->framebufferHeight();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, framebufferWidth, framebufferHeight);

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	auto background = scene.visualWorld()->background();

	if (holds_alternative<shared_ptr<Texture>>(background)) {

		auto texture = get<shared_ptr<Texture>>(background);

		if (dynamic_pointer_cast<CubeImage>(texture->contents())) {

			if (auto pov = scene.visualWorld()->pointOfView().lock()) {

				auto skyboxMesh = scene.visualWorld()->skyboxMesh();

				RenderSkybox(*skyboxMesh,
							 *pov,
							 _meshElementGLMapping,
							 _textureGLMapping,
							 _activeTextures);

				// save reference for housekeeping
				_activeMeshElements.emplace(skyboxMesh->elements().front().get());
			}
			else {
				A3D_LOG_W("PointOfView has gone missing.");
			}
		}
	}
	else if (holds_alternative<shared_ptr<Color>>(background)) {

		auto color = get<shared_ptr<Color>>(background);

		glClearColor(color->r, color->g, color->b, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	SendEnvironmentUniforms(_glEnvironmentUBO, scene, stats);
	
	Program::Default().bindUniformBlock("EnvironmentBlock", _glEnvironmentUBO);
}

void OpenGLRenderer::render(Mesh& mesh,
							const mat4& modelMat,
							const mat4& viewMat,
							const mat4& projectionMat,
							const DebugOptions& debugOptions,
							Stats& stats) {

	if (A3D_MASK_CONTAINS(debugOptions, DebugOptions::ShowBoundingBoxes)) {

		// will check dirty bit, and create an AABB lines if necessary
		
//		GLuint vbo, vao;
//		GetMeshAABBLinesVertexDataHandles(mesh,
//										  _linesGLMapping,
//										  vbo, vao);

//		bool dirty = false;
//		auto& lines = mesh.aabbLines(dirty);
//		if (dirty) {
//			A3D_LOG_I("Mesh lines were dirty.");
//		}

		render(mesh.aabbLines(), modelMat, viewMat, projectionMat);
	}
}

void OpenGLRenderer::render(MeshElement& element,
							Material& material,
							const mat4& modelMat,
							const mat4& viewMat,
							const mat4& projectionMat,
							const DebugOptions& debugOptions,
							Stats& stats) {

	// check and load vertex data if necessary

	GLuint vbo, vao, ibo;
	GetMeshElementGLVertexDataHandles(element,
									  _meshElementGLMapping,
									  vbo, vao, ibo);

	auto wireframe = A3D_MASK_CONTAINS(debugOptions, DebugOptions::ShowWireframes);

	auto program = wireframe
			? Program::Wireframe()
			: Program::Default();
	
	// and load material contents if necessary

	auto glTextureHandles = map<MaterialPropertyType, GLuint>();
	GetTextureGLTextureHandles(material,
							   _textureGLMapping,
							   _activeTextures,
							   glTextureHandles);

	if (!wireframe) {

		// send material and material property uniforms
		SendMaterialUniforms(material, program, glTextureHandles);
		
		// update material property filtering options
		SetMaterialFilteringOptions(material, glTextureHandles);
	}
	
	// configure OpenGL state

	SetMaterialOpenGLState(material, debugOptions);

	// update

	DrawMeshElement(element, program, modelMat, viewMat, projectionMat, vao, ibo);

	// save reference for housekeeping

	_activeMeshElements.emplace(&element);
}
	
void OpenGLRenderer::render(const std::vector<Line>& lines,
							const glm::mat4& modelMat,
							const glm::mat4& viewMat,
							const glm::mat4& projectionMat) {
	
	auto program = Program::Lines();
	
	// check and load vertex data if necessary
	
	GLuint vbo, vao;
	GetLinesVertexDataHandles(lines,
							  program,
							  _linesGLMapping,
							  vbo, vao);

	// configure OpenGL state

	SetLinesGLState();
	
	// update

	DrawLines(lines,
			  program,
			  modelMat, viewMat, projectionMat,
			  vao);
	
	// save reference for housekeeping

	// TODO: this is slow
//	for (auto line : lines) {
		_activeLines.insert(&lines);
//	}
}

unique_ptr<Image> OpenGLRenderer::snapshot(const RenderContext& context) const {
	
	auto framebufferWidth = context.framebufferWidth();
	auto framebufferHeight = context.framebufferHeight();
	unsigned char pixelBuf[framebufferWidth * framebufferHeight * 4];
	glReadPixels(0, 0, framebufferWidth, framebufferHeight, GL_RGBA, GL_UNSIGNED_BYTE, pixelBuf);
	auto buffer = make_unique<Buffer>((std::byte*)pixelBuf, framebufferWidth * framebufferHeight * 4);
	return make_unique<Image>(std::move(buffer), framebufferWidth, framebufferHeight, 4);
}
	
/*********************************************************************************************
	Static
 *********************************************************************************************/

static void RenderSkybox(Mesh& skyboxMesh,
						 Node& pointOfView,
						 OpenGLRenderer::MeshElementGLMapping& elementGLMapping,
						 OpenGLRenderer::TextureGLMapping& textureGLMapping,
						 unordered_set<Texture*>& activeTextures) {
	
	auto program = Program::Skybox();
	
	auto& element = skyboxMesh.elements().front();
	auto material = skyboxMesh.materials().front();
	auto emissiveProperty = material->emission();
	
	// check and load vertex data if necessary
	
	GLuint vbo, vao, ibo;
	GetSkyboxGLVertexDataHandles(skyboxMesh,
								 elementGLMapping,
								 vbo, vao, ibo);
	
	// and load material contents if necessary
	
	auto glTextureHandles = map<MaterialPropertyType, GLuint>();
	GetTextureGLTextureHandles(*material,
							   textureGLMapping,
							   activeTextures,
							   glTextureHandles);
	auto emissiveGLTextureHandle = glTextureHandles[MaterialPropertyType::Emission];
	
	// send material property uniforms

	SendMaterialPropertyUniforms(emissiveProperty,
								 MaterialPropertyType::Emission,
								 emissiveGLTextureHandle,
								 program);
	
	// update material property filtering options

	if (holds_alternative<shared_ptr<Texture>>(emissiveProperty)) {
		auto texture = get<shared_ptr<Texture>>(emissiveProperty);
		SetTextureSamplingOptions(*texture, emissiveGLTextureHandle);
	}
	
	// configure OpenGL state
	
	SetSkyboxOpenGLState();
	
	// update
	
	DrawSkyboxElement(*element, program, pointOfView, vao, ibo);
}
	
static void GetMeshElementGLVertexDataHandles(MeshElement& element,
											  OpenGLRenderer::MeshElementGLMapping& glMapping,
											  GLuint& glVBO, GLuint& glVAO, GLuint& glIBO) {
	
	// looks up and populates glVBO, glVAO, and glIBO, loading the vertex data if needed

	if (A3D_MASK_CONTAINS(element.dirtyMask(), MeshElementDirtyMask::VertexData)) {

		DeleteMeshElementGLResources(&element, glMapping);

		BufferMeshElementVertexData(element, Program::Default(), glVBO, glVAO, glIBO);
		
		glMapping[&element] = make_tuple(glVBO, glVAO, glIBO);

		element.dirtyMask(A3D_MASK_REMOVE(element.dirtyMask(), MeshElementDirtyMask::VertexData));
	}
	else {
		auto mapping = glMapping[&element];
		glVBO = get<0>(mapping);
		glVAO = get<1>(mapping);
		glIBO = get<2>(mapping);
	}
}
	
static void GetSkyboxGLVertexDataHandles(Mesh& skyboxMesh,
										 OpenGLRenderer::MeshElementGLMapping& glMapping,
										 GLuint& glVBO, GLuint& glVAO, GLuint& glIBO) {
	
	// looks up and populates glVBO, glVAO, and glIBO, loading the vertex data if needed
	//
	// NOTE: this is essentially exactly the same as GetMeshElementGLVertexDataHandles()
	// except if the data needs to be loaded, it uses BufferMeshElementVertexData() as the
	// layout is different.  This will probaly need to be refacted in the future as more layouts are used
	
	auto& element = skyboxMesh.elements().front();

	if (A3D_MASK_CONTAINS(element->dirtyMask(), MeshElementDirtyMask::VertexData)) {

		DeleteMeshElementGLResources(element.get(), glMapping);
		
		BufferSkyboxVertexData(skyboxMesh, Program::Skybox(), glVBO, glVAO, glIBO);
		
		glMapping[element.get()] = make_tuple(glVBO, glVAO, glIBO);

		element->dirtyMask(A3D_MASK_REMOVE(element->dirtyMask(), MeshElementDirtyMask::VertexData));
	}
	else {
		auto mapping = glMapping[element.get()];
		glVBO = get<0>(mapping);
		glVAO = get<1>(mapping);
		glIBO = get<2>(mapping);
	}
}

//static void GetMeshAABBLinesVertexDataHandles(Mesh& mesh,
//											  OpenGLRenderer::LinesGLMapping& linesGLMapping,
//											  GLuint& glVBO, GLuint& glVAO) {
//
//	auto program = Program::Lines();
//
//	bool dirty = false;
//	auto lines = mesh.aabbLines(dirty);
//
//	if (dirty) {
//		DeleteLinesGLResources(lines, linesGLMapping);
//	}
//
//	GetLinesVertexDataHandles(lines,
//							  program,
//							  linesGLMapping,
//							  glVBO, glVAO);
//}

static void GetLinesVertexDataHandles(const vector<Line>& lines,
									  Program& program,
									  OpenGLRenderer::LinesGLMapping& glMapping,
									  GLuint& glVBO, GLuint& glVAO) {

	if (!glMapping.count(&lines)) {

		BufferLinesVertexData(lines, program, glVBO, glVAO);

		glMapping[&lines] = make_pair(glVBO, glVAO);
	}
	else {
		auto mapping = glMapping[&lines];
		glVBO = get<0>(mapping);
		glVAO = get<1>(mapping);
	}
}

static void GetTextureGLTextureHandles(Material& material,
									   OpenGLRenderer::TextureGLMapping& glMapping,
									   unordered_set<Texture*>& activeTextures,
									   map<MaterialPropertyType, GLuint>& glTextureHandles) {

	// looks up and populates glTextureHandle, loading the texture data if needed

	for (auto& [property, type] : material.properties()) {

		if (holds_alternative<shared_ptr<Texture>>(*property)) {

			auto texture = get<shared_ptr<Texture>>(*property).get();

			if (A3D_MASK_CONTAINS(texture->dirtyMask(), TextureDirtyMask::Contents)) {

				A3D_LOG_D("Texture {:p} contents dirty.", static_cast<void*>(texture));

				DeleteTextureGLResources(texture, glMapping);

				GLuint textureID = 0;
				BufferTexture(*texture, textureID);
				if (textureID > 0) {
					glTextureHandles[type] = textureID;
					glMapping[texture] = textureID;
				}

				texture->dirtyMask(A3D_MASK_REMOVE(texture->dirtyMask(), TextureDirtyMask::Contents));
			}
			else {
				auto textureHandle = glMapping[texture];
				glTextureHandles[type] = textureHandle;
			}

			// save reference for housekeeping
			activeTextures.emplace(texture);
		}
	}
}
	
static void BufferMeshElementVertexData(const MeshElement& element,
										Program& program,
										GLuint& glVBO, GLuint& glVAO, GLuint& glIBO) {
	
	A3D_LOG_D("Buffering vertex data for mesh element {:p}...", static_cast<const void*>(&element));
	
	program.use();
	
	auto verticies = element.vertices();
	auto faces = element.faces();
	
	glGenBuffers(1, &glVBO);
	glBindBuffer(GL_ARRAY_BUFFER, glVBO);
	glBufferData(GL_ARRAY_BUFFER,
				 (GLsizeiptr)(verticies.size()*sizeof(Vertex)),
				 &(verticies[0]),
				 GL_STATIC_DRAW);
	
	glGenVertexArrays(1, &glVAO);
	glBindVertexArray(glVAO);
	
	auto positionIndex = program.getAttributeLocation("vertex_position");
	glVertexAttribPointer(positionIndex, 			// attrib index
						  3, 						// num components per attrib (3 float in vec3)
						  GL_FLOAT, 				// component type
						  GL_FALSE, 				// normalize
						  sizeof(Vertex), 			// stride
						  nullptr); 						// start offset
	glEnableVertexAttribArray(positionIndex);
	
	auto normalIndex = program.getAttributeLocation("vertex_normal");
	glVertexAttribPointer(normalIndex, 				// attrib index
						  3, 						// num components per attrib (3 float in vec3)
						  GL_FLOAT, 				// component type
						  GL_FALSE, 				// normalize
						  sizeof(Vertex), 			// stride
						  (void*)sizeof(vec3)); 	// start offset
	glEnableVertexAttribArray(normalIndex);
	
	auto texCoordIndex = program.getAttributeLocation("texture_coordinate");
	glVertexAttribPointer(texCoordIndex, 							// attrib index
						  2, 										// num components per attrib (2 float in vec2)
						  GL_FLOAT, 								// component type
						  GL_FALSE, 								// normalize
						  sizeof(Vertex), 							// stride
						  (void*)(sizeof(vec3) + sizeof(vec3))); 	// start offset
	glEnableVertexAttribArray(texCoordIndex);
	
	glGenBuffers(1, &glIBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, glIBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,
				 (GLsizeiptr)(faces.size() * sizeof(Face)),
				 &(faces[0]),
				 GL_STATIC_DRAW);
	
	//program.unuse();
}

static void BufferSkyboxVertexData(Mesh& skyboxMesh,
								   Program& program,
								   GLuint& glVBO, GLuint& glVAO, GLuint& glIBO) {
	
	A3D_LOG_D("Buffering skybox vertex data...");
	
	program.use();
	
	auto& element = skyboxMesh.elements().front();
	auto verts = element->vertices();
	auto faces = element->faces();
	
	glGenBuffers(1, &glVBO);
	glBindBuffer(GL_ARRAY_BUFFER, glVBO);
	glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr)(verts.size()*sizeof(Vertex)), &(verts[0]), GL_STATIC_DRAW);
	
	glGenVertexArrays(1, &glVAO);
	glBindVertexArray(glVAO);

	GLuint positionIndex = program.getAttributeLocation("vertex_position");
	glVertexAttribPointer(positionIndex, // attrib index
						  3, // num components per attrib (3 float in vec3)
						  GL_FLOAT, // component type
						  GL_FALSE, // normalize
						  sizeof(Vertex), // stride
						  nullptr); // start offset
	glEnableVertexAttribArray(positionIndex);
	
	glGenBuffers(1, &glIBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, glIBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,
				 (GLsizeiptr)(faces.size() * sizeof(Face)),
				 &(faces[0]),
				 GL_STATIC_DRAW);
	
	//program.unuse();
}

static void BufferLinesVertexData(const vector<Line>& lines,
								  Program& program,
								  GLuint& glVBO, GLuint& glVAO) {
	A3D_LOG_D("Buffering vertex data for {} lines...", lines.size());

	// pack each Line into a vector with format <fromLocation, fromColor, toLocation, toColor>

	auto massagedBuffer = vector<vec3>();
	massagedBuffer.reserve(lines.size()*4);

	// TODO: stupid. change this.
	for (auto& line : lines) {
		auto fromLocation = line.fromLocation();
		const auto& fromColor = line.fromColor();
		auto toLocation = line.toLocation();
		const auto& toColor = line.toColor();
		massagedBuffer.push_back(fromLocation);
		massagedBuffer.push_back({fromColor->r, fromColor->g, fromColor->b});
		massagedBuffer.push_back(toLocation);
		massagedBuffer.push_back({toColor->r, toColor->g, toColor->b});
	}

	glGenBuffers(1, &glVBO);
	glBindBuffer(GL_ARRAY_BUFFER, glVBO);
	glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr)(massagedBuffer.size()*sizeof(vec3)), massagedBuffer.data(), GL_STATIC_DRAW);

	glGenVertexArrays(1, &glVAO);
	glBindVertexArray(glVAO);

	auto positionIndex = program.getAttributeLocation("vertex_position");
	glVertexAttribPointer(positionIndex, 		// attrib index
						  3, 					// num components per attrib (3 float in vec3)
						  GL_FLOAT, 			// component type
						  GL_FALSE, 			// normalize
						  sizeof(vec3)*2, 		// stride
						  nullptr); 					// start offset
	glEnableVertexAttribArray(positionIndex);

	auto colorIndex = program.getAttributeLocation("vertex_color");
	glVertexAttribPointer(colorIndex, 			// attrib index
						  3, 					// num components per attrib (3 float in vec3)
						  GL_FLOAT, 			// component type
						  GL_FALSE, 			// normalize
						  sizeof(vec3)*2, 		// stride
						  (void*)sizeof(vec3));	// start offset
	glEnableVertexAttribArray(colorIndex);
}

static void BufferTexture(const Texture &texture,
						  GLuint& glTextureHandle) {

	auto contents = texture.contents();

	if (dynamic_pointer_cast<CubeImage>(contents)) {

		A3D_LOG_D("Buffering cube texture {:p}...", static_cast<const void*>(&contents));
		
		auto cubeImage = dynamic_pointer_cast<CubeImage>(contents);
		
		Image* images[] = {
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
						 image->buffer().data());
		}

		auto sampler = texture.sampler();
		SetTextureMinificationFilter(glTextureHandle, true, sampler->minificationFilter());
		SetTextureMagnificationFilter(glTextureHandle, true, sampler->magnificationFilter());
		SetTextureMaxAnisotropy(glTextureHandle, true, sampler->maxAnisotropy());
		SetTextureWrapS(glTextureHandle, true, sampler->wrapS());
		SetTextureWrapT(glTextureHandle, true, sampler->wrapT());
		SetTextureWrapR(glTextureHandle, sampler->wrapR());
	}
	else if (dynamic_pointer_cast<Image>(contents)) {

		A3D_LOG_D("Buffering 2D texture {:p}...", static_cast<const void*>(&contents));
		
		auto image = dynamic_pointer_cast<Image>(contents);
		
		glGenTextures(1, &glTextureHandle);
		A3D_LOG_D("Binding new texture handle: {}", glTextureHandle);
		glBindTexture(GL_TEXTURE_2D, glTextureHandle);

//		unsigned bytesPerPixel = image->bytesPerPixel();
//		GLint glInternalFormat;
//		if (bytesPerPixel == 3) glInternalFormat = GL_RGB;
//		else if (bytesPerPixel == 1) glInternalFormat = GL_RED;

		A3D_LOG_D("Buffering image {:p}: width: {}, height: {}, bytesPerPixel: {}, data size: {}",
				 static_cast<void*>(image.get()), image->width(), image->height(), image->bytesPerPixel(),
				 image->width() * image->height() * image->bytesPerPixel());

		glTexImage2D(GL_TEXTURE_2D,
					 0,
					 GL_RGBA,//glInternalFormat,//GL_RGBA,//GL_SRGB_ALPHA,
					 image->width(),
					 image->height(),
					 0,
					 GL_RGBA,//(image->bytesPerPixel() == 3 ? GL_RGB : GL_RGBA),
					 GL_UNSIGNED_BYTE,
					 image->buffer().data());

		auto sampler = texture.sampler();
		SetTextureMinificationFilter(glTextureHandle, false, sampler->minificationFilter());
		SetTextureMagnificationFilter(glTextureHandle, false, sampler->magnificationFilter());
		SetTextureMaxAnisotropy(glTextureHandle, false, sampler->maxAnisotropy());
		SetTextureWrapS(glTextureHandle, false, sampler->wrapS());
		SetTextureWrapT(glTextureHandle, false, sampler->wrapT());
	}
}

static void SendMaterialUniforms(const Material& material,
								 Program& program,
								 map<MaterialPropertyType, GLuint>& glTextureHandles) {

	// sends uniforms for the Material, and MaterialProperties it has

	program.use();

	program.setUniform("specularExponent", material.specularExponent());
	program.setUniform("uvScale", material.uvScale());
	program.setUniform("locksAmbientWithDiffuse", material.locksAmbientWithDiffuse());
	program.setUniform("emissionMode", 0); // 0 = MaterialMode_None -- why is this here?

	for (auto& [property, type] : material.properties()) {

		if (!holds_alternative<monostate>(*property)) {

			SendMaterialPropertyUniforms(*property,
										 type,
										 glTextureHandles[type],
										 program);
		}
	}

	//program.unuse();
}

static void SendMaterialPropertyUniforms(const MaterialProperty& property,
										 MaterialPropertyType type,
										 GLuint glTextureHandle,
										 Program& program) {

	program.use();

	if (holds_alternative<shared_ptr<Texture>>(property)) {

		auto texture = get<shared_ptr<Texture>>(property);

		if (dynamic_pointer_cast<Image>(texture->contents())) {

			string modeUniformName;
			string samplerUniformName;
			GLenum slot;
			GLint index;

			switch (type) {
				case MaterialPropertyType::Ambient:
					modeUniformName = "ambientMode";
					samplerUniformName = "samplers.ambient";
					slot = GL_TEXTURE0;
					index = 0;
					break;
				case MaterialPropertyType::Diffuse:
					modeUniformName = "diffuseMode";
					samplerUniformName = "samplers.diffuse";
					slot = GL_TEXTURE1;
					index = 1;
					break;
				case MaterialPropertyType::Specular:
					modeUniformName = "specularMode";
					samplerUniformName = "samplers.specular";
					slot = GL_TEXTURE2;
					index = 2;
					break;
				case MaterialPropertyType::Emission:
					modeUniformName = "emissionMode";
					samplerUniformName = "samplers.emission";
					slot = GL_TEXTURE3;
					index = 3;
					break;
				default:
					cout << "Invalid MaterialPropertyType: "
						 << static_cast<underlying_type<MaterialPropertyType>::type>(type) << endl;
					return;
			}

			program.setUniform(modeUniformName.c_str(),
							   static_cast<underlying_type<MATERIAL_MODE>::type>(MATERIAL_MODE::SAMPLER));
			program.bindTexture(samplerUniformName.c_str(), GL_TEXTURE_2D, slot, glTextureHandle, index);
		}
		else if (dynamic_pointer_cast<CubeImage>(texture->contents())) {

				program.bindTexture("cubeSampler", GL_TEXTURE_CUBE_MAP, GL_TEXTURE0, glTextureHandle, 0);
		}

//		glActiveTexture(slot);
	}
	else if (holds_alternative<shared_ptr<Color>>(property)) {

		auto color = get<shared_ptr<Color>>(property);

		string modeUniformName;
		string colorUniformName;

		switch (type) {
			case MaterialPropertyType::Ambient:
				modeUniformName = "ambientMode";
				colorUniformName = "colors.ambient";
				break;
			case MaterialPropertyType::Diffuse:
				modeUniformName = "diffuseMode";
				colorUniformName = "colors.diffuse";
				break;
			case MaterialPropertyType::Specular:
				modeUniformName = "specularMode";
				colorUniformName = "colors.specular";
				break;
			case MaterialPropertyType::Emission:
				modeUniformName = "emissionMode";
				colorUniformName = "colors.emission";
				break;
			default:
				cout << "Invalid MaterialPropertyType: "
					 << static_cast<underlying_type<MaterialPropertyType>::type>(type) << endl;
				return;
		}

		program.setUniform(modeUniformName.c_str(),
						   static_cast<underlying_type<MATERIAL_MODE>::type>(MATERIAL_MODE::COLOR));
		program.setUniform(colorUniformName.c_str(), color->r, color->g, color->b);
	}
	else {
		A3D_LOG_W("NULL material property contents.");
	}
	
	//program.unuse();
}
	
static void SendEnvironmentUniforms(GLuint glEnvironmentUBO, const Scene& scene, Stats& stats) {

	// TODO: this is super greedy.  instead, accumulate a list of lights as we visit each node?
	
	// program "Default" must be active
	
	// lights
	
	auto lights = vector<Node*>();
	Node* ambientLightNode = nullptr;
	
	// find all lights in the scene
	for (auto& node : scene.rootNode()->children(true)) {
		if (!node->hidden()) {
			if (auto light = node->light()) {
				if (light->type() == LightType::Point) {
					lights.push_back(node.get());
				}
				else if (light->type() == LightType::Ambient) {
					ambientLightNode = node.get();
				}
			}
		}
	}
	
	if (lights.size() > MAX_DYNAMIC_LIGHTS) {

		// find all light distances from the camera

		auto lightsUnsorted = map<Node*, float>();
		auto cameraPos_world = scene.visualWorld()->pointOfView().lock()->worldPosition();
		for (auto lightNode : lights) {
			auto lightPos_world = lightNode->worldPosition();
			auto lightToCamera = lightPos_world - cameraPos_world;
			auto lightToCameraDistance = length(lightToCamera);
			lightsUnsorted[lightNode] = lightToCameraDistance;
		}

		lights = SortedLights(lightsUnsorted);

		unsigned endIndex = std::min((unsigned)lights.size(), (unsigned)(MAX_DYNAMIC_LIGHTS));
		auto first = lights.begin() + 0;
		auto last = lights.begin() + endIndex;
		auto lightsSlice = vector<Node*>(first, last);

		lights = lightsSlice;
	}

	if (ambientLightNode) lights.push_back(ambientLightNode);

	auto numLights = lights.size();
	LightGLSLStruct lightStruct[numLights];

	stats.lights = numLights - 1; // not counting ambient

	for (int l=0; l<numLights; ++l) {
		auto node = lights[l];
		auto light = node->light();

		// TODO: static_cast
		lightStruct[l].type = (int)(light->type());
		lightStruct[l].position_world = node->worldPosition();
		lightStruct[l].attenuationFactor = light->attenuationFactor();

		auto color = *light->color();
		lightStruct[l].color = {color.r, color.g, color.b};
	}

	// fog

	auto visualWorld = scene.visualWorld();
	FogGLSLStruct fogStruct;
	fogStruct.startDistance = visualWorld->fogStartDistance();
	fogStruct.endDistance = visualWorld->fogEndDistance();
	fogStruct.densityExponent = visualWorld->fogDensityExponent();
	fogStruct.startDistance = visualWorld->fogStartDistance();
	auto fogColor = visualWorld->fogColor();
	if (visualWorld->fogColor()) fogStruct.color = {fogColor->r,
													fogColor->g,
													fogColor->b,
													fogColor->a};
	else fogStruct.color = {0.0, 0.0, 0.0, 0.0};

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
	environmentBlock.numLights = (int)numLights;
	memcpy(&environmentBlock.lights, &lightStruct, sizeof(lightStruct));
	memcpy(&environmentBlock.fog, &fogStruct, sizeof(fogStruct));

	glBindBuffer(GL_UNIFORM_BUFFER, glEnvironmentUBO);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(environmentBlock), &environmentBlock, GL_DYNAMIC_DRAW);
}

static void SetTextureSamplingOptions(Texture& texture,
									  GLuint glTextureHandle) {

	auto sampler = texture.sampler();
	bool isCubemap = dynamic_pointer_cast<CubeImage>(texture.contents()) != nullptr;

	if (A3D_MASK_CONTAINS(sampler->dirtyMask(),
						  SamplerDirtyMask::MinificationFilter)) {
		SetTextureMinificationFilter(glTextureHandle, isCubemap, sampler->minificationFilter());
		sampler->dirtyMask(A3D_MASK_REMOVE(sampler->dirtyMask(),
										   SamplerDirtyMask::MinificationFilter));
	}

	if (A3D_MASK_CONTAINS(sampler->dirtyMask(),
						  SamplerDirtyMask::MagnificationFilter)) {
		SetTextureMagnificationFilter(glTextureHandle, isCubemap, sampler->magnificationFilter());
		sampler->dirtyMask(A3D_MASK_REMOVE(sampler->dirtyMask(),
										   SamplerDirtyMask::MagnificationFilter));
	}

	if (A3D_MASK_CONTAINS(sampler->dirtyMask(),
						  SamplerDirtyMask::WrapS)) {
		SetTextureWrapS(glTextureHandle, isCubemap, sampler->wrapS());
		sampler->dirtyMask(A3D_MASK_REMOVE(sampler->dirtyMask(),
										   SamplerDirtyMask::WrapS));
	}

	if (A3D_MASK_CONTAINS(sampler->dirtyMask(),
						  SamplerDirtyMask::WrapT)) {
		SetTextureWrapT(glTextureHandle, isCubemap, sampler->wrapT());
		sampler->dirtyMask(A3D_MASK_REMOVE(sampler->dirtyMask(),
										   SamplerDirtyMask::WrapT));
	}

	if (isCubemap) {
		if (A3D_MASK_CONTAINS(sampler->dirtyMask(),
							  SamplerDirtyMask::WrapR)) {
			SetTextureWrapR(glTextureHandle, sampler->wrapR());
			sampler->dirtyMask(A3D_MASK_REMOVE(sampler->dirtyMask(),
											   SamplerDirtyMask::WrapR));
		}
	}

	if (A3D_MASK_CONTAINS(sampler->dirtyMask(),
						  SamplerDirtyMask::MaxAnisotropy)) {
		SetTextureMaxAnisotropy(glTextureHandle, isCubemap, sampler->maxAnisotropy());
		sampler->dirtyMask(A3D_MASK_REMOVE(sampler->dirtyMask(),
										   SamplerDirtyMask::MaxAnisotropy));
	}
}

static void SetMaterialFilteringOptions(const Material& material,
										map<MaterialPropertyType, GLuint>& glTextureHandles) {

	for (auto& [property, type] : material.properties()) {

		if (holds_alternative<shared_ptr<Texture>>(*property)) {
			auto texture = get<shared_ptr<Texture>>(*property);
			SetTextureSamplingOptions(*texture, glTextureHandles[type]);
		}
	}
}
	
static void SetMaterialOpenGLState(const Material& material, 
								   const DebugOptions& debugOptions) {
	
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

	if (A3D_MASK_CONTAINS(debugOptions, DebugOptions::ShowPhysicsWireframes)) {
		// can create zbuffer problems
		// https://www.opengl.org/archives/resources/faq/technical/polygonoffset.htm
		//glDepthRange(0.1, 1.0);
		//		glEnable(GL_POLYGON_OFFSET_FILL);
		//		glPolygonOffset(20.0, 0.0);
	}
	
	if (A3D_MASK_CONTAINS(debugOptions, DebugOptions::ShowWireframes)) {
		//_program = Program::Wireframe();
		
#ifdef OPENGL_DESKTOP
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glEnable(GL_LINE_SMOOTH);
#endif
	}
	else {
#ifdef OPENGL_DESKTOP
		if (material.fillMode() == FillMode::Lines) {
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		}
		else if (material.fillMode() == FillMode::Points) {
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
#ifdef OPENGL_DESKTOP
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
#endif
	glDisable(GL_CULL_FACE);
//	glDisable(GL_BLEND);
}

static void SetLinesGLState() {
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glDepthMask(GL_TRUE);
#ifdef OPENGL_DESKTOP
	glEnable(GL_LINE_SMOOTH);
#endif
	
	// https://www.opengl.org/archives/resources/faq/technical/polygonoffset.htm
	//glDepthRange(0.0, 0.9);
//	glDisable(GL_POLYGON_OFFSET_FILL);
//	glPolygonOffset(0.0, 0.0);
}

static void DrawMeshElement(MeshElement& element,
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
	auto numFaces = element.faces().size();
	glDrawElements(GL_TRIANGLES, (GLsizei)numFaces*3, GL_UNSIGNED_INT, nullptr);
}

static void DrawSkyboxElement(MeshElement& element,
							  Program& program,
							  Node& pointOfView,
							  GLuint vao, GLuint ibo) {
	
	program.use();

	auto viewMat = lookAt({0.0f, 0.0f, 0.0f}, // eye - location
						  pointOfView.worldForward(), // center - look at
						  pointOfView.worldUp()); // up
	
	auto projectionMat = pointOfView.camera()->projection();
	
	program.setUniform("view", viewMat);
	program.setUniform("projection", projectionMat);
	
	// update

	auto& faces = element.faces();
	
	glBindVertexArray(vao);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	auto numFaces = faces.size();
	glDrawElements(GL_TRIANGLES, (GLsizei)numFaces*3, GL_UNSIGNED_INT, nullptr);
}

static void DrawLines(const vector<Line>& lines,
					  Program& program,
					  mat4 modelMat,
					  mat4 viewMat,
					  mat4 projectionMat,
					  GLuint glVAO) {
	
	program.use();
	
	// uniforms
	
	program.setUniform("model", modelMat);
	program.setUniform("view", inverse(viewMat));
	program.setUniform("projection", projectionMat);
	
	// update
	
	glBindVertexArray(glVAO);
	glDrawArrays(GL_LINES, 0, (GLsizei)lines.size() * 4);
}

static void CleanupMeshElementResources(unordered_set<MeshElement*>& active,
										OpenGLRenderer::MeshElementGLMapping& glMapping) {
#ifndef DISABLE_RESOURCE_MANAGEMENT
	
	// gather sorted vector of elements used this frame
	auto activeElementsSorted = vector<MeshElement*>();
	activeElementsSorted.reserve(active.size());
	copy(active.begin(), active.end(), back_inserter(activeElementsSorted));
	sort(activeElementsSorted.begin(), activeElementsSorted.end());
	
	// gather sorted vector of elements in the mapping
	auto storedElementsSorted = vector<MeshElement*>();
	storedElementsSorted.reserve(glMapping.size());
	for (auto it = glMapping.begin(); it != glMapping.end(); ++it) {
		storedElementsSorted.push_back(it->first);
	}
	sort(storedElementsSorted.begin(), storedElementsSorted.end());
	
	// find unused elements
	auto unused = vector<MeshElement*>(storedElementsSorted.size());
	vector<MeshElement*>::iterator it;
	it = set_difference(storedElementsSorted.begin(), storedElementsSorted.end(),
						activeElementsSorted.begin(), activeElementsSorted.end(),
						unused.begin());
	unused.resize(it - unused.begin());
	
	// deallocate unused elements
	if (!unused.empty()) {
		//A3D_LOG_D("Deleting GL resources for {} mesh elements...", unused.size());
		
		for (it=unused.begin(); it!=unused.end(); ++it) {
			MeshElement* element = *it;
			DeleteMeshElementGLResources(element, glMapping);
		}
	}
	
#endif
}
	
static void CleanupTextureResources(unordered_set<Texture*> &active,
									OpenGLRenderer::TextureGLMapping& glMapping) {
#ifndef DISABLE_RESOURCE_MANAGEMENT
	
	// gather sorted vector of properties used this frame
	auto activeTexturesSorted = vector<Texture*>();
	activeTexturesSorted.reserve(glMapping.size());
	copy(active.begin(), active.end(), back_inserter(activeTexturesSorted));
	sort(activeTexturesSorted.begin(), activeTexturesSorted.end());
	
	// gather sorted vector of properties in the mapping
	auto storedTexturesSorted = vector<Texture*>();
	storedTexturesSorted.reserve(glMapping.size());
	for (auto it = glMapping.begin(); it != glMapping.end(); ++it) {
		storedTexturesSorted.push_back(it->first);
	}
	sort(storedTexturesSorted.begin(), storedTexturesSorted.end());
	
	// find unused properties
	auto unused = vector<Texture*>(storedTexturesSorted.size());
	vector<Texture*>::iterator it;
	it = set_difference(storedTexturesSorted.begin(), storedTexturesSorted.end(),
						activeTexturesSorted.begin(), activeTexturesSorted.end(),
						unused.begin());
	unused.resize(it - unused.begin());
	
	// deallocate unused properties
	if (!unused.empty()) {
		//A3D_LOG_D("Deleting GL resources for {} textures...", unused.size());
		
		for (it=unused.begin(); it!=unused.end(); ++it) {
			Texture* texture = *it;
			DeleteTextureGLResources(texture, glMapping);
		}
	}
	
#endif
}

// this has to be the slowest way on earth to do this.
static void CleanupLinesResources(unordered_set<const vector<Line>*>& active,
								  OpenGLRenderer::LinesGLMapping& glMapping) {
#ifndef DISABLE_RESOURCE_MANAGEMENT
	
//	// gather sorted vector of Lines used this frame
//	auto activeLineSetsSorted = vector<Line*>();
//	activeLineSetsSorted.reserve(glMapping.size());
//	copy(active.begin(), active.end(), back_inserter(activeLineSetsSorted));
//	sort(activeLineSetsSorted.begin(), activeLineSetsSorted.end());
//
//	// gather sorted vector of Lines in the mapping
//	auto storedLineSetsSorted = vector<Line*>();
//	storedLineSetsSorted.reserve(glMapping.size());
//	for (auto it = glMapping.begin(); it != glMapping.end(); ++it) {
//		storedLineSetsSorted.push_back(it->first);
//	}
//	sort(storedLineSetsSorted.begin(), storedLineSetsSorted.end());
//
//	// find unused LineSets
//	auto unused = vector<Line*>(storedLineSetsSorted.size());
//	vector<Line*>::iterator it;
//	it = set_difference(storedLineSetsSorted.begin(), storedLineSetsSorted.end(),
//						activeLineSetsSorted.begin(), activeLineSetsSorted.end(),
//						unused.begin());
//	unused.resize(it - unused.begin());
//
//	// deallocate unused LineSets
//	if (unused.size()) {
//		//A3D_LOG_D("Deleting GL resources for {} line sets...", unused.size());
//
//		for (it=unused.begin(); it!=unused.end(); ++it) {
//			Line* lines = *it;
//			DeleteLinesGLResources(lines, glMapping);
//		}
//	}

	// gather sorted vector of Lines used this frame
	auto activeSorted = vector<const vector<Line>*>();
	activeSorted.reserve(glMapping.size());
	copy(active.begin(), active.end(), back_inserter(activeSorted));
	sort(activeSorted.begin(), activeSorted.end());

	// gather sorted vector of Lines in the mapping
	auto allSorted = vector<const vector<Line>*>();
	allSorted.reserve(glMapping.size());
	for (auto [lines, glRes] : glMapping) {
		allSorted.push_back(lines);
	}
	sort(allSorted.begin(), allSorted.end());

	//A3D_LOG_D("allSorted: {}", allSorted.size());
	//A3D_LOG_D("activeSorted: {}", activeSorted.size());

	auto unused = vector<const vector<Line>*>();
	vector<const vector<Line>*>::iterator it;
	it = set_difference(allSorted.begin(), allSorted.end(),
						activeSorted.begin(), activeSorted.end(),
						unused.begin());
	unused.resize(it - unused.begin());

	//A3D_LOG_D("unused: {}", unused.size());

	if (unused.size()) {
		//A3D_LOG_D("Deleting GL resources for {} line sets...", unused.size());

		for (auto lines : unused) {
			DeleteLinesGLResources(*lines, glMapping);
		}
	}
	
#endif
}

static void DeleteMeshElementGLResources(MeshElement* element,
										 OpenGLRenderer::MeshElementGLMapping& glMapping) {
#ifndef DISABLE_RESOURCE_MANAGEMENT
	
	if (glMapping.count(element)) {
		
		A3D_LOG_D("Deleting GL resources for MeshElement {:p}...", static_cast<void*>(element));
		
		auto glHandles = glMapping[element];
		
		GLuint vbo = get<0>(glHandles);
		GLuint vao = get<1>(glHandles);
		GLuint ibo = get<2>(glHandles);
		
		glDeleteBuffers(1, &vbo);
		glDeleteVertexArrays(1, &vao);
		glDeleteBuffers(1, &ibo);
		
		glMapping.erase(element);

		element->dirtyMask(A3D_MASK_REMOVE(element->dirtyMask(),
										  MeshElementDirtyMask::VertexData));
	}
	
#endif
}

static void DeleteTextureGLResources(Texture* texture,
									 OpenGLRenderer::TextureGLMapping& glMapping) {
#ifndef DISABLE_RESOURCE_MANAGEMENT
	
	if (glMapping.count(texture)) {
		
		A3D_LOG_D("Deleting GL resources for MaterialProperty {:p}...", static_cast<void*>(texture));

		auto handle = glMapping[texture];
		
		glDeleteTextures(1, &handle);

		glMapping.erase(texture);

		texture->dirtyMask(A3D_MASK_REMOVE(texture->dirtyMask(),
										  TextureDirtyMask::All));
	}

#endif
}

static void DeleteLinesGLResources(const vector<Line>& lines,
									 OpenGLRenderer::LinesGLMapping& glMapping) {
#ifndef DISABLE_RESOURCE_MANAGEMENT
	
	if (glMapping.count(&lines)) {

		A3D_LOG_T("Deleting GL resources for Lines {:p}..", static_cast<const void*>(&lines));

		auto glHandles = glMapping[&lines];

		auto vbo = get<0>(glHandles);
		auto vao = get<1>(glHandles);

		glDeleteBuffers(1, &vbo);
		glDeleteVertexArrays(1, &vao);

		glMapping.erase(&lines);
	}
	
#endif
}

static vector<Node*> SortedLights(map<Node*, float> lights) {
	// map: <node, distance from camera>
	
	// http://thispointer.com/how-to-sort-a-map-by-value-in-c/
	
	typedef function<bool(pair<Node*, float>, pair<Node*, float>)> Comparator;
	
	Comparator compFunctor = [](pair<Node*, float> elem1, pair<Node*, float> elem2) {
		return elem1.second < elem2.second;
	};

	set<pair<Node*, float>, Comparator> lightsSorted(lights.begin(),
													 lights.end(),
													 compFunctor);
	
	auto sortedVector = vector<Node*>();
	for (pair<Node*, float> element : lightsSorted) {
		sortedVector.push_back(element.first);
	}
	
	return sortedVector;
}
	
void DrawStatsOverlay(Stats& stats, const Scene& scene) {

	using namespace ImGui;

//#ifdef OPENGL_DESKTOP

	auto renderContext = scene.visualWorld()->renderContext();

	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	NewFrame();

	ImGuiWindowFlags windowFlags = 0;
	windowFlags |= ImGuiWindowFlags_NoTitleBar;
	windowFlags |= ImGuiWindowFlags_NoScrollbar;
//	windowFlags |= ImGuiWindowFlags_NoMove;
	windowFlags |= ImGuiWindowFlags_NoResize;
	windowFlags |= ImGuiWindowFlags_NoCollapse;
	windowFlags |= ImGuiWindowFlags_NoNav;
	windowFlags |= ImGuiWindowFlags_AlwaysAutoResize;

	SetNextWindowBgAlpha(.25);
	Begin("Stats", nullptr, windowFlags);
	ImGuiStyle& style = ImGui::GetStyle();
	style.WindowBorderSize = 0;
//	style.WindowRounding = 6;

//	ImGui::ShowDemoWindow(nullptr);
//	ImGui::GetIO();

	ImGui::SetWindowPos((ImVec2){10.0, 10.0});

	if (renderContext->recordingGIF()) {
		auto numFrames = renderContext->recordedGIFFrames();
		Text("%-14s %.2f ms\n" \
					"%-14s %.2f ms\n" \
					"%-14s %.2f ms\n" \
					"%-14s %.2f ms\n" \
					"%-14s %.0f fps %s\n" \
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

			 "frametime", stats.averageFrametime,
			 " draw", stats.averageDrawtime,
			 " physics", stats.averagePhysicstime,
			 " user", stats.averageUsertime,
			 "framerate", stats.averageFramerate, (renderContext->vSyncEnabled() ? "[vsync]" : ""),

			 "nodes", stats.nodes,
			 "meshes", stats.meshes,
			 "elements", stats.elements,
			 "polygons", stats.polygons,
			 "lights", stats.lights,

			 "physics bodies", stats.dynamicBodies + stats.kinematicBodies + stats.staticBodies,
			 " static", stats.staticBodies,
			 " dynamic", stats.dynamicBodies,
			 " kinematic", stats.kinematicBodies,
			 "physics shapes", stats.concavePolyhedronShapes + stats.boundingBoxShapes + stats.convexHullShapes,
			 " bounding box", stats.boundingBoxShapes,
			 " convex hull", stats.convexHullShapes,
			 " concave polyh", stats.concavePolyhedronShapes,

			 "camera pos", stats.cameraPosition.x, stats.cameraPosition.y, stats.cameraPosition.z,
			 "RECORDING", numFrames, (numFrames==1 ? "frame" : "frames"));
	}
	else {
		Text("%-14s %.2f ms\n" \
					"%-14s %.2f ms\n" \
					"%-14s %.2f ms\n" \
					"%-14s %.2f ms\n" \
					"%-14s %.0f fps %s\n" \
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

			 "frametime", stats.averageFrametime,
			 " draw", stats.averageDrawtime,
			 " physics", stats.averagePhysicstime,
			 " user", stats.averageUsertime,
			 "framerate", stats.averageFramerate, (renderContext->vSyncEnabled() ? "[vsync]" : ""),

			 "nodes", stats.nodes,
			 "meshes", stats.meshes,
			 "elements", stats.elements,
			 "polygons", stats.polygons,
			 "lights", stats.lights,

			 "physics bodies", stats.dynamicBodies + stats.kinematicBodies + stats.staticBodies,
			 " static", stats.staticBodies,
			 " dynamic", stats.dynamicBodies,
			 " kinematic", stats.kinematicBodies,
			 "physics shapes", stats.concavePolyhedronShapes + stats.boundingBoxShapes + stats.convexHullShapes,
			 " bounding box", stats.boundingBoxShapes,
			 " convex hull", stats.convexHullShapes,
			 " concave polyh", stats.concavePolyhedronShapes,

			 "camera pos", stats.cameraPosition.x, stats.cameraPosition.y, stats.cameraPosition.z);
	}

	End();
	Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

//#endif // OPENGL_DESKTOP
}

static void SetTextureMinificationFilter(GLuint glTextureHandle, bool cube, FilterMode mode) {
	
	auto texType = (cube ? GL_TEXTURE_CUBE_MAP : GL_TEXTURE_2D);

	switch (mode) {
		case FilterMode::NearestMipmapNearest:
		case FilterMode::NearestMipmapLinear:
		case FilterMode::LinearMipmapNearest:
		case FilterMode::LinearMipmapLinear:
			glGenerateMipmap(texType);
			break;
		default:
			break;
	}

	glBindTexture(texType, glTextureHandle);
	glTexParameteri(texType, GL_TEXTURE_MIN_FILTER, (GLint)GLFilterModeForFilterMode(mode));
}

static void SetTextureMagnificationFilter(GLuint glTextureHandle, bool cube, FilterMode mode) {

	auto texType = (cube ? GL_TEXTURE_CUBE_MAP : GL_TEXTURE_2D);

	switch (mode) {
		case FilterMode::Nearest:
		case FilterMode::Linear:
			glBindTexture(texType, glTextureHandle);
			glTexParameteri(texType, GL_TEXTURE_MAG_FILTER, (GLint)GLFilterModeForFilterMode(mode));
			break;
		default:
			A3D_LOG_W("Unsupported magnification filter: {}", magic_enum::enum_name(mode));
		break;
	}
}

static void SetTextureMaxAnisotropy(GLuint glTextureHandle, bool cube, float max) {
#ifdef OPENGL_DESKTOP

	auto texType = (cube ? GL_TEXTURE_CUBE_MAP : GL_TEXTURE_2D);
	
	float anisotropy = max;
	glBindTexture(texType, glTextureHandle);
	float largest;
	glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &largest);
	if (max > largest) anisotropy = largest;
	glTexParameterf(texType, GL_TEXTURE_MAX_ANISOTROPY_EXT, anisotropy);

#endif
}

static void SetTextureWrapS(GLuint glTextureHandle, bool cube, WrapMode mode) {

	auto texType = (cube ? GL_TEXTURE_CUBE_MAP : GL_TEXTURE_2D);
	
	glBindTexture(texType, glTextureHandle);
	glTexParameteri(texType, GL_TEXTURE_WRAP_S, (GLint)GLWrapModeForWrapMode(mode));
}

static void SetTextureWrapT(GLuint glTextureHandle, bool cube, WrapMode mode) {

	auto texType = (cube ? GL_TEXTURE_CUBE_MAP : GL_TEXTURE_2D);
	
	glBindTexture(texType, glTextureHandle);
	glTexParameteri(texType, GL_TEXTURE_WRAP_T, (GLint)GLWrapModeForWrapMode(mode));
}

static void SetTextureWrapR(GLuint glTextureHandle, WrapMode mode) {
	
	glBindTexture(GL_TEXTURE_CUBE_MAP, glTextureHandle);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, (GLint)GLWrapModeForWrapMode(mode));
}

static GLenum GLFilterModeForFilterMode(FilterMode mode) {
	switch (mode) {
		case FilterMode::Nearest: 				return GL_NEAREST;
		case FilterMode::Linear: 				return GL_LINEAR;
		case FilterMode::NearestMipmapNearest:	return GL_NEAREST_MIPMAP_NEAREST;
		case FilterMode::LinearMipmapNearest: 	return GL_LINEAR_MIPMAP_NEAREST;
		case FilterMode::NearestMipmapLinear: 	return GL_NEAREST_MIPMAP_LINEAR;
		case FilterMode::LinearMipmapLinear: 	return GL_LINEAR_MIPMAP_LINEAR; }
}

static GLenum GLWrapModeForWrapMode(WrapMode mode) {
	switch (mode) {
		case WrapMode::ClampToEdge:				return GL_CLAMP_TO_EDGE;
//#ifdef OPENGL_DESKTOP
//		case WRAP_MODE::CLAMP_TO_BORDER:		return GL_CLAMP_TO_BORDER;
//#endif
		case WrapMode::Repeat:					return GL_REPEAT;
        default: /* MIRRORED_REPEAT */   		return GL_MIRRORED_REPEAT; }
}

static void CheckGLError() {
	auto err = glGetError();
	if (err != GL_NO_ERROR) {
		A3D_LOG_E("*** GL error: 0x{:X} ***", err);
	}
}
