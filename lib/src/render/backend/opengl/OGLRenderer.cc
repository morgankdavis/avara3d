//
//  OGLRenderer.cc
//  avara3d
//
//  Created by Morgan Davis on 4/24/18.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#include "a3d/render/backend/opengl/OGLRenderer.h"

#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <utility>
#include <vector>

#include "a3d/render/backend/opengl/gl.h" // must be before imgui_impl_opengl3.h

#include <imgui/backends/imgui_impl_opengl3.h>

#ifdef A3D_GL_WEB
    #include <emscripten/html5_webgl.h>
#endif

#include "a3d/Assert.h"
#include "a3d/Buffer.h"
#include "a3d/Color.h"
#include "a3d/CubeImage.h"
#include "a3d/log/Log.h"
#include "a3d/Image.h"
#include "a3d/Math.h"
#include "a3d/mesh/Mesh.h"
#include "a3d/mesh/MeshElement.h"
#include "a3d/mesh/primitive/Box.h"
#include "a3d/profile/FrameStats.h"
#include "a3d/profile/FrameStatsHistory.h"
#include "a3d/profile/Profiler.h"
#include "a3d/render/DrawPacket.h"
#include "a3d/render/backend/opengl/OGLResourceCache.h"
#include "a3d/render/backend/opengl/GLSLProgram.h"
#include "a3d/render/context/RenderContext.h"
#include "a3d/scene/Node.h"
#include "a3d/scene/Scene.h"
#include "a3d/util/Enum.h"
#include "a3d/util/Flow.h"
#include "a3d/visual/VisualWorld.h"
#include "a3d/visual/light/AmbientLight.h"
#include "a3d/visual/light/DirectionalLight.h"
#include "a3d/visual/light/PointLight.h"
#include "a3d/visual/light/SpotLight.h"
#include "a3d/visual/material/Material.h"
#include "a3d/visual/material/Sampler.h"
#include "a3d/visual/material/Texture.h"

#define A3D_GL_CHECK()                                                                                         \
    do {                                                                                                       \
        GLenum err;                                                                                            \
        while ((err = glGetError()) != GL_NO_ERROR) {                                                          \
            log::e()("GL error 0x{:X}", err);                                                                  \
        }                                                                                                      \
    } while (0);

using namespace a3d::math;
using namespace std;

namespace a3d {
namespace {

    // [Private Constants]

    constexpr std::size_t MAX_AMBIENT_LIGHTS {16};
    constexpr std::size_t MAX_DIRECTIONAL_LIGHTS {16};
    constexpr std::size_t MAX_POINT_LIGHTS {128};
    constexpr std::size_t MAX_SPOT_LIGHTS {64};

    const GLuint ENV_BINDING_POINT {0};

    constexpr bool ENABLE_GPU_TIMING {true};
    // ring buffer size for GL timing queries
    const unsigned DRAW_TIMER_BUFFER_SIZE {4};

    constexpr gl::uint_t IMGUI_VERTEX_BUFFER_ALLOCATION_ID {1};
    constexpr gl::uint_t IMGUI_INDEX_BUFFER_ALLOCATION_ID {2};
    constexpr gl::uint_t IMGUI_TEXTURE_ALLOCATION_ID {3};

    // [Private Types]

    enum class MaterialContentsType : unsigned {
        None = 0,
        Color = 1,
        Sampler = 2
    };

    static_assert(sizeof(vec4) == 16);
    static_assert(sizeof(vec3) == 12);

    struct AmbientLightGLSLStruct {
        vec4 color;
    };

    static_assert(sizeof(AmbientLightGLSLStruct) == 16);

    struct DirectionalLightGLSLStruct {
        vec4 color;
        vec3 direction_eye;
        f32  intensity;
    };

    static_assert(sizeof(DirectionalLightGLSLStruct) == 32);

    struct PointLightGLSLStruct {
        vec4 color;
        vec3 position_eye;
        f32  intensity;
        f32  constantAttenuation;
        f32  linearAttenuation;
        f32  quadraticAttenuation;
        f32  _pad_0_;
    };

    static_assert(sizeof(PointLightGLSLStruct) == 48);

    struct SpotLightGLSLStruct {
        vec4     color;
        vec3     position_eye;
        f32      intensity;
        vec3     direction_eye;
        f32      _pad_0_;
        f32      innerAngleCos;
        f32      outerAngleCos;
        uint32_t featheringMode;
        f32      constantAttenuation;
        f32      linearAttenuation;
        f32      quadraticAttenuation;
        f32      _pad_1_;
        f32      _pad_2_;
    };

    static_assert(sizeof(SpotLightGLSLStruct) == 80);

    enum class SurfaceType : uint32_t {
        None = 0,
        Plane = 1,
        Sphere = 2
    };

    struct SurfaceGLSLStruct {
        uint32_t type;
        f32      planeHeight;
        f32      _pad_0_;
        f32      _pad_1_;
        vec3     sphereCenter;
        f32      sphereRadius;
    };

    static_assert(sizeof(SurfaceGLSLStruct) == 32);

    struct FogGLSLStruct {
        vec4     color;
        f32      startDistance;
        f32      endDistance;
        f32      transitionExponent;
        uint32_t enabled;
    };

    static_assert(sizeof(FogGLSLStruct) == 32);

    struct AtmosphereHazeGLSLStruct {
        vec4     color;
        f32      density;
        uint32_t enabled;
        f32      _pad_0_;
        f32      _pad_1_;
    };

    static_assert(sizeof(AtmosphereHazeGLSLStruct) == 32);

    struct AtmosphereLimbGlowGLSLStruct {
        vec4     color;
        f32      intensity;
        uint32_t enabled;
        f32      _pad_0_;
        f32      _pad_1_;
    };

    static_assert(sizeof(AtmosphereLimbGlowGLSLStruct) == 32);

    struct AtmosphereGLSLStruct {
        f32                          scaleHeight;
        uint32_t                     enabled;
        f32                          _pad_0_;
        f32                          _pad_1_;
        AtmosphereHazeGLSLStruct     haze;
        AtmosphereLimbGlowGLSLStruct limbGlow;
    };

    static_assert(sizeof(AtmosphereGLSLStruct) == 80);

