//
//  Panel.cc
//  avara3d
//
//  Created by Morgan Davis on 8/10/26.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#include "a3d/ui/Panel.h"

#include <algorithm>
#include <cmath>
#include <stdexcept>
#include <string>

#include <imgui/imgui.h>

#include "a3d/Assert.h"

using namespace a3d::ui;
using namespace std;

/// Private Constants ///

static constexpr float PANEL_FONT_SIZE {15.0f};
static constexpr float SHADOW_OFFSET {1.0f};
static constexpr float VALUE_GAP {12.0f};
static constexpr float SECTION_LINE_GAP {8.0f};
static constexpr float TOGGLE_LABEL_GAP {8.0f};

static constexpr int PANEL_STYLE_VAR_COUNT {7};
static constexpr int PANEL_STYLE_COLOR_COUNT {6};

/// Private Static Non-Member Prototypes ///

static float SnapPixel(float value);
static void  DrawShadowedText(const ImVec2&      position,
                              const std::string& text,
                              ImU32              color,
                              float              wrapWidth = 0.0f);

static void DrawShadowedLine(const ImVec2& start, const ImVec2& end, ImU32 color, float thickness);

/// Public Lifecycle Functions ///

Panel::Panel(string_view id, const PanelOptions& options):
    _windowName {},
    _rowItemsRemaining {0},
    _rowItemWidth {0.0f},
    _rowSpacing {0.0f},
    _visible {false} {

    if (!ImGui::GetCurrentContext()) {
        throw logic_error("Cannot create a Panel without an active UI context.");
    }

    if (id.empty()) {
        throw invalid_argument("Panel id cannot be empty.");
    }

    if (!std::isfinite(options.width) || options.width <= 0.0f) {
        throw invalid_argument("Panel width must be finite and greater than zero.");
    }

    if (!std::isfinite(options.margin) || options.margin < 0.0f) {
        throw invalid_argument("Panel margin must be finite and non-negative.");
    }

    _windowName = "##a3d_panel_";
    _windowName.append(id.data(), id.size());

    ImGuiIO& io = ImGui::GetIO();

    ImFont* panelFont = io.FontDefault;

    if (!panelFont && io.Fonts->Fonts.Size > 0) {
        panelFont = io.Fonts->Fonts[0];
    }

    if (!panelFont) {
        throw logic_error("Cannot create a Panel without an available UI font.");
    }

    ImGui::PushFont(panelFont, PANEL_FONT_SIZE);

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(6.0f, 6.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(8.0f, 4.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 2.0f);

    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.03f, 0.03f, 0.03f, 0.68f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.16f, 0.16f, 0.16f, 0.88f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.28f, 0.28f, 0.28f, 0.96f));
    ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(1.0f, 1.0f, 1.0f, 0.70f));
    ImGui::PushStyleColor(ImGuiCol_BorderShadow, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));

    ImGui::SetNextWindowPos(ImVec2(io.DisplaySize.x - options.margin, options.margin), ImGuiCond_Always,
                            ImVec2(1.0f, 0.0f));
    ImGui::SetNextWindowSize(ImVec2(options.width, 0.0f), ImGuiCond_Always);

    constexpr ImGuiWindowFlags flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove
                                       | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoNav
                                       | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoScrollbar
                                       | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_AlwaysAutoResize
                                       | ImGuiWindowFlags_NoFocusOnAppearing;

    _visible = ImGui::Begin(_windowName.c_str(), nullptr, flags);
}

Panel::~Panel() {

    A3D_ASSERT_MSG(_rowItemsRemaining == 0, "Panel destroyed with an incomplete row.");

    ImGui::End();

    ImGui::PopStyleColor(PANEL_STYLE_COLOR_COUNT);
    ImGui::PopStyleVar(PANEL_STYLE_VAR_COUNT);
    ImGui::PopFont();
}

/// Public Member Functions ///

void Panel::section(string_view text) {

    const float width = beginItem();

    if (!_visible) {
        endItem();
        return;
    }

    const string sectionText {text};
    const ImVec2 position = ImGui::GetCursorScreenPos();
    const ImVec2 textSize = ImGui::CalcTextSize(sectionText.c_str());
    const float  height = std::max(textSize.y, ImGui::GetTextLineHeight());

    ImGui::Dummy(ImVec2(width, height));

    const ImVec2 textPosition {SnapPixel(position.x), SnapPixel(position.y + (height - textSize.y) * 0.5f)};
    DrawShadowedText(textPosition, sectionText, IM_COL32(255, 255, 255, 255));

    const float lineStartX = position.x + textSize.x + SECTION_LINE_GAP;
    const float lineEndX = position.x + width;

    if (lineStartX < lineEndX) {
        const float lineY = SnapPixel(position.y + height * 0.5f);
        DrawShadowedLine(ImVec2(lineStartX, lineY), ImVec2(lineEndX, lineY), IM_COL32(255, 255, 255, 128),
                         1.0f);
    }

    endItem();
}

