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
#include <cstdio>
#include <stdexcept>
#include <string>

#include <imgui/imgui.h>

#include "a3d/Assert.h"
#include "a3d/Math.h"
#include "a3d/util/String.h"

using namespace a3d::ui;
using namespace std;

// [Private Constants]

static constexpr float PANEL_FONT_SIZE {15.0f};
static constexpr float SHADOW_OFFSET {1.0f};
static constexpr float VALUE_GAP {12.0f};
static constexpr float SECTION_LINE_GAP {4.0f};
static constexpr float TOGGLE_BOX_SIZE {16.0f};
static constexpr float TOGGLE_LABEL_GAP {8.0f};
static constexpr float SLIDER_GRAB_WIDTH {8.0f};

static constexpr int PANEL_STYLE_VAR_COUNT {7};
static constexpr int PANEL_STYLE_COLOR_COUNT {6};

// [Private Non-Member Prototypes]

static float SnapPixel(float value);
static void  DrawShadowedText(const ImVec2&      position,
                              const std::string& text,
                              ImU32              color,
                              float              wrapWidth = 0.0f);

static void DrawShadowedLine(const ImVec2& start, const ImVec2& end, ImU32 color, float thickness);

// [Public Lifecycle Functions]

Panel::Panel(string_view id, const Options& options):
    _windowName {},
    _rowItemsRemaining {0},
    _rowItemCount {0},
    _rowItemWidth {0.0f},
    _rowSpacing {0.0f},
    _segmentedRow {false},
    _visible {false},
    _hovered {false} {

    if (!ImGui::GetCurrentContext()) {
        throw logic_error("Cannot create a Panel without an active UI context.");
    }

    if (id.empty()) {
        throw invalid_argument("Panel id cannot be empty.");
    }

    if (options.width <= 0.0f) {
        throw invalid_argument("Panel width must be greater than zero.");
    }

    if (options.margin < 0.0f) {
        throw invalid_argument("Panel margin must be non-negative.");
    }

    _windowName = "##a3d_panel_";
    _windowName.append(id.data(), id.size());

    ImGuiIO& io = ImGui::GetIO();

    ImFont* panelFont = io.FontDefault;

    if (!panelFont) {
        throw logic_error("Cannot create a Panel without the default A3D UI font.");
    }

    static const float BUTTON_Y_PAD = 1.0f;

    ImGui::PushFont(panelFont, PANEL_FONT_SIZE);

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(6.0f, 2.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(8.0f, BUTTON_Y_PAD));
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
    _hovered = ImGui::IsWindowHovered(ImGuiHoveredFlags_RootAndChildWindows);
}

Panel::~Panel() {

    A3D_ASSERT_MSG(_rowItemsRemaining == 0, "Panel destroyed with an incomplete row.");

    ImGui::End();

    ImGui::PopStyleColor(PANEL_STYLE_COLOR_COUNT);
    ImGui::PopStyleVar(PANEL_STYLE_VAR_COUNT);
    ImGui::PopFont();
}

// [Public Member Functions]

void Panel::section(string_view text, SectionConfig config, Padding padding) {

    const float width = beginItem();

    if (!_visible) {
        endItem();
        return;
    }

    const string sectionText = config.uppercase ? util::string::Uppercase(text) : string {text};
    const ImVec2 position = ImGui::GetCursorScreenPos();
    const float  contentWidth = a3d::math::max(1.0f, width - padding.left - padding.right);
    const ImVec2 contentPosition {
        position.x + padding.left,
        position.y + padding.top,
    };
    const ImVec2 textSize = ImGui::CalcTextSize(sectionText.c_str());

    const float textHeight = a3d::math::max(textSize.y, ImGui::GetTextLineHeight());
    const float lineSpace = config.line ? SECTION_LINE_GAP + 1.0f : 0.0f;

    const float height = padding.top + textHeight + lineSpace + padding.bottom;

    ImGui::Dummy(ImVec2(width, height));

    const ImVec2 textPosition {
        SnapPixel(contentPosition.x),
        SnapPixel(contentPosition.y + (textHeight - textSize.y) * 0.5f),
    };

    DrawShadowedText(textPosition, sectionText, IM_COL32(255, 255, 255, 255));

    if (config.line) {

        const float lineY = SnapPixel(contentPosition.y + textHeight + SECTION_LINE_GAP);

        DrawShadowedLine(ImVec2(contentPosition.x, lineY), ImVec2(contentPosition.x + contentWidth, lineY),
                         IM_COL32(255, 255, 255, 128), 1.0f);
    }

    endItem();
}

