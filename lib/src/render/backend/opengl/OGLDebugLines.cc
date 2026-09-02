//
//  OGLDebugLines.cc
//  avara3d
//
//  Created by Morgan Davis on 1/4/26.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#include "a3d/render/backend/opengl/OGLDebugLines.h"

#include "a3d/render/backend/opengl/gl.h"
#include "a3d/render/backend/opengl/OGLMemoryTracker.h"


namespace a3d {

// [Internal Lifecycle Functions]

OGLDebugLines::OGLDebugLines(OGLMemoryTracker& memoryTracker):
    memoryTracker {memoryTracker} {}

// [Internal Member Functions]

void OGLDebugLines::ensureBuffers() {
    if (vao && vbo) {
        return;
    }

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    // position (location 0)
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(OGLDebugLines::Vertex),
                          (void*) offsetof(OGLDebugLines::Vertex, pos));

    // color (location 1)
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(OGLDebugLines::Vertex),
                          (void*) offsetof(OGLDebugLines::Vertex, color));

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void OGLDebugLines::upload(const std::vector<Line>& lines) {
    ensureBuffers();

    cpuVerts.clear();
    cpuVerts.reserve(lines.size() * 2);

    for (const Line& l : lines) {
        cpuVerts.push_back({l.fromLocation(), l.fromColor().rgb()});
        cpuVerts.push_back({l.toLocation(), l.toColor().rgb()});
    }

    vertexCount = (gl::sizei_t) cpuVerts.size();

    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    const size_t bytes = cpuVerts.size() * sizeof(OGLDebugLines::Vertex);

    // orphan
    glBufferData(GL_ARRAY_BUFFER, bytes, nullptr, GL_STREAM_DRAW);

    memoryTracker.setAllocation(
        {
            OGLMemoryTracker::ObjectNamespace::Buffer,
            vbo,
        },
        OGLMemoryTracker::Source::A3D, OGLMemoryTracker::Category::VertexBuffer, bytes);

    // upload
    if (bytes) {
        glBufferSubData(GL_ARRAY_BUFFER, 0, bytes, cpuVerts.data());
    }

    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void OGLDebugLines::destroy() {
    if (vbo) {
        memoryTracker.removeAllocation({
            OGLMemoryTracker::ObjectNamespace::Buffer,
            vbo,
        });

        glDeleteBuffers(1, &vbo);
    }
    if (vao) {
        glDeleteVertexArrays(1, &vao);
    }
    vbo = 0;
    vao = 0;
    vertexCount = 0;
    cpuVerts.clear();
}

} // namespace a3d
