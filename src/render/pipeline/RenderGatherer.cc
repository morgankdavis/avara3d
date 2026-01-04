
#include "a3d/render/pipeline/RenderGatherer.h"

#include "a3d/mesh/Line.h"
#include "a3d/mesh/Mesh.h"
#include "a3d/mesh/MeshElement.h"
#include "a3d/physics/PhysicalWorld.h"
#include "a3d/physics/backend/bullet/BulletWorldProxy.h"
#include "a3d/render/context/RenderContext.h"
#include "a3d/scene/Node.h"
#include "a3d/scene/Scene.h"
#include "a3d/visual/VisualWorld.h"
#include "a3d/visual/material/Material.h"

using namespace a3d;
using namespace a3d::math;
using namespace std;

struct GatherEntry {
	Node* 		node;
	math::mat4 	parentWorld;
};

// walk the scene, compute transforms/AABBs/depth, pick logical materials/elements/styles
// "gather / collect / cull"
GatherOutput RenderGatherer::Gather(const Scene& scene,
									const math::mat4 &view,
									const PhysicalWorld* physicalWorld,
											   //const vector<Line>& bulletDebugLines,
											   const DebugOptions& debugOptions, // temporary
											   FrameStats& stats) {

	GatherOutput output{};
	output.renderItems.reserve(1024);
	output.lightNodes.reserve(64);
	output.meshInstances.reserve(512);

	vector<GatherEntry> stack;
	stack.reserve(256);
	stack.push_back({scene.rootNode().get(), mat4(1.0)});

	while (!stack.empty()) {
		auto [n, parentWorld] = stack.back();
		stack.pop_back();

		if (n->hidden()) continue;

		const mat4 world = parentWorld * n->transform();
		const bool wireframe = A3D_MASK_CONTAINS(debugOptions, DebugOptions::ShowWireframes);

		if (auto* mesh = n->mesh().get()) {

			// temporary?
			output.meshInstances.push_back({mesh, world});

			const auto& elements = mesh->elements();
			const auto& materials = mesh->materials();

			for (size_t e = 0; e < elements.size(); ++e) {
				auto* element = elements[e].get();

				Material* mat = nullptr;
				if (!materials.empty()) {
					const size_t index = e % materials.size();
					mat = materials[index].get();
				}
				if (!mat) mat = Material::DefaultMaterial().get();

				RenderItem item;
				item.mesh = mesh;
				item.elementIndex = e;
				item.element = element;
				item.material = mat;
				item.model = world;
				item.aabb = element->worldAABB(world, false);
				item.transparent = (mat->blendFunction() != BlendFunction::Disabled);

				// ! temnporary !
#warning TEMPORARY
				if (wireframe) item.style = RenderStyle::Wireframe; // TODO: test WireframeOverlay
				else item.style = RenderStyle::Normal;

				// depth: view-space Z of bounds center
				const vec3 center = (item.aabb.min + item.aabb.max) / 2.0f;
				const vec4 vpos = view * vec4(center, 1.0f);
				item.depth = vpos.z;

				output.renderItems.push_back(item);

				++stats.numElements;
				stats.numPolygons += element->faces().size();
			}

			++stats.numMeshes;
		}

		output.scene = &scene; // TODO: maybe change to AABB directly?

		output.backgroundMaterial = scene.visualWorld()->backgroundMaterial();

		if (n->light()) {
			output.lightNodes.push_back(n);
			++stats.numLights;
		}

		for (const auto& c : n->children()) {
			stack.push_back({c.get(), world});
		}
	}

	output.physicsDebugLines = [&]{
		if (physicalWorld) {
			if (auto bwp = dynamic_cast<BulletWorldProxy*>(physicalWorld->proxy())) {
				return bwp->debugLines(debugOptions);
			}
		}
		return vector<Line>{};
	}();

//	if (physicalWorld) {
//		if (auto bwp = dynamic_cast<BulletWorldProxy*>(physicalWorld->proxy())) {
//			output.physicsDebugLines = &(bwp->debugLines(debugOptions));
//		}
//	}

	return output;
}
