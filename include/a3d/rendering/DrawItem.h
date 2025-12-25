//
// Created by mkd on 12/24/25.
//

#ifndef AVARA3D_DRAWITEM_H
#define AVARA3D_DRAWITEM_H

#include "a3d/Math.h"
#include "a3d/rendering/RenderResourceCacheOGL.h"

namespace a3d {

	class Mesh;
	class MeshElement;

	struct DrawItem {

		PipelineHandle 	pipeline = 		INVALID_PIPELINE;

		Mesh* 			mesh = 			nullptr;
		uint32_t 		elementIndex = 	0;
		MeshElement* 	element = 		nullptr;
		Material* 		material = 		nullptr;

		math::mat4 		model = 		math::mat4(1.0);
		float 			depth =			0.0f;

		PassKind 		pass = 			PassKind::Main;

		uint64_t 		sortKey = 		0;
	};
}

#endif //AVARA3D_DRAWITEM_H
