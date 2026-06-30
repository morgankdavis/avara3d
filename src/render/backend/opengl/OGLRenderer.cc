//
//  OGLRenderer.cc
//  avara3d
//
//  Created by Morgan Davis on 4/24/18.
//  Copyright © 2024 Morgan K Davis. All rights reserved.
//

#include "a3d/render/backend/opengl/OGLRenderer.h"

#include <format>
#include <set>
#include <utility>
#include <vector>

#include "a3d/render/backend/opengl/gl.h" // needs to be before imgui_impl_opengl3.h ?

//#ifdef A3D_GL_DESKTOP
#include <imgui/imgui.h>
#include <imgui/backends/imgui_impl_opengl3.h>
//#include "implot.h"
//#endif

#include <magic_enum/magic_enum.hpp>

#include "a3d/Assert.h"
#include "a3d/Buffer.h"
#include "a3d/BuildInfo.h"
#include "a3d/Color.h"
#include "a3d/Configuration.h"
#include "a3d/CubeImage.h"
#include "a3d/log/Log.h"
#include "a3d/Font.h"
#include "a3d/Image.h"
#include "a3d/Math.h"
#include "a3d/mesh/Mesh.h"
#include "a3d/mesh/MeshElement.h"
#include "a3d/mesh/primitive/Box.h"
#include "a3d/profile/FrameStats.h"
#include "a3d/render/DrawPacket.h"
#include "a3d/render/backend/opengl/OGLResourceCache.h"
#include "a3d/render/backend/opengl/GLSLProgram.h"
#include "a3d/render/context/RenderContext.h"
#include "a3d/scene/Node.h"
#include "a3d/scene/Scene.h"
#include "a3d/util/chrono.h"
#include "a3d/util/filesystem.h"
#include "a3d/util/flow.h"
#include "a3d/visual/VisualWorld.h"
#include "a3d/visual/camera/Camera.h"
#include "a3d/visual/light/AmbientLight.h"
#include "a3d/visual/light/DirectionalLight.h"
#include "a3d/visual/light/PointLight.h"
#include "a3d/visual/light/SpotLight.h"
#include "a3d/visual/material/Material.h"
#include "a3d/visual/material/Sampler.h"
#include "a3d/visual/material/Texture.h"

#define A3D_GL_CHECK()									\
    do {												\
        GLenum err;										\
        while ((err = glGetError()) != GL_NO_ERROR) {	\
            log::e()("GL error 0x{:X}", err);			\
        }												\
    } while (0);

using namespace a3d;
using namespace a3d::math;
using namespace std;

///  Private Constants ///

const std::string 	STATS_TITLE_FONT_NAME  		{"SourceCodePro-Bold"};
const std::string 	STATS_TITLE_FONT_TYPE  		{"otf"};
const float 		STATS_TITLE_FONT_SIZE 		{23.0};
const std::string 	STATS_BODY_FONT_NAME  		{"SourceCodePro-Semibold"};
const std::string 	STATS_BODY_FONT_TYPE  		{"otf"};
const float 		STATS_BODY_FONT_SIZE 		{15.0};

const GLuint 		ENV_BINDING_POINT 			{0};

/// Private Types ///

enum class MaterialContentsType : unsigned {
	None = 		0,
	Color = 	1,
	Sampler = 	2
};

static_assert(sizeof(vec4) == 16);
static_assert(sizeof(vec3) == 12);

struct AmbientLightGLSLStruct {
	vec4		color;
};
static_assert(sizeof(AmbientLightGLSLStruct) == 16);

struct DirectionalLightGLSLStruct {
	vec4		color;
	vec3		direction_world;
	f32			_pad_0_;
};
static_assert(sizeof(DirectionalLightGLSLStruct) == 32);

struct PointLightGLSLStruct {
	vec4		color;
	vec3		position_world;
	f32			_pad_0_;
	f32			constantAttenuation;
	f32			linearAttenuation;
	f32			quadraticAttenuation;
	f32			_pad_1_;
};
static_assert(sizeof(PointLightGLSLStruct) == 48);

struct SpotLightGLSLStruct {
	vec4		color;
	vec3		position_world;
	f32			_pad_0_;
	vec3		direction_world;
	f32			_pad_1_;
	f32			innerAngleCos;
	f32			outerAngleCos;
	uint32_t	featheringMode;
	f32			constantAttenuation;
	f32			linearAttenuation;
	f32			quadraticAttenuation;
	f32			_pad_2_;
	f32			_pad_3_;
};
static_assert(sizeof(SpotLightGLSLStruct) == 80);

struct FogGLSLStruct {
	vec4		color;
	f32			startDistance;
	f32			endDistance;
	f32			densityExponent;
	f32			_pad_0_;
};
static_assert(sizeof(FogGLSLStruct) == 32);

struct EnvironmentBlock {
	uint32_t useDefaultLighting;
	uint32_t _pad0_[3];
	uint32_t numAmbientLights;
	uint32_t _pad1_[3];
	AmbientLightGLSLStruct ambientLights[config::MAX_AMBIENT_LIGHTS];
	uint32_t numDirectionalLights;
	uint32_t _pad2_[3];
	DirectionalLightGLSLStruct directionalLights[config::MAX_DIRECTIONAL_LIGHTS];
	uint32_t numPointLights;
	uint32_t _pad3_[3];
	PointLightGLSLStruct pointLights[config::MAX_POINT_LIGHTS];
	uint32_t numSpotLights;
	uint32_t _pad4_[3];
	SpotLightGLSLStruct spotLights[config::MAX_SPOT_LIGHTS];
	FogGLSLStruct fog;
};
static_assert(offsetof(EnvironmentBlock, useDefaultLighting) == 0);
static_assert(offsetof(EnvironmentBlock, numAmbientLights)   == 16);
static_assert(offsetof(EnvironmentBlock, ambientLights)      == 32);

struct FBORestore {
	GLint drawFbo = 0, readFbo = 0;
	FBORestore() {
		glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &drawFbo);
		glGetIntegerv(GL_READ_FRAMEBUFFER_BINDING, &readFbo);
	}
	~FBORestore() {
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, drawFbo);
		glBindFramebuffer(GL_READ_FRAMEBUFFER, readFbo);
	}
};

/// Private Static Non-Member Prototypes ///

static void 	LogGLInfo();
static void 	SendMaterialUniforms(const Material& material,
									GLSLProgram& program,
									const std::array<GLuint, 4>& glTextureHandles,
									OGLRenderer::GLStateCache& state);
static void 	SendMaterialPropertyUniforms(const Material::Property& property,
											Material::PropertyType type,
											GLuint glTextureHandle,
											GLSLProgram& program,
											OGLRenderer::GLStateCache& state);
static void		SendEnvironmentUniforms(GLuint glEnvironmentUBO,
										   const Scene& scene,
										   const vector<Node*>& lightNodes,
										   FrameStats& stats);
static void		ApplyBlendFunction(Material::BlendFunction func);
static GLenum	GLDepthFuncFromDepthFunc(DepthFunc func);
static GLenum	GLFilterModeForFilterMode(Sampler::FilterMode mode);
static GLenum	GLWrapModeForWrapMode(Sampler::WrapMode mode);
static void 	DrawOverlay(const RenderContext& context,
						   const Scene& scene,
						   FrameStats& stats,
						   const FrameStatsHistory& statsHistory,
						   Scene::DebugOptions debugOptions,
						   ImFont& titleFont,
						   ImFont& bodyFont);
static void 	DrawStats(FrameStats& stats,
						 const FrameStatsHistory& statsHistory,
						 const RenderContext& context,
						 ImFont& titleFont,
						 ImFont& bodyFont);
static void 	DrawDebugOptions(Scene& scene,
								ImFont& bodyFont);
static void		ImguiInit(const RenderContext& context,
							 ImFont*& titleFont,
							 ImFont*& bodyFont);
static void 	ImguiUpdateScale(const RenderContext& context);
static void 	ImguiAddFont(const RenderContext& context,
							const Font& font,
							ImFont*& imFont);
void 			ImguiBeginOverlay(int id, bool allowsInput);
void 			ImguiEndOverlay();
void 			ImguiDrawText(float x,
							  float y,
							  const char* text,
							  ImFont& font,
							  float size);
void 			ImguiDrawPlot(float x, float y, float w, float h,
							  const float* values,
							  int valuesCount,
							  int valuesOffset,
							  const char* overlayText,
							  float scaleMin,
							  float scaleMax,
							  int stride,
							  bool outlined,
							  int id);
bool 			ImguiDrawCheckbox(float x,
								  float y,
								  const char* text,
								  bool& checked,
								  ImFont& font,
								  float size,
								  int id);