void Panel::text(string_view text, Padding padding) {

    const float width = beginItem();

    if (!_visible) {
        endItem();
        return;
    }

    const string bodyText {text};
    const ImVec2 position = ImGui::GetCursorScreenPos();
    const float  contentWidth = a3d::math::max(1.0f, width - padding.left - padding.right);
    const ImVec2 contentPosition {
        position.x + padding.left,
        position.y + padding.top,
    };
    const ImVec2 textSize = ImGui::CalcTextSize(bodyText.c_str(), nullptr, false, contentWidth);
    const float  textHeight = textSize.y > 0.0f ? textSize.y : ImGui::GetTextLineHeight();
    const float  height = padding.top + textHeight + padding.bottom;

    ImGui::Dummy(ImVec2(width, height));
    DrawShadowedText(ImVec2(SnapPixel(contentPosition.x), SnapPixel(contentPosition.y)), bodyText,
                     IM_COL32(255, 255, 255, 255), contentWidth);

    endItem();
}

void Panel::value(string_view label, string_view value, Padding padding) {

    const float width = beginItem();

    if (!_visible) {
        endItem();
        return;
    }

    const string labelText {label};
    const string valueText {value};

    const ImVec2 position = ImGui::GetCursorScreenPos();
    const float  contentWidth = a3d::math::max(1.0f, width - padding.left - padding.right);
    const ImVec2 contentPosition {
        position.x + padding.left,
        position.y + padding.top,
    };
    const ImVec2 labelSize = ImGui::CalcTextSize(labelText.c_str());
    const ImVec2 valueSize = ImGui::CalcTextSize(valueText.c_str());
    const float  textHeight = a3d::math::max(labelSize.y, valueSize.y);
    const float  contentHeight = ImGui::GetTextLineHeight();
    const float  height = padding.top + contentHeight + padding.bottom;

    ImGui::Dummy(ImVec2(width, height));

    const float textY = SnapPixel(contentPosition.y + (contentHeight - textHeight) * 0.5f);

    DrawShadowedText(ImVec2(SnapPixel(contentPosition.x), textY), labelText, IM_COL32(255, 255, 255, 255));

    float       valueX = contentPosition.x + contentWidth - valueSize.x;
    const float minimumValueX = contentPosition.x + labelSize.x + VALUE_GAP;

    if (valueX < minimumValueX) {
        valueX = minimumValueX;
    }

    DrawShadowedText(ImVec2(SnapPixel(valueX), textY), valueText, IM_COL32(255, 255, 255, 255));

    endItem();
}

