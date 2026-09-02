//
//  StatsOverlay.cc
//  avara3d
//
//  Created by Morgan Davis on 8/10/26.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#include "a3d/render/backend/opengl/StatsOverlay.h"

#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <format>
#include <locale>
#include <optional>
#include <string>
#include <string_view>
#include <tuple>
#include <utility>
#include <vector>

#include <imgui/imgui.h>

#include "a3d/Buffer.h"
#include "a3d/BuildInfo.h"
#include "a3d/Font.h"
#include "a3d/Math.h"
#include "a3d/log/Log.h"
#include "a3d/profile/FrameStats.h"
#include "a3d/profile/FrameStatsHistory.h"
#include "a3d/render/backend/opengl/ImguiContext.h"
#include "a3d/render/context/RenderContext.h"
#include "a3d/util/Bitmask.h"
#include "a3d/util/Chrono.h"
#include "a3d/util/Filesystem.h"
#include "a3d/util/String.h"

using namespace a3d;
using namespace a3d::math;
using namespace std;

// [Private Constants]

static const std::string STATS_TITLE_FONT {"Neuropol Nova Xp.ttf"};
static const float       STATS_TITLE_FONT_SIZE {21.0};
static const float       STATS_BODY_FONT_SIZE {15.0};

// duration of sample history to average over
static constexpr std::chrono::milliseconds FRAME_STATS_AVERAGING_DURATION {250};

// how often to recompute the frame stats
static constexpr std::chrono::milliseconds FRAME_STATS_AVERAGE_UPDATE_INTERVAL {100};

// [Private Types]

struct ImguiStatsTextLayout {
    float xLeft;
    float xRight;
    float gap; // min gap between label and value
};

// [Private Non-Member Prototypes]

static void DrawOverlay(const RenderContext&     context,
                        const Scene&             scene,
                        FrameStats&              stats,
                        const FrameStatsHistory& statsHistory,
                        Scene::DebugOptions      debugOptions,
                        bool                     gpuTimingAvailable,
                        ImFont&                  titleFont,
                        ImFont&                  bodyFont);

static void DrawHeader(ImFont& titleFont, ImFont& bodyFont, bool active, float& yPos_out, int& id_out);

static void DrawStats(FrameStats&              stats,
                      const FrameStatsHistory& statsHistory,
                      const RenderContext&     context,
                      bool                     gpuTimingAvailable,
                      ImFont&                  bodyFont,
                      float                    yPos,
                      int                      id);

static void DrawDebugOptions(Scene& scene, ImFont& bodyFont); // TODO: remove?

static void ImguiBeginOverlay(int id, bool allowsInput);

static void ImguiEndOverlay();

static void ImguiDrawText(float       x,
                          float       y,
                          const char* text,
                          ImFont&     font,
                          float       size,
                          ImU32       color,
                          bool        shadow);

static void ImguiDrawSimulationPlot(float        x,
                                    float&       y,
                                    float        w,
                                    float        h,
                                    const float* stepValues,
                                    const float* discardMarkers,
                                    int          valuesCount,
                                    float        scaleMax,
                                    int          id,
                                    float        lineStep);

static void ImguiDrawPlot(float        x,
                          float        y,
                          float        w,
                          float        h,
                          const float* values,
                          int          valuesCount,
                          int          valuesOffset,
                          const char*  overlayText,
                          ImFont*      overlayFont,
                          float        overlayFontSize,
                          float        scaleMin,
                          float        scaleMax,
                          int          stride,
                          bool         outlined,
                          int          id,
                          bool         disabled);

static bool ImguiDrawCheckbox(float       x,
                              float       y,
                              const char* text,
                              bool&       checked,
                              ImFont&     font,
                              float       size,
                              bool        disabled,
                              int         id);

static void ImguiDrawLabelValue(float&                      y,
                                const ImguiStatsTextLayout& layout,
                                const char*                 label,
                                const std::string&          value,
                                ImFont&                     font,
                                float                       fontSize,
                                float                       lineStep,
                                ImU32                       color = IM_COL32(255, 255, 255, 255),
                                std::optional<ImU32>        valueColor = std::nullopt);

static void ImguiDrawLabelValueIndented(float&                      y,
                                        const ImguiStatsTextLayout& layout,
                                        const char*                 label,
                                        const std::string&          value,
                                        ImFont&                     font,
                                        float                       fontSize,
                                        float                       indentPx,
                                        float                       lineStep,
                                        ImU32                       color = IM_COL32(255, 255, 255, 255));

static void ImguiDrawPlot(float        x,
                          float&       y,
                          float        w,
                          float        h,
                          const float* values,
                          int          valuesCount,
                          int          valuesOffset,
                          const char*  overlayText,
                          ImFont*      overlayFont,
                          float        overlayFontSize,
                          float        scaleMin,
                          float        scaleMax,
                          int          stride,
                          bool         outlined,
                          int          id,
                          float        lineStep,
                          bool         disabled);

static string FormatMemorySize(uint64_t bytes);

// [Internal Lifecycle Functions]

StatsOverlay::StatsOverlay():
    _titleImFont {nullptr},
    _bodyImFont {nullptr} {}

StatsOverlay::~StatsOverlay() = default;

// [Internal Member Functions]

