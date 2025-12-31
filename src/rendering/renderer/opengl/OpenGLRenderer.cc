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

//#ifdef A3D_GL_ES
//#include <EGL/egl.h>
//#include <GLES3/gl3.h>
//#else
//#include "glad/glad.h"
//#endif

//#ifdef A3D_GL_DESKTOP
#include "imgui.h"
#include "imgui_impl_opengl3.h"
//#include "implot.h"
//#endif

#include "magic_enum/magic_enum.hpp"

#include "a3d/Buffer.h"
#include "a3d/BuildInfo.h"
#include "a3d/Color.h"
#include "a3d/Configuration.h"
#include "a3d/CubeImage.h"
#include "a3d/diagnostic/log/Log.h"
#include "a3d/Font.h"
#include "a3d/Image.h"
#include "a3d/Math.h"
#include "a3d/mesh/Line.h"
#include "a3d/mesh/Mesh.h"
#include "a3d/mesh/MeshElement.h"
#include "a3d/mesh/primitive/Box.h"
#include "a3d/rendering/DrawItem.h"
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

//#define DISABLE_RESOURCE_MANAGEMENT

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

static void 		GetTextureGLTextureHandles(Material& material,
											  OpenGLRenderer::TextureGLMapping& glMapping,
											  map<MaterialPropertyType, GLuint>& glTextureHandles);
static void 		BufferTexture(const Texture &texture,
								 GLuint& glTextureHandle);
static void 		SendMaterialUniforms(const Material& material,
										Program& program,
										map<MaterialPropertyType, GLuint>& glTextureHandles,
										GLStateCache& state);
static void 		SendMaterialPropertyUniforms(const MaterialProperty& property,
												MaterialPropertyType type,
												GLuint glTextureHandle,
												Program& program,
												GLStateCache& state);
static void 		SendEnvironmentUniforms(GLuint glEnvironmentUBO,
											const Scene& scene,
											const vector<Node*>& lightNodes,
											FrameStats& stats);
static void 		SetTextureSamplingOptions(Texture& texture,
											 GLuint glTextureHandle);
static void 		SetMaterialFilteringOptions(const Material& material,
											   map<MaterialPropertyType, GLuint>& glTextureHandles);
static vector<Node*>SortedLights(map<Node*, float> lights);
static void 		DrawOverlay(const RenderContext& context,
							   const Scene& scene,
							   FrameStats& stats,
							   const FrameStatsHistory& statsHistory,
							   DebugOptions debugOptions,
							   ImFont& titleFont,
							   ImFont& bodyFont);
static void 		DrawStats(FrameStats& stats,
							 const FrameStatsHistory& statsHistory,
							 const RenderContext& context,
							 ImFont& titleFont,
							 ImFont& bodyFont);
static void 		DrawDebugOptions(Scene& scene,
									const RenderContext& context,
									ImFont& titleFont,
									ImFont& bodyFont);
static void			ImguiInit(const RenderContext& context,
								 ImFont*& titleFont,
								 ImFont*& bodyFont);
static void 		ImguiUpdateScale(const RenderContext& context);
static void 		ImguiAddFont(const RenderContext& context,
								const Font& font,
								ImFont*& imFont);
void 				ImguiBeginOverlay(int id, bool allowsInput);
void 				ImguiEndOverlay();
void 				ImguiDrawText(float x,
								  float y,
								  const char* text,
								  ImFont& font,
								  float size);
void 				ImguiDrawPlot(float x, float y, float w, float h,
								  const float* values,
								  int valuesCount,
								  int valuesOffset,
								  const char* overlayText,
								  float scaleMin,
								  float scaleMax,
								  int stride,
								  bool outlined,
								  int id);
bool 				ImguiDrawCheckbox(float x,
									  float y,
									  const char* text,
									  bool& checked,
									  ImFont& font,
									  float size,
									  int id);
static GLenum 		GLDepthFuncFromDepthFunc(DepthFunc f);
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





static void ApplyBlendFunction(BlendFunction blend);







/// Private Static Members ///

bool OpenGLRenderer::InitGL(GLGetProcAddress getProcAddress) {

	static bool initialized = false;
	if (initialized) return true;

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

	initialized = true;

	LogGLInfo();

	return true;
}

/// Internal Lifecycle Functions ///

OpenGLRenderer::OpenGLRenderer():
		Renderer{},
		_isInitialized{false},
		_meshElementGLMapping{}, // TODO: REMOVE
		_textureGLMapping{},
		_glEnvironmentUBO{0},
		_overlayTitleImFont{nullptr},
		_overlayBodyImFont{nullptr},
		_drawTimer{config::GL_DRAW_TIMER_BUFFER_SIZE},
		_cache{},
		_skyboxMesh{},


		_meshElementGL{} {}