/// Private Static Members ///

bool OGLRenderer::InitGL(GLGetProcAddress getProcAddress) {

	static bool initialized = false;
	if (initialized) return true;

#ifdef A3D_GL_DESKTOP
	if (!getProcAddress) {
		// log error, return false
		log::e()("getProcAddress is null.");
		return false;
	}

	int status = gladLoadGLLoader((GLADloadproc)getProcAddress);
	if (status == 0) {
		// log "Failed to initialize GLAD"
		log::e()("gladLoadGLLoader");
		return false;
	}
#elif A3D_GL_WEB
	(void)getProcAddress; // shut up!
#else
	#error "No OpenGL function loader defined for this platform."
#endif

	initialized = true;

	LogGLInfo();

	return true;
}

/// Internal Lifecycle Functions ///

OGLRenderer::OGLRenderer():
		Renderer{},
		_isInitialized{false},
		_resourceCache{},
		_skyboxMesh{},
		_glEnvironmentUBO{0},
		_overlayTitleImFont{nullptr},
		_overlayBodyImFont{nullptr},
		_drawTimer{config::GL_DRAW_TIMER_BUFFER_SIZE} {}

OGLRenderer::~OGLRenderer() {
	log::d()("Destroying OpenGLRenderer {:p}", static_cast<void*>(this));

	glDeleteBuffers(1, &_glEnvironmentUBO);

	_debugLines.destroy();

	ImGui_ImplOpenGL3_Shutdown();
//	ImPlot::DestroyContext();
	ImGui::DestroyContext();
}
	
/// Renderer Internal Member Functions ///

bool OGLRenderer::initialize(const RenderContext& context) {
	log::i();

	GLint maxSize = 0;
	glGetIntegerv(GL_MAX_UNIFORM_BLOCK_SIZE, &maxSize);
	A3D_ASSERT(sizeof(EnvironmentBlock) <= (size_t)maxSize);

	glGenBuffers(1, &_glEnvironmentUBO);
	glBindBuffer(GL_UNIFORM_BUFFER, _glEnvironmentUBO);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(EnvironmentBlock), nullptr, GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_UNIFORM_BUFFER, ENV_BINDING_POINT, _glEnvironmentUBO);

	// just do it here even if not used
	_skyboxMesh = make_unique<a3d::Mesh>(make_unique<Box>(1, 1, 1), nullptr);

	// TODO: do something better
	auto bindBlock = [&](GLuint program, const char* blockName) {
		GLuint idx = glGetUniformBlockIndex(program, blockName);
		if (idx == GL_INVALID_INDEX) return; // program doesn't have the block
		glUniformBlockBinding(program, idx, ENV_BINDING_POINT);
	};
	bindBlock(GLSLProgram::Default().glID(), "EnvironmentBlock");
	bindBlock(GLSLProgram::Wireframe().glID(), "EnvironmentBlock");

	_drawTimer.initialize();

	// TODO: make failable?
	ImguiInit(context, _overlayTitleImFont, _overlayBodyImFont);

	_isInitialized = true;

	return true;
}

bool OGLRenderer::isInitialized() const {
	return _isInitialized;
}

void OGLRenderer::beginFrame(const Scene& scene,
							 const RenderContext& context,
							 const Scene::DebugOptions& debugOptions,
							 FrameStats& stats,
							 Profiler& profiler) {

	_drawTimer.begin();

	_state = {};
	_state.pipelineId = INVALID_PIPELINE_ID;
	_state.program = 0;
	_state.material = nullptr;

	_boundElement = {};
}

void OGLRenderer::endFrame(const Scene& scene,
						   const RenderContext& context,
						   const Scene::DebugOptions& debugOptions,
						   FrameStats& stats,
						   Profiler& profiler,
						   const FrameStatsHistory& statsHistory) {

	DrawOverlay(context, scene, stats, statsHistory, debugOptions,
				*_overlayTitleImFont, *_overlayBodyImFont);

	A3D_GL_CHECK();

	profiler.add(Profiler::Tag::RenderGpu, _drawTimer.end());
}

void OGLRenderer::preTraversal(const Scene& scene,
							   const RenderContext& context,
							   const Scene::DebugOptions& debugOptions,
							   FrameStats& stats) {

}

void OGLRenderer::postTraversal(const Scene& scene,
								const RenderContext& context,
								const vector<Node*>& lightNodes,
								const Scene::DebugOptions& debugOptions,
								FrameStats& stats) {

	SendEnvironmentUniforms(_glEnvironmentUBO, scene, lightNodes, stats);
}

void OGLRenderer::clear(const ClearCommand& cmd,
						const RenderContext& context) {
	// target-specific clear:
	// if (cmd.bindFramebuffer) glBindFramebuffer(GL_FRAMEBUFFER, cmd.framebuffer);

	FBORestore restore;

	auto fb = context.defaultFramebuffer();
	auto fbSize = context.framebufferSize();
	glBindFramebuffer(GL_FRAMEBUFFER, fb);
	glViewport(0, 0, (GLsizei)fbSize.x, (GLsizei)fbSize.y);

	// save state we might stomp
	GLboolean prevScissorEnabled = GL_FALSE;
	GLint prevScissorBox[4] = {0,0,0,0};
	glGetBooleanv(GL_SCISSOR_TEST, &prevScissorEnabled);
	glGetIntegerv(GL_SCISSOR_BOX, prevScissorBox);

	GLboolean prevColorMask[4] = {GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE};
	GLboolean prevDepthMask = GL_TRUE;
	GLint prevStencilMask = ~0;
	glGetBooleanv(GL_COLOR_WRITEMASK, prevColorMask);
	glGetBooleanv(GL_DEPTH_WRITEMASK, &prevDepthMask);
	glGetIntegerv(GL_STENCIL_WRITEMASK, &prevStencilMask);

	// apply scissor
	if (cmd.useScissor) {
		glEnable(GL_SCISSOR_TEST);
		glScissor(cmd.scissorRect.x, cmd.scissorRect.y,
				  cmd.scissorRect.w, cmd.scissorRect.h);
	}
	else if (prevScissorEnabled) {
		// leave as-is
	}
	else {
		glDisable(GL_SCISSOR_TEST);
	}

	// ensure clears actually write
	if (cmd.forceWriteMasks) {
		glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
		glDepthMask(GL_TRUE);
		glStencilMask(0xFFFFFFFF);
	}

	// set clear values
	GLbitfield mask = 0;

	if (cmd.clearColor) {
		glClearColor(cmd.color.x, cmd.color.y, cmd.color.z, cmd.color.w);
		mask |= GL_COLOR_BUFFER_BIT;
	}

	if (cmd.clearDepth) {
#ifdef A3D_GL_DESKTOP
		glClearDepth(cmd.depth);

#else
		glClearDepthf(cmd.depth);
#endif
		mask |= GL_DEPTH_BUFFER_BIT;
	}

	if (cmd.clearStencil) {
		glClearStencil(cmd.stencil);
		mask |= GL_STENCIL_BUFFER_BIT;
	}

	if (mask) {
		glClear(mask);
	}

	// restore state
	if (cmd.forceWriteMasks) {
		glColorMask(prevColorMask[0], prevColorMask[1], prevColorMask[2], prevColorMask[3]);
		glDepthMask(prevDepthMask);
		glStencilMask((GLuint)prevStencilMask);
	}

	if (cmd.useScissor) {
		if (prevScissorEnabled) {
			glEnable(GL_SCISSOR_TEST);
			glScissor(prevScissorBox[0], prevScissorBox[1], prevScissorBox[2], prevScissorBox[3]);
		}
		else {
			glDisable(GL_SCISSOR_TEST);
		}
	}
}

void OGLRenderer::renderPacket(DrawPacket& packet, const FrameParams& frame) {
	resolvePacket(packet, frame);
	drawPacket(packet, frame);
}

