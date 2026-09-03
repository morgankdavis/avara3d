//
//  Constants.h
//  janus
//
//  Created by Morgan Davis on 8/30/26.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_DEMO_JANUS_CONSTANTS_H
#define AVARA3D_DEMO_JANUS_CONSTANTS_H

#include "a3d/Math.h"

namespace demo::janus {

// [Public Constants]

const a3d::math::vec3 GRAVITY_EARTH {0.0f, -9.807f, 0.0f};
const a3d::math::vec3 GRAVITY_MOON {0.0f, -1.62f, 0.0f};
const a3d::math::vec3 GRAVITY_ZERO {0.0f, 0.0f, 0.0f};

const float STONE_FRICTION {0.7f};
const float STONE_RESTITUTION {0.015f};

} // namespace demo::janus

#endif // AVARA3D_DEMO_JANUS_CONSTANTS_H