void StatsOverlay::initialize(ImguiContext& context) {

    _bodyImFont = context.defaultFont();

    if (!_bodyImFont) {
        throw runtime_error("ImguiContext has no default UI font.");
    }

    auto overlayTitleFont = util::fs::FontAt(STATS_TITLE_FONT);

    if (overlayTitleFont && overlayTitleFont->buffer() && overlayTitleFont->buffer()->size()) {
        _titleImFont = context.addFont(std::move(overlayTitleFont));
    }
    else {
        throw runtime_error(std::format("Unable to load font: {}", STATS_TITLE_FONT));
    }
}

void StatsOverlay::draw(const RenderContext&     context,
                        const Scene&             scene,
                        FrameStats&              stats,
                        const FrameStatsHistory& statsHistory,
                        Scene::DebugOptions      debugOptions,
                        bool                     gpuTimingAvailable) {

    DrawOverlay(context, scene, stats, statsHistory, debugOptions, gpuTimingAvailable, *_titleImFont,
                *_bodyImFont);
}

// [Private Non-Member Functions]

void DrawOverlay(const RenderContext&     context,
                 const Scene&             scene,
                 FrameStats&              stats,
                 const FrameStatsHistory& statsHistory,
                 Scene::DebugOptions      debugOptions,
                 bool                     gpuTimingAvailable,
                 ImFont&                  titleFont,
                 ImFont&                  bodyFont) {

    ImguiBeginOverlay(0, false);
    //DrawDebugOptions(const_cast<Scene&>(scene), bodyFont); // TODO: const_cast CHEATING

    float yPos = 0;
    int   id = 0;
    bool  showStats = util::bitmask::contains(debugOptions, Scene::DebugOptions::ShowStatsOverlay);

    DrawHeader(titleFont, bodyFont, showStats, yPos, id);

    if (showStats) {
        DrawStats(stats, statsHistory, context, gpuTimingAvailable, bodyFont, yPos, id);
    }

    ImguiEndOverlay();
}

void DrawHeader(ImFont& titleFont, ImFont& bodyFont, bool active, float& yPos_out, int& id_out) {
    using namespace ImGui;

    //ShowMetricsWindow();

    static const float X_POS = 12.0;

    float yPos = 0;
    int   id = 0;

    ImGuiIO& io = GetIO();
    auto     fonts = io.Fonts->Fonts;

    auto color = active ? IM_COL32(255, 255, 255, 255)
                        : IM_COL32(255, 255, 255, static_cast<uint8_t>(math::round(0.35f * 255.0f)));

    yPos += 4;
    ImguiDrawText(X_POS, yPos, "avara3d", titleFont, STATS_TITLE_FONT_SIZE, color, active);

    static const auto& buildInfo = BuildInfo::Info();
    static auto        version = buildInfo.version();
    static auto        buildStr = std::format("v{}.{}.{} build {}\n"
                                              "{}\n"
                                              "\n",
                                              version.major, version.minor, version.patch, buildInfo.number(),
                                              util::string::Lowercase(BuildInfo::TypeString(buildInfo.type())));
    yPos += 24;
    ImguiDrawText(X_POS, yPos, buildStr.c_str(), bodyFont, STATS_BODY_FONT_SIZE, color, active);

    yPos_out = yPos;
    id_out = id;
}