void Panel::spacer(float height) {

    if (height < 0.0f) {
        throw invalid_argument("Panel spacer height must be non-negative.");
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
    _rowItemCount = 0;
    _segmentedRow = false;

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

void Panel::segmentedRow(unsigned itemCount) {

    if (itemCount == 0) {
        throw invalid_argument("Panel segmented row item count must be greater than zero.");
    }

    if (_rowItemsRemaining != 0) {
        throw logic_error("Cannot begin a Panel segmented row before the previous row is complete.");
    }

    _rowItemsRemaining = itemCount;
    _rowItemCount = itemCount;
    _segmentedRow = true;

    if (!_visible) {
        _rowItemWidth = 0.0f;
        _rowSpacing = 0.0f;
        return;
    }

    _rowSpacing = 0.0f;

    const float availableWidth = ImGui::GetContentRegionAvail().x;

    _rowItemWidth = availableWidth / static_cast<float>(itemCount);

    if (_rowItemWidth < 1.0f) {
        _rowItemWidth = 1.0f;
    }
}

bool Panel::button(string_view label, Padding padding) {

    return drawButton(label, false, padding);
}

bool Panel::option(string_view label, bool selected, Padding padding) {

    return drawButton(label, selected, padding);
}

bool Panel::subOption(string_view label, bool selected, Padding padding) {

    const auto framePadding = ImGui::GetStyle().FramePadding;

    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(framePadding.x, 0.0f));

    if (!selected) {
        // ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.135f, 0.135f, 0.135f, 0.94f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.24f, 0.24f, 0.24f, 0.94f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.34f, 0.34f, 0.34f, 0.98f));
    }

    const bool pressed = drawButton(label, selected, padding);

    if (!selected) {
        ImGui::PopStyleColor(2);
    }

    ImGui::PopStyleVar();

    return pressed;
}

bool Panel::slider(string_view label,
                   float&      value,
                   float       minimum,
                   float       maximum,
                   string_view formatString,
                   Padding     padding) {

    if (label.empty()) {
        throw invalid_argument("Panel slider label cannot be empty.");
    }

    if (minimum >= maximum) {
        throw invalid_argument("Panel slider range must be increasing.");
    }

    const float width = beginItem();

    if (!_visible) {
        endItem();
        return false;
    }

    const string labelText {label};
    const string formatText {formatString};

    const ImVec2 position = ImGui::GetCursorScreenPos();
    const float  contentWidth = a3d::math::max(1.0f, width - padding.left - padding.right);
    const ImVec2 contentPosition {
        position.x + padding.left,
        position.y + padding.top,
    };
    const ImVec2 labelSize = ImGui::CalcTextSize(labelText.c_str());
    const float  contentHeight = ImGui::GetFrameHeight();
    const float  height = padding.top + contentHeight + padding.bottom;

    const float sliderX = contentPosition.x + labelSize.x + VALUE_GAP;
    const float sliderWidth = a3d::math::max(1.0f, contentPosition.x + contentWidth - sliderX);

    const float labelY = SnapPixel(contentPosition.y + (contentHeight - labelSize.y) * 0.5f);

    ImGui::BeginGroup();
    ImGui::Dummy(ImVec2(width, height));

    DrawShadowedText(ImVec2(SnapPixel(contentPosition.x), labelY), labelText, IM_COL32(255, 255, 255, 255));

    ImGui::SetCursorScreenPos(ImVec2(sliderX, contentPosition.y));
    ImGui::SetNextItemWidth(sliderWidth);

    ImGui::PushID(labelText.c_str());

    ImGui::PushStyleVar(ImGuiStyleVar_GrabMinSize, SLIDER_GRAB_WIDTH);

    ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.03f, 0.03f, 0.03f, 0.68f));
    ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImVec4(0.03f, 0.03f, 0.03f, 0.68f));
    ImGui::PushStyleColor(ImGuiCol_FrameBgActive, ImVec4(0.03f, 0.03f, 0.03f, 0.68f));

    ImGui::PushStyleColor(ImGuiCol_SliderGrab, ImVec4(0.65f, 0.65f, 0.65f, 0.9f));
    ImGui::PushStyleColor(ImGuiCol_SliderGrabActive, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));

    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));

    const bool changed = ImGui::SliderFloat("##slider", &value, minimum, maximum, formatText.c_str(),
                                            ImGuiSliderFlags_AlwaysClamp);

    const ImVec2 sliderMin = ImGui::GetItemRectMin();
    const ImVec2 sliderMax = ImGui::GetItemRectMax();

    char valueBuffer[64];
    std::snprintf(valueBuffer, sizeof(valueBuffer), formatText.c_str(), value);

    const ImVec2 valueSize = ImGui::CalcTextSize(valueBuffer);

    const ImVec2 valuePosition {
        SnapPixel(sliderMin.x + ((sliderMax.x - sliderMin.x) - valueSize.x) * 0.5f),
        SnapPixel(sliderMin.y + ((sliderMax.y - sliderMin.y) - valueSize.y) * 0.5f),
    };

    ImDrawList* drawList = ImGui::GetWindowDrawList();

    // Normal white value text.
    DrawShadowedText(valuePosition, valueBuffer, IM_COL32(255, 255, 255, 255));

