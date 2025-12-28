//
// Created by mkd on 12/14/25.
//

#ifndef AVARA3D_RENDERITEM_H
#define AVARA3D_RENDERITEM_H

#include "a3d/Math.h"

namespace a3d {

	class Light;
	class Material;
	class Mesh;
	class MeshElement;

	// TODO: move
	struct RenderItem {

		Mesh* 			mesh = 			nullptr;
		uint32_t 		elementIndex = 	0;
		MeshElement* 	element = 		nullptr; // TODO: remove
		Material* 		material = 		nullptr;

		RenderStyle 	style =			RenderStyle::Normal;

		math::mat4 		model =			math::mat4(1.0f);
		AABB 			aabb = 			AABB::InvalidAABB(); // world space
		float 			depth = 		0.0f; // view-space depth for later
		bool 			transparent = 	false; // for later -- always false in BlendFunction
	};
}

#endif //AVARA3D_RENDERITEM_H