unique_ptr<Image> OGLRenderer::snapshot(const RenderContext& context) const {

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

/// Renderer Protected Member Functions ///

void OGLRenderer::drawBackground(const BackgroundPass& backgroundPass,
								 const math::mat4& view,
								 const math::mat4& proj) {

	if (!backgroundPass.material) return;

	auto bgEmission = backgroundPass.material->emission();
	if (auto color = std::get_if<std::shared_ptr<Color>>(&bgEmission)) {
		if (*color) {
			auto rgba = (*color)->rgba();
			glClearColor(rgba.r, rgba.g, rgba.b, rgba.a);
			glClear(GL_COLOR_BUFFER_BIT);
			return;
		}
	}

	// TODO: check equality?
	// TODO: stop using shared_ptr???????

	if (_skyboxMesh->materials().empty()) {
		_skyboxMesh->addMaterial(backgroundPass.material);
	}
	else {
		_skyboxMesh->replaceMaterial(0, backgroundPass.material);
	}

	bindPipeline(backgroundPass.pipelineId, _resourceCache);
	bindMaterial(*(_skyboxMesh->materials().front()));
	bindMeshElement(*(_skyboxMesh->elements().front()));
	applyMVP(mat4(1.0f), mat4(mat3(view)), proj); // strip transform off view mat

	drawElements();
}

void OGLRenderer::bindPipeline(PipelineId pipelineId,
                               const OGLResourceCache& cache) {

	if (_state.pipelineId == pipelineId) {
		GLint cur = 0;
		glGetIntegerv(GL_CURRENT_PROGRAM, &cur);
		if ((GLuint)cur == _state.program) return; // truly already bound
		// else: stale cache, fallthrough and rebind
	}

	const OGLPipeline& pipeline = cache.pipeline(pipelineId);
	glUseProgram(pipeline.program);
	_state.program = pipeline.program;
	_state.material = nullptr;

	if (pipeline.desc.doubleSided) {
		glDisable(GL_CULL_FACE);
	}
	else {
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
	}

	if (pipeline.desc.depthTest) {
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GLDepthFuncFromDepthFunc(pipeline.desc.depthFunc));
	}
	else {
		glDisable(GL_DEPTH_TEST);
	}
	glDepthMask(pipeline.desc.depthWrite ? GL_TRUE : GL_FALSE);

	ApplyBlendFunction(pipeline.desc.blendFunction);

#ifdef A3D_GL_DESKTOP
	switch (pipeline.desc.fillMode) {
		case Material::FillMode::Fill:   glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);  break;
		case Material::FillMode::Lines:  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);  break;
		case Material::FillMode::Points: glPolygonMode(GL_FRONT_AND_BACK, GL_POINT); break;
	}
	const bool lineSmooth = (pipeline.desc.passKind == PassKind::Lines)
	                        || (pipeline.desc.passKind == PassKind::Wireframe);
	if (lineSmooth) {
		glEnable(GL_LINE_SMOOTH);
		glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
	}
	else {
		glDisable(GL_LINE_SMOOTH);
	}

	if (pipeline.desc.polygonOffset) {
		glEnable(GL_POLYGON_OFFSET_LINE);
		glPolygonOffset(.01, 0); // ! check !
	}
	else {
		glDisable(GL_POLYGON_OFFSET_LINE);
	}
#endif

	_state.pipelineId = pipelineId;
}

void OGLRenderer::bindMaterial(const Material& material) {
	//if (_state.material == &material) return;
	if (_state.material == &material) {
		_resourceCache.ensureMaterial(const_cast<Material&>(material)); // will apply sampler dirties
		return;
	}

	// look at the currently bound pipeline
	const OGLPipeline& pipeline = _resourceCache.pipeline(_state.pipelineId);

// TODO: !!! THIS IS A DIRTY HACK !!!
	GLSLProgram* program;
	switch (pipeline.desc.shaderKind) {
		case ShaderKind::Default:
			program = &GLSLProgram::Default();
			break; // chill
		case ShaderKind::Skybox:
			program = &GLSLProgram::Skybox();
			break; // chill
		default:
			_state.material = &material;
			return; // not chill
	}

	// resolve material once (uploads textures  applies sampler states via cache)
	const auto& mr = _resourceCache.ensureMaterial(const_cast<Material&>(material));
	std::array<GLuint, 4> glTextureHandles = {
			(GLuint)mr.tex[0],
			(GLuint)mr.tex[1],
			(GLuint)mr.tex[2],
			(GLuint)mr.tex[3],
	};

	//Program& program = Program::Default();
	// OK if SendMaterialUniforms still calls prog.use() because it matches the pipeline now

	// TODO: !!! THIS IS A DIRTY HACK !!!
	if (pipeline.desc.shaderKind == ShaderKind::Default) {
		SendMaterialUniforms(material, *program, glTextureHandles, _state);
	}

	_state.material = &material;
}

void OGLRenderer::bindMeshElement(const MeshElement& element) {

	const OGLPipeline& pipe = _resourceCache.pipeline(_state.pipelineId);

	const VertexLayout elemLayout = element.vertexLayout();
	const VertexLayout pipeLayout = pipe.desc.vertexLayoutKey;

	// TODO: change?
	if (A3D_UNLIKELY(elemLayout != pipeLayout)) {
		log::e()("VertexLayout mismatch: element={}, pipeline={}", (uint32_t)elemLayout, (uint32_t)pipeLayout);
		return; // skip draw
	}

	auto* e = const_cast<MeshElement*>(&element);
	const auto& res = _resourceCache.ensureMeshElement(*e);

	glBindVertexArray((GLuint)res.vao);

	_boundElement.vao        = (GLuint)res.vao;
	_boundElement.indexCount = (GLsizei)res.indexCount;
	_boundElement.indexType  = (GLenum)res.indexType;     // <-- USE CACHED TYPE
	_boundElement.vertexCount = (GLsizei)res.vertexCount; // <-- for drawArrays fallback
}

void OGLRenderer::applyMVP(const mat4& model, const mat4& view, const mat4& proj) {

	// TEMP: query locations from currently bound program each call (slow but fine)
	// Later: cache these per Program.
	GLint program = 0;
	glGetIntegerv(GL_CURRENT_PROGRAM, &program);
	if (!program) return;

	GLint locM = glGetUniformLocation(program, "modelMat");
	GLint locV = glGetUniformLocation(program, "viewMat");
	GLint locP = glGetUniformLocation(program, "projMat");

	if (locM >= 0) glUniformMatrix4fv(locM, 1, GL_FALSE, value_ptr(model));
	if (locV >= 0) glUniformMatrix4fv(locV, 1, GL_FALSE, value_ptr(view));
	if (locP >= 0) glUniformMatrix4fv(locP, 1, GL_FALSE, value_ptr(proj));
}

void OGLRenderer::drawElements() {
	if (_boundElement.vao == 0) return;

	glBindVertexArray(_boundElement.vao);

	if (_boundElement.indexCount > 0) {
		glDrawElements(GL_TRIANGLES,
					   _boundElement.indexCount,
					   _boundElement.indexType,
					   (void*)0);
	}
	else if (_boundElement.vertexCount > 0) {
		// non-indexed fallback
		glDrawArrays(GL_TRIANGLES, 0, _boundElement.vertexCount);
	}
}

void OGLRenderer::draw(const DrawCommand& cmd) {
	// wrapper over bindPipeline/bindMaterial/bindMeshElement/applyMVP/drawElements
	bindPipeline(cmd.pipelineId, *cmd.cache);
	if (cmd.material) {
		bindMaterial(*cmd.material);
	}
	if (cmd.element) {
		bindMeshElement(*cmd.element);
	}
	applyMVP(cmd.model, cmd.view, cmd.proj);
	drawElements();
}

void OGLRenderer::drawDebugLines(const math::mat4& model,
								 const math::mat4& view,
								 const math::mat4& proj) {
	// helper for debug line geometry which uses a separate VAO/VBO (_debugLines)
	// and doesn't follow the normal mesh binding pipeline.
	applyMVP(model, view, proj);

	glBindVertexArray(_debugLines.vao);
	glDrawArrays(GL_LINES, 0, _debugLines.vertexCount);
	glBindVertexArray(0);
}

void OGLRenderer::renderLinesPass(const LinesPass& pass,
								  const RenderContext& context,
								  const mat4& view,
								  const mat4& proj) {

	if (pass.pipelineId == INVALID_PIPELINE_ID) return;
	if (pass.lines.empty()) return;

	// debug lines follow a different path:
	// - they upload on-the-fly (not baked into a MeshElement)
	// - they use their own VAO/VBO (_debugLines)
	// - they render GL_LINES topology, not GL_TRIANGLES
	// see drawDebugLines() for why this pattern is separate.

	bindPipeline(pass.pipelineId, _resourceCache);
	_debugLines.upload(pass.lines);
	drawDebugLines(pass.model, view, proj);
}

/// Protected Member Functions ///

void OGLRenderer::resolvePacket(DrawPacket& packet, const FrameParams& frame) {

	// "resolve / prepare / compile / bake"

	auto resolvePipeline = [&](PipelineId &pipelineId, const PipelineDesc &desc) -> PipelineId {
		if (pipelineId == INVALID_PIPELINE_ID) pipelineId = _resourceCache.ensurePipeline(desc);
		return pipelineId;
	};

	// background
	if (packet.backgroundPass.material) {
		resolvePipeline(packet.backgroundPass.pipelineId, packet.backgroundPass.desc);
	}

	// main + wireframe items
	for (auto &di: packet.mainPassItems) {
		resolvePipeline(di.pipelineId, di.desc);
	}
	for (auto &di: packet.wireframePassItems) {
		resolvePipeline(di.pipelineId, di.desc);
	}

	// lines
	if (!packet.linesPass.lines.empty()) {
		resolvePipeline(packet.linesPass.pipelineId, packet.linesPass.desc);
	}
	else {
		packet.linesPass.pipelineId = INVALID_PIPELINE_ID;
	}
}