#ifdef INVERT_SLIDER_TEXT
    constexpr float GRAB_PADDING = 2.0f;
    const float     sliderSize = (sliderMax.x - sliderMin.x) - GRAB_PADDING * 2.0f;
    const float     grabSize = a3d::math::min(SLIDER_GRAB_WIDTH, sliderSize);
    const float     usableSize = sliderSize - grabSize;
    const float     usableMin = sliderMin.x + GRAB_PADDING + grabSize * 0.5f;
    const float     usableMax = sliderMax.x - GRAB_PADDING - grabSize * 0.5f;
    const float     t = a3d::math::clamp_01((value - minimum) / (maximum - minimum));
    const float     grabPosition = usableMin + (usableMax - usableMin) * t;

    const ImVec2 grabMin {
        grabPosition - grabSize * 0.5f,
        sliderMin.y + GRAB_PADDING,
    };

    const ImVec2 grabMax {
        grabPosition + grabSize * 0.5f,
        sliderMax.y - GRAB_PADDING,
    };

    drawList->PushClipRect(grabMin, grabMax, true);

    drawList->AddText(valuePosition, IM_COL32(0, 0, 0, 255), valueBuffer);

    drawList->PopClipRect();
#endif

    ImGui::PopStyleColor();

    ImGui::PopStyleColor(5);
    ImGui::PopStyleVar();
    ImGui::PopID();

    ImGui::EndGroup();

    /*
     * SliderFloat was positioned after our manually drawn label. Restore
     * vertical flow to the panel's left edge when this isn't part of row().
     */
    if (_rowItemsRemaining == 0) {
        ImGui::SetCursorScreenPos(ImVec2(position.x, position.y + height + ImGui::GetStyle().ItemSpacing.y));
    }

    endItem();

    return changed;
}

