//
//  DebugLinesBuilder.cc
//  avara3d
//
//  Created by Morgan Davis on 1/4/26.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#include "a3d/render/DebugLinesBuilder.h"

#include "a3d/Color.h"
#include "a3d/mesh/AABB.h"
#include "a3d/mesh/Line.h"

using namespace a3d::math;
using namespace std;

namespace a3d {
namespace {
    // [Private Non-Member Prototypes]

    void AppendBoxLinesFromCorners(vector<Line>& out, const vec3 c[8], const Color& color);
} // namespace

// [Internal Static Member Functions]

void DebugLinesBuilder::AppendAABB(std::vector<Line>& out, const AABB& aabb, const Color& color) {

    vec3 c[8] = {
        {aabb.min.x, aabb.max.y, aabb.min.z}, // 0
        {aabb.min.x, aabb.max.y, aabb.max.z}, // 1
        {aabb.max.x, aabb.max.y, aabb.max.z}, // 2
        {aabb.max.x, aabb.max.y, aabb.min.z}, // 3
        {aabb.min.x, aabb.min.y, aabb.min.z}, // 4
        {aabb.min.x, aabb.min.y, aabb.max.z}, // 5
        {aabb.max.x, aabb.min.y, aabb.max.z}, // 6
        {aabb.max.x, aabb.min.y, aabb.min.z},
    }; // 7

    AppendBoxLinesFromCorners(out, c, color);
}

void DebugLinesBuilder::AppendOBBFromLocalAABB(std::vector<Line>& out,
                                               const AABB&        local,
                                               const mat4&        model,
                                               const Color&       color) {

    vec3 lc[8] = {{local.min.x, local.max.y, local.min.z}, {local.min.x, local.max.y, local.max.z},
                  {local.max.x, local.max.y, local.max.z}, {local.max.x, local.max.y, local.min.z},
                  {local.min.x, local.min.y, local.min.z}, {local.min.x, local.min.y, local.max.z},
                  {local.max.x, local.min.y, local.max.z}, {local.max.x, local.min.y, local.min.z}};

    vec3 wc[8];
    for (int i = 0; i < 8; ++i) {
        auto h = model * vec4(lc[i], 1.0f);
        wc[i] = vec3(h.x, h.y, h.z); // assuming affine; otherwise divide by h.w
    }

    AppendBoxLinesFromCorners(out, wc, color);
}

void DebugLinesBuilder::AppendFrame(vector<Line>& out, const mat4& transform, const vec3& size) {

    const vec3 origin = vec3(transform * vec4 {0.0f, 0.0f, 0.0f, 1.0f});

    const vec3 xEnd = vec3(transform * vec4 {size.x, 0.0f, 0.0f, 1.0f});
    const vec3 yEnd = vec3(transform * vec4 {0.0f, size.y, 0.0f, 1.0f});
    const vec3 zEnd = vec3(transform * vec4 {0.0f, 0.0f, size.z, 1.0f});

    out.emplace_back(origin, xEnd, Color {vec3 {1.0f, 0.3f, 0.3f}});
    out.emplace_back(origin, yEnd, Color {vec3 {0.3f, 1.0f, 0.3f}});
    out.emplace_back(origin, zEnd, Color {vec3 {0.3f, 0.3f, 1.0f}});
}

namespace {
    // [Private Non-Member Functions]

    void AppendBoxLinesFromCorners(vector<Line>& out, const vec3 c[8], const Color& color) {
        // indices:
        // 	0..3: top ring
        // 	4..7: bottom ring

        auto add = [&](int a, int b) {
            out.push_back(Line {c[a], c[b], color});
        };

        // top
        add(0, 1);
        add(1, 2);
        add(2, 3);
        add(3, 0);
        // bottom
        add(4, 5);
        add(5, 6);
        add(6, 7);
        add(7, 4);
        // verticals
        add(0, 4);
        add(1, 5);
        add(2, 6);
        add(3, 7);
    }
} // namespace
} // namespace a3d
