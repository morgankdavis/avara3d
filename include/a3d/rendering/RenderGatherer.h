
#ifndef AVARA3D_RENDERGATHERER_H
#define AVARA3D_RENDERGATHERER_H

#include <memory>
#include <vector>

#include "a3d/Math.h"
#include "a3d/Types.h"
#include "a3d/rendering/PipelineKey.h"

namespace a3d {

	struct RenderItem;
	struct RenderPacket;
	struct RenderResourceCacheOGL;

	class Line;
	class Material;
	class Mesh;
	class Node;
	class PhysicalWorld;
	class RenderContext;
	class Scene;

	// ! temporary !
	struct MeshInstance {
		Mesh*		mesh;
		math::mat4 	model;
		// put world AABB in here?
	};

	struct GatherOutput {
		std::vector<RenderItem>		renderItems;
		const Scene*				scene; // debug AABB
		std::shared_ptr<Material>	backgroundMaterial;
		std::vector<Node*> 			lightNodes;
		std::vector<MeshInstance> 	meshInstances; // debug AABBs
		//const std::vector<Line>*	physicsDebugLines; // notice NON-OWNING
		std::vector<Line>			physicsDebugLines;
	};

	class RenderGatherer {

	public:

		static GatherOutput GatherRenderItems(const Scene& scene,
											  const math::mat4& view,
											  const PhysicalWorld* physicalWorld,
//											  const std::vector<Line>& bulletDebugLines,
											  const DebugOptions& debugOptions, // temporary?
											  FrameStats& stats);






		static PipelineKey MakePipelineKey(const Material& material, uint32_t vertexLayoutKey);
		static PipelineKey MakeBackgroundPipelineKey();
		static PipelineKey MakeMainOpaquePipelineKey(const RenderItem& item, uint32_t vertexLayoutKey);
		static PipelineKey MakeMainMaskPipelineKey(const RenderItem& item, uint32_t vertexLayoutKey);
		static PipelineKey MakeMainTransparentPipelineKey(const RenderItem& item, uint32_t vertexLayoutKey);
		static PipelineKey MakeWireframePipelineKey(const RenderItem& item, uint32_t vertexLayoutKey);
		static PipelineKey MakeLinesPipelineKey();




		static RenderPacket BuildRenderPacket(GatherOutput& gatherOutput,
											  RenderResourceCacheOGL& cache,
											  uint32_t vertexLayoutKey,
											  const DebugOptions& debugOptions);
	};
}

#endif //AVARA3D_RENDERGATHERER_H