bool Panel::slider(string_view label,
                   int&        value,
                   int         minimum,
                   int         maximum,
                   string_view formatString,
                   Padding     padding) {

    if (label.empty()) {
        throw invalid_argument("Panel slider label cannot be empty.");
    }

    if (minimum >= maximum) {
        throw invalid_argument("Panel slider range must be increasing.");
    }

    const float width = beginItem();

    if (!_visible) {
        endItem();
        return false;
    }

    const string labelText {label};
    const string formatText {formatString};

    const ImVec2 position = ImGui::GetCursorScreenPos();
    const float  contentWidth = a3d::math::max(1.0f, width - padding.left - padding.right);
    const ImVec2 contentPosition {
        position.x + padding.left,
        position.y + padding.top,
    };
    const ImVec2 labelSize = ImGui::CalcTextSize(labelText.c_str());
    const float  contentHeight = ImGui::GetFrameHeight();
    const float  height = padding.top + contentHeight + padding.bottom;

    const float sliderX = contentPosition.x + labelSize.x + VALUE_GAP;
    const float sliderWidth = a3d::math::max(1.0f, contentPosition.x + contentWidth - sliderX);

    const float labelY = SnapPixel(contentPosition.y + (contentHeight - labelSize.y) * 0.5f);

    ImGui::BeginGroup();
    ImGui::Dummy(ImVec2(width, height));

    DrawShadowedText(ImVec2(SnapPixel(contentPosition.x), labelY), labelText, IM_COL32(255, 255, 255, 255));

    ImGui::SetCursorScreenPos(ImVec2(sliderX, contentPosition.y));
    ImGui::SetNextItemWidth(sliderWidth);

    ImGui::PushID(labelText.c_str());

    ImGui::PushStyleVar(ImGuiStyleVar_GrabMinSize, SLIDER_GRAB_WIDTH);

    ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.03f, 0.03f, 0.03f, 0.68f));
    ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImVec4(0.03f, 0.03f, 0.03f, 0.68f));
    ImGui::PushStyleColor(ImGuiCol_FrameBgActive, ImVec4(0.03f, 0.03f, 0.03f, 0.68f));

    ImGui::PushStyleColor(ImGuiCol_SliderGrab, ImVec4(0.65f, 0.65f, 0.65f, 0.9f));
    ImGui::PushStyleColor(ImGuiCol_SliderGrabActive, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));

    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));

    const bool changed = ImGui::SliderInt("##slider", &value, minimum, maximum, formatText.c_str(),
                                          ImGuiSliderFlags_AlwaysClamp);

    const ImVec2 sliderMin = ImGui::GetItemRectMin();
    const ImVec2 sliderMax = ImGui::GetItemRectMax();

    char valueBuffer[64];
    std::snprintf(valueBuffer, sizeof(valueBuffer), formatText.c_str(), value);

    const ImVec2 valueSize = ImGui::CalcTextSize(valueBuffer);

    const ImVec2 valuePosition {
        SnapPixel(sliderMin.x + ((sliderMax.x - sliderMin.x) - valueSize.x) * 0.5f),
        SnapPixel(sliderMin.y + ((sliderMax.y - sliderMin.y) - valueSize.y) * 0.5f),
    };

    ImDrawList* drawList = ImGui::GetWindowDrawList();

    // Normal white value text.
    DrawShadowedText(valuePosition, valueBuffer, IM_COL32(255, 255, 255, 255));

#ifdef INVERT_SLIDER_TEXT
    constexpr float GRAB_PADDING = 2.0f;
    const float     sliderSize = (sliderMax.x - sliderMin.x) - GRAB_PADDING * 2.0f;
    const float     grabSize = a3d::math::min(SLIDER_GRAB_WIDTH, sliderSize);
    const float     usableSize = sliderSize - grabSize;
    const float     usableMin = sliderMin.x + GRAB_PADDING + grabSize * 0.5f;
    const float     usableMax = sliderMax.x - GRAB_PADDING - grabSize * 0.5f;
    const float     t =
        a3d::math::clamp_01(static_cast<float>(value - minimum) / static_cast<float>(maximum - minimum));
    const float grabPosition = usableMin + (usableMax - usableMin) * t;

    const ImVec2 grabMin {
        grabPosition - grabSize * 0.5f,
        sliderMin.y + GRAB_PADDING,
    };

    const ImVec2 grabMax {
        grabPosition + grabSize * 0.5f,
        sliderMax.y - GRAB_PADDING,
    };

    drawList->PushClipRect(grabMin, grabMax, true);

    drawList->AddText(valuePosition, IM_COL32(0, 0, 0, 255), valueBuffer);

    drawList->PopClipRect();
#endif

    ImGui::PopStyleColor();

    ImGui::PopStyleColor(5);
    ImGui::PopStyleVar();
    ImGui::PopID();

    ImGui::EndGroup();

    /*
     * SliderInt was positioned after our manually drawn label. Restore
     * vertical flow to the panel's left edge when this isn't part of row().
     */
    if (_rowItemsRemaining == 0) {
        ImGui::SetCursorScreenPos(ImVec2(position.x, position.y + height + ImGui::GetStyle().ItemSpacing.y));
    }

    endItem();

    return changed;
}

