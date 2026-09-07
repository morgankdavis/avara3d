//
//  Renderer.cc
//  avara3d
//
//  Created by Morgan Davis on 4/17/2024.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#include "a3d/render/Renderer.h"

#include "a3d/log/Log.h"
#include "a3d/visual/VisualWorld.h"

using namespace std;

namespace a3d {

// [Internal Lifescycle Functions]

Renderer::Renderer() {}

Renderer::~Renderer() {
    log::d()("Destroying Renderer {:p}", static_cast<void*>(this));
}

} // namespace a3d
