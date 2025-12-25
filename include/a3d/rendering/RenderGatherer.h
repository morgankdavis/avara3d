
#ifndef AVARA3D_RENDERGATHERER_H
#define AVARA3D_RENDERGATHERER_H

#include <vector>

#include "a3d/Math.h"
#include "a3d/Types.h"
#include "a3d/rendering/PipelineKey.h"

namespace a3d {

	struct RenderItem;
	struct RenderPacket;
	struct RenderResourceCacheOGL;

	class Material;
	class Mesh;
	class Node;
	class RenderContext;
	class Scene;

	// ! temporary !
	struct MeshInstance {
		Mesh*		mesh;
		math::mat4 	model;
		// put world AABB in here so it doesn't have to be re-computed in renderer
	};

	struct GatherOutput {
		std::vector<RenderItem>		renderItems;
		std::vector<MeshInstance> 	temp_meshInstances; // for debug AABBs
		std::vector<Node*> 			temp_lightNodes; // for postTraversal selection
	};

	class RenderGatherer {

	public:

		static GatherOutput GatherRenderItems(const a3d::Scene& scene,
											  const a3d::RenderContext& context,
											  const a3d::math::mat4& view,
											  const DebugOptions& debugOptions, // ! temporary !
											  a3d::FrameStats& stats);







		static PipelineKey ComputePipelineKey(const Material& material, uint32_t vertexLayoutKey);
		static PipelineKey MakeMainKey(const RenderItem& item, uint32_t vertexLayoutKey);
		static PipelineKey MakeWireKey(const RenderItem& item, uint32_t vertexLayoutKey);




		static RenderPacket BuildRenderPacket(const GatherOutput& gather,
											  const DebugOptions& debugOptions,
											  RenderResourceCacheOGL& cache,
											  uint32_t vertexLayoutKey);
	};
}

#endif //AVARA3D_RENDERGATHERER_H