void OGLRenderer::drawPacket(const DrawPacket& packet, const FrameParams& frame) {

	// "render / execute / submit / draw"

	clear(Renderer::ClearCommand{}, frame.context);

	if (packet.backgroundPass.material) {
		// uses _skyboxMesh internally, binds + draws
		drawBackground(packet.backgroundPass, frame.view, frame.proj);
	}

	// NOTE: items are already sorted by pass + desc hash, so this will batch nicely
	for (const auto &di: packet.mainPassItems) {
		if (di.pipelineId == INVALID_PIPELINE_ID) continue;
		if (!di.element) continue;

		bindPipeline(di.pipelineId, _resourceCache);

		// only bind material for shaderKinds that use it (bindMaterial() already early-outs)
		if (di.material) bindMaterial(*di.material);

		bindMeshElement(*di.element);
		applyMVP(di.model, frame.view, frame.proj);
		drawElements();
	}

	for (const auto &di: packet.wireframePassItems) {
		if (di.pipelineId == INVALID_PIPELINE_ID) continue;
		if (!di.element) continue;

		bindPipeline(di.pipelineId, _resourceCache);
		// no bindMaterial (wire shader typically ignores it)
		bindMeshElement(*di.element);
		applyMVP(di.model, frame.view, frame.proj);
		drawElements();
	}

	renderLinesPass(packet.linesPass, frame.context, frame.view, frame.proj);
}

/// Private Static Non-Member Functions ///

void LogGLInfo() {

	const GLubyte *vendor = glGetString(GL_VENDOR);
	const GLubyte* renderer = glGetString(GL_RENDERER);
	const GLubyte* version = glGetString(GL_VERSION);

	log::i()("GL_VENDOR: {}", reinterpret_cast<const char*>(vendor));
	log::i()("GL_RENDERER: {}", reinterpret_cast<const char*>(renderer));
	log::i()("GL_VERSION: {}", reinterpret_cast<const char*>(version));
}

void SendMaterialUniforms(const Material& material,
						  GLSLProgram& program,
						  const std::array<GLuint, 4>& glTextureHandles,
						  OGLRenderer::GLStateCache& state) {

	// sends uniforms for the Material, and MaterialProperties it has

	program.setUniform("specularExponent", material.specularExponent());
	program.setUniform("uvScale", material.uvScale());
	//program.setUniform("locksAmbientWithDiffuse", material.locksAmbientWithDiffuse());
	program.setUniform("emissionContentsType", (unsigned)0); // 0 = MaterialType_None -- why is this here?
	//program.setUniform("defaultLighting", 0);

	for (auto& [property, type] : material.properties()) {

		if (!holds_alternative<monostate>(*property)) {

			const int slot = static_cast<underlying_type<Material::PropertyType>::type>(type);

			const GLuint h = (slot >= 0) ? glTextureHandles[(size_t)slot] : 0u;

			SendMaterialPropertyUniforms(*property,
										 type,
										 h,
										 program,
										 state);
		}
	}
}

void SendMaterialPropertyUniforms(const Material::Property& property,
								  Material::PropertyType type,
								  GLuint glTextureHandle,
								  GLSLProgram& program,
								  OGLRenderer::GLStateCache& state) {

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
						case Material::PropertyType::Ambient:
							modeUniformName = "ambientContentsType";
							samplerUniformName = "samplers.ambient";
							slot = GL_TEXTURE0;
							index = 0;
							break;
						case Material::PropertyType::Diffuse:
							modeUniformName = "diffuseContentsType";
							samplerUniformName = "samplers.diffuse";
							slot = GL_TEXTURE1;
							index = 1;
							break;
						case Material::PropertyType::Specular:
							modeUniformName = "specularContentsType";
							samplerUniformName = "samplers.specular";
							slot = GL_TEXTURE2;
							index = 2;
							break;
						case Material::PropertyType::Emission:
							modeUniformName = "emissionContentsType";
							samplerUniformName = "samplers.emission";
							slot = GL_TEXTURE3;
							index = 3;
							break;
						default:
							log::e()("Invalid MaterialPropertyType: {}",
									  magic_enum::enum_name<Material::PropertyType>(type));
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
					log::e()("Empty texture variant.");
				}

			}, property->contents());
		}
		else if constexpr (std::is_same_v<T, shared_ptr<Color>>) {

			string modeUniformName;
			string colorUniformName;

			switch (type) {
				case Material::PropertyType::Ambient:
					modeUniformName = "ambientContentsType";
					colorUniformName = "colors.ambient";
					break;
				case Material::PropertyType::Diffuse:
					modeUniformName = "diffuseContentsType";
					colorUniformName = "colors.diffuse";
					break;
				case Material::PropertyType::Specular:
					modeUniformName = "specularContentsType";
					colorUniformName = "colors.specular";
					break;
				case Material::PropertyType::Emission:
					modeUniformName = "emissionContentsType";
					colorUniformName = "colors.emission";
					break;
				default:
					log::e()("Invalid MaterialPropertyType: {}",
							  magic_enum::enum_name<Material::PropertyType>(type));
					return;
			}

			program.setUniform(modeUniformName.c_str(),
							   static_cast<underlying_type<MaterialContentsType>::type>(MaterialContentsType::Color));
			program.setUniform(colorUniformName.c_str(),
							   property->r(), property->g(), property->b());
		}
		else if constexpr (std::is_same_v<T, std::monostate>) {

			log::w()("NULL material property contents.");
		}

	}, property);
}

void SendEnvironmentUniforms(GLuint glEnvironmentUBO,
							 const Scene& scene,
							 const vector<Node*>& lightNodes,
							 FrameStats& stats) {
	// block

	EnvironmentBlock environmentStruct{};

	// lights

	auto numLights = lightNodes.size();

	if (scene.visualWorld()->usesDefaultLighting()
		|| ((numLights == 0) && scene.visualWorld()->autoEnablesDefaultLighting())) {

		environmentStruct.useDefaultLighting = 1u;
	}
	else {

		environmentStruct.useDefaultLighting = 0u;

		stats.numLights = numLights;

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
				if (ambientStructs.size() < config::MAX_AMBIENT_LIGHTS) {
					AmbientLightGLSLStruct lightStruct{};
					lightStruct.color = ambientLight->color()->rgba();
					ambientStructs.push_back(lightStruct);
				}
			}
			else if (auto directionalLight = dynamic_cast<DirectionalLight*>(light)) {
				if (directionalStructs.size() < config::MAX_DIRECTIONAL_LIGHTS) {
					DirectionalLightGLSLStruct lightStruct{};
					lightStruct.color = directionalLight->color()->rgba();
					lightStruct.direction_world = node->worldForward();
					directionalStructs.push_back(lightStruct);
				}
			}
			else if (auto pointLight = dynamic_cast<PointLight*>(light)) {
				if (pointStructs.size() < config::MAX_POINT_LIGHTS) {
					PointLightGLSLStruct lightStruct{};
					lightStruct.color = pointLight->color()->rgba();
					lightStruct.position_world = node->worldPosition();
					lightStruct.constantAttenuation = pointLight->attenuation().constant;
					lightStruct.linearAttenuation = pointLight->attenuation().linear;
					lightStruct.quadraticAttenuation = pointLight->attenuation().quadratic;
					pointStructs.push_back(lightStruct);
				}
			}
			else if (auto spotLight = dynamic_cast<SpotLight*>(light)) {
				if (spotStructs.size() < config::MAX_SPOT_LIGHTS) {
					SpotLightGLSLStruct lightStruct{};
					lightStruct.color = spotLight->color()->rgba();
					lightStruct.position_world = node->worldPosition();
					lightStruct.direction_world = node->worldForward();
					lightStruct.innerAngleCos = spotLight->innerAngleCos();
					lightStruct.outerAngleCos = spotLight->outerAngleCos();
					lightStruct.featheringMode = magic_enum::enum_underlying(spotLight->featheringMode());
					lightStruct.constantAttenuation = spotLight->attenuation().constant;
					lightStruct.linearAttenuation = spotLight->attenuation().linear;
					lightStruct.quadraticAttenuation = spotLight->attenuation().quadratic;
					spotStructs.push_back(lightStruct);
				}
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

	// fog

	auto visualWorld = scene.visualWorld();
	FogGLSLStruct fogStruct{};
	fogStruct.startDistance = visualWorld->fogStartDistance();
	fogStruct.endDistance = visualWorld->fogEndDistance();
	fogStruct.densityExponent = visualWorld->fogDensityExponent();
	auto fogColor = visualWorld->fogColor();
	if (visualWorld->fogColor()) {
		fogStruct.color = fogColor->rgba();
	}
	else {
		fogStruct.color = {0.0, 0.0, 0.0, 0.0};
	}

	memcpy(&environmentStruct.fog, &fogStruct, sizeof(fogStruct));

	// send 'em

//	glBindBuffer(GL_UNIFORM_BUFFER, glEnvironmentUBO);
//	glBufferData(GL_UNIFORM_BUFFER, sizeof(EnvironmentBlock), nullptr, GL_DYNAMIC_DRAW); // orphan
//	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(EnvironmentBlock), &environmentStruct);

	glBindBuffer(GL_UNIFORM_BUFFER, glEnvironmentUBO);
	void* dst = glMapBufferRange(GL_UNIFORM_BUFFER, 0, sizeof(EnvironmentBlock),
								 GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);
	memcpy(dst, &environmentStruct, sizeof(EnvironmentBlock));
	glUnmapBuffer(GL_UNIFORM_BUFFER);
}

void ApplyBlendFunction(Material::BlendFunction func) {
	if (func == Material::BlendFunction::Disabled) {
		glDisable(GL_BLEND);
		return;
	}

	glEnable(GL_BLEND);
	glBlendEquation(GL_FUNC_ADD);

	switch (func) {
		case Material::BlendFunction::Alpha:
			// out = src*a + dst*(1-a)
			glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA,
								GL_ONE,       GL_ONE_MINUS_SRC_ALPHA);
			break;

		case Material::BlendFunction::PremultipliedAlpha:
			// src already multiplied by alpha: out = src + dst*(1-a)
			glBlendFuncSeparate(GL_ONE, GL_ONE_MINUS_SRC_ALPHA,
								GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
			break;

		case Material::BlendFunction::Additive:
			// common additive: out = src*a + dst
			glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE,
								GL_ONE,       GL_ONE);
			break;

		default: break;
	}
}

