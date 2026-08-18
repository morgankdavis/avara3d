//
//  OGLRenderer.h
//  avara3d
//
//  Created by Morgan Davis on 4/24/18.
//  Copyright © 2024 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_RENDER_BACKEND_OPENGL_OGLRENDERER_H
#define AVARA3D_RENDER_BACKEND_OPENGL_OGLRENDERER_H

#include "a3d/render/Renderer.h"
#include "a3d/render/backend/opengl/GLCapabilities.h"
#include "a3d/render/backend/opengl/GLTypes.h"
#include "a3d/render/backend/opengl/ImguiContext.h"
#include "a3d/render/backend/opengl/OGLDebugLines.h"
#include "a3d/render/backend/opengl/OGLDrawTimer.h"
#include "a3d/render/backend/opengl/OGLMemoryTracker.h"
#include "a3d/render/backend/opengl/OGLResourceCache.h"
#include "a3d/render/backend/opengl/StatsOverlay.h"

namespace a3d {

    struct LinesPass;

    class Color;
    class GLSLProgram;
    class Mesh;
    class MeshElement;
    class Line;
    class Texture;

    class OGLRenderer : public Renderer {

    public:
        /// Internal Types ///

        struct GLStateCache {
            PipelineId      pipelineId = INVALID_PIPELINE_ID;
            gl::uint_t      program    = 0; // currently bound GL program
            const Material* material   = nullptr; // last bound material
            uint32_t        indexCount = 0;
        };

        struct BoundElement {
            gl::uint_t  vao         = 0;
            gl::sizei_t indexCount  = 0;
            gl::enum_t  indexType   = gl::value::unsigned_int;
            gl::sizei_t vertexCount = 0;
        };

        /// Internal Static Members ///

        using GLGetProcAddress = void* (*) (const char* name);
        static bool                    InitGL(GLGetProcAddress getProcAddress);

        /// Internal Lifecycle Functions ///

        OGLRenderer();

        OGLRenderer(const OGLRenderer& other)            = delete;
        OGLRenderer& operator=(const OGLRenderer& other) = delete;

        OGLRenderer(OGLRenderer&& other)            = delete;
        OGLRenderer& operator=(OGLRenderer&& other) = delete;

        ~OGLRenderer() override;

        /// Renderer Internal Member Functions ///

        bool                   initialize(const RenderContext& context) override;
        bool                   isInitialized() const override;

        const Capabilities&    capabilities() const override;

        void                   beginFrame(const Scene&               scene,
                                          const RenderContext&       context,
                                          const Scene::DebugOptions& debugOptions,
                                          FrameStats&                stats,
                                          Profiler&                  profiler) override;
        void                   endFrame(const Scene&               scene,
                                        const RenderContext&       context,
                                        const Scene::DebugOptions& debugOptions,
                                        FrameStats&                stats,
                                        Profiler&                  profiler,
                                        const FrameStatsHistory&   statsHistory) override;

        void                   preTraversal(const Scene&               scene,
                                            const RenderContext&       context,
                                            const Scene::DebugOptions& debugOptions,
                                            FrameStats&                stats) override;
        void                   postTraversal(const Scene&               scene,
                                             const RenderContext&       context,
                                             const math::mat4&          view,
                                             const std::vector<Node*>&  lightNodes,
                                             const Scene::DebugOptions& debugOptions,
                                             FrameStats&                stats) override;

        void                   clear(const ClearCommand& cmd, const RenderContext& context) override;

        void                   renderPacket(DrawPacket& packet, const FrameParams& frame) override;

        std::unique_ptr<Image> snapshot(const RenderContext& context) const override;

    protected:
        /// Renderer Protected Member Functions ///

        void drawBackground(const BackgroundPass& backgroundPass,
                            const math::mat4&     view,
                            const math::mat4&     proj) override;
        void drawGround(const GroundPass& groundPass, const math::mat4& view, const math::mat4& proj) override;
        void bindPipeline(PipelineId pipelineId) override;
        void bindMaterial(const Material& material) override;
        void bindMeshElement(const MeshElement& element) override;
        void applyMVP(const math::mat4& model, const math::mat4& view, const math::mat4& proj) override;
        void drawElements() override;
        void drawLines(const LinesPass&     pass,
                       const RenderContext& context,
                       const math::mat4&    view,
                       const math::mat4&    proj) override;
        void resolvePacket(DrawPacket& packet, const FrameParams& frame) override;
        void drawPacket(const DrawPacket& packet, const FrameParams& frame) override;

    private:
        /// Private Member Functions ///

        GLSLProgram&   programForShaderKind(ShaderKind kind) const;
        void           drawDebugLines(const math::mat4& model, const math::mat4& view, const math::mat4& proj);
        void           syncImguiMemoryStats();

        /// Private Member Variables ///

        bool           _isInitialized;
        GLCapabilities _glCapabilities;
        Capabilities   _capabilities;
        unsigned       _glEnvironmentUBO;
        std::unique_ptr<GLSLProgram> _skyboxProgram;
        std::unique_ptr<GLSLProgram> _groundProgram;
        std::unique_ptr<GLSLProgram> _defaultProgram;
        std::unique_ptr<GLSLProgram> _wireframeProgram;
        std::unique_ptr<GLSLProgram> _linesProgram;
        OGLMemoryTracker             _memoryTracker;
        OGLResourceCache             _resourceCache;
        GLStateCache                 _state;
        BoundElement                 _boundElement;
        std::unique_ptr<Mesh>        _skyboxMesh; // should be value?
        gl::uint_t                   _fullscreenTriangleVao;
        OGLDebugLines                _debugLines;
        ImguiContext                 _imguiContext;
        StatsOverlay                 _statsOverlay;
        OGLDrawTimer                 _drawTimer;
    };

}

#endif // AVARA3D_RENDER_BACKEND_OPENGL_OGLRENDERER_H
