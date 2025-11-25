//
//  OpenGLRenderer.cc
//  avara3d
//
//  Created by Morgan Davis on 4/24/18.
//  Copyright © 2024 Morgan K Davis. All rights reserved.
//

#include "a3d/rendering/renderer/opengl/OpenGLRenderer.h"

#include <algorithm>
#include <iostream>
#include <set>
#include <utility>
#include <vector>

#ifdef OPENGL_ES
#include <EGL/egl.h>
#include <GLES3/gl3.h>
#else
#include "glad/glad.h"
#endif

//#ifdef OPENGL_CORE
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
//#endif

#include "magic_enum.hpp"

#include "a3d/Buffer.h"
#include "a3d/BuildInfo.h"
#include "a3d/Casting.h"
#include "a3d/Color.h"
#include "a3d/Configuration.h"
#include "a3d/CubeImage.h"
#include "a3d/Font.h"
#include "a3d/Image.h"
#include "a3d/RTTI.h"
#include "a3d/Utilities.h"
#include "a3d/diagnostic/exception/Exception.h"
#include "a3d/diagnostic/logging/Logger.h"
#include "a3d/mesh/Mesh.h"
#include "a3d/mesh/MeshElement.h"
#include "a3d/mesh/Line.h"
#include "a3d/rendering/VisualWorld.h"
#include "a3d/rendering/camera/Camera.h"
#include "a3d/rendering/context/RenderContext.h"
#include "a3d/rendering/context/Window.h"
#include "a3d/rendering/light/AmbientLight.h"
#include "a3d/rendering/light/DirectionalLight.h"
#include "a3d/rendering/light/Light.h"
#include "a3d/rendering/light/PointLight.h"
#include "a3d/rendering/light/SpotLight.h"
#include "a3d/rendering/material/Material.h"
#include "a3d/rendering/material/Sampler.h"
#include "a3d/rendering/material/Texture.h"
#include "a3d/rendering/renderer/opengl/Program.h"
#include "a3d/scene/Node.h"
#include "a3d/scene/Scene.h"


using namespace a3d;
using namespace glm;
using namespace std;


//#define DISABLE_RESOURCE_MANAGEMENT


/*********************************************************************************************
	 Private Constant Definitions
 *********************************************************************************************/

const std::string 	OpenGLRenderer::STATS_TITLE_FONT_NAME = 		"Take cover";
const std::string 	OpenGLRenderer::STATS_TITLE_FONT_TYPE = 		"ttf";
const float 		OpenGLRenderer::STATS_TITLE_FONT_SIZE =			21.0;
const std::string 	OpenGLRenderer::STATS_BODY_FONT_NAME = 			"SourceCodePro-Semibold";
const std::string 	OpenGLRenderer::STATS_BODY_FONT_TYPE = 			"otf";
const float 		OpenGLRenderer::STATS_BODY_FONT_SIZE =			15.0;
const float 		OpenGLRenderer::STATS_TITLE_TO_BODY_PADDING =	0.0;

/*********************************************************************************************
	Private Types
 *********************************************************************************************/

enum class MaterialContentsType : unsigned {
	None = 		0,
	Color = 	1,
	Sampler = 	2
};

typedef struct {
	vec4		color;
} AmbientLightGLSLStruct;

typedef struct {
	vec4		color;
	vec3		direction_world;
	float32_t	PAD0_;
} DirectionalLightGLSLStruct;

typedef struct {
	vec4		color;
	vec3		position_world;
	float32_t	PAD0_;
	float32_t	constantAttenuation;
	float32_t	linearAttenuation;
	float32_t	quadraticAttenuation;
	float32_t	PAD1_;
} PointLightGLSLStruct;

typedef struct {
	vec4		color;
	vec3		position_world;
	float32_t	PAD0_;
	vec3		direction_world;
	float32_t	PAD1_;
	float32_t	innerAngleCos;
	float32_t	outerAngleCos;
	uint32_t	featheringMode;
	float32_t	constantAttenuation;
	float32_t	linearAttenuation;
	float32_t	quadraticAttenuation;
	float32_t	PAD2_;
	float32_t	PAD3_;
} SpotLightGLSLStruct;

typedef struct {
	vec4		color;
	float32_t	startDistance;
	float32_t	endDistance;
	float32_t	densityExponent;
	float32_t	PAD0_;
} FogGLSLStruct;

typedef struct {
	alignas(16)	uint32_t 					numAmbientLights;
	alignas(16) AmbientLightGLSLStruct		ambientLights[MAX_AMBIENT_LIGHTS];
	alignas(16) uint32_t 					numDirectionalLights;
	alignas(16) DirectionalLightGLSLStruct	directionalLights[MAX_DIRECTIONAL_LIGHTS];
	alignas(16) uint32_t 					numPointLights;
	alignas(16) PointLightGLSLStruct		pointLights[MAX_POINT_LIGHTS];
	alignas(16) uint32_t 					numSpotLights;
	alignas(16) SpotLightGLSLStruct			spotLights[MAX_SPOT_LIGHTS];
	alignas(16) FogGLSLStruct				fog;
} EnvironmentBlock;

/*********************************************************************************************
	Private Static Non-Member Prototypes
 *********************************************************************************************/

static void 		RenderSkybox(Mesh& skyboxMesh,
								Node& pointOfView,
								OpenGLRenderer::MeshElementGLMapping& elementGLMapping,
								OpenGLRenderer::TextureGLMapping& textureGLMapping,
								unordered_set<Texture*>& activeTextures);
static void 		GetMeshElementGLVertexDataHandles(MeshElement& element,
													 OpenGLRenderer::MeshElementGLMapping& glMapping,
													 GLuint& glVBO, GLuint& glVAO, GLuint& glEBO);
static void 		GetSkyboxGLVertexDataHandles(Mesh& skyboxMesh,
												OpenGLRenderer::MeshElementGLMapping& glMapping,
												GLuint& glVBO, GLuint& glVAO, GLuint& glEBO);
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
											   GLuint& glVBO, GLuint& glVAO, GLuint& glEBO);
static void 		BufferSkyboxVertexData(Mesh& skyboxMesh,
										  Program& program,
										  GLuint& glVBO, GLuint& glVAO, GLuint& glEBO);
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
static void 		SendEnvironmentUniforms(GLuint glEnvironmentUBO,
											const Scene& scene,
											const vector<Node*>& lightNodes,
											Stats& stats);
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
								   const mat4& modelMat,
								   const mat4& viewMat,
								   const mat4& projectionMat,
								   GLuint vao, GLuint ebo);
static void 		DrawSkyboxElement(MeshElement& element,
									 Program& program,
									 Node& pointOfView,
									 GLuint vao, GLuint ebo);
static void 		DrawLines(const vector<Line>& lines,
							 Program& program,
							 const mat4& modelMat,
							 const mat4& viewMat,
							 const mat4& projectionMat,
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
static void			InitImgui(const RenderContext& context);
void 				UpdateImguiScale(const RenderContext& context, const Font& overLayFont, const Font& bodyFont);
void 				AddImguiFont(const RenderContext& context, const Font& font, float size);
static void 		DrawStatsOverlay(Stats& stats, const RenderContext& context);
static string 		StatusOverlayDescriptionForAntialiasingMode(AntialiasingMode mode);
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
	Internal Lifecycle Functions
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
		_overlayTitleFont{},
		_overlayBodyFont{} { }

OpenGLRenderer::~OpenGLRenderer() {
	A3D_LOG_D("Destroying OpenGLRenderer {:p}", static_cast<void*>(this));
	
	_activeMeshElements.clear();
	_activeTextures.clear();
	_activeLines.clear();
	
	glDeleteBuffers(1, &_glEnvironmentUBO);

	CleanupMeshElementResources(_activeMeshElements, _meshElementGLMapping);
	CleanupTextureResources(_activeTextures, _textureGLMapping);
	CleanupLinesResources(_activeLines, _linesGLMapping);

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}
	
/*********************************************************************************************
	Renderer Internal Member Functions
 *********************************************************************************************/

RenderingApi OpenGLRenderer::renderingApi() const {
	return RenderingApi::OpenGL;
}

bool OpenGLRenderer::initialize(const RenderContext& context) {
	
	A3D_LOG_I("");

	// create environment UBO
	
	uint32 ubo;
	glGenBuffers(1, &ubo);
	_glEnvironmentUBO = ubo;

	// setup Imgui

	InitImgui(context);

	_overlayTitleFont = utils::FontNamed(STATS_TITLE_FONT_NAME, STATS_TITLE_FONT_TYPE);
	if (_overlayTitleFont->buffer()->size()) {
		_overlayBodyFont = utils::FontNamed(STATS_BODY_FONT_NAME, STATS_BODY_FONT_TYPE);
		if (_overlayBodyFont->buffer()->size()) {
			UpdateImguiScale(context, *_overlayTitleFont, *_overlayBodyFont);
		}
		else {
			A3D_LOG_E("Unable to load font: {}.{}", STATS_TITLE_FONT_NAME, STATS_TITLE_FONT_TYPE);
		}
	}
	else {
		A3D_LOG_E("Unable to load font: {}.{}", STATS_TITLE_FONT_NAME, STATS_TITLE_FONT_TYPE);
	}



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
		DrawStatsOverlay(stats, context);
	}

	CleanupMeshElementResources(_activeMeshElements, _meshElementGLMapping);
	CleanupTextureResources(_activeTextures, _textureGLMapping);
	CleanupLinesResources(_activeLines, _linesGLMapping);

	CheckGLError();
}

