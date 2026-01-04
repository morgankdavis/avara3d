//
//  DrawPacket.h
//  avara3d
//
//  Created by Morgan Davis on 12/28/25.
//  Copyright © 2025 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_RENDER_DRAWPACKET_H
#define AVARA3D_RENDER_DRAWPACKET_H

#include "a3d/Math.h"
#include "a3d/mesh/Line.h"
#include "a3d/render/PipelineKey.h"

namespace a3d {

	class Material;
	class MeshElement;
	class Node;

	struct BackgroundPass {
		PipelineHandle 				pipeline = 	INVALID_PIPELINE_HANDLE;
		PipelineKey					key	=		{};
		std::shared_ptr<Material> 	material =	nullptr; // gross
	};

	struct LinesPass {
		PipelineHandle 		pipeline = 	INVALID_PIPELINE_HANDLE;
		PipelineKey			key =		{};
		math::mat4     		model =		math::mat4(1.0f); // identity for world-space lines
		std::vector<Line> 	lines =		{};
	};

	struct DrawItem {

		PassKind 		pass = 			PassKind::MainOpaque;

		PipelineHandle 	pipeline = 		INVALID_PIPELINE_HANDLE;
		PipelineKey 	key =			{};

		uint32_t 		elementIndex = 	0;
		MeshElement* 	element = 		nullptr;
		Material* 		material = 		nullptr;

		math::mat4 		model = 		math::mat4(1.0);
		float 			depth =			0.0f;

		uint64_t 		sortKey = 		0;
		uint32_t		sequence =		0;

		bool 			transparent = 	false;
	};

	struct DrawPacket {

		BackgroundPass			backgroundPass;
		std::vector<DrawItem> 	mainPassItems;
		// opaqueItems
		// maskItems
		// transparentItems
		std::vector<DrawItem> 	wireframePassItems;
		LinesPass				linesPass;
		std::vector<Node*> 		lightNodes;
	};
}

#endif //AVARA3D_RENDER_DRAWPACKET_H
