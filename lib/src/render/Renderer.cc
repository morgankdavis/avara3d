//
//  Renderer.cc
//  avara3d
//
//  Created by Morgan Davis on 4/17/2024.
//  Copyright © 2024-2024 Morgan K Davis. All rights reserved.
//

#include "a3d/render/Renderer.h"

#include "a3d/log/Log.h"
#include "a3d/mesh/MeshElement.h"
#include "a3d/render/context/RenderContext.h"
#include "a3d/scene/Scene.h"
#include "a3d/visual/VisualWorld.h"
#include "a3d/visual/material/Material.h"

using namespace a3d;
using namespace std;

/// Internal Lifescycle ///

Renderer::Renderer() {}

Renderer::~Renderer() {
    log::d()("Destroying Renderer {:p}", static_cast<void*>(this));
}