GLenum GLDepthFuncFromDepthFunc(DepthFunc func) {
	switch (func) {
		case DepthFunc::Less:     return GL_LESS;
		case DepthFunc::Lequal:   return GL_LEQUAL;
		case DepthFunc::Equal:    return GL_EQUAL;
		case DepthFunc::Greater:  return GL_GREATER;
		case DepthFunc::Gequal:   return GL_GEQUAL;
		case DepthFunc::Notequal: return GL_NOTEQUAL;
		case DepthFunc::Always:   return GL_ALWAYS;
		case DepthFunc::Never:    return GL_NEVER;
	}
	return GL_LESS;
}

GLenum GLFilterModeForFilterMode(Sampler::FilterMode mode) {
	switch (mode) {
		case Sampler::FilterMode::Nearest: 				return GL_NEAREST;
		case Sampler::FilterMode::Linear: 				return GL_LINEAR;
		case Sampler::FilterMode::NearestMipmapNearest:	return GL_NEAREST_MIPMAP_NEAREST;
		case Sampler::FilterMode::LinearMipmapNearest: 	return GL_LINEAR_MIPMAP_NEAREST;
		case Sampler::FilterMode::NearestMipmapLinear: 	return GL_NEAREST_MIPMAP_LINEAR;
		case Sampler::FilterMode::LinearMipmapLinear: 	return GL_LINEAR_MIPMAP_LINEAR; }
}

GLenum GLWrapModeForWrapMode(Sampler::WrapMode mode) {
	switch (mode) {
		case Sampler::WrapMode::ClampToEdge:				return GL_CLAMP_TO_EDGE;
//#ifdef A3D_GL_DESKTOP
//		case WRAP_MODE::CLAMP_TO_BORDER:		return GL_CLAMP_TO_BORDER;
//#endif
		case Sampler::WrapMode::Repeat:					return GL_REPEAT;
		default: /* MIRRORED_REPEAT */   		return GL_MIRRORED_REPEAT; }
}