OpenGLRenderer::~OpenGLRenderer() {
	log::d()("Destroying OpenGLRenderer {:p}", static_cast<void*>(this));

	glDeleteBuffers(1, &_glEnvironmentUBO);

	// TODO: move?
	if (_dbgLinesVBO) {
		glDeleteBuffers(1, &_dbgLinesVBO);
		_dbgLinesVBO = 0;
	}
	if (_dbgLinesVAO) {
		glDeleteVertexArrays(1, &_dbgLinesVAO);
		_dbgLinesVAO = 0;
	}

	ImGui_ImplOpenGL3_Shutdown();
//	ImPlot::DestroyContext();
	ImGui::DestroyContext();
}
	
/// Renderer Internal Member Functions ///

RenderingApi OpenGLRenderer::renderingApi() const {
	return RenderingApi::OpenGL;
}

bool OpenGLRenderer::initialize(const RenderContext& context) {
	log::i()("");

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
	bindBlock(Program::Default().glID(),  "EnvironmentBlock");
	bindBlock(Program::Wireframe().glID(), "EnvironmentBlock");

	_drawTimer.initialize();

	// TODO: make failable?
	ImguiInit(context, _overlayTitleImFont, _overlayBodyImFont);

	_isInitialized = true;

	return true;
}

bool OpenGLRenderer::isInitialized() const {
	return _isInitialized;
}

void OpenGLRenderer::beginFrame(const Scene& scene,
								const RenderContext& context,
								const DebugOptions& debugOptions,
								FrameStats& stats,
								Profiler& profiler) {

	_drawTimer.begin();

	_state = {};
	_state.pipelineHandle = INVALID_PIPELINE_HANDLE;
	_state.program = 0;
	_state.material = nullptr;

	_boundElement = {};
}

void OpenGLRenderer::endFrame(const Scene& scene,
							  const RenderContext& context,
							  const DebugOptions& debugOptions,
							  FrameStats& stats,
							  Profiler& profiler,
							  const FrameStatsHistory& statsHistory) {

	DrawOverlay(context, scene, stats, statsHistory, debugOptions,
				*_overlayTitleImFont, *_overlayBodyImFont);

	A3D_GL_CHECK();

	profiler.add(Profiler::Tag::RenderGpu, _drawTimer.end());
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
	//glBindBufferBase(GL_UNIFORM_BUFFER, ENV_BINDING_POINT, _glEnvironmentUBO); // necessary? -- nope!
}