    struct EnvironmentBlock {
        uint32_t                   defaultLightingEnabled;
        uint32_t                   _pad0_[3];
        uint32_t                   numAmbientLights;
        uint32_t                   _pad1_[3];
        AmbientLightGLSLStruct     ambientLights[MAX_AMBIENT_LIGHTS];
        uint32_t                   numDirectionalLights;
        uint32_t                   _pad2_[3];
        DirectionalLightGLSLStruct directionalLights[MAX_DIRECTIONAL_LIGHTS];
        uint32_t                   numPointLights;
        uint32_t                   _pad3_[3];
        PointLightGLSLStruct       pointLights[MAX_POINT_LIGHTS];
        uint32_t                   numSpotLights;
        uint32_t                   _pad4_[3];
        SpotLightGLSLStruct        spotLights[MAX_SPOT_LIGHTS];
        FogGLSLStruct              fog;
        vec3                       viewPosition_world; // TODO: move this out to a new ViewBlock
        f32                        _pad5_0_;
        SurfaceGLSLStruct          surface;
        AtmosphereGLSLStruct       atmosphere;
    };

    static_assert(offsetof(EnvironmentBlock, fog) == 12112);
    static_assert(offsetof(EnvironmentBlock, viewPosition_world) == 12144);
    static_assert(offsetof(EnvironmentBlock, surface) == 12160);
    static_assert(offsetof(EnvironmentBlock, atmosphere) == 12192);
    static_assert(sizeof(EnvironmentBlock) == 12272);

    // [Private Non-Member Prototypes]

    void LogGLInfo();

    void SendMaterialUniforms(const Material&              material,
                              GLSLProgram&                 program,
                              const std::array<GLuint, 4>& glTextureHandles,
                              OGLRenderer::GLStateCache&   state);

    void SendMaterialPropertyUniforms(const Material::Property&  property,
                                      Material::PropertyType     type,
                                      GLuint                     glTextureHandle,
                                      GLSLProgram&               program,
                                      OGLRenderer::GLStateCache& state);

    void SendDrawUniforms(const DrawItem& item, GLSLProgram& program);

    void SendEnvironmentUniforms(GLuint               glEnvironmentUBO,
                                 const Scene&         scene,
                                 const math::mat4&    view,
                                 const vector<Node*>& lightNodes,
                                 FrameStats&          stats);

    void ApplyBlendFunction(Material::BlendFunction func);

    GLenum GLDepthFuncFromDepthFunc(DepthFunc func);

    GLenum GLFilterModeForFilterMode(Sampler::FilterMode mode);

    GLenum GLWrapModeForWrapMode(Sampler::WrapMode mode);

} // namespace

// [Private Static Members]

bool OGLRenderer::InitGL(GLGetProcAddress getProcAddress) {
    static bool initialized = false;
    if (initialized) {
        return true;
    }

#ifdef A3D_GL_DESKTOP
    if (!getProcAddress) {
        // log error, return false
        log::e()("getProcAddress is null.");
        return false;
    }

    int status = gladLoadGLLoader((GLADloadproc) getProcAddress);
    if (status == 0) {
        // log "Failed to initialize GLAD"
        log::e()("gladLoadGLLoader");
        return false;
    }
#elif A3D_GL_WEB
    (void) getProcAddress; // shut up!
#else
    #error "No OpenGL function loader defined for this platform."
#endif

    initialized = true;

    LogGLInfo();

    return true;
}

// [Internal Lifecycle Functions]

OGLRenderer::OGLRenderer():
    Renderer {},
    _isInitialized {false},
    _glCapabilities {},
    _capabilities {},
    _glEnvironmentUBO {0},
    _skyboxProgram {nullptr},
    _groundProgram {nullptr},
    _defaultProgram {nullptr},
    _wireframeProgram {nullptr},
    _linesProgram {nullptr},
    _resourceCache {_memoryTracker},
    _state {},
    _boundElement {},
    _skyboxMesh {},
    _fullscreenTriangleVao {},
    _debugLines {_memoryTracker},
    _imguiContext {},
    _statsOverlay {},
    _drawTimer {DRAW_TIMER_BUFFER_SIZE} {}

OGLRenderer::~OGLRenderer() {
    log::d()("Destroying OpenGLRenderer {:p}", static_cast<void*>(this));

    if (_fullscreenTriangleVao != 0) {
        glDeleteVertexArrays(1, &_fullscreenTriangleVao);
    }

    if (_glEnvironmentUBO != 0) {
        _memoryTracker.removeAllocation({
            OGLMemoryTracker::ObjectNamespace::Buffer,
            _glEnvironmentUBO,
        });

        glDeleteBuffers(1, &_glEnvironmentUBO);
    }

    _debugLines.destroy();

    _imguiContext.shutdown();
    _memoryTracker.clearSource(OGLMemoryTracker::Source::ImGui);
}

// [Renderer Internal Member Functions]

bool OGLRenderer::initialize(const RenderContext& context) {
    log::i();

#ifdef A3D_GL_DESKTOP
    _glCapabilities.polygonMode = true;
#elif defined(A3D_GL_WEB)
    const auto webGLContext = emscripten_webgl_get_current_context();
    log::i()("WebGL context handle: {}", webGLContext);
    if (webGLContext) {
        char*      extensions = emscripten_webgl_get_supported_extensions();
        const bool advertised = extensions && std::strstr(extensions, "WEBGL_polygon_mode");
        log::i()("WEBGL_polygon_mode advertised: {}", advertised ? "yes" : "no");
        _glCapabilities.polygonMode = emscripten_webgl_enable_WEBGL_polygon_mode(webGLContext);
        log::i()("WEBGL_polygon_mode enabled: {}", _glCapabilities.polygonMode ? "yes" : "no");
        std::free(extensions);
    }
    else {
        _glCapabilities.polygonMode = false;
        log::w()("No current Emscripten WebGL context.");
    }
#elif defined(A3D_GL_ES)
    _glCapabilities.polygonMode = false;
#endif
    _capabilities.wireframeRendering = _glCapabilities.polygonMode;

    _skyboxProgram = make_unique<GLSLProgram>("skybox");
    _groundProgram = make_unique<GLSLProgram>("ground");
    _defaultProgram = make_unique<GLSLProgram>("default");
    if (_capabilities.wireframeRendering) {
        _wireframeProgram = make_unique<GLSLProgram>("wireframe");
    }
    _linesProgram = make_unique<GLSLProgram>("lines");

    glGenVertexArrays(1, &_fullscreenTriangleVao);

    GLint maxSize = 0;
    glGetIntegerv(GL_MAX_UNIFORM_BLOCK_SIZE, &maxSize);
    A3D_ASSERT(sizeof(EnvironmentBlock) <= (size_t) maxSize);

    glGenBuffers(1, &_glEnvironmentUBO);
    glBindBuffer(GL_UNIFORM_BUFFER, _glEnvironmentUBO);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(EnvironmentBlock), nullptr, GL_DYNAMIC_DRAW);