bool Panel::toggle(string_view label, bool& value, Padding padding) {

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
    const float  contentWidth = a3d::math::max(1.0f, width - padding.left - padding.right);
    const ImVec2 contentPosition {
        position.x + padding.left,
        position.y + padding.top,
    };
    const float contentHeight = ImGui::GetFrameHeight();
    const float height = padding.top + contentHeight + padding.bottom;

    ImGui::BeginGroup();
    ImGui::Dummy(ImVec2(width, height));
    ImGui::SetCursorScreenPos(contentPosition);

    ImGui::PushID(toggleLabel.c_str());
    const bool pressed = ImGui::InvisibleButton("##toggle", ImVec2(contentWidth, contentHeight));
    const bool hovered = ImGui::IsItemHovered();
    const bool active = ImGui::IsItemActive();
    ImGui::PopID();

    if (pressed) {
        value = !value;
    }

    ImDrawList* drawList = ImGui::GetWindowDrawList();

    if (active) {
        drawList->AddRectFilled(contentPosition,
                                ImVec2(contentPosition.x + contentWidth, contentPosition.y + contentHeight),
                                IM_COL32(48, 48, 48, 224), 2.0f);
    }
    else if (hovered) {
        drawList->AddRectFilled(contentPosition,
                                ImVec2(contentPosition.x + contentWidth, contentPosition.y + contentHeight),
                                IM_COL32(24, 24, 24, 176), 2.0f);
    }

    const ImVec2 labelSize = ImGui::CalcTextSize(toggleLabel.c_str());
    const ImVec2 labelPosition {
        SnapPixel(contentPosition.x),
        SnapPixel(contentPosition.y + (contentHeight - labelSize.y) * 0.5f),
    };

    const float boxSize = a3d::math::min(TOGGLE_BOX_SIZE, contentHeight);

    const ImVec2 boxMin {
        contentPosition.x + contentWidth - boxSize,
        contentPosition.y + (contentHeight - boxSize) * 0.5f,
    };

    const ImVec2 boxMax {
        boxMin.x + boxSize,
        boxMin.y + boxSize,
    };

    const ImVec2 shadowOffset {SHADOW_OFFSET, SHADOW_OFFSET};

    const float labelClipMaximumX = a3d::math::max(contentPosition.x, boxMin.x - TOGGLE_LABEL_GAP);

    drawList->PushClipRect(contentPosition, ImVec2(labelClipMaximumX, contentPosition.y + contentHeight), true);
    DrawShadowedText(labelPosition, toggleLabel, IM_COL32(255, 255, 255, 255));
    drawList->PopClipRect();

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

    ImGui::EndGroup();

    endItem();

    return pressed;
}

bool Panel::hovered() const {
    return _hovered;
}

// [Private Member Functions]

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

    _rowItemCount = 0;
    _rowItemWidth = 0.0f;
    _rowSpacing = 0.0f;
    _segmentedRow = false;
}