void OpenGLRenderer::EnsureDebugLinesBuffers() {
	if (_dbgLinesVAO != 0 && _dbgLinesVBO != 0) return;

	glGenVertexArrays(1, &_dbgLinesVAO);
	glGenBuffers(1, &_dbgLinesVBO);

	glBindVertexArray(_dbgLinesVAO);
	glBindBuffer(GL_ARRAY_BUFFER, _dbgLinesVBO);

	// Attribute 0: position
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(
			0, 3, GL_FLOAT, GL_FALSE,
			sizeof(DebugLineVertex),
			(void*)offsetof(DebugLineVertex, pos)
	);

	// Attribute 1: color
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(
			1, 3, GL_FLOAT, GL_FALSE,
			sizeof(DebugLineVertex),
			(void*)offsetof(DebugLineVertex, color)
	);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
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

/// Private Static Non-Member Functions ///

void GetTextureGLTextureHandles(Material& material,
								OpenGLRenderer::TextureGLMapping& glMapping,
								std::map<MaterialPropertyType, GLuint>& glTextureHandles) {

	glTextureHandles.clear();

	for (auto& [property, type] : material.properties()) {

		auto textureSP = std::get_if<std::shared_ptr<Texture>>(property);
		if (!textureSP || !(*textureSP)) continue;

		Texture* tex = textureSP->get();

		// Look up WITHOUT inserting
		GLuint handle = 0;
		auto it = glMapping.find(tex);
		if (it != glMapping.end()) handle = it->second;

		const bool contentsDirty =
				A3D_MASK_CONTAINS(tex->dirtyMask(), TextureDirtyMask::Contents);

		const bool missingOrZero = (it == glMapping.end()) || (handle == 0);

		if (contentsDirty || missingOrZero) {

			log::d()("Uploading texture {:p} (dirty={}, missingOrZero={})",
					  (void*)tex, contentsDirty, missingOrZero);

			// If we had an old handle, delete it cleanly
			if (handle != 0) {
				glDeleteTextures(1, &handle);
				handle = 0;
			}

			GLuint newID = 0;
			BufferTexture(*tex, newID);

			if (newID == 0) {
				log::e()("BufferTexture failed for texture {:p}", (void*)tex);
				// leave handle 0; still record it so you can see the failure downstream
				glMapping.erase(tex);
			} else {
				handle = newID;
				glMapping[tex] = handle;
			}

			// Clear only the Contents bit (don’t wipe other bits unless you mean to)
			tex->dirtyMask(A3D_MASK_REMOVE(tex->dirtyMask(), TextureDirtyMask::Contents));
		}

		glTextureHandles[type] = handle;
	}
}

void BufferTexture(const Texture& texture,
				   GLuint& glTextureHandle) {

	auto contents = texture.contents();

	std::visit([&texture, &glTextureHandle](auto&& contents) -> void {

		using T = std::decay_t<decltype(contents)>;

		if constexpr (std::is_same_v<T, shared_ptr<CubeImage>>) {

			log::d()("Buffering cube texture {:p}...", static_cast<const void*>(&contents));

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

				assert(image->bytesPerPixel() == 4);
//				unsigned bytesPerPixel = image->bytesPerPixel();
//				GLint glInternalFormat = GL_RGBA;
//				if (bytesPerPixel == 3) glInternalFormat = GL_RGB;
//				else if (bytesPerPixel == 1) glInternalFormat = GL_RED;

				glTexImage2D(side,
							 0,
							 GL_RGBA8,//glInternalFormat,//GL_RGB, //GL_SRGB_ALPHA,
							 image->width(),
							 image->height(),
							 0,
							 GL_RGBA,//(image->bytesPerPixel() == 3 ? GL_RGB : GL_RGBA),
							 GL_UNSIGNED_BYTE,
							 *(image->buffer()));
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

			log::d()("Buffering 2D texture {:p}...", static_cast<const void*>(&contents));

			auto image = dynamic_pointer_cast<Image>(contents);

			glGenTextures(1, &glTextureHandle);
			log::d()("Binding new texture handle: {}", glTextureHandle);
			glBindTexture(GL_TEXTURE_2D, glTextureHandle);

//		unsigned bytesPerPixel = image->bytesPerPixel();
//		GLint glInternalFormat;
//		if (bytesPerPixel == 3) glInternalFormat = GL_RGB;
//		else if (bytesPerPixel == 1) glInternalFormat = GL_RED;

			log::d()("Buffering image {:p}: width: {}, height: {}, bytesPerPixel: {}, data size: {}",
					  static_cast<void*>(image.get()), image->width(), image->height(), image->bytesPerPixel(),
					  image->width() * image->height() * image->bytesPerPixel());

			glTexImage2D(GL_TEXTURE_2D,
						 0,
						 GL_RGBA8,//glInternalFormat,//GL_RGBA,//GL_SRGB_ALPHA,
						 image->width(),
						 image->height(),
						 0,
						 GL_RGBA,//(image->bytesPerPixel() == 3 ? GL_RGB : GL_RGBA),
						 GL_UNSIGNED_BYTE,
						 *(image->buffer()));

			auto sampler = texture.sampler();
			SetTextureMinificationFilter(glTextureHandle, false, sampler->minificationFilter());
			SetTextureMagnificationFilter(glTextureHandle, false, sampler->magnificationFilter());
			SetTextureMaxAnisotropy(glTextureHandle, false, sampler->maxAnisotropy());
			SetTextureWrapS(glTextureHandle, false, sampler->wrapS());
			SetTextureWrapT(glTextureHandle, false, sampler->wrapT());
		}
		else if constexpr (std::is_same_v<T, std::monostate>) {
			log::e()("Empty texture variant.");
		}

	}, contents);
}

void SendMaterialUniforms(const Material& material,
						  Program& program,
						  map<MaterialPropertyType, GLuint>& glTextureHandles,
						  GLStateCache& state) {

	// sends uniforms for the Material, and MaterialProperties it has

//	program.use();
//	state.program = program.glID();

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
										 program,
										 state);
		}
	}

	//program.unuse();
}