void DrawStats(FrameStats&              stats,
               const FrameStatsHistory& statsHistory,
               const RenderContext&     context,
               bool                     gpuTimingAvailable,
               ImFont&                  bodyFont,
               float                    yPos,
               int                      id) {
    using namespace ImGui;

    static const float COLUMN_WIDTH = 136.0f;
    static const float TOP_PADDING = 36.0f;
    static const float X_POS = 12.0;
    static const bool  PLOT_OUTLINED = true;
    static const float PLOT_HEIGHT_1 = 34.0;
    static const float PLOT_HEIGHT_2 = 24.0;
    static const float PLOT_STR_Y_PAD = 6.0;
    static const float PLOT_Y_PAD = 18.0;
    static const float PLOT_Y_MIN = 0.0;
    static const float PLOT_Y_MAX = 17.0;
    static const float STAT_LINE_STEP = STATS_BODY_FONT_SIZE + 0.5f;
    static const float INDENT_WIDTH = 8.0f;

    ImGuiIO& io = GetIO();
    auto     fonts = io.Fonts->Fonts;

    yPos += 4;

    static chrono::nanoseconds frameNsAvg, engineCpuNsAvg, renderCpuNsAvg, renderGpuNsAvg, physicsNsAvg,
        appCpuNsAvg;
    static float frameMsFAvg, engineCpuMsFAvg, renderCpuMsFAvg, renderGpuMsFAvg, physicsMsFAvg, appCpuMsFAvg;
    static float fpsAvg = 0;

    static auto nextAverageUpdate = chrono::steady_clock::time_point {};

    const auto now = chrono::steady_clock::now();

    if (now >= nextAverageUpdate) {
        FrameStatsHistory::GetAverages(statsHistory, frameNsAvg, engineCpuNsAvg, renderCpuNsAvg, renderGpuNsAvg,
                                       physicsNsAvg, appCpuNsAvg, FRAME_STATS_AVERAGING_DURATION);

        frameMsFAvg = util::chrono::Milliseconds(frameNsAvg);
        engineCpuMsFAvg = util::chrono::Milliseconds(engineCpuNsAvg);
        renderCpuMsFAvg = util::chrono::Milliseconds(renderCpuNsAvg);

        if (gpuTimingAvailable) {
            renderGpuMsFAvg = util::chrono::Milliseconds(renderGpuNsAvg);
        }

        physicsMsFAvg = util::chrono::Milliseconds(physicsNsAvg);
        appCpuMsFAvg = util::chrono::Milliseconds(appCpuNsAvg);

        if (frameMsFAvg > 0) {
            fpsAvg = 1000.0f / frameMsFAvg;
        }

        nextAverageUpdate = now + FRAME_STATS_AVERAGE_UPDATE_INTERVAL;
    }

    static vector<float> frameSamples;
    static vector<float> physSamples;
    static vector<float> engCpuSamples;
    static vector<float> renderCpuSamples;
    static vector<float> renderGpuSamples;
    static vector<float> appSamples;
    static vector<float> simulationStepSamples;
    static vector<float> simulationDiscardMarkers;

    static size_t         frame = 0;
    static const unsigned SKIP_FRAMES = 2;
    if (!((frame++) % SKIP_FRAMES)) {
        auto& samples = statsHistory.samples();

        frameSamples.resize(samples.size());
        physSamples.resize(samples.size());
        engCpuSamples.resize(samples.size());
        renderCpuSamples.resize(samples.size());
        if (gpuTimingAvailable) {
            renderGpuSamples.resize(samples.size());
        }
        appSamples.resize(samples.size());
        simulationStepSamples.resize(samples.size());
        simulationDiscardMarkers.resize(samples.size());

        for (size_t i = 0; i < samples.size(); ++i) {
            auto sample = get<1>(samples[i]);
            frameSamples[i] = util::chrono::Milliseconds(sample.frameTime);
            engCpuSamples[i] = util::chrono::Milliseconds(sample.engineCpuTime);
            physSamples[i] = util::chrono::Milliseconds(sample.physicsTime);
            renderCpuSamples[i] = util::chrono::Milliseconds(sample.renderCpuTime);
            if (gpuTimingAvailable) {
                renderGpuSamples[i] = util::chrono::Milliseconds(sample.renderGpuTime);
            }
            appSamples[i] = util::chrono::Milliseconds(sample.applicationTime);
            simulationStepSamples[i] = static_cast<float>(sample.simulationStepsThisUpdate);
            simulationDiscardMarkers[i] = sample.discardedSimulationTime > 0.0 ? 1.0f : 0.0f;
        }
    }

    ImguiStatsTextLayout     layout {.xLeft = X_POS, .xRight = X_POS + COLUMN_WIDTH, .gap = 12.0f};
    static const std::locale numberLocale("en_US.UTF-8");

    yPos += TOP_PADDING;
    auto rateValue = std::format(numberLocale, "{:.0Lf}fps", fpsAvg);
    ImguiDrawLabelValue(yPos, layout, "", rateValue, bodyFont, STATS_BODY_FONT_SIZE, 15);
    auto frameValue = std::format("{:.1f}ms", frameMsFAvg);
    ImguiDrawLabelValue(yPos, layout, "frame", frameValue, bodyFont, STATS_BODY_FONT_SIZE, PLOT_Y_PAD);
    ImguiDrawPlot(X_POS, yPos, COLUMN_WIDTH, PLOT_HEIGHT_1, frameSamples.data(),
                  static_cast<int>(frameSamples.size()), 0, nullptr, nullptr, 0.0f, PLOT_Y_MIN, PLOT_Y_MAX, 0,
                  PLOT_OUTLINED, ++id, PLOT_HEIGHT_1 + PLOT_STR_Y_PAD, false);

    const bool discardedSteps = stats.discardedSimulationTime > 0.0;
    const auto discardedStepCount =
        discardedSteps && stats.simulationTimeStep > 0.0
            ? static_cast<std::uint64_t>(std::llround(stats.discardedSimulationTime / stats.simulationTimeStep))
            : 0;
    ImguiDrawLabelValue(yPos, layout, "simulation",
                        discardedSteps ? std::format("-{} steps", discardedStepCount)
                                       : std::format("{} steps", stats.simulationStepsThisUpdate),
                        bodyFont, STATS_BODY_FONT_SIZE, PLOT_Y_PAD, IM_COL32(255, 255, 255, 255),
                        discardedSteps ? IM_COL32(255, 48, 48, 255) : IM_COL32(255, 255, 255, 255));
    ImguiDrawSimulationPlot(X_POS, yPos, COLUMN_WIDTH, PLOT_HEIGHT_2, simulationStepSamples.data(),
                            simulationDiscardMarkers.data(), static_cast<int>(simulationStepSamples.size()),
                            math::max(1.0f, static_cast<float>(stats.maxCatchUpSteps)), ++id,
                            PLOT_HEIGHT_2 + PLOT_STR_Y_PAD);

    const auto formatDiscardedTime = [](double seconds) {
        return seconds >= 1.0 ? std::format("{:.2f}s", seconds) : std::format("{:.1f}ms", seconds * 1000.0);
    };
    ImguiDrawLabelValue(yPos, layout, "discarded", formatDiscardedTime(stats.totalDiscardedSimulationTime),
                        bodyFont, STATS_BODY_FONT_SIZE, STAT_LINE_STEP, IM_COL32(255, 255, 255, 255),
                        stats.totalDiscardedSimulationTime > 0.0 ? IM_COL32(255, 48, 48, 255)
                                                                 : IM_COL32(255, 255, 255, 255));

    ImguiDrawLabelValue(yPos, layout, "step #", std::format(numberLocale, "{:L}", stats.simulationStepCount),
                        bodyFont, STATS_BODY_FONT_SIZE, STAT_LINE_STEP);

    ImguiDrawLabelValue(yPos, layout, "sim time", std::format("{:.2f}s", stats.simulationTime), bodyFont,
                        STATS_BODY_FONT_SIZE, STAT_LINE_STEP);

    ImguiDrawLabelValue(yPos, layout, "time step", std::format("1/{:.0f}s", 1.0 / stats.simulationTimeStep),
                        bodyFont, STATS_BODY_FONT_SIZE, STAT_LINE_STEP + PLOT_STR_Y_PAD);

    ImguiDrawLabelValue(yPos, layout, "engine cpu", std::format("{:.1f}ms", engineCpuMsFAvg), bodyFont,
                        STATS_BODY_FONT_SIZE, PLOT_Y_PAD);
    ImguiDrawPlot(X_POS, yPos, COLUMN_WIDTH, PLOT_HEIGHT_2, engCpuSamples.data(),
                  static_cast<int>(engCpuSamples.size()), 0, nullptr, nullptr, 0.0f, PLOT_Y_MIN, PLOT_Y_MAX, 0,
                  PLOT_OUTLINED, ++id, PLOT_HEIGHT_2 + PLOT_STR_Y_PAD, false);

    ImguiDrawLabelValue(yPos, layout, "render sub", std::format("{:.1f}ms", renderCpuMsFAvg), bodyFont,
                        STATS_BODY_FONT_SIZE, PLOT_Y_PAD);
    ImguiDrawPlot(X_POS, yPos, COLUMN_WIDTH, PLOT_HEIGHT_2, renderCpuSamples.data(),
                  static_cast<int>(renderCpuSamples.size()), 0, nullptr, nullptr, 0.0f, PLOT_Y_MIN, PLOT_Y_MAX,
                  0, PLOT_OUTLINED, ++id, PLOT_HEIGHT_2 + PLOT_STR_Y_PAD, false);

    ImguiDrawLabelValue(yPos, layout, "draw",
                        (gpuTimingAvailable ? std::format("{:.1f}ms", renderGpuMsFAvg) : ""), bodyFont,
                        STATS_BODY_FONT_SIZE, PLOT_Y_PAD,
                        gpuTimingAvailable ? IM_COL32(255, 255, 255, 255) : IM_COL32(128, 128, 128, 255));
    ImguiDrawPlot(X_POS, yPos, COLUMN_WIDTH, PLOT_HEIGHT_2, renderGpuSamples.data(),
                  static_cast<int>(renderGpuSamples.size()), 0, gpuTimingAvailable ? nullptr : "unavailable",
                  &bodyFont, STATS_BODY_FONT_SIZE, PLOT_Y_MIN, PLOT_Y_MAX, 0, PLOT_OUTLINED, ++id,
                  PLOT_HEIGHT_2 + PLOT_STR_Y_PAD, !gpuTimingAvailable);

    ImguiDrawLabelValue(yPos, layout, "physics", std::format("{:.1f}ms", physicsMsFAvg), bodyFont,
                        STATS_BODY_FONT_SIZE, PLOT_Y_PAD);
    ImguiDrawPlot(X_POS, yPos, COLUMN_WIDTH, PLOT_HEIGHT_2, physSamples.data(),
                  static_cast<int>(physSamples.size()), 0, nullptr, nullptr, 0.0f, PLOT_Y_MIN, PLOT_Y_MAX, 0,
                  PLOT_OUTLINED, ++id, PLOT_HEIGHT_2 + PLOT_STR_Y_PAD, false);

    ImguiDrawLabelValue(yPos, layout, "application", std::format("{:.1f}ms", appCpuMsFAvg), bodyFont,
                        STATS_BODY_FONT_SIZE, PLOT_Y_PAD);
    ImguiDrawPlot(X_POS, yPos, COLUMN_WIDTH, PLOT_HEIGHT_2, appSamples.data(),
                  static_cast<int>(appSamples.size()), 0, nullptr, nullptr, 0.0f, PLOT_Y_MIN, PLOT_Y_MAX, 0,
                  PLOT_OUTLINED, ++id, 0, false);

    yPos += 34;

    ImguiStatsTextLayout bulkLayout = layout;

    ImguiDrawLabelValue(yPos, bulkLayout, "nodes", std::format("{}", stats.nodes), bodyFont,
                        STATS_BODY_FONT_SIZE, STAT_LINE_STEP);
    ImguiDrawLabelValue(yPos, bulkLayout, "meshes", std::format("{}", stats.meshes), bodyFont,
                        STATS_BODY_FONT_SIZE, STAT_LINE_STEP);
    // ImguiDrawLabelValue(yPos, bulkLayout, "elements", std::format("{}", stats.elements), bodyFont,
    //                     STATS_BODY_FONT_SIZE, STAT_LINE_STEP);
    ImguiDrawLabelValue(yPos, bulkLayout, "polygons", std::format("{:.1f}k", float(stats.polygons) / 1000.0f),
                        bodyFont, STATS_BODY_FONT_SIZE, STAT_LINE_STEP);
    ImguiDrawLabelValue(yPos, bulkLayout, "lights", std::format("{}", stats.lights), bodyFont,
                        STATS_BODY_FONT_SIZE, STAT_LINE_STEP);
    ImguiDrawLabelValue(yPos, bulkLayout, "gpu memory", FormatMemorySize(stats.renderMemory.totalBytes()),
                        bodyFont, STATS_BODY_FONT_SIZE, STAT_LINE_STEP);

    yPos += STAT_LINE_STEP / 2.0f;

    ImguiDrawLabelValue(yPos, bulkLayout, "phys bodies",
                        std::format("{}", stats.dynamicBodies + stats.kinematicBodies + stats.staticBodies),
                        bodyFont, STATS_BODY_FONT_SIZE, STAT_LINE_STEP);
    ImguiDrawLabelValueIndented(yPos, bulkLayout, "static", std::format("{}", stats.staticBodies), bodyFont,
                                STATS_BODY_FONT_SIZE, INDENT_WIDTH, STAT_LINE_STEP);
    ImguiDrawLabelValueIndented(yPos, bulkLayout, "dynamic", std::format("{}", stats.dynamicBodies), bodyFont,
                                STATS_BODY_FONT_SIZE, INDENT_WIDTH, STAT_LINE_STEP);
    ImguiDrawLabelValueIndented(yPos, bulkLayout, "kinematic", std::format("{}", stats.kinematicBodies),
                                bodyFont, STATS_BODY_FONT_SIZE, INDENT_WIDTH, STAT_LINE_STEP);

    yPos += STAT_LINE_STEP / 2.0f;

    ImguiDrawLabelValue(yPos, bulkLayout, "phys shapes",
                        std::format("{}", stats.primitiveShapes + stats.concavePolyhedronShapes
                                              + stats.boundingBoxShapes + stats.convexHullShapes),
                        bodyFont, STATS_BODY_FONT_SIZE, STAT_LINE_STEP);
    ImguiDrawLabelValueIndented(yPos, bulkLayout, "primitive", std::format("{}", stats.primitiveShapes),
                                bodyFont, STATS_BODY_FONT_SIZE, INDENT_WIDTH, STAT_LINE_STEP);
    ImguiDrawLabelValueIndented(yPos, bulkLayout, "bbox", std::format("{}", stats.boundingBoxShapes), bodyFont,
                                STATS_BODY_FONT_SIZE, INDENT_WIDTH, STAT_LINE_STEP);
    ImguiDrawLabelValueIndented(yPos, bulkLayout, "convex", std::format("{}", stats.convexHullShapes), bodyFont,
                                STATS_BODY_FONT_SIZE, INDENT_WIDTH, STAT_LINE_STEP);
    ImguiDrawLabelValueIndented(yPos, bulkLayout, "concave", std::format("{}", stats.concavePolyhedronShapes),
                                bodyFont, STATS_BODY_FONT_SIZE, INDENT_WIDTH, STAT_LINE_STEP);

    yPos += STAT_LINE_STEP / 2.0f;

    ImguiDrawLabelValue(yPos, bulkLayout, "contacts", std::format("{}", stats.activeContacts), bodyFont,
                        STATS_BODY_FONT_SIZE, STAT_LINE_STEP);
}

