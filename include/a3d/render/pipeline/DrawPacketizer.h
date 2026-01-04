
#ifndef AVARA3D_DRAWPACKETIZER_H
#define AVARA3D_DRAWPACKETIZER_H

#include "a3d/mesh/Line.h"
#include "a3d/render/pipeline/PipelineKey.h"
#include "a3d/render/pipeline/RenderGatherer.h"

namespace a3d {

	class Material;
	class MeshElement;
	class Node;
	class RenderItem;

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

		PassKind 		pass = 			PassKind::MainOpaque;

		PipelineHandle 	pipeline = 		INVALID_PIPELINE_HANDLE;
		PipelineKey 	key =			{};
		PipelineKey 	pipelineKey =	{}; // TODO: REMOVE

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
		// future: mainOpaque, mainMask, mainTransparent
		std::vector<DrawItem> 	mainPassItems;
		std::vector<DrawItem> 	wireframePassItems;
		//std::vector<Line> 		debugLines;
		LinesPass				linesPass;
		std::vector<Node*> 		lightNodes;
	};

	class DrawPacketizer {

	public:

		static DrawPacket BuildDrawPacket(GatherOutput& gatherOutput,
										  uint32_t vertexLayoutKey,
										  const DebugOptions& debugOptions);
	};
}

#endif //AVARA3D_DRAWPACKETIZER_H
