//
//  OpenGLRenderer.cc
//  avara3d
//
//  Created by Morgan Davis on 4/24/18.
//  Copyright © 2024 Morgan K Davis. All rights reserved.
//

#include "a3d/rendering/renderer/opengl/OpenGLRenderer.h"

#include <algorithm>
#include <format>
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
#include "imgui_impl_opengl3.h"
#include "implot.h"
//#endif

#include "magic_enum.hpp"

#include "a3d/Buffer.h"
#include "a3d/BuildInfo.h"
#include "a3d/Color.h"
#include "a3d/Configuration.h"
#include "a3d/CubeImage.h"
#include "a3d/diagnostic/log/Log.h"
#include "a3d/Font.h"
#include "a3d/Image.h"
#include "a3d/Math.h"
#include "a3d/mesh/Mesh.h"
#include "a3d/mesh/MeshElement.h"
#include "a3d/mesh/Line.h"
#include "a3d/rendering/VisualWorld.h"
#include "a3d/rendering/camera/Camera.h"
#include "a3d/rendering/context/RenderContext.h"
#include "a3d/rendering/light/AmbientLight.h"
#include "a3d/rendering/light/DirectionalLight.h"
#include "a3d/rendering/light/PointLight.h"
#include "a3d/rendering/light/SpotLight.h"
#include "a3d/rendering/material/Material.h"
#include "a3d/rendering/material/Sampler.h"
#include "a3d/rendering/material/Texture.h"
#include "a3d/rendering/renderer/opengl/Program.h"
#include "a3d/scene/Node.h"
#include "a3d/scene/Scene.h"
#include "a3d/Utilities.h"

#define A3D_GL_CHECK() \
    do { \
        GLenum err; \
        while ((err = glGetError()) != GL_NO_ERROR) { \
            A3D_LOG_E("GL error 0x{:X}", err); \
        } \
    } while (0);

using namespace a3d;
using namespace a3d::math;
using namespace std;

//#define DISABLE_RESOURCE_MANAGEMENT

///  Private Constants ///

const std::string 	STATS_TITLE_FONT_NAME  		{"SourceCodePro-Bold"};
const std::string 	STATS_TITLE_FONT_TYPE  		{"otf"};
const float 		STATS_TITLE_FONT_SIZE 		{23.0};
const std::string 	STATS_BODY_FONT_NAME  		{"SourceCodePro-Semibold"};
const std::string 	STATS_BODY_FONT_TYPE  		{"otf"};
const float 		STATS_BODY_FONT_SIZE 		{15.0};

/// Private Types ///

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
	f32			PAD0_;
} DirectionalLightGLSLStruct;

typedef struct {
	vec4		color;
	vec3		position_world;
	f32			PAD0_;
	f32			constantAttenuation;
	f32			linearAttenuation;
	f32			quadraticAttenuation;
	f32			PAD1_;
} PointLightGLSLStruct;

typedef struct {
	vec4		color;
	vec3		position_world;
	f32			PAD0_;
	vec3		direction_world;
	f32			PAD1_;
	f32			innerAngleCos;
	f32			outerAngleCos;
	uint32_t	featheringMode;
	f32			constantAttenuation;
	f32			linearAttenuation;
	f32			quadraticAttenuation;
	f32			PAD2_;
	f32			PAD3_;
} SpotLightGLSLStruct;

typedef struct {
	vec4		color;
	f32			startDistance;
	f32			endDistance;
	f32			densityExponent;
	f32			PAD0_;
} FogGLSLStruct;

typedef struct {
	alignas(16)	uint32_t 					numAmbientLights;
	alignas(16) AmbientLightGLSLStruct		ambientLights[a3d::config::MAX_AMBIENT_LIGHTS];
	alignas(16) uint32_t 					numDirectionalLights;
	alignas(16) DirectionalLightGLSLStruct	directionalLights[a3d::config::MAX_DIRECTIONAL_LIGHTS];
	alignas(16) uint32_t 					numPointLights;
	alignas(16) PointLightGLSLStruct		pointLights[a3d::config::MAX_POINT_LIGHTS];
	alignas(16) uint32_t 					numSpotLights;
	alignas(16) SpotLightGLSLStruct			spotLights[a3d::config::MAX_SPOT_LIGHTS];
	alignas(16) FogGLSLStruct				fog;
} EnvironmentBlock;

//struct GpuTimer {
//	static const int MAX_FRAMES = 4;
//	GLuint queries[MAX_FRAMES]{};
//	int writeIndex = 0;
//	int readIndex = 1;
//	double lastMs = 0.0;
//};
//GpuTimer g_timer;

/// Private Static Non-Member Prototypes ///

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
											FrameStats& stats);
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
static vector<Node*>SortedLights(map<Node*, float> lights);
static void			InitImgui(const RenderContext& context);
static void 		UpdateImguiScale(const RenderContext& context,
									 const Font& overLayFont,
									const Font& bodyFont);
static void 		AddImguiFont(const RenderContext& context, const Font& font, float size);
void 				DigUpdateGlobalFontScale(const RenderContext& context);
void 				DigEndWindow();
void 				DigDrawText(float x,
								float y,
								const char* text,
								int font,
								float xOffset,
								float yOffset,
								int id);
void 				DigDrawPlot(float x, float y, float w, float h,
								const float* values,
								int valuesCount,
								int valuesOffset,
								const char* overlayText,
								float scaleMin,
								float scaleMax,
								int stride,
								bool outlined,
								int id);
bool 				DigDrawCheckbox(float x,
									float y,
									const char* text,
									bool& checked,
									int font,
									int id);
static void 		DrawDebugOptions(Scene& scene, const RenderContext& context);
static void 		DrawStatsOverlay(FrameStats& stats,
									const FrameStatsHistory& statsHistory,
									const RenderContext& context);
static string 		StatusOverlayDescriptionForAntialiasingMode(AntialiasingMode mode);
static void 		SetTextureMinificationFilter(GLuint glTextureHandle,
												bool cube,
												FilterMode mode);
static void 		SetTextureMagnificationFilter(GLuint glTextureHandle,
												 bool cube,
												 FilterMode mode);