void DrawDebugOptions(Scene& scene, ImFont& bodyFont) {
    using namespace ImGui;

    ImGuiIO& io = GetIO();

    const float        WIN_WIDTH = 168;
    const float        xPos = io.DisplaySize.x - WIN_WIDTH;
    float              yPos = 0;
    int                id = 0;
    static const float Y_PAD = 24.0;

    auto debugOptions = scene.debugOptions();

    using DebugOptions = Scene::DebugOptions;

#if defined(A3D_GL_DESKTOP)
    static constexpr bool kSupportsPolygonModeWireframes = true;
#else
    static constexpr bool kSupportsPolygonModeWireframes = false;
#endif

    yPos = 12.0;
    static bool stats = util::bitmask::contains(debugOptions, DebugOptions::ShowStatsOverlay);
    if (ImguiDrawCheckbox(xPos, yPos, "stats", stats, bodyFont, STATS_BODY_FONT_SIZE, false, ++id)) {
        if (stats) {
            scene.debugOptions(util::bitmask::add(debugOptions, DebugOptions::ShowStatsOverlay));
        }
        else {
            scene.debugOptions(util::bitmask::remove(debugOptions, DebugOptions::ShowStatsOverlay));
        }
    }

    yPos += Y_PAD;
    bool meshWF = util::bitmask::contains(debugOptions, DebugOptions::ShowMeshWireframes);
    if (ImguiDrawCheckbox(xPos, yPos, "mesh wireframes", meshWF, bodyFont, STATS_BODY_FONT_SIZE,
                          !kSupportsPolygonModeWireframes, ++id)) {
        if (meshWF) {
            scene.debugOptions(util::bitmask::add(debugOptions, DebugOptions::ShowMeshWireframes));
        }
        else {
            scene.debugOptions(util::bitmask::remove(debugOptions, DebugOptions::ShowMeshWireframes));
        }
    }

    yPos += Y_PAD;
    bool meshAABBs = util::bitmask::contains(scene.debugOptions(), DebugOptions::ShowMeshBounds);
    if (ImguiDrawCheckbox(xPos, yPos, "mesh AABBs", meshAABBs, bodyFont, STATS_BODY_FONT_SIZE, false, ++id)) {
        if (meshAABBs) {
            scene.debugOptions(util::bitmask::add(debugOptions, DebugOptions::ShowMeshBounds));
        }
        else {
            scene.debugOptions(util::bitmask::remove(debugOptions, DebugOptions::ShowMeshBounds));
        }
    }

    yPos += Y_PAD;
    bool physWF = util::bitmask::contains(scene.debugOptions(), DebugOptions::ShowPhysicsWireframes);
    if (ImguiDrawCheckbox(xPos, yPos, "physics wireframes", physWF, bodyFont, STATS_BODY_FONT_SIZE, false,
                          ++id)) {
        if (physWF) {
            scene.debugOptions(util::bitmask::add(debugOptions, DebugOptions::ShowPhysicsWireframes));
        }
        else {
            scene.debugOptions(util::bitmask::remove(debugOptions, DebugOptions::ShowPhysicsWireframes));
        }
    }

    yPos += Y_PAD;
    bool physAABBs = util::bitmask::contains(scene.debugOptions(), DebugOptions::ShowPhysicsBounds);
    if (ImguiDrawCheckbox(xPos, yPos, "physics AABBs", physAABBs, bodyFont, STATS_BODY_FONT_SIZE, false,
                          ++id)) {
        if (physAABBs) {
            scene.debugOptions(util::bitmask::add(debugOptions, DebugOptions::ShowPhysicsBounds));
        }
        else {
            scene.debugOptions(util::bitmask::remove(debugOptions, DebugOptions::ShowPhysicsBounds));
        }
    }

    yPos += Y_PAD;
    bool physContacts = util::bitmask::contains(scene.debugOptions(), DebugOptions::ShowPhysicsContactPoints);
    if (ImguiDrawCheckbox(xPos, yPos, "physics contacts", physContacts, bodyFont, STATS_BODY_FONT_SIZE, false,
                          ++id)) {
        if (physContacts) {
            scene.debugOptions(util::bitmask::add(debugOptions, DebugOptions::ShowPhysicsContactPoints));
        }
        else {
            scene.debugOptions(util::bitmask::remove(debugOptions, DebugOptions::ShowPhysicsContactPoints));
        }
    }

    yPos += Y_PAD;
    bool physNorms = util::bitmask::contains(scene.debugOptions(), DebugOptions::ShowPhysicsNormals);
    if (ImguiDrawCheckbox(xPos, yPos, "physics normals", physNorms, bodyFont, STATS_BODY_FONT_SIZE, false,
                          ++id)) {
        if (physNorms) {
            scene.debugOptions(util::bitmask::add(debugOptions, DebugOptions::ShowPhysicsNormals));
        }
        else {
            scene.debugOptions(util::bitmask::remove(debugOptions, DebugOptions::ShowPhysicsNormals));
        }
    }
}