    _memoryTracker.setAllocation(
        {
            OGLMemoryTracker::ObjectNamespace::Buffer,
            _glEnvironmentUBO,
        },
        OGLMemoryTracker::Source::A3D, OGLMemoryTracker::Category::UniformBuffer, sizeof(EnvironmentBlock));

    glBindBufferBase(GL_UNIFORM_BUFFER, ENV_BINDING_POINT, _glEnvironmentUBO);

    // just do it here even if not used
    _skyboxMesh = make_unique<a3d::Mesh>(make_unique<Box>(1, 1, 1), nullptr);

    // TODO: do something better
    auto bindBlock = [&](GLuint program, const char* blockName) {
        GLuint idx = glGetUniformBlockIndex(program, blockName);
        if (idx == GL_INVALID_INDEX) {
            return; // program doesn't have the block
        }
        glUniformBlockBinding(program, idx, ENV_BINDING_POINT);
    };
    bindBlock(_skyboxProgram->glID(), "EnvironmentBlock");
    bindBlock(_defaultProgram->glID(), "EnvironmentBlock");
    bindBlock(_groundProgram->glID(), "EnvironmentBlock");
    if (_wireframeProgram) {
        bindBlock(_wireframeProgram->glID(), "EnvironmentBlock");
    }

    _glCapabilities.drawTimer = ENABLE_GPU_TIMING && _drawTimer.initialize();
    _capabilities.gpuTiming = _glCapabilities.drawTimer;

    _imguiContext.startup(context);
    _statsOverlay.initialize(_imguiContext);

    _isInitialized = true;

    return true;
}

bool OGLRenderer::isInitialized() const {
    return _isInitialized;
}

const Renderer::Capabilities& OGLRenderer::capabilities() const {
    return _capabilities;
}

void OGLRenderer::beginFrame(const Scene&               scene,
                             const RenderContext&       context,
                             const Scene::DebugOptions& debugOptions,
                             FrameStats&                stats,
                             Profiler&                  profiler) {

    if (_glCapabilities.drawTimer) {
        _drawTimer.begin();
    }

    _state = {};
    _state.pipelineId = INVALID_PIPELINE_ID;
    _state.program = 0;
    _state.material = nullptr;

    _boundElement = {};

    const auto viewportSize = context.viewportLogicalSize();
    const auto viewportScale = context.viewportScale();

    glViewport(0, 0, static_cast<GLsizei>(viewportSize.x * viewportScale.x),
               static_cast<GLsizei>(viewportSize.y * viewportScale.y));

    _imguiContext.beginFrame(context);
}

void OGLRenderer::endFrame(const Scene&               scene,
                           const RenderContext&       context,
                           const Scene::DebugOptions& debugOptions,
                           FrameStats&                stats,
                           Profiler&                  profiler,
                           const FrameStatsHistory&   statsHistory) {

    // give the overlay the latest completed memory snapshot
    stats.renderMemory = _memoryTracker.stats();

    _statsOverlay.draw(context, scene, stats, statsHistory, debugOptions, _capabilities.gpuTiming);

    // ImGui's WebGL backend doesn't manage WEBGL_polygon_mode state.
    // Restore normal polygon rasterization before rendering ImGui.
#ifdef A3D_GL_WEB
    if (_glCapabilities.polygonMode) {
        glPolygonModeWEBGL(GL_FRONT_AND_BACK, GL_FILL_WEBGL);
        glDisable(GL_POLYGON_OFFSET_LINE_WEBGL);
    }
#endif

    _imguiContext.endFrame();
    syncImguiMemoryStats();
    // publish the final memory state for this completed frame
    stats.renderMemory = _memoryTracker.stats();

    A3D_GL_CHECK();

    if (_glCapabilities.drawTimer) {
        profiler.add(Profiler::Tag::RenderGpu, _drawTimer.end());
    }
}

void OGLRenderer::preTraversal(const Scene&               scene,
                               const RenderContext&       context,
                               const Scene::DebugOptions& debugOptions,
                               FrameStats&                stats) {}

void OGLRenderer::postTraversal(const Scene&               scene,
                                const RenderContext&       context,
                                const math::mat4&          view,
                                const vector<Node*>&       lightNodes,
                                const Scene::DebugOptions& debugOptions,
                                FrameStats&                stats) {
    SendEnvironmentUniforms(_glEnvironmentUBO, scene, view, lightNodes, stats);
}

void OGLRenderer::clear(const ClearCommand& cmd, const RenderContext& context) {

    // bind the framebuffer owned by this RenderContext. for a GLFW window this
    // is framebuffer 0. QOpenGLWidget supplies its own non-zero framebuffer.
    const auto fb = context.defaultFramebuffer();
    const auto fbSize = context.framebufferSize();

    glBindFramebuffer(GL_FRAMEBUFFER, fb);
    glViewport(0, 0, (GLsizei) fbSize.x, (GLsizei) fbSize.y);

    // glClear obeys the scissor test. establish the requested clear region
    // explicitly instead of depending on GL state left by the previous frame.
    if (cmd.scissor) {
        glEnable(GL_SCISSOR_TEST);

        const auto& scissor = *cmd.scissor;
        glScissor(scissor.x, scissor.y, scissor.width, scissor.height);
    }
    else {
        glDisable(GL_SCISSOR_TEST);
    }

    // glClear obeys write masks. force writes so state left behind by the
    // previous frame cannot prevent one of the requested buffers from clearing.
    if (cmd.forceWriteMasks) {
        if (cmd.clearColor) {
            glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
        }
        if (cmd.clearDepth) {
            glDepthMask(GL_TRUE);
        }
        if (cmd.clearStencil) {
            glStencilMask(0xFFFFFFFF);
        }
    }

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

    // scissoring is not part of the normal scene pipeline state. leave it
    // disabled after a partial clear.
    if (cmd.scissor) {
        glDisable(GL_SCISSOR_TEST);
    }

    // clear() may have changed state represented by the cached pipeline,
    // most notably the depth write mask. force the first draw of the frame
    // to reapply its complete pipeline state.
    _state.pipelineId = INVALID_PIPELINE_ID;
    _state.material = nullptr;
}

void OGLRenderer::renderPacket(DrawPacket& packet, const FrameParams& frame) {
    resolvePacket(packet, frame);
    drawPacket(packet, frame);
}