static void 		SetTextureMaxAnisotropy(GLuint glTextureHandle, bool cube, float max);
static void 		SetTextureWrapS(GLuint glTextureHandle, bool cube, WrapMode mode);
static void 		SetTextureWrapT(GLuint glTextureHandle, bool cube, WrapMode mode);
static void 		SetTextureWrapR(GLuint glTextureHandle, WrapMode mode);
static GLenum 		GLFilterModeForFilterMode(FilterMode mode);
static GLenum 		GLWrapModeForWrapMode(WrapMode mode);
static void 		LogGLInfo();
//static void 		CheckGLError();

/// Private Static Members ///

bool OpenGLRenderer::InitGL(GLGetProcAddress getProcAddress) {

	static bool initialized = false;
	if (initialized) return true;

	if (!getProcAddress) {
		// log error, return false
		A3D_LOG_E("getProcAddress is null.");
		return false;
	}

	int status = gladLoadGLLoader((GLADloadproc)getProcAddress);
	if (status == 0) {
		// log "Failed to initialize GLAD"
		A3D_LOG_E("gladLoadGLLoader");
		return false;
	}

	initialized = true;

//	const GLubyte *vendor = glGetString(GL_VENDOR);
//	const GLubyte *renderer = glGetString(GL_RENDERER);
//	const GLubyte *version = glGetString(GL_VERSION);
//	A3D_LOG_I("GL_VENDOR  : {}", vendor ? reinterpret_cast<const char *>(vendor) : "null");
//	A3D_LOG_I("GL_RENDERER: {}", renderer ? reinterpret_cast<const char *>(renderer) : "null");
//	A3D_LOG_I("GL_VERSION : {}", version ? reinterpret_cast<const char *>(version) : "null");

	LogGLInfo();

	return true;
}

/// Internal Lifecycle Functions ///

OpenGLRenderer::OpenGLRenderer():
		Renderer{},
		_isInitialized{false},
		_meshElementGLMapping{},
		_textureGLMapping{},
		_linesGLMapping{},
		_activeMeshElements{},
		_activeTextures{},
		_activeLines{},
		_glEnvironmentUBO{0},
		_overlayTitleFont{},
		_overlayBodyFont{}
		/*_defaultFramebuffer{0}*/ { }

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
//	ImGui_ImplGlfw_Shutdown();
	ImPlot::DestroyContext();
	ImGui::DestroyContext();
}
	
/// Renderer Internal Member Functions ///

RenderingApi OpenGLRenderer::renderingApi() const {
	return RenderingApi::OpenGL;
}

bool OpenGLRenderer::initialize(const RenderContext& context) {
	
	A3D_LOG_I("");

	// create environment UBO

	uint32_t ubo;
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

	_isInitialized = true;

	return true;
}

bool OpenGLRenderer::isInitialized() const {
	return _isInitialized;
}

GLuint g_gpuTimeQuery;
void OpenGLRenderer::beginFrame(const Scene& scene,
								const RenderContext& context,
								const DebugOptions& debugOptions,
								FrameStats& stats,
								Profiler& profiler) {


//	GLuint available = GL_FALSE;
//	glGetQueryObjectuiv(g_timer.queries[g_timer.readIndex], GL_QUERY_RESULT_AVAILABLE, &available);
//	if (available) {
//		GLuint64 ns = 0;
//		glGetQueryObjectui64v(g_timer.queries[g_timer.readIndex], GL_QUERY_RESULT, &ns);
//		g_timer.lastMs = ns / 1e6;
//		g_timer.readIndex = (g_timer.readIndex + 1) % GpuTimer::MAX_FRAMES;
//
//		auto gpuTimeNs = std::chrono::nanoseconds{
//				static_cast<std::chrono::nanoseconds::rep>(ns)
//		};
//		profiler.add(Profiler::Tag::RenderGpu, gpuTimeNs);
//	}


	if (g_gpuTimeQuery> 0) {
		GLuint64 ns = 0;
		glGetQueryObjectui64v(g_gpuTimeQuery, GL_QUERY_RESULT, &ns);
		auto gpuTimeNs = std::chrono::nanoseconds{
				static_cast<std::chrono::nanoseconds::rep>(ns)
		};
		profiler.add(Profiler::Tag::RenderGpu, gpuTimeNs);
	}

	glGenQueries(1, &g_gpuTimeQuery);
	glBeginQuery(GL_TIME_ELAPSED, g_gpuTimeQuery);

//	glBeginQuery(GL_TIME_ELAPSED, g_timer.queries[g_timer.writeIndex]);

	_activeMeshElements.clear();
	_activeTextures.clear();
	_activeLines.clear();

	// see ordering note in DrawStatsOverlay()
	ImGui_ImplOpenGL3_NewFrame();
	// these have to be called in this order for input to work.
	// ImGui_ImplOpenGL3_NewFrame(); -> in beginFrame()
	// ImGui_ImplGlfw_NewFrame() -> in GLFWWindow::beginFrame()
}

void OpenGLRenderer::endFrame(const Scene& scene,
							  const RenderContext& context,
							  const DebugOptions& debugOptions,
							  FrameStats& stats,
							  Profiler& profiler,
							  const FrameStatsHistory& statsHistory) {

	ImGui::NewFrame();
	DigUpdateGlobalFontScale(context);
	DrawDebugOptions(const_cast<Scene&>(scene), context); // TODO: CHEATING
	if (A3D_MASK_CONTAINS(debugOptions, DebugOptions::ShowStatsOverlay)) {
		DrawStatsOverlay(stats, statsHistory, context);
	}
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	CleanupMeshElementResources(_activeMeshElements, _meshElementGLMapping);
	CleanupTextureResources(_activeTextures, _textureGLMapping);
	CleanupLinesResources(_activeLines, _linesGLMapping);

//	CheckGLError();
	A3D_GL_CHECK();

	glEndQuery(GL_TIME_ELAPSED);

//	glEndQuery(GL_TIME_ELAPSED);//, g_timer.queries[g_timer.writeIndex]);
//	g_timer.writeIndex = (g_timer.writeIndex + 1) % GpuTimer::MAX_FRAMES;
}

void OpenGLRenderer::preTraversal(const Scene& scene,
								  const RenderContext& context,
								  const DebugOptions& debugOptions,
								  FrameStats& stats) {

}

