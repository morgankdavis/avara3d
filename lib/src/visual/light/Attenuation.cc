//
//  Attenuation.cc
//  avara3d
//
//  Created by Morgan Davis on 1/7/26.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#include "a3d/visual/light/Attenuation.h"

namespace a3d {
Attenuation Attenuation::FromRange(float range, float p) {

    Attenuation a;
    a.constant = 1.0f;
    a.linear = 0.0f;

    if (range <= 0.0f) {
        a.quadratic = 0.0f;
        return a;
    }
    if (p <= 0.0f) {
        p = 0.0001f;
    }
    if (p >= 1.0f) {
        p = 0.9999f;
    }

    a.quadratic = (1.0f / p - 1.0f) / (range * range);

    return a;
}
} // namespace a3d