unique_ptr<Image> OGLRenderer::snapshot(const RenderContext& context) const {
    auto framebufferSize = context.framebufferSize();
    auto framebufferWidth = (unsigned) round(framebufferSize.x);
    auto framebufferHeight = (unsigned) round(framebufferSize.y);

    vector<unsigned char> pixelBuf(framebufferWidth * framebufferHeight * 4);
    // TODO: SEGV under Plasma Wayland
    // info/solution? https://projects.blender.org/blender/blender/issues/98462#issuecomment-127388
    glReadPixels(0, 0, (GLsizei) framebufferWidth, (GLsizei) framebufferHeight, GL_RGBA, GL_UNSIGNED_BYTE,
                 pixelBuf.data());
    auto buffer = make_unique<Buffer>((std::byte*) pixelBuf.data(), framebufferWidth * framebufferHeight * 4);
    return make_unique<Image>(std::move(buffer), framebufferWidth, framebufferHeight, 4);
}

// [Renderer Protected Member Functions]

void OGLRenderer::drawBackground(const BackgroundPass& backgroundPass,
                                 const math::mat4&     view,
                                 const math::mat4&     proj) {

    if (!backgroundPass.material) {
        return;
    }

    bindPipeline(backgroundPass.pipelineId);

    const auto& contents = backgroundPass.material->emission();

    if (const auto* color = get_if<Color>(&contents)) {

        _skyboxProgram->setUniform("backgroundUsesCubemap", false);
        _skyboxProgram->setUniform("backgroundColor", color->rgba());
    }
    else if (get_if<shared_ptr<Texture>>(&contents)) {

        _skyboxProgram->setUniform("backgroundUsesCubemap", true);

        // the Background setter has already validated that this Texture
        // contains a CubeImage. bindMaterial() resolves and binds cubeSampler.
        bindMaterial(*backgroundPass.material);
    }
    else {

        // background validation should make this unreachable.
        return;
    }

    const mat3 sampleRotation = mat3_cast(inverse(backgroundPass.orientation));

    _skyboxProgram->setUniform("backgroundSampleRotation", sampleRotation);

    bindMeshElement(*(_skyboxMesh->elements().front()));

    // keep the background centered on the viewer.
    applyMVP(mat4(1.0f), mat4(mat3(view)), proj);

    drawElements();
}

void OGLRenderer::drawGround(const GroundPass& groundPass, const mat4& view, const mat4& proj) {

    if (!groundPass.ground) {
        return;
    }

    if (groundPass.pipelineId == INVALID_PIPELINE_ID) {
        return;
    }

    const auto& ground = *groundPass.ground;

    bindPipeline(groundPass.pipelineId);

    const mat4 viewProj = proj * view;

    _groundProgram->setUniform("viewMat", view);
    _groundProgram->setUniform("viewProjMat", viewProj);
    _groundProgram->setUniform("inverseViewProjMat", inverse(viewProj));

    if (const auto* procedural = get_if<Ground::Procedural>(&ground.fill)) {

        _groundProgram->setUniform("groundFillType", 0u);

        const auto& grid = get<Ground::Procedural::Grid>(procedural->content);

        _groundProgram->setUniform("groundColor", grid.color.rgb());

        const bool minorGridEnabled = grid.minor.has_value();
        _groundProgram->setUniform("minorGridEnabled", minorGridEnabled);
        if (minorGridEnabled) {
            const auto& component = *grid.minor;
            _groundProgram->setUniform("minorGridColor", component.color.rgba());
            _groundProgram->setUniform("minorGridSpacing", component.spacing);
            _groundProgram->setUniform("minorGridLineWidthPixels", component.lineWidthPixels);
            _groundProgram->setUniform("minorGridReliefStrength", component.reliefStrength);
        }

        const bool majorGridEnabled = grid.major.has_value();
        _groundProgram->setUniform("majorGridEnabled", majorGridEnabled);
        if (majorGridEnabled) {
            const auto& component = *grid.major;
            _groundProgram->setUniform("majorGridColor", component.color.rgba());
            _groundProgram->setUniform("majorGridSpacing", component.spacing);
            _groundProgram->setUniform("majorGridLineWidthPixels", component.lineWidthPixels);
            _groundProgram->setUniform("majorGridReliefStrength", component.reliefStrength);
        }

        _groundProgram->setUniform("groundSpecularIntensity", grid.specularIntensity);
        _groundProgram->setUniform("groundSpecularExponent", grid.specularExponent);
    }
    else {

        _groundProgram->setUniform("groundFillType", 1u);

        const auto& configuredMaterial = get<shared_ptr<Material>>(ground.fill);
        const auto  material = configuredMaterial ? configuredMaterial : Material::DefaultMaterial();

        bindMaterial(*material);
    }

    const bool radialFadeEnabled = ground.radialFade.has_value();
    _groundProgram->setUniform("radialFadeEnabled", radialFadeEnabled);
    if (radialFadeEnabled) {
        const auto& fade = *ground.radialFade;
        _groundProgram->setUniform("radialFadeColor", fade.color.rgba());
        _groundProgram->setUniform("radialFadeCenter", fade.center);
        _groundProgram->setUniform("radialFadeStartDistance", fade.startDistance);
        _groundProgram->setUniform("radialFadeEndDistance", fade.endDistance);
    }

    const bool horizonHazeEnabled = ground.horizonHaze.has_value();
    _groundProgram->setUniform("horizonHazeEnabled", horizonHazeEnabled);
    if (horizonHazeEnabled) {
        const auto& haze = *ground.horizonHaze;
        _groundProgram->setUniform("horizonHazeColor", haze.color.rgba());
        _groundProgram->setUniform("horizonHazeAngularWidth", haze.angularWidth);
    }

    glBindVertexArray(_fullscreenTriangleVao);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glBindVertexArray(0);
}

void OGLRenderer::bindPipeline(PipelineId pipelineId) {
    if (_state.pipelineId == pipelineId) {
        return;
    }

    const OGLPipeline& pipeline = _resourceCache.pipeline(pipelineId);
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
        case Material::FillMode::Fill:
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            break;

        case Material::FillMode::Lines:
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            break;

        case Material::FillMode::Points:
            glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
            break;
    }

    const bool lineSmooth =
        (pipeline.desc.passKind == PassKind::Lines) || (pipeline.desc.passKind == PassKind::Wireframe);

    if (lineSmooth) {
        glEnable(GL_LINE_SMOOTH);
        glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    }
    else {
        glDisable(GL_LINE_SMOOTH);
    }

    if (pipeline.desc.polygonOffset) {
        glEnable(GL_POLYGON_OFFSET_LINE);
        glPolygonOffset(.01f, 0.0f); // ! check !
    }
    else {
        glDisable(GL_POLYGON_OFFSET_LINE);
    }

