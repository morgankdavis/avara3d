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

	struct DebugLineVertex {
		math::vec3 pos;
		math::vec3 color;
	};

	struct OGLDebugLines {
		gl::uint_t 						vbo = 			0;
		gl::uint_t 						vao = 			0;
		gl::sizei_t 					vertexCount = 	0;
		std::vector<DebugLineVertex> 	cpuVerts =		{};

		void ensureBuffers();          // create vao/vbo once
		void upload(const std::vector<Line>& lines); // fill cpuVerts + stream to vbo
		void destroy();
	};
}

#endif //AVARA3D_RENDER_BACKEND_OPENGL_OGLDEBUGLINES_H
