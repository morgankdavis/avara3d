//
//  StatsOverlay.h
//  avara3d
//
//  Created by Morgan Davis on 8/10/26.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_RENDER_BACKEND_OPENGL_STATSOVERLAY_H
#define AVARA3D_RENDER_BACKEND_OPENGL_STATSOVERLAY_H

#include "a3d/scene/Scene.h"

struct ImFont;

namespace a3d {

    struct FrameStats;

    class FrameStatsHistory;
    class ImguiContext;
    class RenderContext;

    /**
     * Draws A3D's built-in statistics and debug-options overlay.
     *
     * The owning ImguiContext must be started before initialize() is called,
     * and a Dear ImGui frame must be active before draw() is called.
     */
    class StatsOverlay {

    public:
        /// Internal Lifecycle Functions ///

        StatsOverlay();

        StatsOverlay(const StatsOverlay&)            = delete;
        StatsOverlay& operator=(const StatsOverlay&) = delete;

        StatsOverlay(StatsOverlay&&)            = delete;
        StatsOverlay& operator=(StatsOverlay&&) = delete;

        ~StatsOverlay();

        /// Internal Member Functions ///

        void initialize(ImguiContext& context);

        void draw(const RenderContext&     context,
                  const Scene&             scene,
                  FrameStats&              stats,
                  const FrameStatsHistory& statsHistory,
                  Scene::DebugOptions      debugOptions,
                  bool                     gpuTimingAvailable);

    private:
        /// Private Member Variables ///

        ImFont* _titleImFont;
        ImFont* _bodyImFont;
    };

}

#endif // AVARA3D_RENDER_BACKEND_OPENGL_STATSOVERLAY_H
