//
//  AABB.cc
//  avara3d
//
//  Created by Morgan Davis on 1/6/26.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#include "a3d/mesh/AABB.h"

namespace a3d {
AABB AABB::Zero() {
    return {{0, 0, 0}, {0, 0, 0}};
}

AABB AABB::Invalid() {
    return {{1, 1, 1}, {-1, -1, -1}};
}

AABB AABB::Union(const AABB& a, const AABB& b) {
    if (!a.valid()) {
        return b;
    }
    if (!b.valid()) {
        return a;
    }
    return {math::min(a.min, b.min), math::max(a.max, b.max)};
}

void AABB::Expand(AABB& a, const math::vec3& p) {
    if (!a.valid()) {
        a.min = p;
        a.max = p;
        return;
    }
    a.min = math::min(a.min, p);
    a.max = math::max(a.max, p);
}

math::vec3 AABB::Center(const AABB& a) {
    return (a.min + a.max) * 0.5f;
}

bool AABB::valid() const {
    return min.x <= max.x && min.y <= max.y && min.z <= max.z;
}

AABB& AABB::operator|=(const AABB& b) {
    *this = Union(*this, b);
    return *this;
}

AABB& AABB::operator|=(const math::vec3& p) {
    Expand(*this, p);
    return *this;
}

AABB operator|(AABB a, const AABB& b) {
    a |= b;
    return a;
}

AABB operator|(AABB a, const math::vec3& p) {
    a |= p;
    return a;
}
} // namespace a3d