#elif defined(A3D_GL_WEB)

    if (_glCapabilities.polygonMode) {
        switch (pipeline.desc.fillMode) {
            case Material::FillMode::Fill:
                glPolygonModeWEBGL(GL_FRONT_AND_BACK, GL_FILL_WEBGL);
                break;

            case Material::FillMode::Lines:
                glPolygonModeWEBGL(GL_FRONT_AND_BACK, GL_LINE_WEBGL);
                break;

            case Material::FillMode::Points:
                // WEBGL_polygon_mode does not support point polygon mode.
                // Explicitly restore fill mode so a previous Lines pipeline
                // does not leave polygon mode stuck on GL_LINE_WEBGL.
                glPolygonModeWEBGL(GL_FRONT_AND_BACK, GL_FILL_WEBGL);
                break;
        }

        if (pipeline.desc.polygonOffset) {
            glEnable(GL_POLYGON_OFFSET_LINE_WEBGL);
            glPolygonOffset(.01f, 0.0f); // ! check !
        }
        else {
            glDisable(GL_POLYGON_OFFSET_LINE_WEBGL);
        }
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
            program = _defaultProgram.get();
            break;
        case ShaderKind::Ground:
            program = _groundProgram.get();
            break;
        case ShaderKind::Skybox:
            program = _skyboxProgram.get();
            break;
        default:
            _state.material = &material;
            return;
    }

    // resolve material once (uploads textures  applies sampler states via cache)
    const auto&           mr = _resourceCache.ensureMaterial(const_cast<Material&>(material));
    std::array<GLuint, 4> glTextureHandles = {
        (GLuint) mr.tex[0],
        (GLuint) mr.tex[1],
        (GLuint) mr.tex[2],
        (GLuint) mr.tex[3],
    };

    //Program& program = Program::Default();
    // OK if SendMaterialUniforms still calls prog.use() because it matches the pipeline now

    // TODO: !!! THIS IS A DIRTY HACK !!!
    if (pipeline.desc.shaderKind == ShaderKind::Default || pipeline.desc.shaderKind == ShaderKind::Ground) {

        SendMaterialUniforms(material, *program, glTextureHandles, _state);
    }
    else if (pipeline.desc.shaderKind == ShaderKind::Skybox) {

        constexpr auto emissionSlot = static_cast<size_t>(Material::PropertyType::Emission);
        program->bindTexture("cubeSampler", GL_TEXTURE_CUBE_MAP, GL_TEXTURE0, glTextureHandles[emissionSlot],
                             0);
    }

    _state.material = &material;
}

void OGLRenderer::bindMeshElement(const MeshElement& element) {
    const OGLPipeline& pipe = _resourceCache.pipeline(_state.pipelineId);

    const VertexLayout elemLayout = element.vertexLayout();
    const VertexLayout pipeLayout = pipe.desc.vertexLayoutKey;

    if (elemLayout != pipeLayout) {
        throw logic_error(std::format("VertexLayout mismatch for pipeline {}: element={}, pipeline={}",
                                      _state.pipelineId, static_cast<uint32_t>(elemLayout),
                                      static_cast<uint32_t>(pipeLayout)));
    }

    auto*       e = const_cast<MeshElement*>(&element);
    const auto& res = _resourceCache.ensureMeshElement(*e);

    glBindVertexArray((GLuint) res.vao);

    _boundElement.vao = (GLuint) res.vao;
    _boundElement.indexCount = (GLsizei) res.indexCount;
    _boundElement.indexType = (GLenum) res.indexType; // <-- USE CACHED TYPE
    _boundElement.vertexCount = (GLsizei) res.vertexCount; // <-- for drawArrays fallback
}

void OGLRenderer::applyMVP(const mat4& model, const mat4& view, const mat4& proj) {

    if (_state.pipelineId == INVALID_PIPELINE_ID) {
        return;
    }

    const auto& pipeline = _resourceCache.pipeline(_state.pipelineId);
    auto&       program = programForShaderKind(pipeline.desc.shaderKind);

    program.setUniform("modelMat", model);
    program.setUniform("viewMat", view);
    program.setUniform("projMat", proj);
}

void OGLRenderer::drawElements() {
    if (_boundElement.vao == 0) {
        return;
    }

    glBindVertexArray(_boundElement.vao);

    if (_boundElement.indexCount > 0) {
        glDrawElements(GL_TRIANGLES, _boundElement.indexCount, _boundElement.indexType, (void*) 0);
    }
    else if (_boundElement.vertexCount > 0) {
        // non-indexed fallback
        glDrawArrays(GL_TRIANGLES, 0, _boundElement.vertexCount);
    }
}

void OGLRenderer::drawLines(const LinesPass&     pass,
                            const RenderContext& context,
                            const mat4&          view,
                            const mat4&          proj) {
    if (pass.pipelineId == INVALID_PIPELINE_ID) {
        return;
    }
    if (pass.lines.empty()) {
        return;
    }

    // debug lines follow a different path:
    // - they upload on-the-fly (not baked into a MeshElement)
    // - they use their own VAO/VBO (_debugLines)
    // - they render GL_LINES topology, not GL_TRIANGLES
    // see drawDebugLines() for why this pattern is separate.
    // see drawDebugLines() for why this pattern is separate.

    bindPipeline(pass.pipelineId);
    _debugLines.upload(pass.lines);
    drawDebugLines(pass.model, view, proj);
}

// [Protected Member Functions]

