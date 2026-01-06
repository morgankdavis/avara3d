//
//  VertexTypes.h
//  avara3d
//
//  Created by Morgan Davis on 1/4/26.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_MESH_VERTEXTYPES_H
#define AVARA3D_MESH_VERTEXTYPES_H

#include "a3d/Math.h"

namespace a3d {

	using VertexPNT = Vertex;

//	struct VertexPNT {
//		math::vec3 pos;
//		math::vec3 norm;
//		math::vec2 uv0;
//	};

	struct VertexPC {
		math::vec3 pos;
		math::vec3 color;
	};
}

#endif //AVARA3D_MESH_VERTEXTYPES_H
