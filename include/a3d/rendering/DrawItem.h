//
// Created by mkd on 12/24/25.
//

#ifndef AVARA3D_DRAWITEM_H
#define AVARA3D_DRAWITEM_H

#include <memory>

#include "a3d/Math.h"
#include "a3d/mesh/Line.h"
#include "a3d/rendering/PipelineKey.h"

namespace a3d {

//	class Line;
	class Material;
	class Mesh;
	class MeshElement;

	struct BackgroundPass {
		PipelineHandle 				pipeline = 		INVALID_PIPELINE_HANDLE;
		PipelineKey					key	=			{};
		std::shared_ptr<Material> 	material =		nullptr; // gross
	};

	struct LinesPass {
		PipelineHandle 		pipeline = 		INVALID_PIPELINE_HANDLE;
		PipelineKey			key =			{};
		math::mat4     		model =			math::mat4(1.0f); // identity for world-space lines
		std::vector<Line> 	lines =			{};
	};

	struct DrawItem {

		PipelineHandle 	pipeline = 		INVALID_PIPELINE_HANDLE;
		PipelineKey 	pipelineKey		{};

		uint32_t 		elementIndex = 	0;
		MeshElement* 	element = 		nullptr;
		Material* 		material = 		nullptr;

		math::mat4 		model = 		math::mat4(1.0);
		float 			depth =			0.0f;

		PassKind 		pass = 			PassKind::MainOpaque;

		uint64_t 		sortKey = 		0;
		uint32_t		sequence =		0;

		bool 			transparent = 	false;

		PipelineKey 	key;
	};
}

#endif //AVARA3D_DRAWITEM_H
