
#ifndef AVARA3D_RENDERPACKETIZER_H
#define AVARA3D_RENDERPACKETIZER_H

#include <vector>

#include "a3d/Math.h"
#include "a3d/Types.h"
#include "a3d/rendering/RenderItem.h"

namespace a3d {

#warning TEMPORARY
	class Node; // temporary
	class RenderContext;
	class Scene;

#warning TEMPORARY
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

	class RenderPacketizer {

	public:

		static GatherOutput GatherRenderItems(const a3d::Scene& scene,
											  const a3d::RenderContext& context,
											  const a3d::math::mat4& view,
											  const DebugOptions& debugOptions, // ! temporary !
											  a3d::FrameStats& stats);
	};
}

#endif //AVARA3D_RENDERPACKETIZER_H
