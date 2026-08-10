//
//  ImguiContext.h
//  avara3d
//
//  Created by Morgan Davis on 8/10/26.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_RENDER_BACKEND_OPENGL_IMGUICONTEXT_H
#define AVARA3D_RENDER_BACKEND_OPENGL_IMGUICONTEXT_H

#include <memory>
#include <vector>

struct ImFont;
struct ImGuiContext;

namespace a3d {

    class Font;
    class RenderContext;

    /**
     * Owns the Dear ImGui context and OpenGL renderer-backend lifecycle.
     *
     * Platform-backend integration, such as the GLFW backend, remains owned by
     * the corresponding RenderContext implementation.
     */
    class ImguiContext {

    public:
        /// Internal Lifecycle Functions ///

        ImguiContext();

        ImguiContext(const ImguiContext&)            = delete;
        ImguiContext& operator=(const ImguiContext&) = delete;

        ImguiContext(ImguiContext&&)            = delete;
        ImguiContext& operator=(ImguiContext&&) = delete;

        ~ImguiContext();

        /// Internal Member Functions ///

        /**
         * Creates the Dear ImGui context and initializes its OpenGL renderer
         * backend.
         *
         * A current, initialized OpenGL context must exist before this is called.
         *
         * @throws std::logic_error if already started.
         * @throws std::runtime_error if context or backend initialization fails.
         */
        void    startup(const RenderContext& context);

        /**
         * Shuts down the OpenGL renderer backend and destroys the Dear ImGui
         * context.
         *
         * This function is idempotent.
         */
        void    shutdown();

        /**
         * Adds a font source to the shared Dear ImGui font atlas.
         *
         * ImguiContext retains ownership of the supplied Font because the atlas
         * is configured not to own the source font data.
         *
         * The returned ImFont remains valid until shutdown().
         *
         * @throws std::logic_error if not started or called during an active frame.
         * @throws std::invalid_argument if the font is null or empty.
         * @throws std::overflow_error if its data exceeds Dear ImGui's size limit.
         * @throws std::runtime_error if Dear ImGui rejects the font.
         */
        ImFont* addFont(std::unique_ptr<Font> font);

        /**
         * Updates Dear ImGui's display metrics and begins a new frame.
         *
         * The platform backend's NewFrame function must already have been called.
         *
         * @throws std::logic_error if not started or a frame is already active.
         * @throws std::runtime_error if device-object creation fails.
         */
        void    beginFrame(const RenderContext& context);

        /**
         * Finalizes and renders the current Dear ImGui frame.
         *
         * @throws std::logic_error if not started or no frame is active.
         */
        void    endFrame();

        bool    isStarted() const;

    private:
        /// Private Member Functions ///

        void                               makeCurrent() const;
        void                               updateDisplayMetrics(const RenderContext& context);
        void                               rebuildDeviceObjects();

        /// Private Member Variables ///

        ImGuiContext*                      _context;
        std::vector<std::unique_ptr<Font>> _fontSources;
        bool                               _fontAtlasDirty;
        bool                               _frameActive;
        bool                               _started;
    };

}

#endif // AVARA3D_RENDER_BACKEND_OPENGL_IMGUICONTEXT_H