void SendMaterialPropertyUniforms(const MaterialProperty& property,
								  MaterialPropertyType type,
								  GLuint glTextureHandle,
								  Program& program,
								  GLStateCache& state) {

//	program.use();
//	state.program = program.glID();

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
							log::e()("Invalid MaterialPropertyType: {}",
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
					log::e()("Empty texture variant.");
				}

			}, property->contents());
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
					log::e()("Invalid MaterialPropertyType: {}",
							  magic_enum::enum_name<MaterialPropertyType>(type));
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
	
	//program.unuse();
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
					lightStruct.constantAttenuation = pointLight->constantAttenuation();
					lightStruct.linearAttenuation = pointLight->linearAttenuation();
					lightStruct.quadraticAttenuation = pointLight->quadraticAttenuation();
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
					lightStruct.constantAttenuation = spotLight->constantAttenuation();
					lightStruct.linearAttenuation = spotLight->linearAttenuation();
					lightStruct.quadraticAttenuation = spotLight->quadraticAttenuation();
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

	glBindBuffer(GL_UNIFORM_BUFFER, glEnvironmentUBO);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(EnvironmentBlock), &environmentStruct, GL_DYNAMIC_DRAW);
}

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
		
#ifdef A3D_GL_DESKTOP
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glEnable(GL_LINE_SMOOTH);
#endif
	}
	else {
#ifdef A3D_GL_DESKTOP
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

void DrawOverlay(const RenderContext& context,
				 const Scene& scene,
				 FrameStats& stats,
				 const FrameStatsHistory& statsHistory,
				 DebugOptions debugOptions,
				 ImFont& titleFont,
				 ImFont& bodyFont) {

	// glBindFramebuffer(GL_FRAMEBUFFER, context.defaultFramebuffer());

	ImguiUpdateScale(context);

	ImGui_ImplOpenGL3_NewFrame();
	ImGui::NewFrame();

	ImguiBeginOverlay(0, true);
	DrawDebugOptions(const_cast<Scene&>(scene), context, titleFont, bodyFont); // TODO: const_cast CHEATING
	if (A3D_MASK_CONTAINS(debugOptions, DebugOptions::ShowStatsOverlay)) {
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

	utils::flow::every(config::FRAME_STATS_AVERAGE_UPDATE_INTERVAL, [&] {

		FrameStatsHistory::GetAverages(statsHistory,
									   frameNsAvg, engineCpuNsAvg, renderCpuNsAvg,
									   renderGpuNsAvg, physicsNsAvg, appCpuNsAvg,
									   config::FRAME_STATS_AVERAGING_DURATION);

		// ! ~zero cost
		frameMsFAvg = utils::chrono::ns_to_ms_f(frameNsAvg);
		engineCpuMsFAvg = utils::chrono::ns_to_ms_f(engineCpuNsAvg);
		renderCpuMsFAvg = utils::chrono::ns_to_ms_f(renderCpuNsAvg);
		renderGpuMsFAvg = utils::chrono::ns_to_ms_f(renderGpuNsAvg);
		physicsMsFAvg = utils::chrono::ns_to_ms_f(physicsNsAvg);
		appCpuMsFAvg = utils::chrono::ns_to_ms_f(appCpuNsAvg);
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
			frameSamples[i] = utils::chrono::ns_to_ms_f(sample.frameTime);
			engCpuSamples[i] = utils::chrono::ns_to_ms_f(sample.engineCpuTime);
			physSamples[i] = utils::chrono::ns_to_ms_f(sample.physicsTime);
			renderCpuSamples[i] = utils::chrono::ns_to_ms_f(sample.renderCpuTime);
			renderGpuSamples[i] = utils::chrono::ns_to_ms_f(sample.renderGpuTime);
			appSamples[i] = utils::chrono::ns_to_ms_f(sample.applicationTime);
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
						   "hull", std::format("{}", stats.numConvexHullShapes),
						   bodyFont, STATS_BODY_FONT_SIZE, INDENT_WIDTH, STAT_LINE_STEP);
	DrawLabelValueIndented(yPos, bulkLayout,
						   "concave", std::format("{}", stats.numConcavePolyhedronShapes),
						   bodyFont, STATS_BODY_FONT_SIZE, INDENT_WIDTH, STAT_LINE_STEP);

	if (context.recordingGIF()) {
		DrawLabelValue(yPos, bulkLayout, "RECORDING",
					   std::format("{:.1f}s / {} {}", context.recordedGIFTime(),
								   context.recordedGIFFrames(),
								   context.recordedGIFFrames() == 1 ? "frame" : "frames"),
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

void DrawDebugOptions(Scene& scene, const RenderContext& context,
					  ImFont& titleFont, ImFont& bodyFont) {

	using namespace ImGui;

	ImGuiIO& io = GetIO();

	const float WIN_WIDTH = 168;
	const float xPos = io.DisplaySize.x - WIN_WIDTH;
	float yPos = 0;
	int id = 0;
	static const float Y_PAD = 24.0;

	auto debugOptions = scene.debugOptions();

	yPos = 12.0;
	static bool stats = A3D_MASK_CONTAINS(debugOptions, DebugOptions::ShowStatsOverlay);
	if (ImguiDrawCheckbox(xPos, yPos, "stats", stats, bodyFont, STATS_BODY_FONT_SIZE, ++id)) {
		if (stats) scene.debugOptions(A3D_MASK_ADD(debugOptions, DebugOptions::ShowStatsOverlay));
		else scene.debugOptions(A3D_MASK_REMOVE(debugOptions, DebugOptions::ShowStatsOverlay));
	}

	yPos += Y_PAD;
	bool meshWF = A3D_MASK_CONTAINS(debugOptions, DebugOptions::ShowWireframes);
	if (ImguiDrawCheckbox(xPos, yPos, "mesh wireframes", meshWF, bodyFont, STATS_BODY_FONT_SIZE, ++id)) {
		if (meshWF) scene.debugOptions(A3D_MASK_ADD(debugOptions, DebugOptions::ShowWireframes));
		else scene.debugOptions(A3D_MASK_REMOVE(debugOptions, DebugOptions::ShowWireframes));
	}

	yPos += Y_PAD;
	bool meshAABBs = A3D_MASK_CONTAINS(scene.debugOptions(), DebugOptions::ShowBoundingBoxes);
	if (ImguiDrawCheckbox(xPos, yPos, "mesh AABBs", meshAABBs, bodyFont, STATS_BODY_FONT_SIZE, ++id)) {
		if (meshAABBs) scene.debugOptions(A3D_MASK_ADD(debugOptions, DebugOptions::ShowBoundingBoxes));
		else scene.debugOptions(A3D_MASK_REMOVE(debugOptions, DebugOptions::ShowBoundingBoxes));
	}

	yPos += Y_PAD;
	bool physWF = A3D_MASK_CONTAINS(scene.debugOptions(), DebugOptions::ShowPhysicsWireframes);
	if (ImguiDrawCheckbox(xPos, yPos, "physics wireframes", physWF, bodyFont, STATS_BODY_FONT_SIZE, ++id)) {
		if (physWF) scene.debugOptions(A3D_MASK_ADD(debugOptions, DebugOptions::ShowPhysicsWireframes));
		else scene.debugOptions(A3D_MASK_REMOVE(debugOptions, DebugOptions::ShowPhysicsWireframes));
	}

	yPos += Y_PAD;
	bool physAABBs = A3D_MASK_CONTAINS(scene.debugOptions(), DebugOptions::ShowPhysicsBoundingBoxes);
	if (ImguiDrawCheckbox(xPos, yPos, "physics AABBs", physAABBs, bodyFont, STATS_BODY_FONT_SIZE, ++id)) {
		if (physAABBs) scene.debugOptions(A3D_MASK_ADD(debugOptions, DebugOptions::ShowPhysicsBoundingBoxes));
		else scene.debugOptions(A3D_MASK_REMOVE(debugOptions, DebugOptions::ShowPhysicsBoundingBoxes));
	}

	yPos += Y_PAD;
	bool physContacts = A3D_MASK_CONTAINS(scene.debugOptions(), DebugOptions::ShowPhysicsContactPoints);
	if (ImguiDrawCheckbox(xPos, yPos, "physics contacts", physContacts, bodyFont, STATS_BODY_FONT_SIZE, ++id)) {
		if (physContacts) scene.debugOptions(A3D_MASK_ADD(debugOptions, DebugOptions::ShowPhysicsContactPoints));
		else scene.debugOptions(A3D_MASK_REMOVE(debugOptions, DebugOptions::ShowPhysicsContactPoints));
	}

	yPos += Y_PAD;
	bool physNorms = A3D_MASK_CONTAINS(scene.debugOptions(), DebugOptions::ShowPhysicsNormals);
	if (ImguiDrawCheckbox(xPos, yPos, "physics normals", physNorms, bodyFont, STATS_BODY_FONT_SIZE, ++id)) {
		if (physNorms) scene.debugOptions(A3D_MASK_ADD(debugOptions, DebugOptions::ShowPhysicsNormals));
		else scene.debugOptions(A3D_MASK_REMOVE(debugOptions, DebugOptions::ShowPhysicsNormals));
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

	auto overlayTitleFont = utils::FontNamed(STATS_TITLE_FONT_NAME, STATS_TITLE_FONT_TYPE);
	if (overlayTitleFont->buffer()->size()) {
		auto overlayBodyFont = utils::FontNamed(STATS_BODY_FONT_NAME, STATS_BODY_FONT_TYPE);
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

GLenum GLDepthFuncFromDepthFunc(DepthFunc f) {
	switch (f) {
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

void SetTextureMinificationFilter(GLuint glTextureHandle, bool cube, FilterMode mode) {

	auto texType = (cube ? GL_TEXTURE_CUBE_MAP : GL_TEXTURE_2D);

	glBindTexture(texType, glTextureHandle);

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

	glTexParameteri(texType, GL_TEXTURE_MIN_FILTER, (GLint)GLFilterModeForFilterMode(mode));
}

void SetTextureMagnificationFilter(GLuint glTextureHandle, bool cube, FilterMode mode) {

	auto texType = (cube ? GL_TEXTURE_CUBE_MAP : GL_TEXTURE_2D);

	glBindTexture(texType, glTextureHandle);

	switch (mode) {
		case FilterMode::Nearest:
		case FilterMode::Linear:
			glTexParameteri(texType, GL_TEXTURE_MAG_FILTER, (GLint)GLFilterModeForFilterMode(mode));
			break;
		default:
			log::w()("Unsupported magnification filter: {}", magic_enum::enum_name(mode));
		break;
	}
}

void SetTextureMaxAnisotropy(GLuint glTextureHandle, bool cube, float max) {
#ifdef A3D_GL_DESKTOP
	auto texType = (cube ? GL_TEXTURE_CUBE_MAP : GL_TEXTURE_2D);
	glBindTexture(texType, glTextureHandle);
	float anisotropy = max;
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
//#ifdef A3D_GL_DESKTOP
//		case WRAP_MODE::CLAMP_TO_BORDER:		return GL_CLAMP_TO_BORDER;
//#endif
		case WrapMode::Repeat:					return GL_REPEAT;
        default: /* MIRRORED_REPEAT */   		return GL_MIRRORED_REPEAT; }
}

void LogGLInfo() {

	const GLubyte *vendor = glGetString(GL_VENDOR);
	const GLubyte* renderer = glGetString(GL_RENDERER);
	const GLubyte* version = glGetString(GL_VERSION);

	log::i()("GL_VENDOR: {}", reinterpret_cast<const char*>(renderer));
	log::i()("GL_RENDERER: {}", reinterpret_cast<const char*>(renderer));
	log::i()("GL_VERSION: {}", reinterpret_cast<const char*>(version));
}






void ApplyBlendFunction(BlendFunction f) {
	if (f == BlendFunction::Disabled) {
		glDisable(GL_BLEND);
		return;
	}

	glEnable(GL_BLEND);
	glBlendEquation(GL_FUNC_ADD);

	switch (f) {
		case BlendFunction::Alpha:
			// out = src*a + dst*(1-a)
			glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA,
								GL_ONE,       GL_ONE_MINUS_SRC_ALPHA);
			break;

		case BlendFunction::PremultipliedAlpha:
			// src already multiplied by alpha: out = src + dst*(1-a)
			glBlendFuncSeparate(GL_ONE, GL_ONE_MINUS_SRC_ALPHA,
								GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
			break;

		case BlendFunction::Additive:
			// common additive: out = src*a + dst
			glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE,
								GL_ONE,       GL_ONE);
			break;

		default: break;
	}
}











RenderResourceCacheOGL& OpenGLRenderer::cache() {
	return _cache;
}




void OpenGLRenderer::clear(const ClearCommand& cmd,
						   const RenderContext& context) {
	// If you later want target-specific clear:
	// if (cmd.bindFramebuffer) glBindFramebuffer(GL_FRAMEBUFFER, cmd.framebuffer);

	FBORestore restore;

	auto fb = context.defaultFramebuffer();
	auto fbSize = context.framebufferSize();
	glBindFramebuffer(GL_FRAMEBUFFER, fb);
	glViewport(0, 0, (GLsizei)fbSize.x, (GLsizei)fbSize.y);

	// Save state we might stomp.
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

	// Apply scissor if requested (partial clear).
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

	// Ensure clears actually write (optional but strongly recommended).
	if (cmd.forceWriteMasks) {
		glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
		glDepthMask(GL_TRUE);
		glStencilMask(0xFFFFFFFF);
	}

	// Set clear values (only when needed).
	GLbitfield mask = 0;

	if (cmd.clearColor) {
		glClearColor(cmd.color.x, cmd.color.y, cmd.color.z, cmd.color.w);
		mask |= GL_COLOR_BUFFER_BIT;
	}

	if (cmd.clearDepth) {
#ifdef A3D_GL_ES
		glClearDepthf(cmd.depth);
#else
		glClearDepth(cmd.depth);
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

	// Restore state we modified.
	if (cmd.forceWriteMasks) {
		glColorMask(prevColorMask[0], prevColorMask[1], prevColorMask[2], prevColorMask[3]);
		glDepthMask(prevDepthMask);
		glStencilMask((GLuint)prevStencilMask);
	}

	if (cmd.useScissor) {
		if (prevScissorEnabled) {
			glEnable(GL_SCISSOR_TEST);
			glScissor(prevScissorBox[0], prevScissorBox[1], prevScissorBox[2], prevScissorBox[3]);
		} else {
			glDisable(GL_SCISSOR_TEST);
		}
	}
}


void OpenGLRenderer::drawBackground(const BackgroundPass& backgroundPass,
									const math::mat4& viewMat,
									const math::mat4& projMat) {

	if (!backgroundPass.material) return;

	// TODO: check equality?
	// TODO: stop using shared_ptr???????

	if (_skyboxMesh->materials().empty()) {
		_skyboxMesh->addMaterial(backgroundPass.material);
	}
	else {
		_skyboxMesh->replaceMaterial(0, backgroundPass.material);
	}

	bindPipeline(backgroundPass.pipeline, _cache);
	bindMaterial(*(_skyboxMesh->materials().front()));
	bindMeshElement(*(_skyboxMesh->elements().front()));
	setPerObject(mat4(1.0f), mat4(mat3(viewMat)), projMat); // strip transform off view mat

	drawBound();
}



void OpenGLRenderer::bindPipeline(PipelineHandle pipelineHandle,
								  const RenderResourceCacheOGL& cache) {

	if (_state.pipelineHandle == pipelineHandle) {
		GLint cur = 0;
		glGetIntegerv(GL_CURRENT_PROGRAM, &cur);
		if ((GLuint)cur == _state.program) return; // truly already bound
		// else: stale cache, fallthrough and rebind
	}

	const PipelineOGL& pipeline = cache.pipeline(pipelineHandle);
	glUseProgram(pipeline.program);
	_state.program = pipeline.program;
	_state.material = nullptr;

	if (pipeline.key.doubleSided) {
		glDisable(GL_CULL_FACE);
	}
	else {
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
	}

	if (pipeline.key.depthTest) {
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GLDepthFuncFromDepthFunc(pipeline.key.depthFunc));
	}
	else {
		glDisable(GL_DEPTH_TEST);
	}
	glDepthMask(pipeline.key.depthWrite ? GL_TRUE : GL_FALSE);

	ApplyBlendFunction(pipeline.key.blendFunction);

#ifndef A3D_GL_ES
	switch (pipeline.key.fillMode) {
		case FillMode::Fill:   glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);  break;
		case FillMode::Lines:  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);  break;
		case FillMode::Points: glPolygonMode(GL_FRONT_AND_BACK, GL_POINT); break;
	}
#endif

#ifndef A3D_GL_ES
	const bool lineSmooth = (pipeline.key.pass == PassKind::Lines)
							|| (pipeline.key.pass == PassKind::Wireframe);
	if (lineSmooth) {
		glEnable(GL_LINE_SMOOTH);
		glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
	}
	else {
		glDisable(GL_LINE_SMOOTH);
	}
#endif

	if (pipeline.key.polygonOffset) {
		glEnable(GL_POLYGON_OFFSET_LINE);
		glPolygonOffset(.01, 0); // ! check !
	}
	else {
		glDisable(GL_POLYGON_OFFSET_LINE);
	}

	_state.pipelineHandle = pipelineHandle;
}

void OpenGLRenderer::bindMaterial(const Material& material) {
	if (_state.material == &material) return;

	// Look at the currently bound pipeline
	const PipelineOGL& pipeline = _cache.pipeline(_state.pipelineHandle);

	// Wireframe / lines shaders should not run the material binding path
//	if ((p.key.shaderKind != ShaderKind::Default) & (p.key.shaderKind != ShaderKind::Skybox)) {
//		_state.material = &material;
//		return;
//	}

// TODO: !!! THIS IS A DIRTY HACK !!!
	Program* program;
	switch (pipeline.key.shaderKind) {
		case ShaderKind::Default:
			program = &Program::Default();
			break; // chill
		case ShaderKind::Skybox:
			program = &Program::Skybox();
			break; // chill
		default:
			_state.material = &material;
			return; // not chill
	}

//	if (p.key.shaderKind != ShaderKind::Default) {
//		log::e()("bindMaterial() skipped for shaderKind != Default (shaderKind=%d).", int(p.key.shaderKind));
//	}

	std::map<MaterialPropertyType, GLuint> glTextureHandles;
	GetTextureGLTextureHandles(const_cast<Material&>(material),
							   _textureGLMapping,
							   glTextureHandles);


	//Program& program = Program::Default();
	// OK if SendMaterialUniforms still calls prog.use() because it matches the pipeline now

	// TODO: !!! THIS IS A DIRTY HACK !!!
	if (pipeline.key.shaderKind == ShaderKind::Default) SendMaterialUniforms(material, *program, glTextureHandles, _state);

	SetMaterialFilteringOptions(material, glTextureHandles);

	_state.material = &material;
}



void OpenGLRenderer::bindMeshElement(const MeshElement& element) {
	auto* e = const_cast<MeshElement*>(&element);
	auto& res = _meshElementGL[e];

	const bool dirty = A3D_MASK_CONTAINS(e->dirtyMask(), MeshElementDirtyMask::VertexData);
	const bool missing = (res.vao == 0);

	if (dirty || missing) {
		if (!missing) {
			glDeleteBuffers(1, &res.vbo);
			glDeleteBuffers(1, &res.ebo);
			glDeleteVertexArrays(1, &res.vao);
			res = {};
		}

		glGenVertexArrays(1, &res.vao);
		glGenBuffers(1, &res.vbo);
		glGenBuffers(1, &res.ebo);

		glBindVertexArray(res.vao);

		glBindBuffer(GL_ARRAY_BUFFER, res.vbo);
		glBufferData(GL_ARRAY_BUFFER,
					 element.vertices().size() * sizeof(Vertex),
					 element.vertices().data(),
					 GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)sizeof(vec3));
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(vec3)*2));
		glEnableVertexAttribArray(2);

		std::vector<uint32_t> indices;
		indices.reserve(element.faces().size() * 3);
		for (auto& f : element.faces()) {
			indices.push_back((uint32_t)f.a);
			indices.push_back((uint32_t)f.b);
			indices.push_back((uint32_t)f.c);
		}

		res.indexCount = (uint32_t)indices.size();

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, res.ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER,
					 indices.size() * sizeof(uint32_t),
					 indices.data(),
					 GL_STATIC_DRAW);

		e->dirtyMask(A3D_MASK_REMOVE(e->dirtyMask(), MeshElementDirtyMask::VertexData));
	}

	// Bind VAO and set draw args
	glBindVertexArray(res.vao);
	_boundElement.vao = res.vao;
	_boundElement.indexCount = (GLsizei)res.indexCount;
	_boundElement.indexType = GL_UNSIGNED_INT;
}

void OpenGLRenderer::setPerObject(const mat4& model, const mat4& view, const mat4& proj) {

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

void OpenGLRenderer::drawBound() {
	if (_boundElement.vao == 0 || _boundElement.indexCount == 0) return;

	glBindVertexArray(_boundElement.vao);
	glDrawElements(GL_TRIANGLES, _boundElement.indexCount, _boundElement.indexType, (void*)0);
}






void OpenGLRenderer::renderLinesPass(const LinesPass& pass,
									 const RenderContext& context,
									 const mat4& viewMat,
									 const mat4& projectionMat) {

	if (pass.pipeline == INVALID_PIPELINE_HANDLE) return;
	if (pass.lines.empty()) return;

	bindPipeline(pass.pipeline, _cache);
	setPerObject(pass.model, viewMat, projectionMat);

	EnsureDebugLinesBuffers();

	_dbgLineVerts.clear();
	_dbgLineVerts.reserve(pass.lines.size() * 2);

	for (const auto& line : pass.lines) {
		_dbgLineVerts.push_back({ line.fromLocation(), line.fromColor().rgb() });
		_dbgLineVerts.push_back({ line.toLocation(),   line.toColor().rgb()   });
	}

	glBindVertexArray(_dbgLinesVAO);
	glBindBuffer(GL_ARRAY_BUFFER, _dbgLinesVBO);

	glBufferData(GL_ARRAY_BUFFER,
				 (GLsizeiptr)(_dbgLineVerts.size() * sizeof(DebugLineVertex)),
				 _dbgLineVerts.data(),
				 GL_STREAM_DRAW);

	glDrawArrays(GL_LINES, 0, (GLsizei)_dbgLineVerts.size());
}