void ImguiBeginOverlay(int id, bool allowsInput) {
    using namespace ImGui;

    ImGuiIO&         io = GetIO();
    ImGuiWindowFlags flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove
                             | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoNav
                             | ImGuiWindowFlags_NoBackground;

    if (!allowsInput) {
        flags |= ImGuiWindowFlags_NoInputs;
    }

    SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);
    SetNextWindowSize(io.DisplaySize, ImGuiCond_Always);
    Begin(("##overlay" + std::to_string(id)).c_str(), nullptr, flags);
}

void ImguiEndOverlay() {

    ImGui::End();
}

void ImguiDrawText(float x, float y, const char* text, ImFont& font, float size, ImU32 color, bool shadow) {
    using namespace ImGui;

    PushFont(&font, size);
    ImDrawList* dl = GetForegroundDrawList();

    if (shadow) {
        dl->AddText(ImVec2(x + 1, y + 1), IM_COL32(0, 0, 0, 255), text);
    }
    dl->AddText(ImVec2(x, y), color, text);

    PopFont();
}

void ImguiDrawSimulationPlot(float        x,
                             float&       y,
                             float        w,
                             float        h,
                             const float* stepValues,
                             const float* discardMarkers,
                             int          valuesCount,
                             float        scaleMax,
                             int          id,
                             float        lineStep) {
    using namespace ImGui;

    const ImVec4 discardColor(1, 0, 0, 0.75f);

    PushStyleColor(ImGuiCol_FrameBg, ImVec4(0, 0, 0, 0));

    // Black line shadow.
    SetCursorScreenPos(ImVec2(x + 1, y + 1));
    SetNextItemAllowOverlap();
    PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0.0f);
    PushStyleColor(ImGuiCol_PlotLines, ImVec4(0, 0, 0, 1));
    PushStyleColor(ImGuiCol_PlotLinesHovered, ImVec4(0, 0, 0, 1));
    PlotLines(("##simulation_steps_shadow" + std::to_string(id)).c_str(), stepValues, valuesCount, 0, nullptr,
              0.0f, scaleMax, ImVec2(w, h), sizeof(float));
    PopStyleColor(2);
    PopStyleVar();

    // White step line and plot outline.
    SetCursorScreenPos(ImVec2(x, y));
    SetNextItemAllowOverlap();
    PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0.5f);
    PushStyleColor(ImGuiCol_Border, ImVec4(1, 1, 1, 0.5f));
    PushStyleColor(ImGuiCol_PlotLines, ImVec4(1, 1, 1, 1));
    PushStyleColor(ImGuiCol_PlotLinesHovered, ImVec4(1, 1, 1, 1));
    PlotLines(("##simulation_steps" + std::to_string(id)).c_str(), stepValues, valuesCount, 0, nullptr, 0.0f,
              scaleMax, ImVec2(w, h), sizeof(float));
    PopStyleColor(3);
    PopStyleVar();

    // Red discard markers, drawn last so they appear over the step line.
    SetCursorScreenPos(ImVec2(x, y));
    PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0.0f);
    PushStyleColor(ImGuiCol_PlotHistogram, discardColor);
    PushStyleColor(ImGuiCol_PlotHistogramHovered, discardColor);
    PlotHistogram(("##simulation_discard" + std::to_string(id)).c_str(), discardMarkers, valuesCount, 0,
                  nullptr, 0.0f, 1.0f, ImVec2(w, h), sizeof(float));
    PopStyleColor(2);
    PopStyleVar();

    PopStyleColor();

    y += lineStep;
}

