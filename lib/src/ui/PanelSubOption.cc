//
//  PanelSubOption.cc
//  avara3d
//
//  Created by Morgan Davis on 8/22/26.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#include "a3d/ui/Panel.h"

#include <stdexcept>
#include <string>

#include <imgui/imgui.h>

#include "a3d/Math.h"

using namespace a3d::ui;
using namespace std;

// [Private Constants]

static constexpr float SHADOW_OFFSET {1.0f};

// [Public Member Functions]

bool Panel::subOption(string_view label, bool selected, Padding padding) {

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
    const float contentHeight = ImGui::GetFrameHeight();
    const float height = padding.top + contentHeight + padding.bottom;
    const float rounding = ImGui::GetStyle().FrameRounding;

    ImGui::BeginGroup();
    ImGui::Dummy(ImVec2(width, height));
    ImGui::SetCursorScreenPos(contentPosition);

    ImDrawList* drawList = ImGui::GetWindowDrawList();
    drawList->AddRect(ImVec2(contentPosition.x + SHADOW_OFFSET, contentPosition.y + SHADOW_OFFSET),
                      ImVec2(contentPosition.x + contentWidth + SHADOW_OFFSET,
                             contentPosition.y + contentHeight + SHADOW_OFFSET),
                      IM_COL32(0, 0, 0, 255), rounding, 0, 1.0f);

    if (selected) {
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.14f, 0.14f, 0.14f, 0.88f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.22f, 0.22f, 0.22f, 0.94f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.30f, 0.30f, 0.30f, 0.98f));
        ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(1.0f, 1.0f, 1.0f, 0.80f));
    }
    else {
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.01f, 0.01f, 0.01f, 0.55f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.10f, 0.10f, 0.10f, 0.80f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.20f, 0.20f, 0.20f, 0.92f));
        ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(1.0f, 1.0f, 1.0f, 0.45f));
    }

    const bool pressed = ImGui::Button(buttonLabel.c_str(), ImVec2(contentWidth, contentHeight));

    ImGui::PopStyleColor(4);

    ImGui::EndGroup();

    endItem();

    return pressed;
}