void OpenGLRenderer::preTraversal(const Scene& scene,
								  const RenderContext& context,
								  const DebugOptions& debugOptions,
								  Stats& stats) {

}

void OpenGLRenderer::postTraversal(const Scene& scene,
								   const RenderContext& context,
								   const vector<Node*>& lightNodes,
								   const DebugOptions& debugOptions,
								   Stats& stats) {

	SendEnvironmentUniforms(_glEnvironmentUBO, scene, lightNodes, stats);
	Program::Default().bindUniformBlock("EnvironmentBlock", _glEnvironmentUBO);
}

void OpenGLRenderer::render(const Scene& scene,
							const DebugOptions& debugOptions,
							Stats& stats) {

	auto renderContext = scene.visualWorld()->renderContext();

	auto framebufferSize = renderContext->framebufferSize();
	auto framebufferWidth = framebufferSize.x;
	auto framebufferHeight = framebufferSize.y;

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, (GLsizei)framebufferWidth, (GLsizei)framebufferHeight);

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	auto background = scene.visualWorld()->background();

	std::visit([this, &scene](auto&& background) -> void {

		using T = std::decay_t<decltype(background)>;

		if constexpr (std::is_same_v<T, shared_ptr<Texture>>) {

			if (auto cubeImage = get_if<shared_ptr<CubeImage>>(&(background->contents()))) {

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
					// TODO: throw?
				}
			}
			else {
				A3D_LOG_E("Image background not supported.");
			}

//			if (dynamic_pointer_cast<CubeImage>(background->contents())) {
//
//				if (auto pov = scene.visualWorld()->pointOfView().lock()) {
//
//					auto skyboxMesh = scene.visualWorld()->skyboxMesh();
//
//					RenderSkybox(*skyboxMesh,
//								 *pov,
//								 _meshElementGLMapping,
//								 _textureGLMapping,
//								 _activeTextures);
//
//					// save reference for housekeeping
//					_activeMeshElements.emplace(skyboxMesh->elements().front().get());
//				}
//				else {
//					A3D_LOG_W("PointOfView has gone missing.");
//					// TODO: throw?
//				}
//			}
//			else {
//				A3D_LOG_E("Image background not supported.");
//			}
		}
		else if constexpr (std::is_same_v<T, shared_ptr<Color>>) {

			glClearColor(background->r(), background->g(), background->b(), 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		}
		else if constexpr (std::is_same_v<T, std::monostate>) {
			// nada
		}

	}, background);

//	SendEnvironmentUniforms(_glEnvironmentUBO, scene, stats);
//	Program::Default().bindUniformBlock("EnvironmentBlock", _glEnvironmentUBO);
}

void OpenGLRenderer::render(Mesh& mesh,
							const mat4& modelMat,
							const mat4& viewMat,
							const mat4& projectionMat,
							const DebugOptions& debugOptions,
							Stats& stats) {

	if (A3D_MASK_CONTAINS(debugOptions, DebugOptions::ShowBoundingBoxes)) {
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

	GLuint vbo, vao, ebo;
	GetMeshElementGLVertexDataHandles(element,
									  _meshElementGLMapping,
									  vbo, vao, ebo);

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

	DrawMeshElement(element, program, modelMat, viewMat, projectionMat, vao, ebo);

	// save reference for housekeeping

	_activeMeshElements.emplace(&element);

	// draw AABB lines

//	if (A3D_MASK_CONTAINS(debugOptions, DebugOptions::ShowBoundingBoxes)) {
//		render(element.aabbLines(), modelMat, viewMat, projectionMat);
//	}
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

	auto framebufferSize = context.framebufferSize();
	auto framebufferWidth = (unsigned)round(framebufferSize.x);
	auto framebufferHeight = (unsigned)round(framebufferSize.y);

	vector<unsigned char> pixelBuf(framebufferWidth * framebufferHeight * 4);
	// TODO: SEGV under Plasma Wayland
	// info/solution? https://projects.blender.org/blender/blender/issues/98462#issuecomment-127388
	glReadPixels(0, 0,
				 (GLsizei)framebufferWidth, (GLsizei)framebufferHeight,
				 GL_RGBA, GL_UNSIGNED_BYTE, pixelBuf.data());
	auto buffer = make_unique<Buffer>((std::byte*)pixelBuf.data(),
									  framebufferWidth * framebufferHeight * 4);
	return make_unique<Image>(std::move(buffer), framebufferWidth, framebufferHeight, 4);
}

void OpenGLRenderer::framebufferScaleChanged(const RenderContext& context) {
	A3D_LOG_D("context: {:p}", static_cast<const void*>(&context));

	UpdateImguiScale(context, *_overlayTitleFont, *_overlayBodyFont);
}
	
/*********************************************************************************************
	Static
 *********************************************************************************************/

void RenderSkybox(Mesh& skyboxMesh,
				  Node& pointOfView,
				  OpenGLRenderer::MeshElementGLMapping& elementGLMapping,
				  OpenGLRenderer::TextureGLMapping& textureGLMapping,
				  unordered_set<Texture*>& activeTextures) {

	auto program = Program::Skybox();
	
	auto& element = skyboxMesh.elements().front();
	auto material = skyboxMesh.materials().front();
	auto emissiveProperty = material->emission();
	
	// check and load vertex data if necessary
	
	GLuint vbo, vao, ebo;
	GetSkyboxGLVertexDataHandles(skyboxMesh,
								 elementGLMapping,
								 vbo, vao, ebo);
	
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

	if (auto texture = get_if<shared_ptr<Texture>>(&emissiveProperty)) {
		SetTextureSamplingOptions(**texture, emissiveGLTextureHandle);
	}

//	if (holds_alternative<shared_ptr<Texture>>(emissiveProperty)) {
//		auto texture = get<shared_ptr<Texture>>(emissiveProperty);
//		SetTextureSamplingOptions(*texture, emissiveGLTextureHandle);
//	}
	
	// configure OpenGL state
	
	SetSkyboxOpenGLState();
	
	// update
	
	DrawSkyboxElement(*element, program, pointOfView, vao, ebo);
}
	
void GetMeshElementGLVertexDataHandles(MeshElement& element,
									   OpenGLRenderer::MeshElementGLMapping& glMapping,
									   GLuint& glVBO, GLuint& glVAO, GLuint& glEBO) {

	// looks up and populates glVBO, glVAO, and glEBO, loading the vertex data if needed

	if (A3D_MASK_CONTAINS(element.dirtyMask(), MeshElementDirtyMask::VertexData)) {

		DeleteMeshElementGLResources(&element, glMapping);

		BufferMeshElementVertexData(element, Program::Default(), glVBO, glVAO, glEBO);
		
		glMapping[&element] = make_tuple(glVBO, glVAO, glEBO);

		element.dirtyMask(A3D_MASK_REMOVE(element.dirtyMask(),
										  MeshElementDirtyMask::VertexData));
	}
	else {
		auto mapping = glMapping[&element];
		glVBO = get<0>(mapping);
		glVAO = get<1>(mapping);
		glEBO = get<2>(mapping);
	}
}

void GetSkyboxGLVertexDataHandles(Mesh& skyboxMesh,
								  OpenGLRenderer::MeshElementGLMapping& glMapping,
								  GLuint& glVBO, GLuint& glVAO, GLuint& glEBO) {

	// looks up and populates glVBO, glVAO, and glEBO, loading the vertex data if needed
	//
	// NOTE: this is essentially exactly the same as GetMeshElementGLVertexDataHandles()
	// except if the data needs to be loaded, it uses BufferMeshElementVertexData() as the
	// layout is different.  This will probaly need to be refacted in the future as more layouts are used
	
	auto& element = skyboxMesh.elements().front();

	if (A3D_MASK_CONTAINS(element->dirtyMask(), MeshElementDirtyMask::VertexData)) {

		DeleteMeshElementGLResources(element.get(), glMapping);
		
		BufferSkyboxVertexData(skyboxMesh, Program::Skybox(), glVBO, glVAO, glEBO);
		
		glMapping[element.get()] = make_tuple(glVBO, glVAO, glEBO);

		element->dirtyMask(A3D_MASK_REMOVE(element->dirtyMask(),
										   MeshElementDirtyMask::VertexData));
	}
	else {
		auto mapping = glMapping[element.get()];
		glVBO = get<0>(mapping);
		glVAO = get<1>(mapping);
		glEBO = get<2>(mapping);
	}
}

void GetLinesVertexDataHandles(const vector<Line>& lines,
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

void GetTextureGLTextureHandles(Material& material,
								OpenGLRenderer::TextureGLMapping& glMapping,
								unordered_set<Texture*>& activeTextures,
								map<MaterialPropertyType, GLuint>& glTextureHandles) {

	// looks up and populates glTextureHandle, loading the texture data if needed

	for (auto& [property, type] : material.properties()) {

		if (auto texture = get_if<shared_ptr<Texture>>(property)) {

			if (A3D_MASK_CONTAINS((*texture)->dirtyMask(), TextureDirtyMask::Contents)) {

				A3D_LOG_D("Texture {:p} contents dirty.", static_cast<void*>(&texture));

				DeleteTextureGLResources(texture->get(), glMapping);

				GLuint textureID = 0;
				BufferTexture(**texture, textureID);
				if (textureID > 0) {
					glTextureHandles[type] = textureID;
					glMapping[texture->get()] = textureID;
				}

				(*texture)->dirtyMask(A3D_MASK_REMOVE((*texture)->dirtyMask(),TextureDirtyMask::Contents));
			}
			else {
				auto textureHandle = glMapping[texture->get()];
				glTextureHandles[type] = textureHandle;
			}

			// save reference for housekeeping
			activeTextures.emplace(texture->get());
		}

//		if (holds_alternative<shared_ptr<Texture>>(*property)) {
//
//			auto texture = get<shared_ptr<Texture>>(*property).get();
//
//			if (A3D_MASK_CONTAINS(texture->dirtyMask(), TextureDirtyMask::Contents)) {
//
//				A3D_LOG_D("Texture {:p} contents dirty.", static_cast<void*>(texture));
//
//				DeleteTextureGLResources(texture, glMapping);
//
//				GLuint textureID = 0;
//				BufferTexture(*texture, textureID);
//				if (textureID > 0) {
//					glTextureHandles[type] = textureID;
//					glMapping[texture] = textureID;
//				}
//
//				texture->dirtyMask(A3D_MASK_REMOVE(texture->dirtyMask(), TextureDirtyMask::Contents));
//			}
//			else {
//				auto textureHandle = glMapping[texture];
//				glTextureHandles[type] = textureHandle;
//			}
//
//			// save reference for housekeeping
//			activeTextures.emplace(texture);
//		}
	}
}

void BufferMeshElementVertexData(const MeshElement& element,
								 Program& program,
								 GLuint& glVBO, GLuint& glVAO, GLuint& glEBO) {

	A3D_LOG_D("Buffering vertex data for mesh element {:p}...",
			  static_cast<const void*>(&element));
	
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

	// from 'layout (location = x)'
	const unsigned POSITION_LOCATION = 0;
	const unsigned NORMAL_LOCATION = 1;
	const unsigned TEXCOORD_LOCATION = 2;
	
	//auto positionIndex = program.getAttributeLocation("vert_vertPos");
	glVertexAttribPointer(POSITION_LOCATION, 			// attrib index
						  3, 						// num components per attrib (3 float in vec3)
						  GL_FLOAT, 				// component type
						  GL_FALSE, 				// normalize
						  sizeof(Vertex), 			// stride
						  nullptr); 						// start offset
	glEnableVertexAttribArray(POSITION_LOCATION);
	
	//auto normalIndex = program.getAttributeLocation("vert_vertNorm");
	glVertexAttribPointer(NORMAL_LOCATION, 				// attrib index
						  3, 						// num components per attrib (3 float in vec3)
						  GL_FLOAT, 				// component type
						  GL_FALSE, 				// normalize
						  sizeof(Vertex), 			// stride
						  (void*)sizeof(vec3)); 	// start offset
	glEnableVertexAttribArray(NORMAL_LOCATION);
	
	//auto texCoordIndex = program.getAttributeLocation("vert_texCoord");
	glVertexAttribPointer(TEXCOORD_LOCATION, 							// attrib index
						  2, 										// num components per attrib (2 float in vec2)
						  GL_FLOAT, 								// component type
						  GL_FALSE, 								// normalize
						  sizeof(Vertex), 							// stride
						  (void*)(sizeof(vec3) + sizeof(vec3))); 	// start offset
	glEnableVertexAttribArray(TEXCOORD_LOCATION);
	
	glGenBuffers(1, &glEBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, glEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,
				 (GLsizeiptr)(faces.size() * sizeof(Face)),
				 &(faces[0]),
				 GL_STATIC_DRAW);
	
	//program.unuse();
}

void BufferSkyboxVertexData(Mesh& skyboxMesh,
							Program& program,
							GLuint& glVBO, GLuint& glVAO, GLuint& glEBO) {

	A3D_LOG_D("Buffering skybox vertex data...");

	program.use();
	
	auto& element = skyboxMesh.elements().front();
	auto verts = element->vertices();
	auto faces = element->faces();
	
	glGenBuffers(1, &glVBO);
	glBindBuffer(GL_ARRAY_BUFFER, glVBO);
	glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr)(verts.size()*sizeof(Vertex)),
				 &(verts[0]), GL_STATIC_DRAW);
	
	glGenVertexArrays(1, &glVAO);
	glBindVertexArray(glVAO);

	// from 'layout (location = x)'
	const unsigned POSITION_LOCATION = 0;

	//GLuint positionIndex = program.getAttributeLocation("vert_vertPos");
	glVertexAttribPointer(POSITION_LOCATION, // attrib index
						  3, // num components per attrib (3 float in vec3)
						  GL_FLOAT, // component type
						  GL_FALSE, // normalize
						  sizeof(Vertex), // stride
						  nullptr); // start offset
	glEnableVertexAttribArray(POSITION_LOCATION);
	
	glGenBuffers(1, &glEBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, glEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,
				 (GLsizeiptr)(faces.size() * sizeof(Face)),
				 &(faces[0]),
				 GL_STATIC_DRAW);
	
	//program.unuse();
}