void ImguiDrawPlot(float        x,
                   float        y,
                   float        w,
                   float        h,
                   const float* values,
                   int          valuesCount,
                   int          valuesOffset,
                   const char*  overlayText,
                   ImFont*      overlayFont,
                   float        overlayFontSize,
                   float        scaleMin,
                   float        scaleMax,
                   int          stride,
                   bool         outlined,
                   int          id,
                   bool         disabled) {
    using namespace ImGui;

    BeginDisabled(disabled);

    PushStyleColor(ImGuiCol_FrameBg, ImVec4(0, 0, 0, 0));

    const int plotStride = stride > 0 ? stride : sizeof(float);

    // shadow pass
    SetCursorScreenPos(ImVec2(x + 1, y + 1));
    PushStyleColor(ImGuiCol_PlotLines, ImVec4(0, 0, 0, 1));
    PlotLines(("##plot_s" + std::to_string(id)).c_str(), values, valuesCount, valuesOffset, nullptr, scaleMin,
              scaleMax, ImVec2(w, h), plotStride);
    PopStyleColor();

    // main pass
    SetCursorScreenPos(ImVec2(x, y));
    PushStyleColor(ImGuiCol_PlotLines, ImVec4(1, 1, 1, 1));
    if (outlined) {
        PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0.5f);
        PushStyleColor(ImGuiCol_Border, ImVec4(1, 1, 1, 0.5f));
    }
    PlotLines(("##plot" + std::to_string(id)).c_str(), values, valuesCount, valuesOffset, nullptr, scaleMin,
              scaleMax, ImVec2(w, h), plotStride);

    // overlay text
    if (overlayText != nullptr && overlayText[0] != '\0') {
        ImFont* font = overlayFont != nullptr ? overlayFont : GetFont();

        ImDrawList* drawList = GetWindowDrawList();

        const ImVec2 textSize = font->CalcTextSizeA(overlayFontSize, math::F32_MAX, 0.0f, overlayText);

        const float textX = x + (w - textSize.x) * 0.5f;
        const float textY = y + (h - textSize.y) * 0.5f - 1.0f;

        drawList->AddText(font, overlayFontSize, ImVec2(math::floor(textX + 0.5f), math::floor(textY + 0.5f)),
                          IM_COL32(128, 128, 128, 255), overlayText);
    }

    if (outlined) {
        PopStyleColor();
        PopStyleVar();
    }
    PopStyleColor();

    PopStyleColor();

    EndDisabled();
}

