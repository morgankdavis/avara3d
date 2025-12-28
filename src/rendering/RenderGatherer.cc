
#include "a3d/rendering/RenderGatherer.h"

#include "a3d/rendering/RenderItem.h"
#include "a3d/rendering/RenderPacket.h"
#include "a3d/rendering/RenderResourceCacheOGL.h"
#include "a3d/rendering/context/RenderContext.h"
#include "a3d/rendering/material/Material.h"
#include "a3d/mesh/Mesh.h"
#include "a3d/mesh/MeshElement.h"
#include "a3d/scene/Node.h"
#include "a3d/scene/Scene.h"

using namespace a3d;
using namespace a3d::math;
using namespace std;

struct GatherEntry {
	Node* 		node;
	math::mat4 	parentWorld;
};

GatherOutput RenderGatherer::GatherRenderItems(const Scene& scene,
											   const RenderContext& context,
											   const mat4& view,
											   const DebugOptions& debugOptions, // ! temporary !
											   FrameStats& stats) {

	GatherOutput out{};
	out.renderItems.reserve(1024);
	out.temp_meshInstances.reserve(512);
	out.temp_lightNodes.reserve(64);

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

			// ! temporary !
			out.temp_meshInstances.push_back({mesh, world});

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

				out.renderItems.push_back(item);

				++stats.numElements;
				stats.numPolygons += element->faces().size();
			}

			++stats.numMeshes;
		}

		// ! temporary !
		if (n->light()) {
			out.temp_lightNodes.push_back(n);
			++stats.numLights;
		}

		for (const auto& c : n->children()) {
			stack.push_back({c.get(), world});
		}
	}

	return out;
}

PipelineKey RenderGatherer::ComputePipelineKey(const Material& material,
											   uint32_t vertexLayoutKey) {
	PipelineKey k{};

	k.vertexLayoutKey = vertexLayoutKey;
	k.fillMode = material.fillMode();
	k.blendFunction = material.blendFunction();
	k.doubleSided = material.doubleSided();
	k.shaderKind = ShaderKind::Default;

//	const bool hasEmission = !std::holds_alternative<std::monostate>(material.emission());
//	k.shaderKind = hasEmission ? ShaderKind::Default : ShaderKind::Default;

	return k;
}

PipelineKey RenderGatherer::MakeMainKey(const RenderItem& item, uint32_t vertexLayoutKey) {
	PipelineKey k = ComputePipelineKey(*item.material, vertexLayoutKey);

	k.pass = PassKind::Main;
	k.shaderKind = ShaderKind::Default;          // main
	k.fillMode = FillMode::Fill;
//	k.blendFunction = BlendFunction::Disabled;
//	k.doubleSided = item.material->doubleSided();
	k.depthWrite = true;
	k.polygonOffset = false;

	return k;
}

PipelineKey RenderGatherer::MakeWireKey(const RenderItem& item, uint32_t vertexLayoutKey) {
	PipelineKey k = ComputePipelineKey(*item.material, vertexLayoutKey);

	k.pass = PassKind::Wire;
	k.shaderKind = ShaderKind::Wireframe;        // wire program
	k.fillMode = FillMode::Lines;                // force lines
//	k.blendFunction = BlendFunction::Disabled;   // force off for now
	k.doubleSided = true;                        // debug preference
	k.depthWrite = false;                        // key point for overlay correctness
	k.polygonOffset = true;                      // key point for z-fighting

	return k;
}

RenderPacket RenderGatherer::BuildRenderPacket(const GatherOutput& gatherItems,
											   RenderResourceCacheOGL& cache,
											   uint32_t vertexLayoutKey) {

	RenderPacket packet;
	packet.lightNodes = gatherItems.temp_lightNodes;
	packet.main.reserve(gatherItems.renderItems.size());
	packet.wire.reserve(gatherItems.renderItems.size());

	for (const RenderItem& ri : gatherItems.renderItems) {

		// effective style for this instance (refine later?)
		RenderStyle style = ri.style;

		if (style == RenderStyle::Normal || style == RenderStyle::WireframeOverlay) {
			DrawItem di;
			di.mesh = ri.mesh;
			di.elementIndex = ri.elementIndex;
			di.element = ri.element;
			di.material = ri.material;
			di.model = ri.model;
			di.depth = ri.depth;
			di.pass = PassKind::Main;

			PipelineKey key = MakeMainKey(ri, vertexLayoutKey);
			di.pipeline = cache.ensurePipeline(key);

			packet.main.push_back(std::move(di));
		}

		if (style == RenderStyle::Wireframe || style == RenderStyle::WireframeOverlay) {
			DrawItem di;
			di.mesh = ri.mesh;
			di.elementIndex = ri.elementIndex;
			di.element = ri.element;
			di.material = ri.material; // optional for wire, fine to keep
			di.model = ri.model;
			di.depth = ri.depth;
			di.pass = PassKind::Wire;

			PipelineKey key = MakeWireKey(ri, vertexLayoutKey);
			di.pipeline = cache.ensurePipeline(key);

			packet.wire.push_back(std::move(di));
		}
	}

	return packet;
}