bool Panel::drawButton(string_view label, bool selected, Padding padding) {

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
    const float  contentWidth = a3d::math::max(1.0f, width - padding.left - padding.right);
    const ImVec2 contentPosition {
        position.x + padding.left,
        position.y + padding.top,
    };
    const float    contentHeight = ImGui::GetFrameHeight();
    const float    height = padding.top + contentHeight + padding.bottom;
    const float    rounding = ImGui::GetStyle().FrameRounding;
    const float    borderSize = ImGui::GetStyle().FrameBorderSize;
    const ImU32    borderColor = ImGui::GetColorU32(ImGuiCol_Border);
    const bool     segmented = _segmentedRow;
    const unsigned segmentIndex = segmented ? _rowItemCount - _rowItemsRemaining : 0;

    ImGui::BeginGroup();
    ImGui::Dummy(ImVec2(width, height));
    ImGui::SetCursorScreenPos(contentPosition);

    ImDrawList* drawList = ImGui::GetWindowDrawList();

    if (!segmented) {
        drawList->AddRect(ImVec2(contentPosition.x + SHADOW_OFFSET, contentPosition.y + SHADOW_OFFSET),
                          ImVec2(contentPosition.x + contentWidth + SHADOW_OFFSET,
                                 contentPosition.y + contentHeight + SHADOW_OFFSET),
                          IM_COL32(0, 0, 0, 255), rounding, 0, 1.0f);
    }
    else if (segmentIndex == 0) {
        const float  groupWidth = contentWidth + static_cast<float>(_rowItemCount - 1) * _rowItemWidth;
        const ImVec2 groupMaximum {
            contentPosition.x + groupWidth,
            contentPosition.y + contentHeight,
        };

        drawList->AddRect(ImVec2(contentPosition.x + SHADOW_OFFSET, contentPosition.y + SHADOW_OFFSET),
                          ImVec2(groupMaximum.x + SHADOW_OFFSET, groupMaximum.y + SHADOW_OFFSET),
                          IM_COL32(0, 0, 0, 255), rounding, ImDrawFlags_RoundCornersAll, 1.0f);
    }

    if (selected) {
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.24f, 0.24f, 0.24f, 0.94f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.32f, 0.32f, 0.32f, 0.98f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.40f, 0.40f, 0.40f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
    }

    bool pressed;

    if (!segmented) {
        pressed = ImGui::Button(buttonLabel.c_str(), ImVec2(contentWidth, contentHeight));
    }
    else {
        ImGui::PushID(buttonLabel.c_str());
        pressed = ImGui::InvisibleButton("##segment", ImVec2(contentWidth, contentHeight));
        const bool hovered = ImGui::IsItemHovered();
        const bool active = ImGui::IsItemActive();
        ImGui::PopID();

        const ImVec2 segmentMaximum {
            contentPosition.x + contentWidth,
            contentPosition.y + contentHeight,
        };

        ImDrawFlags cornerFlags = ImDrawFlags_RoundCornersNone;

        if (segmentIndex == 0) {
            cornerFlags |= ImDrawFlags_RoundCornersTopLeft | ImDrawFlags_RoundCornersBottomLeft;
        }
        if (_rowItemsRemaining == 1) {
            cornerFlags |= ImDrawFlags_RoundCornersTopRight | ImDrawFlags_RoundCornersBottomRight;
        }

        const ImGuiCol fillColorIndex = active    ? ImGuiCol_ButtonActive
                                        : hovered ? ImGuiCol_ButtonHovered
                                                  : ImGuiCol_Button;

        drawList->AddRectFilled(contentPosition, segmentMaximum, ImGui::GetColorU32(fillColorIndex), rounding,
                                cornerFlags);

        if (segmentIndex != 0 && borderSize > 0.0f) {
            drawList->AddLine(ImVec2(contentPosition.x, contentPosition.y + borderSize),
                              ImVec2(contentPosition.x, segmentMaximum.y - borderSize), borderColor,
                              borderSize);
        }

        const ImVec2 textSize = ImGui::CalcTextSize(buttonLabel.c_str());
        const ImVec2 textPosition {
            SnapPixel(contentPosition.x + (contentWidth - textSize.x) * 0.5f),
            SnapPixel(contentPosition.y + (contentHeight - textSize.y) * 0.5f),
        };

        drawList->PushClipRect(contentPosition, segmentMaximum, true);
        drawList->AddText(textPosition, ImGui::GetColorU32(ImGuiCol_Text), buttonLabel.c_str());
        drawList->PopClipRect();

        if (_rowItemsRemaining == 1 && borderSize > 0.0f) {
            const ImVec2 groupMinimum {
                contentPosition.x - static_cast<float>(segmentIndex) * _rowItemWidth,
                contentPosition.y,
            };

            drawList->AddRect(groupMinimum, segmentMaximum, borderColor, rounding, ImDrawFlags_RoundCornersAll,
                              borderSize);
        }
    }

    if (selected) {
        ImGui::PopStyleColor(4);
    }

    ImGui::EndGroup();

    endItem();

    return pressed;
}

// [Private Non-Member Functions]

float SnapPixel(float value) {

    return a3d::math::floor(value + 0.5f);
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
