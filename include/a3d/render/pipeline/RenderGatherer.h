
#ifndef AVARA3D_RENDERGATHERER_H
#define AVARA3D_RENDERGATHERER_H

#include <memory>
#include <vector>

#include "a3d/Math.h"
#include "a3d/Types.h"

namespace a3d {

	struct DrawPacket;
	struct RenderItem;

	class Line;
	class Material;
	class Mesh;
	class MeshElement;
	class Node;
	class PhysicalWorld;
	class RenderContext;
	class Scene;

	struct MeshInstance { // temporary?
		Mesh*		mesh;
		math::mat4 	model;
		// world AABB?
	};

	struct RenderItem {

		Mesh* 			mesh = 			nullptr;
		uint32_t 		elementIndex = 	0;
		MeshElement* 	element = 		nullptr; // TODO: remove
		Material* 		material = 		nullptr;

		RenderStyle 	style =			RenderStyle::Normal;

		math::mat4 		model =			math::mat4(1.0f);
		AABB 			aabb = 			AABB::Zero(); // world space
		float 			depth = 		0.0f; // view-space depth for later
		bool 			transparent = 	false; // for later -- always false in BlendFunction
	};

	struct GatherOutput {
		std::vector<RenderItem>		renderItems;
		const Scene*				scene; // debug AABB
		std::shared_ptr<Material>	backgroundMaterial;
		std::vector<Node*> 			lightNodes;
		std::vector<MeshInstance> 	meshInstances; // debug AABBs
		std::vector<Line>			physicsDebugLines;
	};

	class RenderGatherer {

	public:

		static GatherOutput Gather(const Scene& scene,
								   const math::mat4 &view,
								   const PhysicalWorld* physicalWorld,
								   const DebugOptions& debugOptions, // temporary?
								   FrameStats& stats);

	};
}

#endif //AVARA3D_RENDERGATHERER_H
