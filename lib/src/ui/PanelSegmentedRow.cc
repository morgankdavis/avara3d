//
//  PanelSegmentedRow.cc
//  avara3d
//
//  Created by OpenAI on 9/1/26.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#include "a3d/ui/Panel.h"

#include <stdexcept>

#include <imgui/imgui.h>

using namespace a3d::ui;
using namespace std;

// [Public Member Functions]

void Panel::segmentedRow(unsigned itemCount) {

    if (itemCount == 0) {
        throw invalid_argument("Panel segmented row item count must be greater than zero.");
    }

    if (_rowItemsRemaining != 0) {
        throw logic_error("Cannot begin a Panel segmented row before the previous row is complete.");
    }

    _rowItemsRemaining = itemCount;

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
