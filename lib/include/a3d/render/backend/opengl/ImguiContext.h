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

    class ImguiContext {

    public:
        // [Internal Lifecycle Functions]

        ImguiContext();

        ImguiContext(const ImguiContext&)            = delete;
        ImguiContext& operator=(const ImguiContext&) = delete;

        ImguiContext(ImguiContext&&)            = delete;
        ImguiContext& operator=(ImguiContext&&) = delete;

        ~ImguiContext();

        // [Internal Member Functions]


        void    startup(const RenderContext& context);
        void    shutdown();

        ImFont* addFont(std::unique_ptr<Font> font);
        ImFont* defaultFont() const;

        void    beginFrame(const RenderContext& context);
        void    endFrame();

        bool    isStarted() const;

    private:
        // [Private Member Functions]

        void                               makeCurrent() const;
        void                               updateDisplayMetrics(const RenderContext& context);
        void                               rebuildDeviceObjects();

        // [Private Member Variables]

        ImGuiContext*                      _context;
        std::vector<std::unique_ptr<Font>> _fontSources;
        ImFont*                            _defaultFont;
        bool                               _fontAtlasDirty;
        bool                               _frameActive;
        bool                               _started;
    };

}

#endif // AVARA3D_RENDER_BACKEND_OPENGL_IMGUICONTEXT_H
