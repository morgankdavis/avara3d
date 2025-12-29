
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
		std::vector<Node*> 			lightNodes; // for postTraversal selection
		std::vector<MeshInstance> 	meshInstances; // for debug AABBs
	};

	class RenderGatherer {

	public:

		static GatherOutput GatherRenderItems(const Scene& scene,
											  const RenderContext& context,
											  const math::mat4& view,
											  const DebugOptions& debugOptions, // temporary
											  FrameStats& stats);







		static PipelineKey MakePipelineKey(const Material& material, uint32_t vertexLayoutKey);
		static PipelineKey MakeMainOpaquePipelineKey(const RenderItem& item, uint32_t vertexLayoutKey);
		static PipelineKey MakeMainMaskPipelineKey(const RenderItem& item, uint32_t vertexLayoutKey);
		static PipelineKey MakeMainTransparentPipelineKey(const RenderItem& item, uint32_t vertexLayoutKey);

		static PipelineKey MakeWirePipelineKey(const RenderItem& item, uint32_t vertexLayoutKey);




		static RenderPacket BuildRenderPacket(GatherOutput& gatherOutput,
											  RenderResourceCacheOGL& cache,
											  uint32_t vertexLayoutKey,
											  const DebugOptions& debugOptions);
	};
}

#endif //AVARA3D_RENDERGATHERER_H
