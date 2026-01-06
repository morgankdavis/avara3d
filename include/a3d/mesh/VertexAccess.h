//
//  VertexAccess.h
//  avara3d
//
//  Created by Morgan Davis on 1/5/26.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_MESH_VERTEXACCESS_H
#define AVARA3D_MESH_VERTEXACCESS_H

#include "a3d/Math.h"
#include "a3d/mesh/VertexLayout.h"
#include "a3d/render/VertexLayoutDesc.h"

namespace a3d {

	struct VertexAttribDesc;
	struct VertexLayoutDesc;

	struct VertexAccess {

		static const VertexAttribDesc*	FindAttrib(const VertexLayoutDesc &desc,
													 VertexSemantic semantic);
		static const VertexAttribDesc*	GetPosAttribOrDie(VertexLayout layout); // TODO: rename
		static math::vec3 				ReadVec3(const std::byte *base,
												  uint16_t offset);
		static void 					WriteVec3(std::byte *base,
												 uint16_t offset,
												 const math::vec3 &vec);
	};
}

#endif //AVARA3D_MESH_VERTEXACCESS_H
