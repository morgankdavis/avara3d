//
//  RenderContext.cc
//  avara3d
//
//  Created by Morgan Davis on 4/24/18.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#include "a3d/render/context/RenderContext.h"

#include <stdexcept>

#include "a3d/Buffer.h"
#include "a3d/Image.h"
#include "a3d/log/Log.h"
#include "a3d/render/Renderer.h"
#include "a3d/render/backend/opengl/OGLRenderer.h"
#include "a3d/scene/Node.h"
#include "a3d/visual/camera/PerspectiveCamera.h"

using namespace a3d;
using namespace a3d::math;
using namespace std;

// [Public Member Functions]

//bool RenderContext::vSyncEnabled() const {
//	return _vSyncEnabled;
//}
//
//void RenderContext::vSyncEnabled(bool enabled) {
//	_vSyncEnabled = enabled;
//}

RenderContext::Antialiasing RenderContext::antialiasing() const {
    return _antialiasing;
}

unique_ptr<Image> RenderContext::snapshot() const {
    if (_renderer) {
        return _renderer->snapshot(*this);
    }
    return nullptr;
}

VisualWorld* RenderContext::visualWorld() const {
    return _visualWorld;
}

// [Internal Lifescycle]

RenderContext::RenderContext():
    _antialiasing {Antialiasing::None},
    _visualWorld {},
    _renderer {make_unique<OGLRenderer>()} {}

RenderContext::~RenderContext() {
    log::d()("Destroying RenderContext {:p}", static_cast<void*>(this));
}

// [Internal Member Functions]

void RenderContext::pollEvents() {}

vec2 RenderContext::viewportScale() const {
    // see note in GLFWWindow::viewportLogicalSize().
    // on Windows and X11, DPI/glfwGetWindowContentScale() is mostly a "UI" scaling hint.
    uvec2 fbSize = framebufferSize();
    uvec2 vpLogicalSize = viewportLogicalSize();
    return vec2(float(fbSize.x) / float(vpLogicalSize.x), float(fbSize.y) / float(vpLogicalSize.y));
}

void RenderContext::attachedToVisualWorld(VisualWorld* world) {
    log::t()("world: {:p}", static_cast<void*>(world));

    _visualWorld = world;
}

void RenderContext::detachedFromVisualWorld(VisualWorld* world) {
    log::t()("world: {:p}", static_cast<void*>(world));

    _visualWorld = nullptr;
}

Renderer* RenderContext::renderer() const {
    return _renderer.get();
}
