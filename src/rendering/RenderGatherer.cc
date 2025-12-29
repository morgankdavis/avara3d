
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

PipelineKey RenderGatherer::MakePipelineKey(const Material& material,
											uint32_t vertexLayoutKey) {

	PipelineKey k{};
	k.vertexLayoutKey = vertexLayoutKey;

	k.fillMode      = material.fillMode();
	k.blendFunction = material.blendFunction();
	k.doubleSided   = material.doubleSided();
	k.shaderKind    = ShaderKind::Default;

	// enforce sane combos based on alphaMode
	switch (material.alphaMode()) {
		case AlphaMode::Opaque:
		case AlphaMode::Mask:
			// Mask is *not* blending — it's discard/alpha-test.
			k.blendFunction = BlendFunction::Disabled;
			break;

		case AlphaMode::Blend:
			if (k.blendFunction == BlendFunction::Disabled) {
				k.blendFunction = BlendFunction::Alpha;
			}
			break;
	}

	return k;
}

PipelineKey RenderGatherer::MakeMainOpaquePipelineKey(const RenderItem& item, uint32_t vertexLayoutKey) {
	PipelineKey k = MakePipelineKey(*item.material, vertexLayoutKey);
	k.pass = PassKind::MainOpaque;
	k.depthWrite = true;
	k.polygonOffset = false;
	return k;
}

PipelineKey RenderGatherer::MakeMainMaskPipelineKey(const RenderItem& item, uint32_t vertexLayoutKey) {
	PipelineKey k = MakePipelineKey(*item.material, vertexLayoutKey);
	k.pass = PassKind::MainMask;
	k.depthWrite = true;          // keep early-z, just punch holes in shader
	k.polygonOffset = false;
	return k;
}

PipelineKey RenderGatherer::MakeMainTransparentPipelineKey(const RenderItem& item, uint32_t vertexLayoutKey) {
	PipelineKey k = MakePipelineKey(*item.material, vertexLayoutKey);
	k.pass = PassKind::MainTransparent;
	k.depthWrite = false;         // critical for blending correctness
	k.polygonOffset = false;
	return k;
}

PipelineKey RenderGatherer::MakeWirePipelineKey(const RenderItem& item, uint32_t vertexLayoutKey) {
	PipelineKey k = MakePipelineKey(*item.material, vertexLayoutKey);

	k.pass = PassKind::Wire;
	k.shaderKind = ShaderKind::Wireframe;        // wire program
	k.fillMode = FillMode::Lines;                // force lines
//	k.blendFunction = BlendFunction::Disabled;   // force off for now
	k.doubleSided = true;                        // debug preference
	k.depthWrite = false;                        // key point for overlay correctness
	k.polygonOffset = true;                      // key point for z-fighting

	return k;
}





static uint32_t PtrHash32(const void* p) {
	uintptr_t v = (uintptr_t)p >> 4;              // drop alignment bits
	uint32_t lo = (uint32_t)v;
	uint32_t hi = (uint32_t)(v >> 32);
	uint32_t h  = lo ^ hi;
	h ^= (h >> 16);
	return h;
}

static uint16_t PtrHash16(const void* p) {
	uint32_t h = PtrHash32(p);
	return (uint16_t)(h ^ (h >> 16));
}

static uint64_t MakeSortKey(const DrawItem& it) {
	uint64_t k = (uint64_t)(uint32_t)it.pipeline << 32;
	k |= (uint64_t)PtrHash16(it.material) << 16;
	k |= (uint64_t)PtrHash16(it.element);
	return k;
}



static int PassOrder(PassKind p) {
	switch (p) {
		case PassKind::MainOpaque:      return 0;
		case PassKind::MainMask:        return 1;
		case PassKind::MainTransparent: return 2; // later you’ll change strategy
		case PassKind::Wire:            return 3;
	}
	return 99;
}

static void SortDrawItems(std::vector<DrawItem>& items) {
	std::sort(items.begin(), items.end(),
			  [](const DrawItem& a, const DrawItem& b) {
				  int pa = PassOrder(a.pass), pb = PassOrder(b.pass);
				  if (pa != pb) return pa < pb;
				  if (a.sortKey != b.sortKey) return a.sortKey < b.sortKey;
				  return a.sequence < b.sequence;
			  });
}







RenderPacket RenderGatherer::BuildRenderPacket(const GatherOutput& gatherOutput,
											   RenderResourceCacheOGL& cache,
											   uint32_t vertexLayoutKey) {

	RenderPacket packet{};
	packet.lightNodes = gatherOutput.temp_lightNodes;
	packet.main.reserve(gatherOutput.renderItems.size());
	packet.wireframe.reserve(gatherOutput.renderItems.size());

	uint32_t seq = 0;

	for (const RenderItem& ri : gatherOutput.renderItems) {

		// effective style for this instance (refine later?)
		RenderStyle style = ri.style;

		if (style == RenderStyle::Normal || style == RenderStyle::WireframeOverlay) {

			DrawItem di;
//			di.mesh = ri.mesh;
			di.elementIndex = ri.elementIndex;
			di.element = ri.element;
			di.material = ri.material;
			di.model = ri.model;
			di.depth = ri.depth;

			switch (ri.material->alphaMode()) {
				case AlphaMode::Opaque: {
					di.pass = PassKind::MainOpaque;
					di.key = MakeMainOpaquePipelineKey(ri, vertexLayoutKey);
					break; }
				case AlphaMode::Mask: {
					di.pass = PassKind::MainMask;
					di.key = MakeMainMaskPipelineKey(ri, vertexLayoutKey);
					break; }
				case AlphaMode::Blend: {
					di.pass = PassKind::MainTransparent;
					di.key = MakeMainTransparentPipelineKey(ri, vertexLayoutKey);
					break; }
				default: /* unreachable */ break;
			}

			di.pipeline = cache.ensurePipeline(di.key);

			di.sortKey = MakeSortKey(di);
			di.sequence = seq++;

			packet.main.push_back(std::move(di));
		}

		if (style == RenderStyle::Wireframe || style == RenderStyle::WireframeOverlay) {

			DrawItem di;
//			di.mesh = ri.mesh;
			di.elementIndex = ri.elementIndex;
			di.element = ri.element;
			di.material = ri.material; // optional for wire, fine to keep
			di.model = ri.model;
			di.depth = ri.depth;
			di.pass = PassKind::Wire;

			di.key = MakeWirePipelineKey(ri, vertexLayoutKey);
			di.pipeline = cache.ensurePipeline(di.key);

			di.sortKey = MakeSortKey(di);
			di.sequence = seq++;

			packet.wireframe.push_back(std::move(di));
		}
	}

	return packet;
}