bool ImguiDrawCheckbox(float       x,
                       float       y,
                       const char* text,
                       bool&       checked,
                       ImFont&     font,
                       float       size,
                       bool        disabled,
                       int         id) {
    using namespace ImGui;

    ImGui::PushFont(&font, size);

    const ImVec4 transparent(0, 0, 0, 0);
    const float  shadowOff = 1.0f;
    const float  borderThickness = 1.0f;

    const float boxSize = GetFrameHeight(); // checkbox square size
    const float labelGap = GetStyle().ItemInnerSpacing.x; // spacing between box and label

    // shadow checkbox (non-interactive, non-blocking)
    PushID(id);
    SetCursorScreenPos(ImVec2(x + shadowOff, y + shadowOff));
    BeginDisabled(true);

    PushStyleVar(ImGuiStyleVar_FrameBorderSize, borderThickness);
    PushStyleColor(ImGuiCol_FrameBg, transparent);
    PushStyleColor(ImGuiCol_FrameBgHovered, transparent);
    PushStyleColor(ImGuiCol_FrameBgActive, transparent);
    PushStyleColor(ImGuiCol_Border, ImVec4(0, 0, 0, 1));
    PushStyleColor(ImGuiCol_BorderShadow, transparent);
    PushStyleColor(ImGuiCol_CheckMark, ImVec4(0, 0, 0, 1));

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
    if (disabled) {
        BeginDisabled(true);
    }

    auto primaryColor = disabled ? ImVec4(0.65, 0.65, 0.65, 1) : ImVec4(1, 1, 1, 1);

    PushStyleVar(ImGuiStyleVar_FrameBorderSize, borderThickness);
    PushStyleColor(ImGuiCol_FrameBg, transparent);
    PushStyleColor(ImGuiCol_FrameBgHovered, transparent);
    PushStyleColor(ImGuiCol_FrameBgActive, transparent);
    PushStyleColor(ImGuiCol_Border, primaryColor);
    PushStyleColor(ImGuiCol_BorderShadow, transparent);
    PushStyleColor(ImGuiCol_CheckMark, primaryColor);

    ret = Checkbox("##real", &checked);

    PopStyleColor(6);
    PopStyleVar();
    if (disabled) {
        EndDisabled();
    }
    PopID();

    // raw label ourselves (true solid shadow, like DigDrawText)
    ImDrawList* dl = GetWindowDrawList(); // or GetForegroundDrawList() to match DigDrawText layer exactly

    // align label vertically with checkbox frame (center-ish)
    float textY = y + GetStyle().FramePadding.y;

    ImVec2 labelPos(x + boxSize + labelGap, textY);

    dl->AddText(ImVec2(labelPos.x + shadowOff, labelPos.y + shadowOff), IM_COL32(0, 0, 0, 255), text);
    dl->AddText(labelPos, disabled ? IM_COL32(128, 128, 128, 255) : IM_COL32(255, 255, 255, 255), text);

    PopFont();

    return ret;
}