void OGLRenderer::resolvePacket(DrawPacket& packet, const FrameParams& frame) {
    // "resolve / prepare / compile / bake"

    auto resolvePipeline = [&](PipelineId& pipelineId, const PipelineDesc& desc) -> PipelineId {
        if (pipelineId == INVALID_PIPELINE_ID) {
            auto& program = programForShaderKind(desc.shaderKind);
            pipelineId = _resourceCache.ensurePipeline(desc, program.glID());
        }

        return pipelineId;
    };

    // background
    if (packet.backgroundPass.material) {
        resolvePipeline(packet.backgroundPass.pipelineId, packet.backgroundPass.desc);
    }

    // ground
    if (packet.groundPass.ground) {
        resolvePipeline(packet.groundPass.pipelineId, packet.groundPass.desc);
    }
    else {
        packet.groundPass.pipelineId = INVALID_PIPELINE_ID;
    }

    // main + wireframe items
    for (auto& di : packet.mainPassItems) {
        resolvePipeline(di.pipelineId, di.desc);
    }
    for (auto& di : packet.wireframePassItems) {
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

    clear(Renderer::ClearCommand {}, frame.context);

    if (packet.backgroundPass.material) {
        // uses _skyboxMesh internally, binds + draws
        drawBackground(packet.backgroundPass, frame.view, frame.proj);
    }

    if (packet.groundPass.ground) {
        drawGround(packet.groundPass, frame.view, frame.proj);
    }

    // NOTE: items are already sorted by pass + desc hash, so this will batch nicely
    for (const auto& di : packet.mainPassItems) {
        if (di.pipelineId == INVALID_PIPELINE_ID) {
            continue;
        }
        if (!di.element) {
            continue;
        }

        bindPipeline(di.pipelineId);

        // only bind material for shaderKinds that use it (bindMaterial() already early-outs)
        if (di.material) {
            bindMaterial(*di.material);
        }

        if (di.desc.shaderKind == ShaderKind::Default) {

            SendDrawUniforms(di, *_defaultProgram);

            const mat3 normalMat = transpose(inverse(mat3(frame.view * di.model)));
            _defaultProgram->setUniform("normalMat", normalMat);
        }

        bindMeshElement(*di.element);
        applyMVP(di.model, frame.view, frame.proj);
        drawElements();
    }

    for (const auto& di : packet.wireframePassItems) {
        if (di.pipelineId == INVALID_PIPELINE_ID) {
            continue;
        }
        if (!di.element) {
            continue;
        }

        bindPipeline(di.pipelineId);

        SendDrawUniforms(di, *_wireframeProgram);

        // no bindMaterial (wire shader typically ignores it)
        bindMeshElement(*di.element);
        applyMVP(di.model, frame.view, frame.proj);
        drawElements();
    }

    drawLines(packet.linesPass, frame.context, frame.view, frame.proj);
}

// [Private Member Functions]

GLSLProgram& OGLRenderer::programForShaderKind(ShaderKind kind) const {
    switch (kind) {
        case ShaderKind::Skybox:
            return *_skyboxProgram;
        case ShaderKind::Ground:
            return *_groundProgram;
        case ShaderKind::Default:
            return *_defaultProgram;
        case ShaderKind::Wireframe:
            return *_wireframeProgram;
        case ShaderKind::Lines:
            return *_linesProgram;
    }
    throw runtime_error(std::format("Unsupported shader kind: {}", (uint32_t) kind));
}

void OGLRenderer::drawDebugLines(const math::mat4& model, const math::mat4& view, const math::mat4& proj) {
    // helper for debug line geometry which uses a separate VAO/VBO (_debugLines)
    // and doesn't follow the normal mesh binding pipeline.
    applyMVP(model, view, proj);

    glBindVertexArray(_debugLines.vao);
    glDrawArrays(GL_LINES, 0, _debugLines.vertexCount);
    glBindVertexArray(0);
}

void OGLRenderer::syncImguiMemoryStats() {

    const auto stats = ImGui_ImplOpenGL3_GetMemoryStats();

    _memoryTracker.setAllocation(
        {
            OGLMemoryTracker::ObjectNamespace::Synthetic,
            IMGUI_VERTEX_BUFFER_ALLOCATION_ID,
        },
        OGLMemoryTracker::Source::ImGui, OGLMemoryTracker::Category::VertexBuffer, stats.VertexBufferBytes);

    _memoryTracker.setAllocation(
        {
            OGLMemoryTracker::ObjectNamespace::Synthetic,
            IMGUI_INDEX_BUFFER_ALLOCATION_ID,
        },
        OGLMemoryTracker::Source::ImGui, OGLMemoryTracker::Category::IndexBuffer, stats.IndexBufferBytes);

    _memoryTracker.setAllocation(
        {
            OGLMemoryTracker::ObjectNamespace::Synthetic,
            IMGUI_TEXTURE_ALLOCATION_ID,
        },
        OGLMemoryTracker::Source::ImGui, OGLMemoryTracker::Category::Texture, stats.TextureBytes);
}

namespace {

    // [Private Non-Member Functions]

    void LogGLInfo() {
        const GLubyte* vendor = glGetString(GL_VENDOR);
        const GLubyte* renderer = glGetString(GL_RENDERER);
        const GLubyte* version = glGetString(GL_VERSION);

        log::i()("GL_VENDOR: {}", reinterpret_cast<const char*>(vendor));
        log::i()("GL_RENDERER: {}", reinterpret_cast<const char*>(renderer));
        log::i()("GL_VERSION: {}", reinterpret_cast<const char*>(version));
    }

    void SendMaterialUniforms(const Material&              material,
                              GLSLProgram&                 program,
                              const std::array<GLuint, 4>& glTextureHandles,
                              OGLRenderer::GLStateCache&   state) {
        // sends uniforms for the Material, and MaterialProperties it has

        program.setUniform("specularExponent", material.specularExponent());
        program.setUniform("uvScale", material.uvScale());
        program.setUniform("locksAmbientWithDiffuse", material.locksAmbientWithDiffuse());

        // material uniforms persist across draws -- clear unused properties to prevent
        // state leaking between materials.
        const auto none = static_cast<unsigned>(MaterialContentsType::None);
        program.setUniform("ambientContentsType", none);
        program.setUniform("diffuseContentsType", none);
        program.setUniform("specularContentsType", none);
        program.setUniform("emissionContentsType", none);

        for (auto& [property, type] : material.properties()) {
            if (!holds_alternative<monostate>(*property)) {
                const int    slot = static_cast<underlying_type<Material::PropertyType>::type>(type);
                const GLuint h = (slot >= 0) ? glTextureHandles[(size_t) slot] : 0u;
                SendMaterialPropertyUniforms(*property, type, h, program, state);
            }
        }
    }

    void SendMaterialPropertyUniforms(const Material::Property&  property,
                                      Material::PropertyType     type,
                                      GLuint                     glTextureHandle,
                                      GLSLProgram&               program,
                                      OGLRenderer::GLStateCache& state) {
        std::visit(
            [&type, &program, &glTextureHandle](auto&& property) -> void {
                using T = std::decay_t<decltype(property)>;

                if constexpr (std::is_same_v<T, shared_ptr<Texture>>) {
                    std::visit(
                        [&type, &glTextureHandle, &program](auto&& contents) -> void {
                            using T = std::decay_t<decltype(contents)>;

                            if constexpr (std::is_same_v<T, shared_ptr<Image>>) {
                                string modeUniformName;
                                string samplerUniformName;
                                GLenum slot;
                                GLint  index;

                                switch (type) {
                                    case Material::PropertyType::Ambient:
                                        modeUniformName = "ambientContentsType";
                                        //samplerUniformName = "samplers.ambient";
                                        samplerUniformName = "ambientSampler";
                                        slot = GL_TEXTURE0;
                                        index = 0;
                                        break;
                                    case Material::PropertyType::Diffuse:
                                        modeUniformName = "diffuseContentsType";
                                        //samplerUniformName = "samplers.diffuse";
                                        samplerUniformName = "diffuseSampler";
                                        slot = GL_TEXTURE1;
                                        index = 1;
                                        break;
                                    case Material::PropertyType::Specular:
                                        modeUniformName = "specularContentsType";
                                        //samplerUniformName = "samplers.specular";
                                        samplerUniformName = "specularSampler";
                                        slot = GL_TEXTURE2;
                                        index = 2;
                                        break;
                                    case Material::PropertyType::Emission:
                                        modeUniformName = "emissionContentsType";
                                        //samplerUniformName = "samplers.emission";
                                        samplerUniformName = "emissionSampler";
                                        slot = GL_TEXTURE3;
                                        index = 3;
                                        break;
                                    default:
                                        throw logic_error(std::format("Invalid MaterialPropertyType: {}",
                                                                      util::enums::enum_name<
                                                                          Material::PropertyType>(type)));
                                        return;
                                }

                                program.setUniform(modeUniformName.c_str(),
                                                   static_cast<underlying_type<MaterialContentsType>::
                                                                   type>(MaterialContentsType::Sampler));
                                program.bindTexture(samplerUniformName.c_str(), GL_TEXTURE_2D, slot,
                                                    glTextureHandle, index);
                            }
                            else if constexpr (std::is_same_v<T, shared_ptr<CubeImage>>) {
                                program.bindTexture("cubeSampler", GL_TEXTURE_CUBE_MAP, GL_TEXTURE0,
                                                    glTextureHandle, 0);
                            }
                            else if constexpr (std::is_same_v<T, std::monostate>) {
                                log::e()("Empty texture variant.");
                            }
                        },
                        property->contents());
                }
                else if constexpr (std::is_same_v<T, Color>) {
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
                            throw logic_error(std::format("Invalid MaterialPropertyType: {}",
                                                          util::enums::enum_name<
                                                              Material::PropertyType>(type)));
                            return;
                    }

                    program.setUniform(modeUniformName.c_str(),
                                       static_cast<underlying_type<
                                           MaterialContentsType>::type>(MaterialContentsType::Color));
                    program.setUniform(colorUniformName.c_str(), property.r(), property.g(), property.b());
                }
                else if constexpr (std::is_same_v<T, std::monostate>) {
                    log::w()("NULL material property contents.");
                }
            },
            property);
    }

    void SendDrawUniforms(const DrawItem& item, GLSLProgram& program) {

        program.setUniform("tint", item.tint);
    }

    void SendEnvironmentUniforms(GLuint               glEnvironmentUBO,
                                 const Scene&         scene,
                                 const mat4&          view,
                                 const vector<Node*>& lightNodes,
                                 FrameStats&          stats) {
        // block

        EnvironmentBlock environmentStruct {};

        environmentStruct.viewPosition_world = translation(inverse(view));

        // surface

        SurfaceGLSLStruct surfaceStruct {};

        if (const auto& surface = scene.visualWorld()->surface()) {

            if (const auto* plane = get_if<PlaneSurface>(&*surface)) {

                surfaceStruct.type = static_cast<uint32_t>(SurfaceType::Plane);
                surfaceStruct.planeHeight = plane->height;
            }
            else if (const auto* sphere = get_if<SphereSurface>(&*surface)) {

                surfaceStruct.type = static_cast<uint32_t>(SurfaceType::Sphere);
                surfaceStruct.sphereCenter = sphere->center;
                surfaceStruct.sphereRadius = sphere->radius;
            }
        }

        memcpy(&environmentStruct.surface, &surfaceStruct, sizeof(surfaceStruct));

        // lights

        const mat3 viewRotation = mat3(view);

        auto numLights = lightNodes.size();

        if (scene.visualWorld()->defaultLightingEnabled()) {
            environmentStruct.defaultLightingEnabled = 1u;
        }
        else {
            environmentStruct.defaultLightingEnabled = 0u;

            stats.lights = numLights;

            vector<AmbientLightGLSLStruct>     ambientStructs;
            vector<DirectionalLightGLSLStruct> directionalStructs;
            vector<PointLightGLSLStruct>       pointStructs;
            vector<SpotLightGLSLStruct>        spotStructs;

            ambientStructs.reserve(MAX_AMBIENT_LIGHTS);
            directionalStructs.reserve(MAX_DIRECTIONAL_LIGHTS);
            pointStructs.reserve(MAX_POINT_LIGHTS);
            spotStructs.reserve(MAX_SPOT_LIGHTS);

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
                    if (ambientStructs.size() < MAX_AMBIENT_LIGHTS) {
                        AmbientLightGLSLStruct lightStruct {};
                        lightStruct.color = ambientLight->color().rgba();
                        ambientStructs.push_back(lightStruct);
                    }
                }
                else if (auto directionalLight = dynamic_cast<DirectionalLight*>(light)) {
                    if (directionalStructs.size() < MAX_DIRECTIONAL_LIGHTS) {
                        DirectionalLightGLSLStruct lightStruct {};
                        lightStruct.color = directionalLight->color().rgba();
                        lightStruct.intensity = directionalLight->intensity();
                        lightStruct.direction_eye = normalize(viewRotation * node->worldForward());
                        directionalStructs.push_back(lightStruct);
                    }
                }
                else if (auto pointLight = dynamic_cast<PointLight*>(light)) {
                    if (pointStructs.size() < MAX_POINT_LIGHTS) {
                        PointLightGLSLStruct lightStruct {};
                        lightStruct.color = pointLight->color().rgba();
                        lightStruct.intensity = pointLight->intensity();
                        lightStruct.position_eye = vec3(view * vec4 {node->worldPosition(), 1.0f});
                        lightStruct.constantAttenuation = pointLight->attenuation().constant;
                        lightStruct.linearAttenuation = pointLight->attenuation().linear;
                        lightStruct.quadraticAttenuation = pointLight->attenuation().quadratic;
                        pointStructs.push_back(lightStruct);
                    }
                }
                else if (auto spotLight = dynamic_cast<SpotLight*>(light)) {
                    if (spotStructs.size() < MAX_SPOT_LIGHTS) {
                        SpotLightGLSLStruct lightStruct {};
                        lightStruct.color = spotLight->color().rgba();
                        lightStruct.intensity = spotLight->intensity();
                        lightStruct.position_eye = vec3(view * vec4 {node->worldPosition(), 1.0f});
                        lightStruct.direction_eye = normalize(viewRotation * node->worldForward());
                        lightStruct.innerAngleCos = spotLight->innerAngleCos();
                        lightStruct.outerAngleCos = spotLight->outerAngleCos();
                        lightStruct.featheringMode = util::enums::to_underlying(spotLight->featheringMode());
                        lightStruct.constantAttenuation = spotLight->attenuation().constant;
                        lightStruct.linearAttenuation = spotLight->attenuation().linear;
                        lightStruct.quadraticAttenuation = spotLight->attenuation().quadratic;
                        spotStructs.push_back(lightStruct);
                    }
                }
            }

            environmentStruct.numAmbientLights = ambientStructs.size();
            memcpy(&environmentStruct.ambientLights, ambientStructs.data(),
                   sizeof(AmbientLightGLSLStruct) * ambientStructs.size());

            environmentStruct.numDirectionalLights = directionalStructs.size();
            memcpy(&environmentStruct.directionalLights, directionalStructs.data(),
                   sizeof(DirectionalLightGLSLStruct) * directionalStructs.size());

            environmentStruct.numPointLights = pointStructs.size();
            memcpy(&environmentStruct.pointLights, pointStructs.data(),
                   sizeof(PointLightGLSLStruct) * pointStructs.size());

            environmentStruct.numSpotLights = spotStructs.size();
            memcpy(&environmentStruct.spotLights, spotStructs.data(),
                   sizeof(SpotLightGLSLStruct) * spotStructs.size());
        }

        // fog

        FogGLSLStruct fogStruct {};

        if (const auto& fog = scene.visualWorld()->fog()) {
            fogStruct.color = fog->color.rgba();
            fogStruct.startDistance = fog->startDistance;
            fogStruct.endDistance = fog->endDistance;
            fogStruct.transitionExponent = fog->transitionExponent;
            fogStruct.enabled = 1u;
        }

        memcpy(&environmentStruct.fog, &fogStruct, sizeof(fogStruct));

        // atmosphere

        AtmosphereGLSLStruct atmosphereStruct {};

        if (const auto& atmosphere = scene.visualWorld()->atmosphere()) {

            atmosphereStruct.enabled = 1u;
            atmosphereStruct.scaleHeight = atmosphere->scaleHeight;

            if (atmosphere->haze) {

                atmosphereStruct.haze.color = atmosphere->haze->color.rgba();
                atmosphereStruct.haze.density = atmosphere->haze->density;
                atmosphereStruct.haze.enabled = 1u;
            }

            if (atmosphere->limbGlow) {

                atmosphereStruct.limbGlow.color = atmosphere->limbGlow->color.rgba();
                atmosphereStruct.limbGlow.intensity = atmosphere->limbGlow->intensity;
                atmosphereStruct.limbGlow.enabled = 1u;
            }
        }

        memcpy(&environmentStruct.atmosphere, &atmosphereStruct, sizeof(atmosphereStruct));

        // send 'em

        //	glBindBuffer(GL_UNIFORM_BUFFER, glEnvironmentUBO);
        //	glBufferData(GL_UNIFORM_BUFFER, sizeof(EnvironmentBlock), nullptr, GL_DYNAMIC_DRAW); // orphan
        //	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(EnvironmentBlock), &environmentStruct);

        //	glBindBuffer(GL_UNIFORM_BUFFER, glEnvironmentUBO);
        //	void* dst = glMapBufferRange(GL_UNIFORM_BUFFER, 0, sizeof(EnvironmentBlock),
        //								 GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);
        //	memcpy(dst, &environmentStruct, sizeof(EnvironmentBlock));
        //	glUnmapBuffer(GL_UNIFORM_BUFFER);

        glBindBuffer(GL_UNIFORM_BUFFER, glEnvironmentUBO);