void Panel::text(string_view text) {

    const float width = beginItem();

    if (!_visible) {
        endItem();
        return;
    }

    const string bodyText {text};
    const ImVec2 position = ImGui::GetCursorScreenPos();
    const ImVec2 textSize = ImGui::CalcTextSize(bodyText.c_str(), nullptr, false, width);
    const float  height = textSize.y > 0.0f ? textSize.y : ImGui::GetTextLineHeight();

    ImGui::Dummy(ImVec2(width, height));
    DrawShadowedText(ImVec2(SnapPixel(position.x), SnapPixel(position.y)), bodyText,
                     IM_COL32(255, 255, 255, 255), width);

    endItem();
}

void Panel::value(string_view label, string_view value) {

    const float width = beginItem();

    if (!_visible) {
        endItem();
        return;
    }

    const string labelText {label};
    const string valueText {value};

    const ImVec2 position = ImGui::GetCursorScreenPos();
    const ImVec2 labelSize = ImGui::CalcTextSize(labelText.c_str());
    const ImVec2 valueSize = ImGui::CalcTextSize(valueText.c_str());
    const float  height = ImGui::GetTextLineHeight();
    const float  textHeight = std::max(labelSize.y, valueSize.y);

    ImGui::Dummy(ImVec2(width, height));

    const float textY = SnapPixel(position.y + (height - textHeight) * 0.5f);

    DrawShadowedText(ImVec2(SnapPixel(position.x), textY), labelText, IM_COL32(255, 255, 255, 255));

    float       valueX = position.x + width - valueSize.x;
    const float minimumValueX = position.x + labelSize.x + VALUE_GAP;

    if (valueX < minimumValueX) {
        valueX = minimumValueX;
    }

    DrawShadowedText(ImVec2(SnapPixel(valueX), textY), valueText, IM_COL32(255, 255, 255, 255));

    endItem();
}

void Panel::spacer(float height) {

    if (!std::isfinite(height) || height < 0.0f) {
        throw invalid_argument("Panel spacer height must be finite and non-negative.");
    }

    const float width = beginItem();

    if (_visible) {
        ImGui::Dummy(ImVec2(width, height));
    }

    endItem();
}

void Panel::row(unsigned itemCount) {

    if (itemCount == 0) {
        throw invalid_argument("Panel row item count must be greater than zero.");
    }

    if (_rowItemsRemaining != 0) {
        throw logic_error("Cannot begin a Panel row before the previous row is complete.");
    }

    _rowItemsRemaining = itemCount;

    if (!_visible) {
        _rowItemWidth = 0.0f;
        _rowSpacing = 0.0f;
        return;
    }

    _rowSpacing = ImGui::GetStyle().ItemSpacing.x;

    const float availableWidth = ImGui::GetContentRegionAvail().x;
    const float totalSpacing = _rowSpacing * static_cast<float>(itemCount - 1);

    _rowItemWidth = (availableWidth - totalSpacing) / static_cast<float>(itemCount);

    if (_rowItemWidth < 1.0f) {
        _rowItemWidth = 1.0f;
    }
}

bool Panel::button(string_view label) {

    if (label.empty()) {
        throw invalid_argument("Panel button label cannot be empty.");
    }

    const float width = beginItem();

    if (!_visible) {
        endItem();
        return false;
    }

    const string buttonLabel {label};
    const ImVec2 position = ImGui::GetCursorScreenPos();
    const float  height = ImGui::GetFrameHeight();
    const float  rounding = ImGui::GetStyle().FrameRounding;

    ImDrawList* drawList = ImGui::GetWindowDrawList();
    drawList->AddRectFilled(ImVec2(position.x + SHADOW_OFFSET, position.y + SHADOW_OFFSET),
                            ImVec2(position.x + width + SHADOW_OFFSET, position.y + height + SHADOW_OFFSET),
                            IM_COL32(0, 0, 0, 255), rounding);

    const bool pressed = ImGui::Button(buttonLabel.c_str(), ImVec2(width, height));

    endItem();

    return pressed;
}