void BufferLinesVertexData(const vector<Line>& lines,
						   Program& program,
						   GLuint& glVBO, GLuint& glVAO) {
	//A3D_LOG_D("Buffering vertex data for {} lines...", lines.size());

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
		massagedBuffer.push_back(fromColor.rgb());
		massagedBuffer.push_back(toLocation);
		massagedBuffer.push_back(toColor.rgb());
	}

	glGenBuffers(1, &glVBO);
	glBindBuffer(GL_ARRAY_BUFFER, glVBO);
	glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr)(massagedBuffer.size()*sizeof(vec3)),
				 massagedBuffer.data(), GL_STATIC_DRAW);

	glGenVertexArrays(1, &glVAO);
	glBindVertexArray(glVAO);

	// from 'layout (location = x)'
	const unsigned POSITION_LOCATION = 0;
	const unsigned COLOR_LOCATION = 1;

	//auto positionIndex = program.getAttributeLocation("vert_vertPos");
	glVertexAttribPointer(POSITION_LOCATION, 		// attrib index
						  3, 					// num components per attrib (3 float in vec3)
						  GL_FLOAT, 			// component type
						  GL_FALSE, 			// normalize
						  sizeof(vec3)*2, 		// stride
						  nullptr); 					// start offset
	glEnableVertexAttribArray(POSITION_LOCATION);

	//auto colorIndex = program.getAttributeLocation("vert_vertColor"); // was texCoord?
	glVertexAttribPointer(COLOR_LOCATION, 			// attrib index
						  3, 					// num components per attrib (3 float in vec3)
						  GL_FLOAT, 			// component type
						  GL_FALSE, 			// normalize
						  sizeof(vec3)*2, 		// stride
						  (void*)sizeof(vec3));	// start offset
	glEnableVertexAttribArray(COLOR_LOCATION);
}