#ifdef A3D_GL_WEB
        glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(EnvironmentBlock), &environmentStruct);
#else
        void* dst = glMapBufferRange(GL_UNIFORM_BUFFER, 0, sizeof(EnvironmentBlock),
                                     GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);

        if (dst) {
            memcpy(dst, &environmentStruct, sizeof(EnvironmentBlock));
            glUnmapBuffer(GL_UNIFORM_BUFFER);
        }
#endif
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
                glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
                break;

            case Material::BlendFunction::PremultipliedAlpha:
                // src already multiplied by alpha: out = src + dst*(1-a)
                glBlendFuncSeparate(GL_ONE, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
                break;

            case Material::BlendFunction::Additive:
                // common additive: out = src*a + dst
                glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE, GL_ONE, GL_ONE);
                break;

            default:
                break;
        }
    }

    GLenum GLDepthFuncFromDepthFunc(DepthFunc func) {
        switch (func) {
            case DepthFunc::Less:
                return GL_LESS;
            case DepthFunc::Lequal:
                return GL_LEQUAL;
            case DepthFunc::Equal:
                return GL_EQUAL;
            case DepthFunc::Greater:
                return GL_GREATER;
            case DepthFunc::Gequal:
                return GL_GEQUAL;
            case DepthFunc::Notequal:
                return GL_NOTEQUAL;
            case DepthFunc::Always:
                return GL_ALWAYS;
            case DepthFunc::Never:
                return GL_NEVER;
        }
        return GL_LESS;
    }

    GLenum GLFilterModeForFilterMode(Sampler::FilterMode mode) {
        switch (mode) {
            case Sampler::FilterMode::Nearest:
                return GL_NEAREST;
            case Sampler::FilterMode::Linear:
                return GL_LINEAR;
            case Sampler::FilterMode::NearestMipmapNearest:
                return GL_NEAREST_MIPMAP_NEAREST;
            case Sampler::FilterMode::LinearMipmapNearest:
                return GL_LINEAR_MIPMAP_NEAREST;
            case Sampler::FilterMode::NearestMipmapLinear:
                return GL_NEAREST_MIPMAP_LINEAR;
            case Sampler::FilterMode::LinearMipmapLinear:
                return GL_LINEAR_MIPMAP_LINEAR;
        }
    }

    GLenum GLWrapModeForWrapMode(Sampler::WrapMode mode) {
        switch (mode) {
            case Sampler::WrapMode::ClampToEdge:
                return GL_CLAMP_TO_EDGE;
            //#ifdef A3D_GL_DESKTOP
            //		case WRAP_MODE::CLAMP_TO_BORDER:		return GL_CLAMP_TO_BORDER;
            //#endif
            case Sampler::WrapMode::Repeat:
                return GL_REPEAT;
            default: /* MIRRORED_REPEAT */
                return GL_MIRRORED_REPEAT;
        }
    }

} // namespace
} // namespace a3d
