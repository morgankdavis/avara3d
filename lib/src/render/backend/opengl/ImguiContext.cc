//
//  ImguiContext.cc
//  avara3d
//
//  Created by Morgan Davis on 8/10/26.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#include "a3d/render/backend/opengl/ImguiContext.h"

#include <limits>
#include <stdexcept>
#include <string>
#include <utility>

#include "a3d/render/backend/opengl/gl.h" // must precede imgui_impl_opengl3.h

#include <imgui/imgui.h>
#include <imgui/backends/imgui_impl_opengl3.h>

#include "a3d/Buffer.h"
#include "a3d/Font.h"
#include "a3d/render/context/RenderContext.h"
#include "a3d/util/Filesystem.h"

using namespace std;

// [Private Constants]

static const std::string DEFAULT_UI_FONT {"SourceCodePro-Semibold.otf"};

namespace a3d {

// [Internal Lifecycle Functions]

ImguiContext::ImguiContext():
    _context {nullptr},
    _fontSources {},
    _defaultFont {nullptr},
    _fontAtlasDirty {false},
    _frameActive {false},
    _started {false} {}

ImguiContext::~ImguiContext() {

    shutdown();
}

// [Internal Member Functions]

void ImguiContext::startup(const RenderContext& context) {

    if (_started) {
        throw logic_error("ImguiContext is already started.");
    }

    IMGUI_CHECKVERSION();

    _context = ImGui::CreateContext();

    if (!_context) {
        throw runtime_error("Unable to create Dear ImGui context.");
    }

    makeCurrent();

    ImGuiIO& io = ImGui::GetIO();
    io.IniFilename = nullptr;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    if (!ImGui_ImplOpenGL3_Init()) {
        ImGui::DestroyContext(_context);
        _context = nullptr;

        throw runtime_error("Unable to initialize Dear ImGui OpenGL backend.");
    }

    /*
     * The context is new, but explicitly clear the atlas so its contents are
     * entirely controlled through addFont().
     *
     * Invalidate the OpenGL backend's device objects before the atlas is
     * populated. They will be rebuilt once, immediately before the first frame.
     */
    ImGui_ImplOpenGL3_DestroyDeviceObjects();
    io.Fonts->Clear();

    _fontSources.clear();
    _fontAtlasDirty = true;
    _frameActive = false;
    _started = true;

    auto defaultFont = util::fs::FontAt(DEFAULT_UI_FONT);

    if (!defaultFont || !defaultFont->buffer() || defaultFont->buffer()->size() == 0) {
        shutdown();
        throw runtime_error("Unable to load the default A3D UI font.");
    }

    _defaultFont = addFont(std::move(defaultFont));
    ImGui::GetIO().FontDefault = _defaultFont;

    updateDisplayMetrics(context);
}

void ImguiContext::shutdown() {

    if (!_started) {
        return;
    }

    makeCurrent();

    if (_frameActive) {
        ImGui::EndFrame();
        _frameActive = false;
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui::DestroyContext(_context);

    /*
     * ImGui's atlas may refer to these buffers until its context has been
     * destroyed, so release them afterward.
     */
    _fontSources.clear();

    _context = nullptr;
    _fontAtlasDirty = false;
    _defaultFont = nullptr;
    _started = false;
}

ImFont* ImguiContext::addFont(unique_ptr<Font> font) {

    if (!_started) {
        throw logic_error("Cannot add an ImGui font before startup().");
    }

    if (_frameActive) {
        throw logic_error("Cannot add an ImGui font during an active frame.");
    }

    if (!font || !font->buffer() || font->buffer()->size() == 0) {
        throw invalid_argument("Cannot add a null or empty ImGui font.");
    }

    const size_t fontDataSize = font->buffer()->size();

    if (fontDataSize > static_cast<size_t>(numeric_limits<int>::max())) {
        throw overflow_error("ImGui font data exceeds the supported size.");
    }

    makeCurrent();

    /*
     * Retain the Font before giving ImGui its buffer pointer. Moving the
     * unique_ptr does not move the Font or its Buffer allocation.
     */
    _fontSources.push_back(std::move(font));

    Font& storedFont = *_fontSources.back();

    ImFontConfig fontConfig {};
    fontConfig.FontDataOwnedByAtlas = false;
    fontConfig.OversampleH = 0;
    fontConfig.OversampleV = 0;

    /*
     * A size of zero registers the font as a source. ImGui 1.92 selects the
     * concrete rendered size through PushFont(font, size).
     */
    ImFont* imguiFont =
        ImGui::GetIO().Fonts->AddFontFromMemoryTTF(storedFont.buffer()->data(), static_cast<int>(fontDataSize),
                                                   0.0f, &fontConfig);

    if (!imguiFont) {
        _fontSources.pop_back();
        throw runtime_error("Dear ImGui rejected a font source.");
    }

    _fontAtlasDirty = true;

    return imguiFont;
}

ImFont* ImguiContext::defaultFont() const {

    if (!_started) {
        return nullptr;
    }

    makeCurrent();

    return _defaultFont;
}

void ImguiContext::beginFrame(const RenderContext& context) {

    if (!_started) {
        throw logic_error("Cannot begin an ImGui frame before startup().");
    }

    if (_frameActive) {
        throw logic_error("An ImGui frame is already active.");
    }

    makeCurrent();
    updateDisplayMetrics(context);
    rebuildDeviceObjects();

    ImGui_ImplOpenGL3_NewFrame();
    ImGui::NewFrame();

    _frameActive = true;
}

void ImguiContext::endFrame() {

    if (!_started) {
        throw logic_error("Cannot end an ImGui frame before startup().");
    }

    if (!_frameActive) {
        throw logic_error("No ImGui frame is active.");
    }

    makeCurrent();

    /*
     * The StatsOverlay currently uses a full-viewport ImGui window. Keep its
     * existing input-pass-through behavior when no actual widget is hovered or
     * active.
     */
    if (!(ImGui::IsAnyItemHovered() || ImGui::IsAnyItemActive())) {
        ImGui::GetIO().WantCaptureMouse = false;
    }

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    _frameActive = false;
}

bool ImguiContext::isStarted() const {

    return _started;
}

// [Private Member Functions]

void ImguiContext::makeCurrent() const {

    ImGui::SetCurrentContext(_context);
}

void ImguiContext::updateDisplayMetrics(const RenderContext& context) {

    makeCurrent();

    const auto viewportSize = context.viewportLogicalSize();
    const auto viewportScale = context.viewportScale();

    ImGuiIO& io = ImGui::GetIO();
    io.DisplaySize = ImVec2(static_cast<float>(viewportSize.x), static_cast<float>(viewportSize.y));
    io.DisplayFramebufferScale = ImVec2(viewportScale.x, viewportScale.y);
}

void ImguiContext::rebuildDeviceObjects() {

    if (!_fontAtlasDirty) {
        return;
    }

    makeCurrent();

    ImGui_ImplOpenGL3_DestroyDeviceObjects();

    if (!ImGui_ImplOpenGL3_CreateDeviceObjects()) {
        throw runtime_error("Unable to create Dear ImGui OpenGL device objects.");
    }

    _fontAtlasDirty = false;
}

} // namespace a3d