void BufferTexture(const Texture& texture,
				   GLuint& glTextureHandle) {

	auto contents = texture.contents();

	std::visit([&texture, &glTextureHandle](auto&& contents) -> void {

		using T = std::decay_t<decltype(contents)>;

		if constexpr (std::is_same_v<T, shared_ptr<CubeImage>>) {

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
		else if constexpr (std::is_same_v<T, shared_ptr<Image>>) {

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
		else if constexpr (std::is_same_v<T, std::monostate>) {
			A3D_LOG_E("Empty texture variant.");
		}

	}, contents);
}

//void BufferTexture(const Texture& texture,
//				   GLuint& glTextureHandle) {
//
//	auto contents = texture.contents();
//
//	if (dynamic_pointer_cast<CubeImage>(contents)) {
//
//		A3D_LOG_D("Buffering cube texture {:p}...", static_cast<const void*>(&contents));
//
//		auto cubeImage = dynamic_pointer_cast<CubeImage>(contents);
//
//		Image* images[] = {
//			cubeImage->posX(),
//			cubeImage->negX(),
//			cubeImage->posY(),
//			cubeImage->negY(),
//			cubeImage->posZ(),
//			cubeImage->negZ() };
//
//		GLenum sides[] = {
//			GL_TEXTURE_CUBE_MAP_POSITIVE_X,
//			GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
//			GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
//			GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
//			GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
//			GL_TEXTURE_CUBE_MAP_NEGATIVE_Z };
//
//		glGenTextures(1, &glTextureHandle);
//		glBindTexture(GL_TEXTURE_CUBE_MAP, glTextureHandle);
//
//		for (int s=0; s<6; ++s) {
//			GLenum side = sides[s];
//			auto image = images[s];
//
//			unsigned bytesPerPixel = image->bytesPerPixel();
//			GLint glInternalFormat = GL_RGBA;
//			if (bytesPerPixel == 3) glInternalFormat = GL_RGB;
//			else if (bytesPerPixel == 1) glInternalFormat = GL_RED;
//
//			glTexImage2D(side,
//						 0,
//						 glInternalFormat,//GL_RGB, //GL_SRGB_ALPHA,
//						 image->width(),
//						 image->height(),
//						 0,
//						 GL_RGBA,//(image->bytesPerPixel() == 3 ? GL_RGB : GL_RGBA),
//						 GL_UNSIGNED_BYTE,
//						 image->buffer().data());
//		}
//
//		auto sampler = texture.sampler();
//		SetTextureMinificationFilter(glTextureHandle, true, sampler->minificationFilter());
//		SetTextureMagnificationFilter(glTextureHandle, true, sampler->magnificationFilter());
//		SetTextureMaxAnisotropy(glTextureHandle, true, sampler->maxAnisotropy());
//		SetTextureWrapS(glTextureHandle, true, sampler->wrapS());
//		SetTextureWrapT(glTextureHandle, true, sampler->wrapT());
//		SetTextureWrapR(glTextureHandle, sampler->wrapR());
//	}
//	else if (dynamic_pointer_cast<Image>(contents)) {
//
//		A3D_LOG_D("Buffering 2D texture {:p}...", static_cast<const void*>(&contents));
//
//		auto image = dynamic_pointer_cast<Image>(contents);
//
//		glGenTextures(1, &glTextureHandle);
//		A3D_LOG_D("Binding new texture handle: {}", glTextureHandle);
//		glBindTexture(GL_TEXTURE_2D, glTextureHandle);
//
////		unsigned bytesPerPixel = image->bytesPerPixel();
////		GLint glInternalFormat;
////		if (bytesPerPixel == 3) glInternalFormat = GL_RGB;
////		else if (bytesPerPixel == 1) glInternalFormat = GL_RED;
//
//		A3D_LOG_D("Buffering image {:p}: width: {}, height: {}, bytesPerPixel: {}, data size: {}",
//				 static_cast<void*>(image.get()), image->width(), image->height(), image->bytesPerPixel(),
//				 image->width() * image->height() * image->bytesPerPixel());
//
//		glTexImage2D(GL_TEXTURE_2D,
//					 0,
//					 GL_RGBA,//glInternalFormat,//GL_RGBA,//GL_SRGB_ALPHA,
//					 image->width(),
//					 image->height(),
//					 0,
//					 GL_RGBA,//(image->bytesPerPixel() == 3 ? GL_RGB : GL_RGBA),
//					 GL_UNSIGNED_BYTE,
//					 image->buffer().data());
//
//		auto sampler = texture.sampler();
//		SetTextureMinificationFilter(glTextureHandle, false, sampler->minificationFilter());
//		SetTextureMagnificationFilter(glTextureHandle, false, sampler->magnificationFilter());
//		SetTextureMaxAnisotropy(glTextureHandle, false, sampler->maxAnisotropy());
//		SetTextureWrapS(glTextureHandle, false, sampler->wrapS());
//		SetTextureWrapT(glTextureHandle, false, sampler->wrapT());
//	}
//}

void SendMaterialUniforms(const Material& material,
						  Program& program,
						  map<MaterialPropertyType, GLuint>& glTextureHandles) {

	// sends uniforms for the Material, and MaterialProperties it has

	program.use();

	program.setUniform("specularExponent", material.specularExponent());
	program.setUniform("uvScale", material.uvScale());
	//program.setUniform("locksAmbientWithDiffuse", material.locksAmbientWithDiffuse());
	program.setUniform("emissionContentsType", (unsigned)0); // 0 = MaterialType_None -- why is this here?
	//program.setUniform("defaultLighting", 0);

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

void SendMaterialPropertyUniforms(const MaterialProperty& property,
								  MaterialPropertyType type,
								  GLuint glTextureHandle,
								  Program& program) {

	program.use();

	std::visit([&type, &program, &glTextureHandle](auto&& property) -> void {

		using T = std::decay_t<decltype(property)>;

		if constexpr (std::is_same_v<T, shared_ptr<Texture>>) {



			std::visit([&type, &glTextureHandle, &program](auto&& contents) -> void {

				using T = std::decay_t<decltype(contents)>;

				if constexpr (std::is_same_v<T, shared_ptr<Image>>) {

					string modeUniformName;
					string samplerUniformName;
					GLenum slot;
					GLint index;

					switch (type) {
						case MaterialPropertyType::Ambient:
							modeUniformName = "ambientContentsType";
							samplerUniformName = "samplers.ambient";
							slot = GL_TEXTURE0;
							index = 0;
							break;
						case MaterialPropertyType::Diffuse:
							modeUniformName = "diffuseContentsType";
							samplerUniformName = "samplers.diffuse";
							slot = GL_TEXTURE1;
							index = 1;
							break;
						case MaterialPropertyType::Specular:
							modeUniformName = "specularContentsType";
							samplerUniformName = "samplers.specular";
							slot = GL_TEXTURE2;
							index = 2;
							break;
						case MaterialPropertyType::Emission:
							modeUniformName = "emissionContentsType";
							samplerUniformName = "samplers.emission";
							slot = GL_TEXTURE3;
							index = 3;
							break;
						default:
							A3D_LOG_E("Invalid MaterialPropertyType: {}",
									  magic_enum::enum_name<MaterialPropertyType>(type));
							return;
					}

					program.setUniform(modeUniformName.c_str(),
									   static_cast<underlying_type<MaterialContentsType>::type>(MaterialContentsType::Sampler));
					program.bindTexture(samplerUniformName.c_str(), GL_TEXTURE_2D, slot, glTextureHandle, index);
				}
				else if constexpr (std::is_same_v<T, shared_ptr<CubeImage>>) {

					program.bindTexture("cubeSampler", GL_TEXTURE_CUBE_MAP, GL_TEXTURE0, glTextureHandle, 0);
				}
				else if constexpr (std::is_same_v<T, std::monostate>) {
					A3D_LOG_E("Empty texture variant.");
				}

			}, property->contents());



//			if (dynamic_pointer_cast<Image>(property->contents())) {
//
//				string modeUniformName;
//				string samplerUniformName;
//				GLenum slot;
//				GLint index;
//
//				switch (type) {
//					case MaterialPropertyType::Ambient:
//						modeUniformName = "ambientContentsType";
//						samplerUniformName = "samplers.ambient";
//						slot = GL_TEXTURE0;
//						index = 0;
//						break;
//					case MaterialPropertyType::Diffuse:
//						modeUniformName = "diffuseContentsType";
//						samplerUniformName = "samplers.diffuse";
//						slot = GL_TEXTURE1;
//						index = 1;
//						break;
//					case MaterialPropertyType::Specular:
//						modeUniformName = "specularContentsType";
//						samplerUniformName = "samplers.specular";
//						slot = GL_TEXTURE2;
//						index = 2;
//						break;
//					case MaterialPropertyType::Emission:
//						modeUniformName = "emissionContentsType";
//						samplerUniformName = "samplers.emission";
//						slot = GL_TEXTURE3;
//						index = 3;
//						break;
//					default:
//						A3D_LOG_E("Invalid MaterialPropertyType: {}",
//								  magic_enum::enum_name<MaterialPropertyType>(type));
//						return;
//				}
//
//				program.setUniform(modeUniformName.c_str(),
//								   static_cast<underlying_type<MaterialContentsType>::type>(MaterialContentsType::Sampler));
//				program.bindTexture(samplerUniformName.c_str(), GL_TEXTURE_2D, slot, glTextureHandle, index);
//			}
//			else if (dynamic_pointer_cast<CubeImage>(property->contents())) {
//
//				program.bindTexture("cubeSampler", GL_TEXTURE_CUBE_MAP, GL_TEXTURE0, glTextureHandle, 0);
//			}
		}
		else if constexpr (std::is_same_v<T, shared_ptr<Color>>) {

			string modeUniformName;
			string colorUniformName;

			switch (type) {
				case MaterialPropertyType::Ambient:
					modeUniformName = "ambientContentsType";
					colorUniformName = "colors.ambient";
					break;
				case MaterialPropertyType::Diffuse:
					modeUniformName = "diffuseContentsType";
					colorUniformName = "colors.diffuse";
					break;
				case MaterialPropertyType::Specular:
					modeUniformName = "specularContentsType";
					colorUniformName = "colors.specular";
					break;
				case MaterialPropertyType::Emission:
					modeUniformName = "emissionContentsType";
					colorUniformName = "colors.emission";
					break;
				default:
					A3D_LOG_E("Invalid MaterialPropertyType: {}",
							  magic_enum::enum_name<MaterialPropertyType>(type));
					return;
			}

			program.setUniform(modeUniformName.c_str(),
							   static_cast<underlying_type<MaterialContentsType>::type>(MaterialContentsType::Color));
			program.setUniform(colorUniformName.c_str(),
							   property->r(), property->g(), property->b());
		}
		else if constexpr (std::is_same_v<T, std::monostate>) {

			A3D_LOG_W("NULL material property contents.");
		}

	}, property);
	
	//program.unuse();
}

void SendEnvironmentUniforms(GLuint glEnvironmentUBO,
							 const Scene& scene,
							 const vector<Node*>& lightNodes,
							 Stats& stats) {
	// program "Default" must be active

	// block

	EnvironmentBlock environmentStruct;

	// lights

	if (scene.visualWorld()->usesDefaultLighting()) {

		Program::Default().setUniform("useDefaultLighting", true);
	}
	else {

		auto numLights = lightNodes.size();

		stats.lights = numLights;

		if (((numLights == 0) && scene.visualWorld()->autoEnablesDefaultLighting())) {

			Program::Default().setUniform("useDefaultLighting", true);
		}
		else {

			Program::Default().setUniform("useDefaultLighting", false);

			vector<AmbientLightGLSLStruct> ambientStructs;
			vector<DirectionalLightGLSLStruct> directionalStructs;
			vector<PointLightGLSLStruct> pointStructs;
			vector<SpotLightGLSLStruct> spotStructs;

			ambientStructs.reserve(MAX_AMBIENT_LIGHTS);
			directionalStructs.reserve(MAX_DIRECTIONAL_LIGHTS);
			pointStructs.reserve(MAX_POINT_LIGHTS);
			spotStructs.reserve(MAX_SPOT_LIGHTS);

			for (unsigned l = 0; l < numLights; ++l) {

				auto node = lightNodes[l];
				auto light = node->light();
				auto color = light->color();

				// light_cutoff:
				// if an attenuated light, first make sure it's not past its cutoff distance.
				// this is either a hard-coded distance or calcualted based on a minimum attenuation.
				//
				// for min attenuation:
				// https://gamedev.stackexchange.com/a/56934
				// cuts light off at distance 'd'
				// when attenuation drops below 'a'.
				// d = sqrt(1.0 / (Kq * a))
				//
				// UPDATE: distance from what? the camera?  that doesn't make sense.
				// the fragment?  sure, but probably slow.
				// the vertex?  sure, but maybe messy?

				//if (auto ambientLight = dynamic_cast<AmbientLight*>(light)) {
				if (auto ambientLight = dyn_cast<AmbientLight>(light)) {

					AmbientLightGLSLStruct lightStruct;
					lightStruct.color = ambientLight->color()->rgba();
					ambientStructs.push_back(lightStruct);
				}
				else if (auto directionalLight = dyn_cast<DirectionalLight>(light)) {

					DirectionalLightGLSLStruct lightStruct;
					lightStruct.color = directionalLight->color()->rgba();
					lightStruct.direction_world = node->worldForward();
					directionalStructs.push_back(lightStruct);
				}
				else if (auto pointLight = dyn_cast<PointLight>(light)) {

					PointLightGLSLStruct lightStruct;
					lightStruct.color = pointLight->color()->rgba();
					lightStruct.position_world = node->worldPosition();
					lightStruct.constantAttenuation = pointLight->constantAttenuation();
					lightStruct.linearAttenuation = pointLight->linearAttenuation();
					lightStruct.quadraticAttenuation = pointLight->quadraticAttenuation();
					pointStructs.push_back(lightStruct);
				}
				else if (auto spotLight = dyn_cast<SpotLight>(light)) {

					SpotLightGLSLStruct lightStruct;
					lightStruct.color = spotLight->color()->rgba();
					lightStruct.position_world = node->worldPosition();
					lightStruct.direction_world = node->worldForward();
					lightStruct.innerAngleCos = spotLight->innerAngleCos();
					lightStruct.outerAngleCos = spotLight->outerAngleCos();
					lightStruct.featheringMode = magic_enum::enum_underlying(spotLight->featheringMode());
					lightStruct.constantAttenuation = spotLight->constantAttenuation();
					lightStruct.linearAttenuation = spotLight->linearAttenuation();
					lightStruct.quadraticAttenuation = spotLight->quadraticAttenuation();
					spotStructs.push_back(lightStruct);
				}
			}

			environmentStruct.numAmbientLights = ambientStructs.size();
			memcpy(&environmentStruct.ambientLights,
				   ambientStructs.data(),
				   sizeof(AmbientLightGLSLStruct) * ambientStructs.size());

			environmentStruct.numDirectionalLights = directionalStructs.size();
			memcpy(&environmentStruct.directionalLights,
				   directionalStructs.data(),
				   sizeof(DirectionalLightGLSLStruct) * directionalStructs.size());

			environmentStruct.numPointLights = pointStructs.size();
			memcpy(&environmentStruct.pointLights,
				   pointStructs.data(),
				   sizeof(PointLightGLSLStruct) * pointStructs.size());

			environmentStruct.numSpotLights = spotStructs.size();
			memcpy(&environmentStruct.spotLights,
				   spotStructs.data(),
				   sizeof(SpotLightGLSLStruct) * spotStructs.size());
		}
	}

	// fog

	auto visualWorld = scene.visualWorld();
	FogGLSLStruct fogStruct;
	fogStruct.startDistance = visualWorld->fogStartDistance();
	fogStruct.endDistance = visualWorld->fogEndDistance();
	fogStruct.densityExponent = visualWorld->fogDensityExponent();
	fogStruct.startDistance = visualWorld->fogStartDistance();
	auto fogColor = visualWorld->fogColor();
	if (visualWorld->fogColor()) {
		fogStruct.color = fogColor->rgba();
	}
	else {
		fogStruct.color = {0.0, 0.0, 0.0, 0.0};
	}

	memcpy(&environmentStruct.fog, &fogStruct, sizeof(fogStruct));



	// send 'em

	glBindBuffer(GL_UNIFORM_BUFFER, glEnvironmentUBO);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(EnvironmentBlock), &environmentStruct, GL_DYNAMIC_DRAW);



//	 int programID = Program::Default().glID();






//	const char* names[16] = {
//			"EnvironmentBlock.color",
//			"EnvironmentBlock.numAmbientLights",
//			"EnvironmentBlock.ambientLights[0].color",
//			"EnvironmentBlock.numDirectionalLights",
//			"EnvironmentBlock.directionalLights[0].color",
//			"EnvironmentBlock.numPointLights",
//			"EnvironmentBlock.pointLights[0].color",
//			"EnvironmentBlock.pointLights[0].position_world",
//			"EnvironmentBlock.pointLights[0].constantAttenuation",
//			"EnvironmentBlock.pointLights[0].linearAttenuation",
//			"EnvironmentBlock.pointLights[0].quadraticAttenuation",
//			"EnvironmentBlock.numSpotLights",
//			"EnvironmentBlock.spotLights[0].color",
//			"EnvironmentBlock.fog.color",
//			"EnvironmentBlock.fog.startDistance",
//			"EnvironmentBlock.fog.endDistance",
////			"EnvironmentBlock.fog.densityExponent"
//	};
//	GLuint indices[16]; for (int i = 0; i < 16; ++i) indices[i] = 0;
//	GLint offsets[16]; for (int i = 0; i < 16; ++i) offsets[i] = 0;
//
//	glGetUniformIndices(programID, 16, names, indices);
//	glGetActiveUniformsiv(programID, 16, indices, GL_UNIFORM_OFFSET, offsets);
//
////	for (int i = 0; i < 16; i++) {
////		A3D_LOG_D("Uniform index for {}: {}",
////				  names[i], indices[i] == GL_INVALID_INDEX ? "BAD" : "GOOD");
////	}
//
////	environmentStruct.ambientLights[0].color = rgba(1.0, 0, 0, 1);
////	glBufferSubData(GL_UNIFORM_BUFFER, offsets[1], sizeof(rgba),
////					(const void*)&environmentStruct.ambientLights[0].color);
//
//	environmentStruct.color = vec3(1.0, 0, 0);
//	glBufferSubData(GL_UNIFORM_BUFFER, offsets[0], sizeof(vec3),
//					(const void*)&environmentStruct.color);




	//environmentStruct.color = vec3(1.0, 0, 0);

	//Program::Default().bindUniformBlock("EnvironmentBlock", glEnvironmentUBO);

//	environmentStruct.fog.color = vec4(1.0, 0.0, 0.0, 1.0);
//	glBufferSubData(GL_UNIFORM_BUFFER, offsets[1], sizeof(vec4),
//					(const void*)&environmentStruct.ambientLights[0].color);





//	glBufferSubData(GL_UNIFORM_BUFFER, offsets[0], sizeof(uint32_t),
//					(const void*)0);
////	glBufferSubData(GL_UNIFORM_BUFFER, offsets[0], sizeof(uint32_t),
////					(const void*)&environmentStruct.numAmbientLights);
////	glBufferSubData(GL_UNIFORM_BUFFER, offsets[1], sizeof(AmbientLightGLSLStruct) * MAX_AMBIENT_LIGHTS,
////					(const void*)environmentStruct.ambientLights);
//
//	glBufferSubData(GL_UNIFORM_BUFFER, offsets[2], sizeof(uint32_t),
//					(const void*)0);
////	glBufferSubData(GL_UNIFORM_BUFFER, offsets[3], sizeof(DirectionalLightGLSLStruct) * MAX_DIRECTIONAL_LIGHTS,
////					(const void*)environmentStruct.directionalLights);
//
//	glBufferSubData(GL_UNIFORM_BUFFER, offsets[4], sizeof(uint32_t),
//					(const void*)0);
//
//	glBufferSubData(GL_UNIFORM_BUFFER, offsets[5], sizeof(vec4),
//					(const void*)&environmentStruct.pointLights[0].color);
//	glBufferSubData(GL_UNIFORM_BUFFER, offsets[6], sizeof(vec3),
//					(const void*)&environmentStruct.pointLights[0].position_world);
//	glBufferSubData(GL_UNIFORM_BUFFER, offsets[7], sizeof(float32_t),
//					(const void*)&environmentStruct.pointLights[0].constantAttenuation);
//	glBufferSubData(GL_UNIFORM_BUFFER, offsets[8], sizeof(float32_t),
//					(const void*)&environmentStruct.pointLights[0].linearAttenuation);
//	glBufferSubData(GL_UNIFORM_BUFFER, offsets[9], sizeof(float32_t),
//					(const void*)&environmentStruct.pointLights[0].quadraticAttenuation);
//
//
//
//	glBufferSubData(GL_UNIFORM_BUFFER, offsets[10], sizeof(uint32_t),
//					(const void*)0);
////	glBufferSubData(GL_UNIFORM_BUFFER, offsets[7], sizeof(SpotLightGLSLStruct) * MAX_SPOT_LIGHTS,
////					(const void*)environmentStruct.spotLights);
//
//
//	glBufferSubData(GL_UNIFORM_BUFFER, offsets[12], sizeof(vec4),
//					(const void*)&environmentStruct.fog.color);
//	glBufferSubData(GL_UNIFORM_BUFFER, offsets[13], sizeof(float32_t),
//					(const void*)&environmentStruct.fog.startDistance);
//	glBufferSubData(GL_UNIFORM_BUFFER, offsets[14], sizeof(float32_t),
//					(const void*)&environmentStruct.fog.endDistance);
//	glBufferSubData(GL_UNIFORM_BUFFER, offsets[15], sizeof(float32_t),
//					(const void*)&environmentStruct.fog.densityExponent);






//	 const char* names[9] = {
//	 	"EnvironmentBlock.numAmbientLights", // 0
//	 	"EnvironmentBlock.ambientLights[0].color", // 16
//	 	"EnvironmentBlock.numDirectionalLights", // 80
//	 	"EnvironmentBlock.directionalLights[0].color", // 96
//	 	"EnvironmentBlock.numPointLights", // 224
//	 	"EnvironmentBlock.pointLights[0].color", // 240
//	 	"EnvironmentBlock.numSpotLights", // 1776
//	 	"EnvironmentBlock.spotLights[0].color", // 1792
//	 	"EnvironmentBlock.fog.color" // 2432
//	 };
//	 GLuint indices[9]; for (int i = 0; i < 9; i++) indices[i] = 0;
//	 GLint offsets[9]; for (int i = 0; i < 9; i++) offsets[i] = 0;
//
//	 glGetUniformIndices(programID, 9, names, indices);
//	 glGetActiveUniformsiv(programID, 9, indices, GL_UNIFORM_OFFSET, offsets);
//
//	 for (int i = 0; i < 9; i++) {
//	 	A3D_LOG_D("Uniform index for {}: {}",
//	 		names[i], indices[i] == GL_INVALID_INDEX ? "BAD" : "GOOD");
//	 }
//
//	 glBufferSubData(GL_UNIFORM_BUFFER, offsets[0], sizeof(uint32_t),
//	 	(const void*)&environmentStruct.numAmbientLights);
//	 glBufferSubData(GL_UNIFORM_BUFFER, offsets[1], sizeof(AmbientLightGLSLStruct) * MAX_AMBIENT_LIGHTS,
//	 	(const void*)environmentStruct.ambientLights);
//	 glBufferSubData(GL_UNIFORM_BUFFER, offsets[2], sizeof(uint32_t),
//	 	(const void*)&environmentStruct.numDirectionalLights);
//	 glBufferSubData(GL_UNIFORM_BUFFER, offsets[3], sizeof(DirectionalLightGLSLStruct) * MAX_DIRECTIONAL_LIGHTS,
//	 	(const void*)environmentStruct.directionalLights);
//	 glBufferSubData(GL_UNIFORM_BUFFER, offsets[4], sizeof(uint32_t),
//	 	(const void*)&environmentStruct.numPointLights);
//	 glBufferSubData(GL_UNIFORM_BUFFER, offsets[5], sizeof(PointLightGLSLStruct) * MAX_POINT_LIGHTS,
//	 	(const void*)environmentStruct.pointLights);
//	 glBufferSubData(GL_UNIFORM_BUFFER, offsets[6], sizeof(uint32_t),
//	 	(const void*)&environmentStruct.numSpotLights);
//	 glBufferSubData(GL_UNIFORM_BUFFER, offsets[7], sizeof(SpotLightGLSLStruct) * MAX_SPOT_LIGHTS,
//	 	(const void*)environmentStruct.spotLights);
//	 glBufferSubData(GL_UNIFORM_BUFFER, offsets[8], sizeof(FogGLSLStruct),
//	 	(const void*)&environmentStruct.fog);
}

// ORIGINAL
//void SendEnvironmentUniforms(GLuint glEnvironmentUBO,
//							 const Scene& scene,
//							 const vector<Node*>& lightNodes,
//							 Stats& stats) {
//	// program "Default" must be active
//
//	// block
//
//	EnvironmentBlock environmentStruct;
//
//	// lights
//
//	if (scene.visualWorld()->usesDefaultLighting()) {
//
//		Program::Default().setUniform("useDefaultLighting", true);
//	}
//	else {
//
////		auto lights = vector<Node*>();
//		auto lights = lightNodes;
//		Node* ambientLightNode = nullptr;
//
//		// find all lights in the scene
////		for (auto &node: scene.rootNode()->children(true)) {
////			if (auto light = node->light()) {
////				if (light->type() == LightType::Point) {
////					lights.push_back(node.get());
////				}
////				else if (light->type() == LightType::Ambient) {
////					ambientLightNode = node.get();
////				}
////			}
////		}
//
//		if (lightNodes.size() > MAX_DYNAMIC_LIGHTS) {
//
//			// find all light distances from the camera
//
//			auto lightsUnsorted = map<Node*, float>();
//			auto cameraPos_world = scene.visualWorld()->pointOfView().lock()->worldPosition();
//			for (auto& lightNode : lightNodes) {
//				auto lightPos_world = lightNode->worldPosition();
//				auto lightToCamera = lightPos_world - cameraPos_world;
//				auto lightToCameraDistance = length(lightToCamera);
//				lightsUnsorted[lightNode] = lightToCameraDistance;
//			}
//
//			vector<Node*> sorted;
//			lights = SortedLights(lightsUnsorted);
//
//			unsigned endIndex = std::min((unsigned)lights.size(), (unsigned)MAX_DYNAMIC_LIGHTS);
//			auto first = lights.begin() + 0;
//			auto last = lights.begin() + endIndex;
//			auto lightsSlice = vector<Node*>(first, last);
//
//			lights = lightsSlice;
//		}
//
//		if (ambientLightNode) lights.push_back(ambientLightNode);
//
//		auto numLights = lights.size();
//		LightGLSLStruct lightStruct[numLights];
//
//		stats.lights = std::max(int(0), int(numLights - 1)); // not counting ambient
//
//		// TODO: move this?
//		// should useDefaultLighing be a root uniform or elsewhere?
//		if (((numLights == 0) && scene.visualWorld()->autoEnablesDefaultLighting())) {
//
//			Program::Default().setUniform("useDefaultLighting", true);
//		}
//		else {
//
//			Program::Default().setUniform("useDefaultLighting", false);
//
//			for (unsigned l = 0; l < numLights; ++l) {
//				auto node = lights[l];
//				auto light = node->light();
//
//				lightStruct[l].type = static_cast<unsigned>(light->type());
//				lightStruct[l].position_world = node->worldPosition();
//				lightStruct[l].attenuationFactor = light->attenuationFactor();
//
//				auto color = *light->color();
//				lightStruct[l].color = {color.r, color.g, color.b, color.a};
//			}
//		}
//
//		environmentStruct.numLights = numLights;
//		memcpy(&environmentStruct.lights, &lightStruct, sizeof(lightStruct));
//	}
//
//	// fog
//
//	auto visualWorld = scene.visualWorld();
//	FogGLSLStruct fogStruct;
//	fogStruct.startDistance = visualWorld->fogStartDistance();
//	fogStruct.endDistance = visualWorld->fogEndDistance();
//	fogStruct.densityExponent = visualWorld->fogDensityExponent();
//	fogStruct.startDistance = visualWorld->fogStartDistance();
//	auto fogColor = visualWorld->fogColor();
//	if (visualWorld->fogColor()) fogStruct.color = {fogColor->r,
//													fogColor->g,
//													fogColor->b,
//													fogColor->a};
//	else fogStruct.color = {0.0, 0.0, 0.0, 0.0};
//
//	memcpy(&environmentStruct.fog, &fogStruct, sizeof(fogStruct));
//
//	// send 'em
//
//	glBindBuffer(GL_UNIFORM_BUFFER, glEnvironmentUBO);
//	glBufferData(GL_UNIFORM_BUFFER, sizeof(environmentStruct), &environmentStruct, GL_DYNAMIC_DRAW);
//}

void SetTextureSamplingOptions(Texture& texture,
							   GLuint glTextureHandle) {

	auto sampler = texture.sampler();
	bool isCubemap = holds_alternative<shared_ptr<CubeImage>>(texture.contents());

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

void SetMaterialFilteringOptions(const Material& material,
								 map<MaterialPropertyType, GLuint>& glTextureHandles) {

	for (auto& [property, type] : material.properties()) {

		if (auto texture = get_if<shared_ptr<Texture>>(property)) {
			SetTextureSamplingOptions(**texture, glTextureHandles[type]);
		}

//		if (holds_alternative<shared_ptr<Texture>>(*property)) {
//			auto texture = get<shared_ptr<Texture>>(*property);
//			SetTextureSamplingOptions(*texture, glTextureHandles[type]);
//		}
	}
}

void SetMaterialOpenGLState(const Material& material,
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
		
#ifdef OPENGL_CORE
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glEnable(GL_LINE_SMOOTH);
#endif
	}
	else {
#ifdef OPENGL_CORE
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
	
void SetSkyboxOpenGLState() {
	
	glDepthMask(GL_FALSE);
#ifdef OPENGL_CORE
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
#endif
	glDisable(GL_CULL_FACE);
//	glDisable(GL_BLEND);
}

void SetLinesGLState() {
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

void DrawMeshElement(MeshElement& element,
					 Program& program,
					 const mat4& modelMat,
					 const mat4& viewMat,
					 const mat4& projectionMat,
					 GLuint vao, GLuint ebo) {

	program.use();

	// uniforms
	
	program.setUniform("modelMat", modelMat);
	program.setUniform("viewMat", viewMat);
	program.setUniform("projMat", projectionMat);
	
	// update
	
	glBindVertexArray(vao);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	auto numFaces = element.faces().size();
	glDrawElements(GL_TRIANGLES, (GLsizei)numFaces*3, GL_UNSIGNED_INT, nullptr);
}

void DrawSkyboxElement(MeshElement& element,
					   Program& program,
					   Node& pointOfView,
					   GLuint vao, GLuint ebo) {

	program.use();

	auto viewMat = lookAt({0.0f, 0.0f, 0.0f}, // eye - location
						  pointOfView.worldForward(), // center - look at
						  pointOfView.worldUp()); // up
	
	auto projectionMat = pointOfView.camera()->projection();
	
	program.setUniform("viewMat", viewMat);
	program.setUniform("projMat", projectionMat);
	
	// update

	auto& faces = element.faces();
	
	glBindVertexArray(vao);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	auto numFaces = faces.size();
	glDrawElements(GL_TRIANGLES, (GLsizei)numFaces*3, GL_UNSIGNED_INT, nullptr);
}

void DrawLines(const vector<Line>& lines,
			   Program& program,
			   const mat4& modelMat,
			   const mat4& viewMat,
			   const mat4& projectionMat,
			   GLuint glVAO) {

	program.use();

	// uniforms
	
	program.setUniform("modelMat", modelMat);
	program.setUniform("viewMat", viewMat);
	program.setUniform("projMat", projectionMat);
	
	// update
	
	glBindVertexArray(glVAO);
	glDrawArrays(GL_LINES, 0, (GLsizei)lines.size() * 4);
}

void CleanupMeshElementResources(unordered_set<MeshElement*>& active,
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

void CleanupTextureResources(unordered_set<Texture*> &active,
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
void CleanupLinesResources(unordered_set<const vector<Line>*>& active,
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



//	// gather sorted vector of Lines used this frame
//	auto activeSorted = vector<const vector<Line>*>();
//	activeSorted.reserve(glMapping.size());
//	copy(active.begin(), active.end(), back_inserter(activeSorted));
//	sort(activeSorted.begin(), activeSorted.end());
//
//
//
//	bool activeGZero = false;
//	static bool wasActiveGZero = false;
//	if (active.size() > 0) {
//		activeGZero = true;
//	}
//	else {
//		activeGZero = false;
//	}
//	if (!activeGZero && wasActiveGZero) {
//		A3D_LOG_I("Newly inactive.");
//	}
//	else if (activeGZero && !wasActiveGZero) {
//		A3D_LOG_I("Newly active.");
//	}
//	wasActiveGZero = activeGZero;
//
//
//
//	// gather sorted vector of Lines in the mapping
//	auto allSorted = vector<const vector<Line>*>();
//	allSorted.reserve(glMapping.size());
//	for (auto [lines, glRes] : glMapping) {
//		allSorted.push_back(lines);
//	}
//	sort(allSorted.begin(), allSorted.end());
//
//	//A3D_LOG_D("allSorted: {}", allSorted.size());
//	//A3D_LOG_D("activeSorted: {}", activeSorted.size());
//
//	auto unused = vector<const vector<Line>*>();
//	vector<const vector<Line>*>::iterator it;
//
//	auto allB = allSorted.begin();
//	auto allE = allSorted.end();
//	auto activeB = activeSorted.begin();
//	auto activeE = activeSorted.end();
//	auto unusedB = unused.begin();
//
////	it = set_difference(allSorted.begin(), allSorted.end(),
////						activeSorted.begin(), activeSorted.end(),
////						unused.begin());
////	//unused.resize(it - unused.begin());
////
////	//A3D_LOG_D("unused: {}", unused.size());
////
////	if (unused.size()) {
////		//A3D_LOG_D("Deleting GL resources for {} line sets...", unused.size());
////
////		for (auto lines : unused) {
////			DeleteLinesGLResources(*lines, glMapping);
////		}
////	}
//



	for (auto lines : active) {
		DeleteLinesGLResources(*lines, glMapping);
	}

	active.clear();

#endif
}

void DeleteMeshElementGLResources(MeshElement* element,
								  OpenGLRenderer::MeshElementGLMapping& glMapping) {
#ifndef DISABLE_RESOURCE_MANAGEMENT
	
	if (glMapping.count(element)) {
		
		A3D_LOG_D("Deleting GL resources for MeshElement {:p}...",
				  static_cast<void*>(element));
		
		auto glHandles = glMapping[element];
		
		GLuint vbo = get<0>(glHandles);
		GLuint vao = get<1>(glHandles);
		GLuint ebo = get<2>(glHandles);
		
		glDeleteBuffers(1, &vbo);
		glDeleteVertexArrays(1, &vao);
		glDeleteBuffers(1, &ebo);
		
		glMapping.erase(element);

		element->dirtyMask(A3D_MASK_REMOVE(element->dirtyMask(),
										  MeshElementDirtyMask::VertexData));
	}
	
#endif
}

void DeleteTextureGLResources(Texture* texture,
							  OpenGLRenderer::TextureGLMapping& glMapping) {
#ifndef DISABLE_RESOURCE_MANAGEMENT
	
	if (glMapping.count(texture)) {
		
		A3D_LOG_D("Deleting GL resources for MaterialProperty {:p}...",
				  static_cast<void*>(texture));

		auto handle = glMapping[texture];
		
		glDeleteTextures(1, &handle);

		glMapping.erase(texture);

		texture->dirtyMask(A3D_MASK_REMOVE(texture->dirtyMask(),
										  TextureDirtyMask::All));
	}

#endif
}

void DeleteLinesGLResources(const vector<Line>& lines,
							OpenGLRenderer::LinesGLMapping& glMapping) {
#ifndef DISABLE_RESOURCE_MANAGEMENT
	
//	if (glMapping.count(&lines)) {

		//A3D_LOG_T("Deleting GL resources for Lines {:p}..", static_cast<const void*>(&lines));

		auto glHandles = glMapping[&lines];

		auto vbo = get<0>(glHandles);
		auto vao = get<1>(glHandles);

		glDeleteBuffers(1, &vbo);
		glDeleteVertexArrays(1, &vao);

		glMapping.erase(&lines);
//	}
	
#endif
}

vector<Node*> SortedLights(map<Node*, float> lights) {
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

void InitImgui(const RenderContext& context) {

	using namespace ImGui;

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.IniFilename = nullptr;

	GLFWwindow* glfwWindow = dynamic_cast<const Window*>(&context)->glfwWindow();

	ImGui_ImplGlfw_InitForOpenGL(glfwWindow, true);
	ImGui_ImplOpenGL3_Init();
}

void UpdateImguiScale(const RenderContext& context, const Font& overLayFont, const Font& bodyFont) {
	// https://github.com/ocornut/imgui/blob/master/docs/FAQ.md#q-how-should-i-handle-dpi-in-my-application
	// https://github.com/ocornut/imgui/discussions/3925
	// https://github.com/ocornut/imgui/issues/3757
	// https://gist.github.com/benpm/21afb58f2c8dfdbf881ca90c76ad602e
	// https://gist.github.com/benpm/21afb58f2c8dfdbf881ca90c76ad602e#file-high_dpi-cpp-L2

	using namespace ImGui;

	ImGui_ImplOpenGL3_DestroyFontsTexture();

	// clear all the font data,
	// re-add the fonts with the new oversample scales,
	// and re-create the font atlas data.

	ImGuiIO& io = ImGui::GetIO();
	io.Fonts->Clear(); // works
	io.Fonts->ClearFonts(); // crashes by itself
	io.Fonts->ClearTexData(); // does not work, but does not crash

	AddImguiFont(context, overLayFont, OpenGLRenderer::STATS_TITLE_FONT_SIZE);
	AddImguiFont(context, bodyFont, OpenGLRenderer::STATS_BODY_FONT_SIZE);

	ImGui_ImplOpenGL3_CreateFontsTexture();
}

void AddImguiFont(const RenderContext& context, const Font& font, float size) {

	auto scaleXY = context.framebufferScale();

	ImFontConfig fontConfig;

	fontConfig.OversampleH = (int)std::ceil(scaleXY.x);
	fontConfig.OversampleV = (int)std::ceil(scaleXY.y);

	// by default Imgui transferrs font memory ownership to itself
	// this means Imgui eventually frees the font data, and then the Font/Buffer double-free it
	fontConfig.FontDataOwnedByAtlas = false;

	ImGuiIO& io = ImGui::GetIO();

	io.DisplayFramebufferScale = ImVec2(scaleXY.x, scaleXY.y);

	io.Fonts->AddFontFromMemoryTTF(font.buffer()->data(),
								   (int)font.buffer()->size(),
								   size,
								   &fontConfig);
}

void DrawStatsOverlay(Stats& stats, const RenderContext& context) {

	using namespace ImGui;

#ifdef WINDOWS
	auto scaleXY = context.framebufferScale();
	auto scale = std::max(scaleXY.x, scaleXY.y);
	// this is probably going to need more attention when we start
	// using Imgui for more than just rendering text
	//ImGui::GetStyle().ScaleAllSizes(scale);
	ImGui::GetIO().FontGlobalScale = scale;
#endif

	constexpr int PADDING = 20;

	string recordingStr;
	if (context.recordingGIF()) {
		auto time = context.recordedGIFTime();
		auto numFrames = context.recordedGIFFrames();
		recordingStr = fmt::format("\n{:<{}} {:.1f} s / {} {}",
								   "RECORDING",
								   PADDING,
								   time,
								   numFrames,
								   (numFrames==1 ? "frame" : "frames"));
	}
	else {
		recordingStr = "";
	}

	auto buildInfo = BuildInfo::Info();
	auto version = buildInfo.version();

	auto str = fmt::format(
			"v{}.{}.{} build {}\n" \
			 "{}\n"
			"\n" \

			"{:<{}} {:.2f} ms\n" \
			 "{:<{}} {:.2f} ms\n" \
			 "{:<{}} {:.2f} ms\n" \
			 "{:<{}} {:.2f} ms\n" \
			 "{:<{}} {:.0f} fps {}\n" \
			 "\n" \

			"{:<{}} ({}, {})\n" \
			"{:<{}} {}\n" \
			 "{:<{}} ({:.1f}, {:.1f})\n" \
			 "\n" \

			"{:<{}} {}\n" \
			 "{:<{}} {}\n" \
			 "{:<{}} {}\n" \
			 "{:<{}} {:.1f}k\n" \
			 "{:<{}} {}\n" \
			 "\n" \
			 "{:<{}} {}\n" \
			 "{:<{}} {}\n" \
			 "{:<{}} {}\n" \
			 "{:<{}} {}\n" \
			 "{:<{}} {}\n" \
			 "{:<{}} {}\n" \
			 "{:<{}} {}\n" \
			 "{:<{}} {}\n" \
			 "{:<{}} {}\n" \
			 "\n" \
			 "{:<{}} ({:.1f}, {:.1f}, {:.1f})\n" \
			 "{:<{}} ({:.4f}, {:.4f}, {:.4f}, {:.4f})\n" \
			 "{}",

			version.major, version.minor, version.patch, buildInfo.number(),
			buildInfo.type() == BuildInfo::Type::Debug ? "debug" : "release",

			"frametime", PADDING, stats.averageFrametime,
			" draw", PADDING, stats.averageDrawtime,
			" physics", PADDING, stats.averagePhysicstime,
			" user", PADDING, stats.averageUsertime,
			"framerate", PADDING, stats.averageFramerate, (context.vSyncEnabled() ? "[vsync]" : ""),

			"resolution", PADDING, context.framebufferSize().x, context.framebufferSize().y,
			"antialiasing", PADDING, StatusOverlayDescriptionForAntialiasingMode(context.antialiasingMode()),
			"framebuffer scale", PADDING, context.framebufferScale().x, context.framebufferScale().y,

			"nodes", PADDING, stats.nodes,
			"meshes", PADDING, stats.meshes,
			"elements", PADDING, stats.elements,
			"polygons", PADDING, float(stats.polygons)/1000.0f,//(int)round(float(stats.polygons)/1000.0f),
			"lights", PADDING, stats.lights,

			"physics bodies", PADDING, stats.dynamicBodies + stats.kinematicBodies + stats.staticBodies,
			" static", PADDING, stats.staticBodies,
			" dynamic", PADDING, stats.dynamicBodies,
			" kinematic", PADDING, stats.kinematicBodies,
			"physics shapes", PADDING, stats.concavePolyhedronShapes + stats.boundingBoxShapes + stats.convexHullShapes,
			" primitive", PADDING, stats.primitiveShapes,
			" bounding box", PADDING, stats.boundingBoxShapes,
			" convex hull", PADDING, stats.convexHullShapes,
			" concave polyhedron", PADDING, stats.concavePolyhedronShapes,

			"camera position", PADDING, stats.cameraPosition.x, stats.cameraPosition.y, stats.cameraPosition.z,
			"camera orientation", PADDING, stats.cameraOrientation.x, stats.cameraOrientation.y, stats.cameraOrientation.z, stats.cameraOrientation.w,
			recordingStr);

	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	NewFrame();

	ImGuiWindowFlags windowFlags = 0;
	windowFlags |= ImGuiWindowFlags_NoTitleBar;
	windowFlags |= ImGuiWindowFlags_NoScrollbar;
	windowFlags |= ImGuiWindowFlags_NoMove;
	windowFlags |= ImGuiWindowFlags_NoResize;
	windowFlags |= ImGuiWindowFlags_NoCollapse;
	windowFlags |= ImGuiWindowFlags_NoNav;
	windowFlags |= ImGuiWindowFlags_AlwaysAutoResize;

	ImGuiIO& io = ImGui::GetIO();
	auto fonts = io.Fonts->Fonts;

	// draw the text shadow
	SetNextWindowBgAlpha(0);
	Begin("StatsTextShadow", nullptr, windowFlags);
	ImGuiStyle& style = GetStyle();
	style.WindowBorderSize = 0;
	SetWindowPos({10.0f, 2.0f});
	ImVec2 cursorPos = ImGui::GetCursorPos();
	ImGui::SetCursorPos(ImVec2(cursorPos.x + 1.0, cursorPos.y + 1.0));
	ImGui::PushFont(fonts[0]);
	TextColored(ImVec4{0, 0, 0, .5}, "avara3d");
	ImGui::PopFont();
	cursorPos = ImGui::GetCursorPos();
	ImGui::SetCursorPos(ImVec2(cursorPos.x, cursorPos.y + OpenGLRenderer::STATS_TITLE_TO_BODY_PADDING));
	ImGui::PushFont(fonts[1]);
	TextColored(ImVec4{0, 0, 0, .5}, "%s", str.c_str());
	ImGui::PopFont();
	End();

	// draw the text
	SetNextWindowBgAlpha(0);
	Begin("StatsText", nullptr, windowFlags);
	SetWindowPos({10.0f, 2.0f});
	ImGui::PushFont(fonts[0]);
	TextColored(ImVec4{1, 1, 1, 1}, "avara3d");
	ImGui::PopFont();
	cursorPos = ImGui::GetCursorPos();
	ImGui::SetCursorPos(ImVec2(cursorPos.x, cursorPos.y + OpenGLRenderer::STATS_TITLE_TO_BODY_PADDING));
	ImGui::PushFont(fonts[1]);
	TextColored(ImVec4{1, 1, 1, 1}, "%s", str.c_str());
	ImGui::PopFont();
	End();

	Render();
	ImGui_ImplOpenGL3_RenderDrawData(GetDrawData());
}

string StatusOverlayDescriptionForAntialiasingMode(AntialiasingMode mode) {

	switch (mode) {
		case AntialiasingMode::Msaa2X: return "2x msaa";
		case AntialiasingMode::Msaa4X: return "4x msaa";
		case AntialiasingMode::Msaa8X: return "8x msaa";
		case AntialiasingMode::Msaa16X: return "16x msaa";
		default: return "none";
	}
}

void SetTextureMinificationFilter(GLuint glTextureHandle, bool cube, FilterMode mode) {
	
	auto texType = (cube ? GL_TEXTURE_CUBE_MAP : GL_TEXTURE_2D);

	switch (mode) {
		case FilterMode::NearestMipmapNearest:
		case FilterMode::NearestMipmapLinear:
		case FilterMode::LinearMipmapNearest:
		case FilterMode::LinearMipmapLinear:
			glBindTexture(texType, glTextureHandle);
			glGenerateMipmap(texType);
			break;
		default:
			break;
	}

	glTexParameteri(texType, GL_TEXTURE_MIN_FILTER, (GLint)GLFilterModeForFilterMode(mode));
}

void SetTextureMagnificationFilter(GLuint glTextureHandle, bool cube, FilterMode mode) {

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

void SetTextureMaxAnisotropy(GLuint glTextureHandle, bool cube, float max) {
#ifdef OPENGL_CORE

	auto texType = (cube ? GL_TEXTURE_CUBE_MAP : GL_TEXTURE_2D);
	
	float anisotropy = max;
	glBindTexture(texType, glTextureHandle);
	float largest;
	// EXT_texture_filter_anisotropic
	glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &largest);
	if (max > largest) anisotropy = largest;
	glTexParameterf(texType, GL_TEXTURE_MAX_ANISOTROPY_EXT, anisotropy);

#endif
}

void SetTextureWrapS(GLuint glTextureHandle, bool cube, WrapMode mode) {

	auto texType = (cube ? GL_TEXTURE_CUBE_MAP : GL_TEXTURE_2D);
	
	glBindTexture(texType, glTextureHandle);
	glTexParameteri(texType, GL_TEXTURE_WRAP_S, (GLint)GLWrapModeForWrapMode(mode));
}

void SetTextureWrapT(GLuint glTextureHandle, bool cube, WrapMode mode) {

	auto texType = (cube ? GL_TEXTURE_CUBE_MAP : GL_TEXTURE_2D);
	
	glBindTexture(texType, glTextureHandle);
	glTexParameteri(texType, GL_TEXTURE_WRAP_T, (GLint)GLWrapModeForWrapMode(mode));
}

void SetTextureWrapR(GLuint glTextureHandle, WrapMode mode) {
	
	glBindTexture(GL_TEXTURE_CUBE_MAP, glTextureHandle);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, (GLint)GLWrapModeForWrapMode(mode));
}

GLenum GLFilterModeForFilterMode(FilterMode mode) {
	switch (mode) {
		case FilterMode::Nearest: 				return GL_NEAREST;
		case FilterMode::Linear: 				return GL_LINEAR;
		case FilterMode::NearestMipmapNearest:	return GL_NEAREST_MIPMAP_NEAREST;
		case FilterMode::LinearMipmapNearest: 	return GL_LINEAR_MIPMAP_NEAREST;
		case FilterMode::NearestMipmapLinear: 	return GL_NEAREST_MIPMAP_LINEAR;
		case FilterMode::LinearMipmapLinear: 	return GL_LINEAR_MIPMAP_LINEAR; }
}

GLenum GLWrapModeForWrapMode(WrapMode mode) {
	switch (mode) {
		case WrapMode::ClampToEdge:				return GL_CLAMP_TO_EDGE;
//#ifdef OPENGL_CORE
//		case WRAP_MODE::CLAMP_TO_BORDER:		return GL_CLAMP_TO_BORDER;
//#endif
		case WrapMode::Repeat:					return GL_REPEAT;
        default: /* MIRRORED_REPEAT */   		return GL_MIRRORED_REPEAT; }
}

void CheckGLError() {
	auto err = glGetError();
	if (err != GL_NO_ERROR) {
		A3D_LOG_E("*** GL error: 0x{:X} ***", err);
	}
}