void ImguiDrawLabelValue(float&                      y,
                         const ImguiStatsTextLayout& layout,
                         const char*                 label,
                         const std::string&          value,
                         ImFont&                     font,
                         float                       fontSize,
                         float                       lineStep,
                         ImU32                       color,
                         std::optional<ImU32>        valueColor) {
    const auto SnapPx = [](float x) -> float {
        return math::floor(x + 0.5f);
    };

    const auto TextSizeA = [](ImFont& f, float size, const char* text) -> ImVec2 {
        const float wrapWidth = 0.0f;
        return f.CalcTextSizeA(size, math::F32_MAX, wrapWidth, text);
    };

    // draw label
    ImguiDrawText(SnapPx(layout.xLeft), y, label, font, fontSize, color, true);

    // measure at SAME font+size you draw with
    const ImVec2 labelSz = TextSizeA(font, fontSize, label);
    const ImVec2 valueSz = TextSizeA(font, fontSize, value.c_str());

    float       xValue = layout.xRight - valueSz.x;
    const float minXValue = layout.xLeft + labelSz.x + layout.gap;
    if (xValue < minXValue) {
        xValue = minXValue;
    }

    // pixel snap
    xValue = SnapPx(xValue);

    //ImguiDrawText(xValue, y, value.c_str(), font, fontSize, color, true);
    ImguiDrawText(xValue, y, value.c_str(), font, fontSize, valueColor.value_or(color), true);

    y += lineStep;
}

void ImguiDrawLabelValueIndented(float&                      y,
                                 const ImguiStatsTextLayout& layout,
                                 const char*                 label,
                                 const std::string&          value,
                                 ImFont&                     font,
                                 float                       fontSize,
                                 float                       indentPx,
                                 float                       lineStep,
                                 ImU32                       color) {
    ImguiStatsTextLayout l = layout;
    l.xLeft += indentPx;
    ImguiDrawLabelValue(y, l, label, value, font, fontSize, 0, color);
    y += lineStep;
}

void ImguiDrawPlot(float        x,
                   float&       y,
                   float        w,
                   float        h,
                   const float* values,
                   int          valuesCount,
                   int          valuesOffset,
                   const char*  overlayText,
                   ImFont*      overlayFont,
                   float        overlayFontSize,
                   float        scaleMin,
                   float        scaleMax,
                   int          stride,
                   bool         outlined,
                   int          id,
                   float        lineStep,
                   bool         disabled) {
    ImguiDrawPlot(x, y, w, h, values, valuesCount, valuesOffset, overlayText, overlayFont, overlayFontSize,
                  scaleMin, scaleMax, stride, outlined, id, disabled);
    y += lineStep;
}

string FormatMemorySize(uint64_t bytes) {

    constexpr double KB = 1024.0;
    constexpr double MB = KB * 1024.0;
    constexpr double GB = MB * 1024.0;

    if (bytes >= GB) {
        return std::format("{:.1f}GB", static_cast<double>(bytes) / GB);
    }
    if (bytes >= MB) {
        return std::format("{:.1f}MB", static_cast<double>(bytes) / MB);
    }
    if (bytes >= KB) {
        return std::format("{:.1f}KB", static_cast<double>(bytes) / KB);
    }

    return std::format("{}B", bytes);
}
