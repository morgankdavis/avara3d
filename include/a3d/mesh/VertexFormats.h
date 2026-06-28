//
//  VertexFormats.h
//  avara3d
//
//  Created by Morgan Davis on 1/5/26.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_MESH_VERTEXFORMATS_H
#define AVARA3D_MESH_VERTEXFORMATS_H

#include "a3d/Math.h"
#include "a3d/mesh/VertexLayout.h"

namespace a3d {

	struct VertexPNT {
		math::vec3 position;
		math::vec3 normal;
		math::vec2 texCoord;
	};

	struct VertexPC {
		math::vec3 position;
		math::vec3 color;
	};

//	inline constexpr uint16_t VertexStride(a3d::VertexLayout layout) {
//		switch (layout) {
//			case a3d::VertexLayout::PNT: 	return sizeof(VertexPNT);
//			case a3d::VertexLayout::PC:  	return sizeof(VertexPC);
//			default:                		return 0;
//		}
//	}
}

#endif //AVARA3D_MESH_VERTEXFORMATS_H