void OpenGLRenderer::postTraversal(const Scene& scene,
								   const RenderContext& context,
								   const vector<Node*>& lightNodes,
								   const DebugOptions& debugOptions,
								   FrameStats& stats) {

	SendEnvironmentUniforms(_glEnvironmentUBO, scene, lightNodes, stats);
	Program::Default().bindUniformBlock("EnvironmentBlock", _glEnvironmentUBO);
}

void OpenGLRenderer::render(const Scene& scene,
							const RenderContext& context,
							const DebugOptions& debugOptions,
							FrameStats& stats) {

	GLint prevDrawFbo = 0;
	GLint prevReadFbo = 0;
	glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &prevDrawFbo);
	glGetIntegerv(GL_READ_FRAMEBUFFER_BINDING, &prevReadFbo);

	auto renderContext = scene.visualWorld()->renderContext();

	auto framebufferSize = renderContext->framebufferSize();
	auto framebufferWidth = framebufferSize.x;
	auto framebufferHeight = framebufferSize.y;

	glBindFramebuffer(GL_FRAMEBUFFER, context.defaultFramebuffer());
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

	// this is necessary for Qt to paint the widget properly
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, prevDrawFbo);
	glBindFramebuffer(GL_READ_FRAMEBUFFER, prevReadFbo);
}

void OpenGLRenderer::render(Mesh& mesh,
							const RenderContext& context,
							const mat4& modelMat,
							const mat4& viewMat,
							const mat4& projectionMat,
							const DebugOptions& debugOptions,
							FrameStats& stats) {

	if (A3D_MASK_CONTAINS(debugOptions, DebugOptions::ShowBoundingBoxes)) {
		render(mesh.aabbLines(), context, modelMat, viewMat, projectionMat);
	}
}

void OpenGLRenderer::render(MeshElement& element,
							const RenderContext& context,
							Material& material,
							const mat4& modelMat,
							const mat4& viewMat,
							const mat4& projectionMat,
							const DebugOptions& debugOptions,
							FrameStats& stats) {

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
							const RenderContext& context,
							const mat4& modelMat,
							const mat4& viewMat,
							const mat4& projectionMat) {
	
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
	//A3D_LOG_D("context: {:p}", static_cast<const void*>(&context));

	UpdateImguiScale(context, *_overlayTitleFont, *_overlayBodyFont);
}
	
