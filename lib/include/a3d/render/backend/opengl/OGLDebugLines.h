//
//  OGLDebugLines.h
//  avara3d
//
//  Created by Morgan Davis on 1/4/26.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_RENDER_BACKEND_OPENGL_OGLDEBUGLINES_H
#define AVARA3D_RENDER_BACKEND_OPENGL_OGLDEBUGLINES_H

#include <vector>

#include "a3d/Math.h"
#include "a3d/mesh/Line.h"
#include "a3d/render/backend/opengl/GLTypes.h"

namespace a3d {

class OGLMemoryTracker;

struct OGLDebugLines {

    // [Internal Types]

    struct Vertex {
        math::vec3 pos;
        math::vec3 color;
    };

    // [Internal Lifecycle Functions]

    explicit OGLDebugLines(OGLMemoryTracker& memoryTracker);

    // [Internal Member Functions]

    void                ensureBuffers();
    void                upload(const std::vector<Line>& lines);
    void                destroy();

    // [Internal Member Variables]

    OGLMemoryTracker&   memoryTracker;
    gl::uint_t          vbo         = 0;
    gl::uint_t          vao         = 0;
    gl::sizei_t         vertexCount = 0;
    std::vector<Vertex> cpuVerts    = {};
};

} // namespace a3d

#endif // AVARA3D_RENDER_BACKEND_OPENGL_OGLDEBUGLINES_H