void DrawOverlay(const RenderContext& context,
				 const Scene& scene,
				 FrameStats& stats,
				 const FrameStatsHistory& statsHistory,
				 Scene::DebugOptions debugOptions,
				 ImFont& titleFont,
				 ImFont& bodyFont) {

	ImguiUpdateScale(context);

	ImGui_ImplOpenGL3_NewFrame();
	ImGui::NewFrame();

	ImguiBeginOverlay(0, true);
	DrawDebugOptions(const_cast<Scene&>(scene), bodyFont); // TODO: const_cast CHEATING
	if (util::bitmask::contains(debugOptions, Scene::DebugOptions::ShowStatsOverlay)) {
		DrawStats(stats, statsHistory, context, titleFont, bodyFont);
	}

	ImguiEndOverlay();

	// pass input through Imgui window
	if (!(ImGui::IsAnyItemHovered() || ImGui::IsAnyItemActive()))
		ImGui::GetIO().WantCaptureMouse = false;

	ImGui::Render();

	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void DrawStats(FrameStats& stats,
			   const FrameStatsHistory& statsHistory,
			   const RenderContext& context,
			   ImFont& titleFont,
			   ImFont& bodyFont) {

	using namespace ImGui;

	//ShowMetricsWindow();

	static const float X_POS = 12.0;
	static const float COLUMN_WIDTH = 130.0f;
	static const bool PLOT_OUTLINED = true;
	static const float PLOT_HEIGHT_1 = 48.0;
	static const float PLOT_HEIGHT_2 = 24.0;
	static const float PLOT_X_OFFSET = 0.0;
	static const float PLOT_STR_Y_PAD = 8.0;
	static const float PLOT_Y_PAD = 18.0;
	static const float PLOT_Y_MIN = 0.0;
	static const float PLOT_Y_MAX = 17.0;
	static const int TEXT_PADDING = 15;
	static const float STAT_LINE_STEP = STATS_BODY_FONT_SIZE + 1.0f;
	static const float INDENT_WIDTH = 8.0f;

	struct StatsTextLayout {
		float xLeft;
		float xRight;
		float gap; // min gap between label and value
	};

	const auto DrawLabelValue = [](float& y,
								   const StatsTextLayout& layout,
								   const char* label,
								   const std::string& value,
								   ImFont& font,
								   float fontSize,
								   float lineStep) {

		const auto SnapPx = [](float x) -> float {
			return math::floor(x + 0.5f);
		};

		const auto TextSizeA = [](ImFont& f, float size, const char* text) -> ImVec2 {
			const float wrapWidth = 0.0f;
			return f.CalcTextSizeA(size, math::f32_max(), wrapWidth, text);
		};

		// draw label
		ImguiDrawText(SnapPx(layout.xLeft), y, label, font, fontSize);

		// measure at SAME font+size you draw with
		const ImVec2 labelSz = TextSizeA(font, fontSize, label);
		const ImVec2 valueSz = TextSizeA(font, fontSize, value.c_str());

		float xValue = layout.xRight - valueSz.x;
		const float minXValue = layout.xLeft + labelSz.x + layout.gap;
		if (xValue < minXValue) xValue = minXValue;

		// pixel snap
		xValue = SnapPx(xValue);

		ImguiDrawText(xValue, y, value.c_str(), font, fontSize);

		y += lineStep;
	};

	const auto DrawLabelValueIndented = [DrawLabelValue](float& y,
														 const StatsTextLayout& layout,
														 const char* label,
														 const std::string& value,
														 ImFont& font,
														 float fontSize,
														 float indentPx,
														 float lineStep) {
		StatsTextLayout l = layout;
		l.xLeft += indentPx;
		DrawLabelValue(y, l, label, value, font, fontSize, 0);
		y += lineStep;
	};

	const auto DrawPlot = [](float x, float& y, float w, float h,
							 const float* values,
							 int valuesCount,
							 int valuesOffset,
							 const char* overlayText,
							 float scaleMin,
							 float scaleMax,
							 int stride,
							 bool outlined,
							 int id,
							 float lineStep) {

		ImguiDrawPlot(x, y, w, h,
					  values,
					  valuesCount,
					  valuesOffset,
					  overlayText,
					  scaleMin, scaleMax,
					  stride,
					  outlined,
					  id);
		y += lineStep;
	};


	float yPos = 0;
	int id = 0;

	ImGuiIO& io = GetIO();
	auto fonts = io.Fonts->Fonts;

	yPos += 4;
	ImguiDrawText(X_POS, yPos, "avara3d", titleFont, STATS_TITLE_FONT_SIZE);

	auto buildInfo = BuildInfo::Info();
	auto version = buildInfo.version();
	static auto buildStr = std::format(
			"v{}.{}.{} build {}\n" \
             "{}\n"
			"\n" ,
			version.major, version.minor, version.patch, buildInfo.number(),
			buildInfo.type() == BuildInfo::Type::Debug ? "debug" : "release");
	yPos += 25;
	ImguiDrawText(X_POS, yPos, buildStr.c_str(), bodyFont, STATS_BODY_FONT_SIZE);

	static chrono::nanoseconds frameNsAvg, engineCpuNsAvg, renderCpuNsAvg,
			renderGpuNsAvg, physicsNsAvg, appCpuNsAvg;
	static float frameMsFAvg, engineCpuMsFAvg, renderCpuMsFAvg,
			renderGpuMsFAvg, physicsMsFAvg, appCpuMsFAvg;
	static float fpsAvg = 0;

	util::flow::every(config::FRAME_STATS_AVERAGE_UPDATE_INTERVAL, [&] {

		FrameStatsHistory::GetAverages(statsHistory,
									   frameNsAvg, engineCpuNsAvg, renderCpuNsAvg,
									   renderGpuNsAvg, physicsNsAvg, appCpuNsAvg,
									   config::FRAME_STATS_AVERAGING_DURATION);

		// ! ~zero cost
		frameMsFAvg = util::chrono::ns_to_ms_f(frameNsAvg);
		engineCpuMsFAvg = util::chrono::ns_to_ms_f(engineCpuNsAvg);
		renderCpuMsFAvg = util::chrono::ns_to_ms_f(renderCpuNsAvg);
		renderGpuMsFAvg = util::chrono::ns_to_ms_f(renderGpuNsAvg);
		physicsMsFAvg = util::chrono::ns_to_ms_f(physicsNsAvg);
		appCpuMsFAvg = util::chrono::ns_to_ms_f(appCpuNsAvg);
		if (frameMsFAvg > 0) fpsAvg = 1000.0f / frameMsFAvg;
	});

	static vector<float> frameSamples;
	static vector<float> physSamples;
	static vector<float> engCpuSamples;
	static vector<float> renderCpuSamples;
	static vector<float> renderGpuSamples;
	static vector<float> appSamples;

	static size_t frame = 0;
	static const unsigned SKIP_FRAMES = 2;
	if (!((frame++) % SKIP_FRAMES)) {

		auto &samples = statsHistory.samples();

		frameSamples.resize(samples.size());
		physSamples.resize(samples.size());
		engCpuSamples.resize(samples.size());
		renderCpuSamples.resize(samples.size());
		renderGpuSamples.resize(samples.size());
		appSamples.resize(samples.size());

		for (size_t i = 0; i < samples.size(); ++i) {
			auto sample = get<1>(samples[i]);
			frameSamples[i] = util::chrono::ns_to_ms_f(sample.frameTime);
			engCpuSamples[i] = util::chrono::ns_to_ms_f(sample.engineCpuTime);
			physSamples[i] = util::chrono::ns_to_ms_f(sample.physicsTime);
			renderCpuSamples[i] = util::chrono::ns_to_ms_f(sample.renderCpuTime);
			renderGpuSamples[i] = util::chrono::ns_to_ms_f(sample.renderGpuTime);
			appSamples[i] = util::chrono::ns_to_ms_f(sample.applicationTime);
		}
	}

	StatsTextLayout layout {
			.xLeft = X_POS,
			.xRight = X_POS + COLUMN_WIDTH,
			.gap = 12.0f
	};

	yPos += 48;
	auto rateValue = std::format("{:.0f}fps", fpsAvg);
	DrawLabelValue(yPos, layout, "", rateValue,
				   bodyFont, STATS_BODY_FONT_SIZE, 15);

	auto frameValue = std::format("{:.1f}ms", frameMsFAvg);
	DrawLabelValue(yPos, layout, "frame", frameValue,
				   bodyFont, STATS_BODY_FONT_SIZE, PLOT_Y_PAD);

	DrawPlot(X_POS, yPos, COLUMN_WIDTH, PLOT_HEIGHT_1,
			 frameSamples.data(),
			 static_cast<int>(frameSamples.size()),
			 0,
			 nullptr,
			 PLOT_Y_MIN, PLOT_Y_MAX,
			 0,
			 PLOT_OUTLINED,
			 ++id,
			 PLOT_HEIGHT_1 + PLOT_STR_Y_PAD);

	auto engineCpuValue = std::format("{:.1f}ms", engineCpuMsFAvg);
	DrawLabelValue(yPos, layout, "engine cpu", engineCpuValue,
				   bodyFont, STATS_BODY_FONT_SIZE, PLOT_Y_PAD);

	DrawPlot(X_POS, yPos, COLUMN_WIDTH, PLOT_HEIGHT_2,
			 engCpuSamples.data(),
			 static_cast<int>(engCpuSamples.size()),
			 0,
			 nullptr,
			 PLOT_Y_MIN, PLOT_Y_MAX,
			 0,
			 PLOT_OUTLINED,
			 ++id,
			 PLOT_HEIGHT_2 + PLOT_STR_Y_PAD);

	auto renderCpuValue = std::format("{:.1f}ms", renderCpuMsFAvg);
	DrawLabelValue(yPos, layout, "render sub", renderCpuValue,
				   bodyFont, STATS_BODY_FONT_SIZE, PLOT_Y_PAD);

	DrawPlot(X_POS, yPos, COLUMN_WIDTH, PLOT_HEIGHT_2,
			 renderCpuSamples.data(),
			 static_cast<int>(renderCpuSamples.size()),
			 0,
			 nullptr,
			 PLOT_Y_MIN, PLOT_Y_MAX,
			 0,
			 PLOT_OUTLINED,
			 ++id,
			 PLOT_HEIGHT_2 + PLOT_STR_Y_PAD);

	auto renderGpuValue = std::format("{:.1f}ms", renderGpuMsFAvg);
	DrawLabelValue(yPos, layout, "draw", renderGpuValue,
				   bodyFont, STATS_BODY_FONT_SIZE, PLOT_Y_PAD);

	DrawPlot(X_POS, yPos, COLUMN_WIDTH, PLOT_HEIGHT_2,
			 renderGpuSamples.data(),
			 static_cast<int>(renderGpuSamples.size()),
			 0,
			 nullptr,
			 PLOT_Y_MIN, PLOT_Y_MAX,
			 0,
			 PLOT_OUTLINED,
			 ++id,
			 PLOT_HEIGHT_2 + PLOT_STR_Y_PAD);

	auto physValue = std::format("{:.1f}ms", physicsMsFAvg);
	DrawLabelValue(yPos, layout, "physics", physValue,
				   bodyFont, STATS_BODY_FONT_SIZE, PLOT_Y_PAD);

	DrawPlot(X_POS, yPos, COLUMN_WIDTH, PLOT_HEIGHT_2,
			 physSamples.data(),
			 static_cast<int>(physSamples.size()),
			 0,
			 nullptr,
			 PLOT_Y_MIN, PLOT_Y_MAX,
			 0,
			 PLOT_OUTLINED,
			 ++id,
			 PLOT_HEIGHT_2 + PLOT_STR_Y_PAD);

	auto appValue = std::format("{:.1f}ms", appCpuMsFAvg);
	DrawLabelValue(yPos, layout, "app", appValue,
				   bodyFont, STATS_BODY_FONT_SIZE, PLOT_Y_PAD);

	DrawPlot(X_POS, yPos, COLUMN_WIDTH, PLOT_HEIGHT_2,
			 appSamples.data(),
			 static_cast<int>(appSamples.size()),
			 0,
			 nullptr,
			 PLOT_Y_MIN, PLOT_Y_MAX,
			 0,
			 PLOT_OUTLINED,
			 ++id,
			 0);

	yPos += 42;

	StatsTextLayout bulkLayout = layout;

	DrawLabelValue(yPos, bulkLayout,
				   "nodes", std::format("{}", stats.numNodes),
				   bodyFont, STATS_BODY_FONT_SIZE, STAT_LINE_STEP);
	DrawLabelValue(yPos, bulkLayout,
				   "meshes", std::format("{}", stats.numMeshes),
				   bodyFont, STATS_BODY_FONT_SIZE, STAT_LINE_STEP);
	DrawLabelValue(yPos, bulkLayout,
				   "elements", std::format("{}", stats.numElements),
				   bodyFont, STATS_BODY_FONT_SIZE, STAT_LINE_STEP);
	DrawLabelValue(yPos, bulkLayout,
				   "polygons", std::format("{:.1f}k", float(stats.numPolygons) / 1000.0f),
				   bodyFont, STATS_BODY_FONT_SIZE, STAT_LINE_STEP);
	DrawLabelValue(yPos, bulkLayout,
				   "lights", std::format("{}", stats.numLights),
				   bodyFont, STATS_BODY_FONT_SIZE, STAT_LINE_STEP);

	yPos += STAT_LINE_STEP;

	DrawLabelValue(yPos, bulkLayout,
				   "phys bodies", std::format("{}",
											  stats.numDynamicBodies
											  + stats.numKinematicBodies
											  + stats.numStaticBodies),
				   bodyFont, STATS_BODY_FONT_SIZE, STAT_LINE_STEP);
	DrawLabelValueIndented(yPos, bulkLayout,
						   "static", std::format("{}", stats.numStaticBodies),
						   bodyFont, STATS_BODY_FONT_SIZE, INDENT_WIDTH, STAT_LINE_STEP);
	DrawLabelValueIndented(yPos, bulkLayout,
						   "dynamic", std::format("{}", stats.numDynamicBodies),
						   bodyFont, STATS_BODY_FONT_SIZE, INDENT_WIDTH, STAT_LINE_STEP);
	DrawLabelValueIndented(yPos, bulkLayout,
						   "kinematic", std::format("{}", stats.numKinematicBodies),
						   bodyFont, STATS_BODY_FONT_SIZE, INDENT_WIDTH, STAT_LINE_STEP);

	yPos += STAT_LINE_STEP;

	DrawLabelValue(yPos, bulkLayout,
				   "phys shapes", std::format("{}",
											  stats.numConcavePolyhedronShapes
											  + stats.numBoundingBoxShapes
											  + stats.numConvexHullShapes),
				   bodyFont, STATS_BODY_FONT_SIZE, STAT_LINE_STEP);
	DrawLabelValueIndented(yPos, bulkLayout,
						   "primitive", std::format("{}", stats.numPrimitiveShapes),
						   bodyFont, STATS_BODY_FONT_SIZE, INDENT_WIDTH, STAT_LINE_STEP);
	DrawLabelValueIndented(yPos, bulkLayout,
						   "bbox", std::format("{}", stats.numBoundingBoxShapes),
						   bodyFont, STATS_BODY_FONT_SIZE, INDENT_WIDTH, STAT_LINE_STEP);
	DrawLabelValueIndented(yPos, bulkLayout,
						   "convex", std::format("{}", stats.numConvexHullShapes),
						   bodyFont, STATS_BODY_FONT_SIZE, INDENT_WIDTH, STAT_LINE_STEP);
	DrawLabelValueIndented(yPos, bulkLayout,
						   "concave", std::format("{}", stats.numConcavePolyhedronShapes),
						   bodyFont, STATS_BODY_FONT_SIZE, INDENT_WIDTH, STAT_LINE_STEP);

	if (context.recordingGIF()) {
		yPos += STAT_LINE_STEP;
		DrawLabelValue(yPos, bulkLayout, "RECORDING",
					   std::format("{:.0f}s", context.recordedGIFTime()),
					   bodyFont, STATS_BODY_FONT_SIZE, STAT_LINE_STEP);
	}

	// input test

//	Begin("Input test", nullptr, 0);
//	SetWindowPos({10.0f, 2.0f});
//	ImGuiIO& io = GetIO();
//	auto fonts = io.Fonts->Fonts;
//	PushFont(fonts[1]);
//	// --- Button + hover ---
//	if (Button("Click me")) {
//		log::i()("ImGui button was CLICKED");
//	}
//	if (IsItemHovered()) {
//		SameLine();
//		Text("(hovering)");
//	}
//	static bool toggled = false;
//	if (Checkbox("Toggle", &toggled)) {
//		log::i()("Toggle is now: {}", toggled ? "ON" : "OFF");
//	}
//	static char textBuf[128] = "type here";
//	if (InputText("Text field", textBuf, sizeof(textBuf))) {
//		log::i()("Text changed: '{}'", textBuf);
//	}
//	Text("MousePos: (%.1f, %.1f)", io.MousePos.x, io.MousePos.y);
//	Text("MouseDown[0]: %s", io.MouseDown[0] ? "true" : "false");
//	Text("WantCaptureMouse: %s", io.WantCaptureMouse ? "true" : "false");
//	PopFont();
//	End();
}

void DrawDebugOptions(Scene& scene, ImFont& bodyFont) {

	using namespace ImGui;

	ImGuiIO& io = GetIO();

	const float WIN_WIDTH = 168;
	const float xPos = io.DisplaySize.x - WIN_WIDTH;
	float yPos = 0;
	int id = 0;
	static const float Y_PAD = 24.0;

	auto debugOptions = scene.debugOptions();

	using DebugOptions = Scene::DebugOptions;

	yPos = 12.0;
	static bool stats = util::bitmask::contains(debugOptions, DebugOptions::ShowStatsOverlay);
	if (ImguiDrawCheckbox(xPos, yPos, "stats", stats, bodyFont, STATS_BODY_FONT_SIZE, ++id)) {
		if (stats) scene.debugOptions(util::bitmask::add(debugOptions, DebugOptions::ShowStatsOverlay));
		else scene.debugOptions(util::bitmask::remove(debugOptions, DebugOptions::ShowStatsOverlay));
	}

	yPos += Y_PAD;
	bool meshWF = util::bitmask::contains(debugOptions, DebugOptions::ShowWireframes);
	if (ImguiDrawCheckbox(xPos, yPos, "mesh wireframes", meshWF, bodyFont, STATS_BODY_FONT_SIZE, ++id)) {
		if (meshWF) scene.debugOptions(util::bitmask::add(debugOptions, DebugOptions::ShowWireframes));
		else scene.debugOptions(util::bitmask::remove(debugOptions, DebugOptions::ShowWireframes));
	}

	yPos += Y_PAD;
	bool meshAABBs = util::bitmask::contains(scene.debugOptions(), DebugOptions::ShowBoundingBoxes);
	if (ImguiDrawCheckbox(xPos, yPos, "mesh AABBs", meshAABBs, bodyFont, STATS_BODY_FONT_SIZE, ++id)) {
		if (meshAABBs) scene.debugOptions(util::bitmask::add(debugOptions, DebugOptions::ShowBoundingBoxes));
		else scene.debugOptions(util::bitmask::remove(debugOptions, DebugOptions::ShowBoundingBoxes));
	}

	yPos += Y_PAD;
	bool physWF = util::bitmask::contains(scene.debugOptions(), DebugOptions::ShowPhysicsWireframes);
	if (ImguiDrawCheckbox(xPos, yPos, "physics wireframes", physWF, bodyFont, STATS_BODY_FONT_SIZE, ++id)) {
		if (physWF) scene.debugOptions(util::bitmask::add(debugOptions, DebugOptions::ShowPhysicsWireframes));
		else scene.debugOptions(util::bitmask::remove(debugOptions, DebugOptions::ShowPhysicsWireframes));
	}

	yPos += Y_PAD;
	bool physAABBs = util::bitmask::contains(scene.debugOptions(), DebugOptions::ShowPhysicsBoundingBoxes);
	if (ImguiDrawCheckbox(xPos, yPos, "physics AABBs", physAABBs, bodyFont, STATS_BODY_FONT_SIZE, ++id)) {
		if (physAABBs) scene.debugOptions(util::bitmask::add(debugOptions, DebugOptions::ShowPhysicsBoundingBoxes));
		else scene.debugOptions(util::bitmask::remove(debugOptions, DebugOptions::ShowPhysicsBoundingBoxes));
	}

	yPos += Y_PAD;
	bool physContacts = util::bitmask::contains(scene.debugOptions(), DebugOptions::ShowPhysicsContactPoints);
	if (ImguiDrawCheckbox(xPos, yPos, "physics contacts", physContacts, bodyFont, STATS_BODY_FONT_SIZE, ++id)) {
		if (physContacts) scene.debugOptions(util::bitmask::add(debugOptions, DebugOptions::ShowPhysicsContactPoints));
		else scene.debugOptions(util::bitmask::remove(debugOptions, DebugOptions::ShowPhysicsContactPoints));
	}

	yPos += Y_PAD;
	bool physNorms = util::bitmask::contains(scene.debugOptions(), DebugOptions::ShowPhysicsNormals);
	if (ImguiDrawCheckbox(xPos, yPos, "physics normals", physNorms, bodyFont, STATS_BODY_FONT_SIZE, ++id)) {
		if (physNorms) scene.debugOptions(util::bitmask::add(debugOptions, DebugOptions::ShowPhysicsNormals));
		else scene.debugOptions(util::bitmask::remove(debugOptions, DebugOptions::ShowPhysicsNormals));
	}
}

void ImguiInit(const RenderContext& context, ImFont*& titleFont, ImFont*& bodyFont) {

	using namespace ImGui;

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
//	ImPlot::CreateContext();
	ImGuiIO& io = GetIO();
	io.IniFilename = nullptr;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	ImGui_ImplOpenGL3_Init();
//	ImGui_ImplOpenGL3_Init("#version 330 core");
	// Linux: "#version 330"
	// macOS core: "#version 150" ?
	// GLES: "#version 300 es"

	auto overlayTitleFont = util::filesystem::FontNamed(STATS_TITLE_FONT_NAME, STATS_TITLE_FONT_TYPE);
	if (overlayTitleFont->buffer()->size()) {
		auto overlayBodyFont = util::filesystem::FontNamed(STATS_BODY_FONT_NAME, STATS_BODY_FONT_TYPE);
		if (overlayBodyFont->buffer()->size()) {

			ImGui_ImplOpenGL3_DestroyDeviceObjects(); // was DestroyFontsTexture()

			auto fbSize = context.framebufferSize();
			auto fbScale = context.viewportScale();
			ImGuiIO& io = ImGui::GetIO();
			io.DisplaySize = ImVec2(float(fbSize.x), float(fbSize.y));
			io.DisplayFramebufferScale = ImVec2(fbScale.x, fbScale.y);

			ImGui::GetIO().Fonts->Clear();

			ImguiAddFont(context, *overlayTitleFont, titleFont);
			ImguiAddFont(context, *overlayBodyFont, bodyFont);

			ImGui_ImplOpenGL3_CreateDeviceObjects();  // was CreateFontsTexture()
		}
		else {
			log::e()("Unable to load font: {}.{}", STATS_TITLE_FONT_NAME, STATS_TITLE_FONT_TYPE);
		}
	}
	else {
		log::e()("Unable to load font: {}.{}", STATS_TITLE_FONT_NAME, STATS_TITLE_FONT_TYPE);
	}

	ImguiUpdateScale(context);
}

void ImguiUpdateScale(const RenderContext& context) {

	using namespace ImGui;

	auto vpSize = context.viewportLogicalSize();
	auto vpScale = context.viewportScale();
	ImGuiIO& io = ImGui::GetIO();
	io.DisplaySize = ImVec2(float(vpSize.x), float(vpSize.y));
	io.DisplayFramebufferScale = ImVec2(vpScale.x, vpScale.y);

	glViewport(0, 0, vpSize.x*vpScale.x, vpSize.y*vpScale.y);
}

void ImguiAddFont(const RenderContext& context, const Font& font, ImFont*& imFont) {

	using namespace ImGui;

	ImFontConfig fontConfig{};
	fontConfig.FontDataOwnedByAtlas = false;

	// ! leave oversampling automatic (0) in modern ImGui
	fontConfig.OversampleH = 0;
	fontConfig.OversampleV = 0;

	// size_pixels = 0.0f => treat as a font source; size picked via PushFont(font, size_px)
	imFont = GetIO().Fonts->AddFontFromMemoryTTF(font.buffer()->data(),
												 (int)font.buffer()->size(),
												 0.0f,
												 &fontConfig);
}

void ImguiBeginOverlay(int id, bool allowsInput) {

	using namespace ImGui;

	ImGuiIO& io = GetIO();
	ImGuiWindowFlags flags =
			ImGuiWindowFlags_NoDecoration |
			ImGuiWindowFlags_NoMove |
			ImGuiWindowFlags_NoSavedSettings |
			ImGuiWindowFlags_NoNav |
			ImGuiWindowFlags_NoBackground;

	if (!allowsInput) flags |= ImGuiWindowFlags_NoInputs;

	SetNextWindowPos(ImVec2(0,0), ImGuiCond_Always);
	SetNextWindowSize(io.DisplaySize, ImGuiCond_Always);
	Begin(("##overlay" + std::to_string(id)).c_str(), nullptr, flags);
}

void ImguiEndOverlay() {
	ImGui::End();
}

void ImguiDrawText(float x,
				   float y,
				   const char* text,
				   ImFont& font,
				   float size) {

	using namespace ImGui;

	PushFont(&font, size); // modern: choose size here :contentReference[oaicite:3]{index=3}
	ImDrawList* dl = GetForegroundDrawList();

	dl->AddText(ImVec2(x+1, y+1), IM_COL32(0,0,0,255), text);
	dl->AddText(ImVec2(x,   y  ), IM_COL32(255,255,255,255), text);

	ImGui::PopFont();
}

void ImguiDrawPlot(float x, float y, float w, float h,
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

	PushStyleColor(ImGuiCol_FrameBg, ImVec4(0,0,0,0));

	// shadow pass
	SetCursorScreenPos(ImVec2(x+1, y+1));
	PushStyleColor(ImGuiCol_PlotLines, ImVec4(0,0,0,1));
	PlotLines(("##plot_s" + std::to_string(id)).c_str(),
			  values, valuesCount, 0, nullptr, scaleMin, scaleMax, ImVec2(w,h));
	PopStyleColor();

	// main pass
	SetCursorScreenPos(ImVec2(x, y));
	PushStyleColor(ImGuiCol_PlotLines, ImVec4(1,1,1,1));
	if (outlined) {
		PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0.5f);
		PushStyleColor(ImGuiCol_Border, ImVec4(1,1,1,0.5f));
	}
	PlotLines(("##plot" + std::to_string(id)).c_str(),
			  values, valuesCount, 0, nullptr, scaleMin, scaleMax, ImVec2(w,h));
	if (outlined) {
		PopStyleColor();
		PopStyleVar();
	}
	PopStyleColor();

	PopStyleColor();
}