bool Panel::toggle(string_view label, bool& value) {

    if (label.empty()) {
        throw invalid_argument("Panel toggle label cannot be empty.");
    }

    const float width = beginItem();

    if (!_visible) {
        endItem();
        return false;
    }

    const string toggleLabel {label};
    const ImVec2 position = ImGui::GetCursorScreenPos();
    const float  height = ImGui::GetFrameHeight();

    ImGui::PushID(toggleLabel.c_str());
    const bool pressed = ImGui::InvisibleButton("##toggle", ImVec2(width, height));
    const bool hovered = ImGui::IsItemHovered();
    const bool active = ImGui::IsItemActive();
    ImGui::PopID();

    if (pressed) {
        value = !value;
    }

    ImDrawList* drawList = ImGui::GetWindowDrawList();

    if (active) {
        drawList->AddRectFilled(position, ImVec2(position.x + width, position.y + height),
                                IM_COL32(48, 48, 48, 224), 2.0f);
    }
    else if (hovered) {
        drawList->AddRectFilled(position, ImVec2(position.x + width, position.y + height),
                                IM_COL32(24, 24, 24, 176), 2.0f);
    }

    const ImVec2 labelSize = ImGui::CalcTextSize(toggleLabel.c_str());
    const ImVec2 labelPosition {SnapPixel(position.x), SnapPixel(position.y + (height - labelSize.y) * 0.5f)};

    const float  boxSize = height;
    const ImVec2 boxMin {position.x + width - boxSize, position.y};
    const ImVec2 boxMax {boxMin.x + boxSize, boxMin.y + boxSize};
    const ImVec2 shadowOffset {SHADOW_OFFSET, SHADOW_OFFSET};

    const float labelClipMaximumX = std::max(position.x, boxMin.x - TOGGLE_LABEL_GAP);

    drawList->PushClipRect(position, ImVec2(labelClipMaximumX, position.y + height), true);
    DrawShadowedText(labelPosition, toggleLabel, IM_COL32(255, 255, 255, 255));
    drawList->PopClipRect();

    drawList->AddRectFilled(ImVec2(boxMin.x + shadowOffset.x, boxMin.y + shadowOffset.y),
                            ImVec2(boxMax.x + shadowOffset.x, boxMax.y + shadowOffset.y),
                            IM_COL32(0, 0, 0, 255), 2.0f);
    drawList->AddRect(ImVec2(boxMin.x + shadowOffset.x, boxMin.y + shadowOffset.y),
                      ImVec2(boxMax.x + shadowOffset.x, boxMax.y + shadowOffset.y), IM_COL32(0, 0, 0, 255),
                      2.0f, 0, 1.0f);

    const ImU32 boxFill = active    ? IM_COL32(72, 72, 72, 240)
                          : hovered ? IM_COL32(48, 48, 48, 224)
                                    : IM_COL32(8, 8, 8, 176);

    drawList->AddRectFilled(boxMin, boxMax, boxFill, 2.0f);
    drawList->AddRect(boxMin, boxMax, IM_COL32(255, 255, 255, 224), 2.0f, 0, 1.0f);

    if (value) {
        const float pad = boxSize * 0.22f;

        const ImVec2 p1 {boxMin.x + pad, boxMin.y + boxSize * 0.54f};
        const ImVec2 p2 {boxMin.x + boxSize * 0.43f, boxMax.y - pad};
        const ImVec2 p3 {boxMax.x - pad, boxMin.y + pad};

        drawList->AddLine(ImVec2(p1.x + SHADOW_OFFSET, p1.y + SHADOW_OFFSET),
                          ImVec2(p2.x + SHADOW_OFFSET, p2.y + SHADOW_OFFSET), IM_COL32(0, 0, 0, 255), 3.0f);
        drawList->AddLine(ImVec2(p2.x + SHADOW_OFFSET, p2.y + SHADOW_OFFSET),
                          ImVec2(p3.x + SHADOW_OFFSET, p3.y + SHADOW_OFFSET), IM_COL32(0, 0, 0, 255), 3.0f);

        drawList->AddLine(p1, p2, IM_COL32(255, 255, 255, 255), 2.0f);
        drawList->AddLine(p2, p3, IM_COL32(255, 255, 255, 255), 2.0f);
    }

    endItem();

    return pressed;
}

/// Private Member Functions ///

float Panel::beginItem() const {

    if (!_visible) {
        return 0.0f;
    }

    if (_rowItemsRemaining != 0) {
        return _rowItemWidth;
    }

    const float availableWidth = ImGui::GetContentRegionAvail().x;

    return availableWidth > 1.0f ? availableWidth : 1.0f;
}

void Panel::endItem() {

    if (_rowItemsRemaining == 0) {
        return;
    }

    --_rowItemsRemaining;

    if (_rowItemsRemaining != 0) {
        if (_visible) {
            ImGui::SameLine(0.0f, _rowSpacing);
        }

        return;
    }

    _rowItemWidth = 0.0f;
    _rowSpacing = 0.0f;
}

/// Private Static Non-Member Functions ///

float SnapPixel(float value) {

    return std::floor(value + 0.5f);
}

void DrawShadowedText(const ImVec2& position, const string& text, ImU32 color, float wrapWidth) {

    ImDrawList* drawList = ImGui::GetWindowDrawList();
    ImFont*     font = ImGui::GetFont();
    const float fontSize = ImGui::GetFontSize();

    const char* textBegin = text.c_str();
    const char* textEnd = textBegin + text.size();

    drawList->AddText(font, fontSize, ImVec2(position.x + SHADOW_OFFSET, position.y + SHADOW_OFFSET),
                      IM_COL32(0, 0, 0, 255), textBegin, textEnd, wrapWidth);
    drawList->AddText(font, fontSize, position, color, textBegin, textEnd, wrapWidth);
}

void DrawShadowedLine(const ImVec2& start, const ImVec2& end, ImU32 color, float thickness) {

    ImDrawList* drawList = ImGui::GetWindowDrawList();

    drawList->AddLine(ImVec2(start.x + SHADOW_OFFSET, start.y + SHADOW_OFFSET),
                      ImVec2(end.x + SHADOW_OFFSET, end.y + SHADOW_OFFSET), IM_COL32(0, 0, 0, 255), thickness);
    drawList->AddLine(start, end, color, thickness);
}