/// Private Static Non-Member Functions ///

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
							 FrameStats& stats) {
	// program "Default" must be active

	// block

	EnvironmentBlock environmentStruct;

	// lights

	if (scene.visualWorld()->usesDefaultLighting()) {

		Program::Default().setUniform("useDefaultLighting", true);
	}
	else {

		auto numLights = lightNodes.size();

		stats.numLights = numLights;

		if (((numLights == 0) && scene.visualWorld()->autoEnablesDefaultLighting())) {

			Program::Default().setUniform("useDefaultLighting", true);
		}
		else {

			Program::Default().setUniform("useDefaultLighting", false);

			vector<AmbientLightGLSLStruct> ambientStructs;
			vector<DirectionalLightGLSLStruct> directionalStructs;
			vector<PointLightGLSLStruct> pointStructs;
			vector<SpotLightGLSLStruct> spotStructs;

			ambientStructs.reserve(config::MAX_AMBIENT_LIGHTS);
			directionalStructs.reserve(config::MAX_DIRECTIONAL_LIGHTS);
			pointStructs.reserve(config::MAX_POINT_LIGHTS);
			spotStructs.reserve(config::MAX_SPOT_LIGHTS);

			for (unsigned l = 0; l < numLights; ++l) {

				auto node = lightNodes[l];
				auto light = node->light().get();
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

				if (auto ambientLight = dynamic_cast<AmbientLight*>(light)) {

					AmbientLightGLSLStruct lightStruct;
					lightStruct.color = ambientLight->color()->rgba();
					ambientStructs.push_back(lightStruct);
				}
				else if (auto directionalLight = dynamic_cast<DirectionalLight*>(light)) {

					DirectionalLightGLSLStruct lightStruct;
					lightStruct.color = directionalLight->color()->rgba();
					lightStruct.direction_world = node->worldForward();
					directionalStructs.push_back(lightStruct);
				}
				else if (auto pointLight = dynamic_cast<PointLight*>(light)) {

					PointLightGLSLStruct lightStruct;
					lightStruct.color = pointLight->color()->rgba();
					lightStruct.position_world = node->worldPosition();
					lightStruct.constantAttenuation = pointLight->constantAttenuation();
					lightStruct.linearAttenuation = pointLight->linearAttenuation();
					lightStruct.quadraticAttenuation = pointLight->quadraticAttenuation();
					pointStructs.push_back(lightStruct);
				}
				else if (auto spotLight = dynamic_cast<SpotLight*>(light)) {

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

	auto viewMat = look_at({0.0f, 0.0f, 0.0f}, // eye - location
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
	ImPlot::CreateContext();
	ImGuiIO& io = GetIO();
	io.IniFilename = nullptr;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	ImGui_ImplOpenGL3_Init();
	//ImGui_ImplOpenGL3_Init("#version 330 core");
}

void UpdateImguiScale(const RenderContext& context,
					  const Font& overLayFont,
					  const Font& bodyFont) {

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

	ImGuiIO& io = GetIO();
	io.Fonts->Clear(); // works
	io.Fonts->ClearFonts(); // crashes by itself
	io.Fonts->ClearTexData(); // does not work, but does not crash

	AddImguiFont(context, overLayFont, STATS_TITLE_FONT_SIZE);
	AddImguiFont(context, bodyFont, STATS_BODY_FONT_SIZE);

	ImGui_ImplOpenGL3_CreateFontsTexture();
}

void AddImguiFont(const RenderContext& context, const Font& font, float size) {

	using namespace ImGui;

	auto scaleXY = context.framebufferScale();

	ImFontConfig fontConfig;

	fontConfig.OversampleH = math::ceil(scaleXY.x);
	fontConfig.OversampleV = math::ceil(scaleXY.y);

	// by default Imgui transferrs font memory ownership to itself
	// this means Imgui eventually frees the font data, and then the Font/Buffer double-free it
	fontConfig.FontDataOwnedByAtlas = false;

	ImGuiIO& io = GetIO();

	io.DisplayFramebufferScale = ImVec2(scaleXY.x, scaleXY.y);

	io.Fonts->AddFontFromMemoryTTF(font.buffer()->data(),
								   (int)font.buffer()->size(),
								   size,
								   &fontConfig);
}

void DigUpdateGlobalFontScale(const RenderContext& context) {

#ifdef WINDOWS
	auto scaleXY = context.framebufferScale();
	auto scale = std::max(scaleXY.x, scaleXY.y);
	// this is probably going to need more attention when we start
	// using Imgui for more than just rendering text
	//GetStyle().ScaleAllSizes(scale);
	ImGui::GetIO().FontGlobalScale = scale;
#endif
}

void DigBeginWindow(float x,
					float y,
					int id) {

	using namespace ImGui;

	static ImGuiWindowFlags windowFlags = 0;
	windowFlags |= ImGuiWindowFlags_NoTitleBar;
	windowFlags |= ImGuiWindowFlags_NoScrollbar;
	windowFlags |= ImGuiWindowFlags_NoMove;
	windowFlags |= ImGuiWindowFlags_NoResize;
	windowFlags |= ImGuiWindowFlags_NoCollapse;
	windowFlags |= ImGuiWindowFlags_NoNav;
	windowFlags |= ImGuiWindowFlags_AlwaysAutoResize;

	SetNextWindowBgAlpha(0);
	Begin(to_string(id).c_str(), nullptr, windowFlags);
	ImGuiStyle &style = GetStyle();
	style.WindowBorderSize = 0;
	SetWindowPos({x, y});
}

void DigEndWindow() {

	using namespace ImGui;
	End();
}

void DigDrawText(float x,
				 float y,
				 const char* text,
				 int font,
				 float xOffset,
				 float yOffset,
				 int id) {

	using namespace ImGui;

	static ImGuiIO &io = GetIO();
	static auto fonts = io.Fonts->Fonts;
	static const ImVec4 textColor {1, 1, 1, 1};

	PushFont(fonts[font]);

	DigBeginWindow(x+1, y+1, id);
	TextColored({0, 0, 0, .5}, "%s", text);
	DigEndWindow();

	DigBeginWindow(x, y, id+1000);
	TextColored(textColor, "%s", text);
	DigEndWindow();

	PopFont();
}

void DigDrawPlot(float x, float y, float w, float h,
				 const float* values,
				 int valuesCount,
				 int valuesOffset,
				 const char* overlayText,
				 float scaleMin,
				 float scaleMax,
				 int stride,
				 bool outlined,
				 int id) {

	using namespace ImGui;

	static const ImVec4 bg(0, 0, 0, 0);
	static const ImVec4 shadow(0.0, 0.0, 0.0, 0.5);
	static const ImVec4 white(1.0, 1.0, 1.0, 1.0);
	static const ImVec4 milk(1.0, 1.0, 1.0, 0.5);

	PushStyleColor(ImGuiCol_FrameBg, bg);

	DigBeginWindow(x+1, y+1, id);
	PushStyleColor(ImGuiCol_Text, shadow);
	PushStyleColor(ImGuiCol_PlotLines, shadow);
	PlotLines("",
			  values,
			  valuesCount,
			  0,
			  nullptr,
			  scaleMin, scaleMax,
			  ImVec2(w, h));
	PopStyleColor(); // black lines
	PopStyleColor(); // black text
	DigEndWindow();

	DigBeginWindow(x, y, id+1000);
	PushStyleColor(ImGuiCol_Text, shadow);
	PushStyleColor(ImGuiCol_PlotLines, white);
	if (outlined) {
		PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0.5f);
		PushStyleColor(ImGuiCol_Border, milk);
	}
	PlotLines("",
			  values,
			  valuesCount,
			  0,
			  nullptr,
			  scaleMin, scaleMax,
			  ImVec2(w, h));
	if (outlined) {
		PopStyleColor(); // ImGuiCol_Border
		PopStyleVar(); // ImGuiStyleVar_FrameBorderSize
	}
	PopStyleColor(); // black lines
	PopStyleColor(); // black text
	DigEndWindow();

	PopStyleColor(); // background
}

bool DigDrawCheckbox(float x,
					 float y,
					 const char* text,
					 bool& checked,
					 int font,
					 int id) {

	using namespace ImGui;

	static const ImVec4 transparent(0, 0, 0, 0);
	static const ImVec4 shadow(0.0, 0.0, 0.0, 0.5);
	static const ImVec4 checkbg1(0.25, 0.25, 0.25, 0.5);
	static const ImVec4 checkbg2(0.5, 0.5, 0.5, 0.5);
	static const ImVec4 white(1.0, 1.0, 1.0, 1.0);

	ImGuiIO& io = GetIO();
	auto fonts = io.Fonts->Fonts;
	PushFont(fonts[font]);

	DigBeginWindow(x+1, y+1, id);
	BeginDisabled();
	PushStyleColor(ImGuiCol_Text, shadow);
	PushStyleColor(ImGuiCol_FrameBg, transparent);
	PushStyleColor(ImGuiCol_FrameBgHovered, transparent);
	PushStyleColor(ImGuiCol_FrameBgActive, transparent);
	PushStyleColor(ImGuiCol_CheckMark, transparent);
	static bool dummy = false;
	Checkbox(text, &dummy);
	EndDisabled();
	PopStyleColor(5);
	DigEndWindow();

	DigBeginWindow(x, y, id+5000);
	PushStyleColor(ImGuiCol_FrameBg,        checkbg1);
	PushStyleColor(ImGuiCol_FrameBgHovered, checkbg2);
	PushStyleColor(ImGuiCol_FrameBgActive,  checkbg1);
	PushStyleColor(ImGuiCol_CheckMark,      white);
	bool ret = Checkbox(text, &checked);
	PopStyleColor(4);
	DigEndWindow();

	PopFont();

	return ret;
}


void DrawDebugOptions(Scene& scene, const RenderContext& context) {

	using namespace ImGui;

	// TODO: refactor
#ifdef WINDOWS
	auto scaleXY = context.framebufferScale();
	auto scale = std::max(scaleXY.x, scaleXY.y);
	// this is probably going to need more attention when we start
	// using Imgui for more than just rendering text
	//GetStyle().ScaleAllSizes(scale);
	GetIO().FontGlobalScale = scale;
#endif

	ImGuiIO& io = GetIO();

	const float WIN_WIDTH = 180;
	const float xPos = io.DisplaySize.x - WIN_WIDTH;
	float yPos = 0;
	int id = 0;
	static const float Y_PAD = 24.0;

	auto debugOptions = scene.debugOptions();

	yPos = 3.0;
	static bool stats = A3D_MASK_CONTAINS(debugOptions, DebugOptions::ShowStatsOverlay);
	if (DigDrawCheckbox(xPos, yPos, "Stats", stats, 1, ++id)) {
		if (stats) scene.debugOptions(A3D_MASK_ADD(debugOptions, DebugOptions::ShowStatsOverlay));
		else scene.debugOptions(A3D_MASK_REMOVE(debugOptions, DebugOptions::ShowStatsOverlay));
	}

	yPos += Y_PAD;
	bool meshWF = A3D_MASK_CONTAINS(debugOptions, DebugOptions::ShowWireframes);
	if (DigDrawCheckbox(xPos, yPos, "Mesh wireframes", meshWF, 1, ++id)) {
		if (meshWF) scene.debugOptions(A3D_MASK_ADD(debugOptions, DebugOptions::ShowWireframes));
		else scene.debugOptions(A3D_MASK_REMOVE(debugOptions, DebugOptions::ShowWireframes));
	}

	yPos += Y_PAD;
	bool meshAABBs = A3D_MASK_CONTAINS(scene.debugOptions(), DebugOptions::ShowBoundingBoxes);
	if (DigDrawCheckbox(xPos, yPos, "Mesh AABBs", meshAABBs, 1, ++id)) {
		if (meshAABBs) scene.debugOptions(A3D_MASK_ADD(debugOptions, DebugOptions::ShowBoundingBoxes));
		else scene.debugOptions(A3D_MASK_REMOVE(debugOptions, DebugOptions::ShowBoundingBoxes));
	}

	yPos += Y_PAD;
	bool physWF = A3D_MASK_CONTAINS(scene.debugOptions(), DebugOptions::ShowPhysicsWireframes);
	if (DigDrawCheckbox(xPos, yPos, "Physics wireframes", physWF, 1, ++id)) {
		if (physWF) scene.debugOptions(A3D_MASK_ADD(debugOptions, DebugOptions::ShowPhysicsWireframes));
		else scene.debugOptions(A3D_MASK_REMOVE(debugOptions, DebugOptions::ShowPhysicsWireframes));
	}

	yPos += Y_PAD;
	bool physAABBs = A3D_MASK_CONTAINS(scene.debugOptions(), DebugOptions::ShowPhysicsBoundingBoxes);
	if (DigDrawCheckbox(xPos, yPos, "Physics AABBs", physAABBs, 1, ++id)) {
		if (physAABBs) scene.debugOptions(A3D_MASK_ADD(debugOptions, DebugOptions::ShowPhysicsBoundingBoxes));
		else scene.debugOptions(A3D_MASK_REMOVE(debugOptions, DebugOptions::ShowPhysicsBoundingBoxes));
	}

	yPos += Y_PAD;
	bool physContacts = A3D_MASK_CONTAINS(scene.debugOptions(), DebugOptions::ShowPhysicsContactPoints);
	if (DigDrawCheckbox(xPos, yPos, "Physics contacts", physContacts, 1, ++id)) {
		if (physContacts) scene.debugOptions(A3D_MASK_ADD(debugOptions, DebugOptions::ShowPhysicsContactPoints));
		else scene.debugOptions(A3D_MASK_REMOVE(debugOptions, DebugOptions::ShowPhysicsContactPoints));
	}

	yPos += Y_PAD;
	bool physNorms = A3D_MASK_CONTAINS(scene.debugOptions(), DebugOptions::ShowPhysicsNormals);
	if (DigDrawCheckbox(xPos, yPos, "Physics normals", physNorms, 1, ++id)) {
		if (physNorms) scene.debugOptions(A3D_MASK_ADD(debugOptions, DebugOptions::ShowPhysicsNormals));
		else scene.debugOptions(A3D_MASK_REMOVE(debugOptions, DebugOptions::ShowPhysicsNormals));
	}
}

void DrawStatsOverlay(FrameStats& stats,
					  const FrameStatsHistory& statsHistory,
					  const RenderContext& context) {

	using namespace ImGui;

	static const int TEXT_PADDING = 14;
	static const float xPos = 10.0;
	float yPos = 0;
	int id = 0;

	ImGuiIO &io = GetIO();
	auto fonts = io.Fonts->Fonts;

	yPos += 0;
	DigDrawText(xPos, yPos, "avara3d", 0, 0.0, 0.0, id);

	auto buildInfo = BuildInfo::Info();
	auto version = buildInfo.version();
	static auto buildStr = std::format(
			"v{}.{}.{} build {}\n" \
             "{}\n"
			"\n" ,
			version.major, version.minor, version.patch, buildInfo.number(),
			buildInfo.type() == BuildInfo::Type::Debug ? "debug" : "release");
	yPos += 25;
	DigDrawText(xPos, yPos, buildStr.c_str(), 1, 0.0, 0.0, ++id);

	static chrono::nanoseconds frameNsAvg, engineCpuNsAvg, renderCpuNsAvg,
			renderGpuNsAvg, physicsNsAvg, appCpuNsAvg;
	static float frameMsFAvg, engineCpuMsFAvg, renderCpuMsFAvg,
			renderGpuMsFAvg, physicsMsFAvg, appCpuMsFAvg;
	static float fpsAvg;

	FrameStatsHistory::GetAverages(statsHistory,
								   frameNsAvg, engineCpuNsAvg, renderCpuNsAvg,
								   renderGpuNsAvg, physicsNsAvg, appCpuNsAvg,
								   a3d::config::FRAMETIME_AVERAGING_INTERVAL);

	frameMsFAvg = chrono::duration<float, std::milli>(frameNsAvg).count();
	engineCpuMsFAvg = chrono::duration<float, std::milli>(engineCpuNsAvg).count();
	renderCpuMsFAvg = chrono::duration<float, std::milli>(renderCpuNsAvg).count();
	renderGpuMsFAvg = chrono::duration<float, std::milli>(renderGpuNsAvg).count();
	physicsMsFAvg = chrono::duration<float, std::milli>(physicsNsAvg).count();
	appCpuMsFAvg = chrono::duration<float, std::milli>(appCpuNsAvg).count();
	fpsAvg = 1000.0f / frameMsFAvg;

	auto &samples = statsHistory.samples();

	// TODO: convert to one loop & use stride

	static vector<float> frameSamples;
	frameSamples.resize(samples.size());
	static vector<float> physSamples;
	physSamples.resize(samples.size());
	static vector<float> engCpuSamples;
	engCpuSamples.resize(samples.size());
	static vector<float> renderCpuSamples;
	renderCpuSamples.resize(samples.size());
	static vector<float> renderGpuSamples;
	renderGpuSamples.resize(samples.size());
	static vector<float> appSamples;
	appSamples.resize(samples.size());

	for (size_t i = 0; i < samples.size(); ++i) {
		auto sample = get<1>(samples[i]);
		frameSamples[i] = chrono::duration<float, milli>(sample.frameTime).count();
		physSamples[i] = chrono::duration<float, milli>(sample.physicsTime).count();
		renderCpuSamples[i] = chrono::duration<float, milli>(sample.renderCpuTime).count();
		renderGpuSamples[i] = chrono::duration<float, milli>(sample.renderGpuTime).count();
		appSamples[i] = chrono::duration<float, milli>(sample.applicationTime).count();
	}

//	static const float PLOT_WIDTH = 96.0;
//	static const float PLOT_WIDTH = 158.0;
	static const float PLOT_WIDTH = 122.0;
	static const bool PLOT_OUTLINED = true;
	static const float PLOT_HEIGHT_1 = 42.0;
	static const float PLOT_HEIGHT_2 = 24.0;
	static const float PLOT_X_OFFSET = 0.0;
	static const float PLOT_STR_Y_PAD = 5.0;
	static const float PLOT_Y_PAD = 18.0;
	static const float PLOT_Y_MIN = 0.0;
	static const float PLOT_Y_MAX = 17.0;
	static const int RT_TEXT_PADDING = TEXT_PADDING - 3;

//	auto frameTimeStr = std::format(
//			"{:<{}} {:.1f}ms\n",
//			"frame", RT_TEXT_PADDING, frameMsFAvg);
	auto frameTimeStr = std::format(
			"{}{:10.1f}ms\n",
			"frame", frameMsFAvg);
	yPos += 48;
	DigDrawText(xPos, yPos, frameTimeStr.c_str(), 1, 0.0, 0.0, ++id);
	yPos += PLOT_Y_PAD;
	DigDrawPlot(xPos + PLOT_X_OFFSET, yPos, PLOT_WIDTH, PLOT_HEIGHT_1,
				frameSamples.data(),
				static_cast<int>(frameSamples.size()),
				0,
				nullptr,
				PLOT_Y_MIN, PLOT_Y_MAX,
				0,
				PLOT_OUTLINED,
				++id);

	// engine cpu

	auto engineCpuTimeStr = std::format(
			"{:<{}} {:.1f}ms\n",
			"engine cpu", RT_TEXT_PADDING, engineCpuMsFAvg);
	yPos += PLOT_HEIGHT_1 + PLOT_STR_Y_PAD;
	DigDrawText(xPos, yPos, engineCpuTimeStr.c_str(), 1, 0.0, 0.0, ++id);
	yPos += PLOT_Y_PAD;
	DigDrawPlot(xPos + PLOT_X_OFFSET, yPos, PLOT_WIDTH, PLOT_HEIGHT_2,
				engCpuSamples.data(),
				static_cast<int>(engCpuSamples.size()),
				0,
				nullptr,
				PLOT_Y_MIN, PLOT_Y_MAX,
				0,
				PLOT_OUTLINED,
				++id);

	// physics

	auto physTimeStr = std::format(
			"{:<{}} {:.1f}ms\n",
			"physics", RT_TEXT_PADDING, physicsMsFAvg);
	yPos += PLOT_HEIGHT_2 + PLOT_STR_Y_PAD;
	DigDrawText(xPos, yPos, physTimeStr.c_str(), 1, 0.0, 0.0, ++id);
	yPos += PLOT_Y_PAD;
	DigDrawPlot(xPos + PLOT_X_OFFSET, yPos, PLOT_WIDTH, PLOT_HEIGHT_2,
				physSamples.data(),
				static_cast<int>(physSamples.size()),
				0,
				nullptr,
				PLOT_Y_MIN, PLOT_Y_MAX,
				0,
				PLOT_OUTLINED,
				++id);

	// render cpu

	auto renderCpuTimeStr = std::format(
			"{:<{}} {:.1f}ms\n",
			"render cpu", RT_TEXT_PADDING, renderCpuMsFAvg);
	yPos += PLOT_HEIGHT_2 + PLOT_STR_Y_PAD;
	DigDrawText(xPos, yPos, renderCpuTimeStr.c_str(), 1, 0.0, 0.0, ++id);
	yPos += PLOT_Y_PAD;
	DigDrawPlot(xPos + PLOT_X_OFFSET, yPos, PLOT_WIDTH, PLOT_HEIGHT_2,
				renderCpuSamples.data(),
				static_cast<int>(renderCpuSamples.size()),
				0,
				nullptr,
				PLOT_Y_MIN, PLOT_Y_MAX,
				0,
				PLOT_OUTLINED,
				++id);

	// render gpu

	auto renderGpuTimeStr = std::format(
			"{:<{}} {:.1f}ms\n",
			"render gpu", RT_TEXT_PADDING, renderGpuMsFAvg);
	yPos += PLOT_HEIGHT_2 + PLOT_STR_Y_PAD;
	DigDrawText(xPos, yPos, renderGpuTimeStr.c_str(), 1, 0.0, 0.0, ++id);
	yPos += PLOT_Y_PAD;
	DigDrawPlot(xPos + PLOT_X_OFFSET, yPos, PLOT_WIDTH, PLOT_HEIGHT_2,
				renderGpuSamples.data(),
				static_cast<int>(renderGpuSamples.size()),
				0,
				nullptr,
				PLOT_Y_MIN, PLOT_Y_MAX,
				0,
				PLOT_OUTLINED,
				++id);

	// application

	auto appTimeStr = std::format(
			"{:<{}} {:.1f}ms\n",
			"app", RT_TEXT_PADDING, appCpuMsFAvg);
	yPos += PLOT_HEIGHT_2 + PLOT_STR_Y_PAD;
	DigDrawText(xPos, yPos, appTimeStr.c_str(), 1, 0.0, 0.0, ++id);
	yPos += PLOT_Y_PAD;
	DigDrawPlot(xPos + PLOT_X_OFFSET, yPos, PLOT_WIDTH, PLOT_HEIGHT_2,
				appSamples.data(),
				static_cast<int>(appSamples.size()),
				0,
				nullptr,
				PLOT_Y_MIN, PLOT_Y_MAX,
				0,
				PLOT_OUTLINED,
				++id);

	string recordingStr = "";
	if (context.recordingGIF()) {
		auto time = context.recordedGIFTime();
		auto numFrames = context.recordedGIFFrames();
		recordingStr = std::format("\n{:<{}} {:.1f} s / {} {}",
								   "RECORDING",
								   TEXT_PADDING,
								   time,
								   numFrames,
								   (numFrames == 1 ? "frame" : "frames"));
	}

//	auto rateStr = std::format(
//			"{:<{}} {:.1f}fps\n",
//			"rate", RT_TEXT_PADDING, fpsAvg);
	auto rateStr = std::format(
			"{}{:10.1f}fps\n",
			"rate", fpsAvg);
	yPos += 36;
	DigDrawText(xPos, yPos, rateStr.c_str(), 1, 0.0, 0.0, ++id);

	auto bulkStatsStr = std::format(
	/*		"{:<{}} ({}, {})\n" \
            "{:<{}} {}\n" \
             "{:<{}} ({:.1f}, {:.1f})\n" \*/
            /* "\n" \*/
			"{:<{}} {}\n" \
             "{:<{}} {}\n" \
             "{:<{}} {}\n" \
             "{:<{}} {:.1f}k\n" \
             "{:<{}} {}\n" \
             /*"\n" \*/
             "{:<{}} {}\n" \
             "{:<{}} {}\n" \
             "{:<{}} {}\n" \
             "{:<{}} {}\n" \
             "{:<{}} {}\n" \
             "{:<{}} {}\n" \
             "{:<{}} {}\n" \
             "{:<{}} {}\n" \
             "{:<{}} {}\n" \
            /* "\n" \*/
           /*  "{:<{}} ({:.1f}, {:.1f}, {:.1f})\n" \*/
             /*"{:<{}} ({:.4f}, {:.4f}, {:.4f}, {:.4f})\n" \*/
			"{}",

//			"resolution", TEXT_PADDING, context.framebufferSize().x, context.framebufferSize().y,
//			"antialiasing", TEXT_PADDING, StatusOverlayDescriptionForAntialiasingMode(context.antialiasingMode()),
//			"fb scale", TEXT_PADDING, context.framebufferScale().x, context.framebufferScale().y,

			"nodes", TEXT_PADDING, stats.numNodes,
			"meshes", TEXT_PADDING, stats.numMeshes,
			"elements", TEXT_PADDING, stats.numElements,
			"polygons", TEXT_PADDING, float(stats.numPolygons) / 1000.0f,
			"lights", TEXT_PADDING, stats.numLights,

			"phys bodies", TEXT_PADDING, stats.numDynamicBodies + stats.numKinematicBodies + stats.numStaticBodies,
			" static", TEXT_PADDING, stats.numStaticBodies,
			" dynamic", TEXT_PADDING, stats.numDynamicBodies,
			" kinematic", TEXT_PADDING, stats.numKinematicBodies,
			"phys shapes", TEXT_PADDING, stats.numConcavePolyhedronShapes + stats.numBoundingBoxShapes + stats.numConvexHullShapes,
			" primitive", TEXT_PADDING, stats.numPrimitiveShapes,
			" bounding box", TEXT_PADDING, stats.numBoundingBoxShapes,
			" convex hull", TEXT_PADDING, stats.numConvexHullShapes,
			" concave poly", TEXT_PADDING, stats.numConcavePolyhedronShapes,

//			"camera pos", TEXT_PADDING, stats.cameraPosition.x, stats.cameraPosition.y, stats.cameraPosition.z,
			//"camera orientation", TEXT_PADDING, stats.cameraOrientation.x, stats.cameraOrientation.y, stats.cameraOrientation.z, stats.cameraOrientation.w,
			recordingStr);

	yPos += 36;
	DigDrawText(xPos, yPos, bulkStatsStr.c_str(), 1, 0.0, 0.0, ++id);

//	{
//		static ImPlotAxisFlags flags = ImPlotAxisFlags_NoTickLabels;
//
//		if (ImPlot::BeginPlot("##Scrolling", ImVec2(-1,ImGui::GetTextLineHeight()*10))) {
//			ImPlot::SetupAxes(nullptr, nullptr, flags, flags);
//	//		ImPlot::SetupAxisLimits(ImAxis_X1, t - history, t, ImGuiCond_Always);
//	//		ImPlot::SetupAxisLimits(ImAxis_Y1, 0, 1);
//			//ImPlot::SetupAxisLimits(ImAxis_X1, 0, 5000, ImGuiCond_Always);
//			ImPlot::SetupAxisLimits(ImAxis_X1, 0, 5000);
//			ImPlot::SetupAxisLimits(ImAxis_Y1, 0, 20);
//	//		ImPlot::SetNextFillStyle(IMPLOT_AUTO_COL, 0.5f);
//			//ImPlot::PlotShaded("Mouse X", &sdata1.Data[0].x, &sdata1.Data[0].y, sdata1.Data.size(), -INFINITY, 0, sdata1.Offset, 2 * sizeof(float));
//			//ImPlot::PlotLine("Mouse Y", &sdata2.Data[0].x, &sdata2.Data[0].y, sdata2.Data.size(), 0, sdata2.Offset, 2*sizeof(float));
//
//	//		ImPlot::PlotLine("Frame",
//	//						 frameSamples.data(),
//	//						 static_cast<int>(frameSamples.size()),
//	//						 flags,
//	//						 0,
//	//						 sizeof(float));
//	//
//	//		ImPlot::PlotLine("Physics",
//	//						 physSamples.data(),
//	//						 static_cast<int>(physSamples.size()),
//	//						 flags,
//	//						 0,
//	//						 sizeof(float));
//
//			ImPlot::PlotLine("Frame", frameSamples.data(), static_cast<int>(frameSamples.size()), -INFINITY, 0);
//			ImPlot::PlotLine("Physics", physSamples.data(), static_cast<int>(physSamples.size()), -INFINITY, 0);
//
//			ImPlot::EndPlot();
//
//		}
//	}


	// input test

//	Begin("Input test", nullptr, windowFlags);
//	SetWindowPos({10.0f, 2.0f});
//	PushFont(fonts[1]);
//	// --- Button + hover ---
//	if (Button("Click me")) {
//		A3D_LOG_I("ImGui button was CLICKED");
//	}
//	if (IsItemHovered()) {
//		SameLine();
//		Text("(hovering)");
//	}
//	static bool toggled = false;
//	if (Checkbox("Toggle", &toggled)) {
//		A3D_LOG_I("Toggle is now: {}", toggled ? "ON" : "OFF");
//	}
//	static char textBuf[128] = "type here";
//	if (InputText("Text field", textBuf, sizeof(textBuf))) {
//		A3D_LOG_I("Text changed: '{}'", textBuf);
//	}
//	Text("MousePos: (%.1f, %.1f)", io.MousePos.x, io.MousePos.y);
//	Text("MouseDown[0]: %s", io.MouseDown[0] ? "true" : "false");
//	Text("WantCaptureMouse: %s", io.WantCaptureMouse ? "true" : "false");
//	PopFont();
//	End();
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

void LogGLInfo() {

	const GLubyte *vendor = glGetString(GL_VENDOR);
	const GLubyte* renderer = glGetString(GL_RENDERER);
	const GLubyte* version = glGetString(GL_VERSION);

	A3D_LOG_I("GL_VENDOR: {}", reinterpret_cast<const char*>(renderer));
	A3D_LOG_I("GL_RENDERER: {}", reinterpret_cast<const char*>(renderer));
	A3D_LOG_I("GL_VERSION: {}", reinterpret_cast<const char*>(version));

//	// extensions
//
//	GLint numExtensions;
//	glGetIntegerv(GL_NUM_EXTENSIONS, &numExtensions);
//	ostringstream extensionsStream;
//	extensionsStream << "Extensions:" << endl;
//	for (GLint e=0; e < numExtensions; ++e) {
//		extensionsStream << "\t" << glGetStringi(GL_EXTENSIONS, e);
//		if (e < numExtensions-1) extensionsStream << endl;
//	}
//	A3D_LOG_I("{}", extensionsStream.str());
//
//	// context info
//
//	GLenum contextParams[] = {
//			GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS,
//			GL_MAX_CUBE_MAP_TEXTURE_SIZE,
//			GL_MAX_DRAW_BUFFERS,
//			GL_MAX_FRAGMENT_UNIFORM_COMPONENTS,
//			GL_MAX_TEXTURE_IMAGE_UNITS,
//			GL_MAX_TEXTURE_SIZE,
//			GL_MAX_VARYING_FLOATS,
//			GL_MAX_VERTEX_ATTRIBS,
//			GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS,
//			GL_MAX_VERTEX_UNIFORM_COMPONENTS,
//			GL_MAX_VIEWPORT_DIMS,
//			GL_STEREO,
//	};
//	const char* contextParamNames[] = {
//			"GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS",
//			"GL_MAX_CUBE_MAP_TEXTURE_SIZE",
//			"GL_MAX_DRAW_BUFFERS",
//			"GL_MAX_FRAGMENT_UNIFORM_COMPONENTS",
//			"GL_MAX_TEXTURE_IMAGE_UNITS",
//			"GL_MAX_TEXTURE_SIZE",
//			"GL_MAX_VARYING_FLOATS",
//			"GL_MAX_VERTEX_ATTRIBS",
//			"GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS",
//			"GL_MAX_VERTEX_UNIFORM_COMPONENTS",
//			"GL_MAX_VIEWPORT_DIMS",
//			"GL_STEREO",
//	};
//
//	// (integers)
//
//	ostringstream contextParamsStream;
//	contextParamsStream << "Context parameters:" << endl;
//	const int numIntParams = 10;
//	for (int p=0; p<numIntParams; ++p) {
//		GLint intValue = 0;
//		glGetIntegerv(contextParams[p], &intValue);
//		contextParamsStream << "\t" << contextParamNames[p] << ": " << intValue << endl;
//	}
//
//	// (int vec2)
//
//	GLint maxViewportDims[2];
//	glGetIntegerv(contextParams[10], maxViewportDims);
//	contextParamsStream << "\t" << contextParamNames[10] << ": " << maxViewportDims[0]
//		<< ", " << maxViewportDims[0] << endl;
//
//	// (boolean)
//
//	GLboolean stereo = 0;
//	glGetBooleanv(contextParams[11], &stereo);
//	contextParamsStream << "\t" << contextParamNames[11] << ": " << (stereo ? "true" : "false");
//
//	A3D_LOG_I("{}", contextParamsStream.str());
}

//void CheckGLError() {
//	//A3D_GL_CHECK();
////	auto err = glGetError();
////	if (err != GL_NO_ERROR) {
////		A3D_LOG_E("*** GL error: 0x{:X} ***", err);
////	}
//}
//
//void DrainGLErrors(const char* where) {
//	GLenum err;
//	while ((err = glGetError()) != GL_NO_ERROR) {
//		A3D_LOG_E("*** GL error 0x{:X} at %s ***", err, where);
//	}
//}