bool ImguiDrawCheckbox(float x, float y,
					   const char* text,
					   bool& checked,
					   ImFont& font,
					   float size,
					   int id) {

	using namespace ImGui;

	ImGui::PushFont(&font, size);

	const ImVec4 transparent(0,0,0,0);
	const float shadow_off = 1.0f;
	const float border_thickness = 1.0f;

	const float box_size = GetFrameHeight(); // checkbox square size
	const float label_gap = GetStyle().ItemInnerSpacing.x; // spacing between box and label

	// shadow checkbox (non-interactive, non-blocking)
	PushID(id);
	SetCursorScreenPos(ImVec2(x + shadow_off, y + shadow_off));
	BeginDisabled(true);

	PushStyleVar(ImGuiStyleVar_FrameBorderSize, border_thickness);
	PushStyleColor(ImGuiCol_FrameBg,        transparent);
	PushStyleColor(ImGuiCol_FrameBgHovered, transparent);
	PushStyleColor(ImGuiCol_FrameBgActive,  transparent);
	PushStyleColor(ImGuiCol_Border,         ImVec4(0,0,0,1));
	PushStyleColor(ImGuiCol_BorderShadow,   transparent);
	PushStyleColor(ImGuiCol_CheckMark,      ImVec4(0,0,0,1));

	bool dummy = checked;
	SetNextItemAllowOverlap();
	Checkbox("##shadow", &dummy);

	PopStyleColor(6);
	PopStyleVar();
	EndDisabled();
	PopID();

	// real checkbox (interactive, NO label)
	bool ret = false;

	PushID(id);
	SetCursorScreenPos(ImVec2(x, y));

	PushStyleVar(ImGuiStyleVar_FrameBorderSize, border_thickness);
	PushStyleColor(ImGuiCol_FrameBg,        	transparent);
	PushStyleColor(ImGuiCol_FrameBgHovered, 	transparent);
	PushStyleColor(ImGuiCol_FrameBgActive,  	transparent);
	PushStyleColor(ImGuiCol_Border,         	ImVec4(1,1,1,1));
	PushStyleColor(ImGuiCol_BorderShadow,   	transparent);
	PushStyleColor(ImGuiCol_CheckMark,      	ImVec4(1,1,1,1));

	ret = Checkbox("##real", &checked);

	PopStyleColor(6);
	PopStyleVar();
	PopID();

	// raw label ourselves (true solid shadow, like DigDrawText)
	ImDrawList* dl = GetWindowDrawList(); // or GetForegroundDrawList() to match DigDrawText layer exactly

	// align label vertically with checkbox frame (center-ish)
	float text_y = y + GetStyle().FramePadding.y;

	ImVec2 label_pos(x + box_size + label_gap, text_y);

	dl->AddText(ImVec2(label_pos.x + shadow_off, label_pos.y + shadow_off),
				IM_COL32(0,0,0,255),
				text);
	dl->AddText(label_pos,
				IM_COL32(255,255,255,255),
				text);

	PopFont();
	return ret;
}
