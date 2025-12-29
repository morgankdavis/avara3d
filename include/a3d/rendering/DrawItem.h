//
// Created by mkd on 12/24/25.
//

#ifndef AVARA3D_DRAWITEM_H
#define AVARA3D_DRAWITEM_H

#include "a3d/Math.h"
#include "a3d/rendering/PipelineKey.h"

namespace a3d {

	class Mesh;
	class MeshElement;

	struct DrawItem {

		PipelineHandle 	pipeline = 		INVALID_PIPELINE;

//		Mesh* 			mesh = 			nullptr;
		uint32_t 		elementIndex = 	0;
		MeshElement* 	element = 		nullptr;
		Material* 		material = 		nullptr;

		math::mat4 		model = 		math::mat4(1.0);
		float 			depth =			0.0f;

		PassKind 		pass = 			PassKind::MainOpaque;

		uint64_t 		sortKey = 		0;
		uint32_t		sequence =		0;



		// Optional: cache for faster sort. Can be derived from material/key.
		bool transparent = false;
//		bool isTransparent(const DrawItem& it) {
//			return it.material && it.material->alphaMode() == AlphaMode::Blend;
//			// or: return it.key.blendFunction != BlendFunction::Disabled;
//		}
		// item.transparent = (item.material->alphaMode() == AlphaMode::Blend);

		// Optional: keep key only until pipeline resolve is done
		PipelineKey key;
	};
}

#endif //AVARA3D_DRAWITEM_H
