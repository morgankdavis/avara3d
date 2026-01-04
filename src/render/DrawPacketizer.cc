//
//  DrawPacketizer.cc
//  avara3d
//
//  Created by Morgan Davis on 12/28/25.
//  Copyright © 2025 Morgan K Davis. All rights reserved.
//

#include "a3d/render/DrawPacketizer.h"

#include "a3d/mesh/Line.h"
#include "a3d/mesh/Mesh.h"
#include "a3d/mesh/MeshElement.h"
#include "a3d/render/DrawPacket.h"
#include "a3d/render/GatherOutput.h"
#include "a3d/render/PipelineKey.h"
#include "a3d/scene/Scene.h"
#include "a3d/visual/material/Material.h"

using namespace a3d;
using namespace a3d::math;
using namespace std;







static PipelineKey MakePipelineKey(const Material& material, uint32_t vertexLayoutKey);
static PipelineKey MakeBackgroundPipelineKey();
static PipelineKey MakeMainOpaquePipelineKey(const RenderItem& item, uint32_t vertexLayoutKey);
static PipelineKey MakeMainMaskPipelineKey(const RenderItem& item, uint32_t vertexLayoutKey);
static PipelineKey MakeMainTransparentPipelineKey(const RenderItem& item, uint32_t vertexLayoutKey);
static PipelineKey MakeWireframePipelineKey(const RenderItem& item, uint32_t vertexLayoutKey);
static PipelineKey MakeLinesPipelineKey();
static void ApplyDepthPolicy(PipelineKey& key);



PipelineKey MakePipelineKey(const Material& material,
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
		case AlphaMode::Mask: // mask is not blending — it's discard/alpha-test
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

PipelineKey MakeBackgroundPipelineKey() {
	PipelineKey k{};
	k.pass          = PassKind::Background;
	k.shaderKind    = ShaderKind::Skybox;
	k.fillMode      = FillMode::Fill;
	k.doubleSided   = true; // maybe change winding order instead
	k.depthTest  = true;
	k.depthWrite    = false;
	k.depthFunc = DepthFunc::Lequal;
	k.blendFunction = BlendFunction::Disabled;
	k.polygonOffset = false;
	//ApplyDepthPolicy(k);
	return k;
}

PipelineKey MakeMainOpaquePipelineKey(const RenderItem& item, uint32_t vertexLayoutKey) {
	PipelineKey k = MakePipelineKey(*item.material, vertexLayoutKey);
	k.pass = PassKind::MainOpaque;
	k.depthWrite = true;
	k.polygonOffset = false;
	ApplyDepthPolicy(k);
	return k;
}

PipelineKey MakeMainMaskPipelineKey(const RenderItem& item, uint32_t vertexLayoutKey) {
	PipelineKey k = MakePipelineKey(*item.material, vertexLayoutKey);
	k.pass = PassKind::MainMask;
	k.depthWrite = true;          // keep early-z, just punch holes in shader
	k.polygonOffset = false;
	ApplyDepthPolicy(k);
	return k;
}

PipelineKey MakeMainTransparentPipelineKey(const RenderItem& item, uint32_t vertexLayoutKey) {
	PipelineKey k = MakePipelineKey(*item.material, vertexLayoutKey);
	k.pass = PassKind::MainTransparent;
	k.depthWrite = false;         // critical for blending correctness
	k.polygonOffset = false;
	ApplyDepthPolicy(k);
	return k;
}

PipelineKey MakeWireframePipelineKey(const RenderItem& item, uint32_t vertexLayoutKey) {
	PipelineKey k = MakePipelineKey(*item.material, vertexLayoutKey);
	k.pass = PassKind::Wireframe;
	k.shaderKind = ShaderKind::Wireframe;        // wire program
	k.fillMode = FillMode::Lines;                // force lines
//	k.blendFunction = BlendFunction::Disabled;   // force off for now
	k.doubleSided = true;                        // debug preference
	k.depthWrite = false;                        // key point for overlay correctness
	k.polygonOffset = true;                      // key point for z-fighting
	ApplyDepthPolicy(k);
	return k;
}

PipelineKey MakeLinesPipelineKey() {
	PipelineKey k{};
	k.pass          = PassKind::Lines;
	k.shaderKind    = ShaderKind::Lines;          // ✅ REQUIRED
	k.fillMode      = FillMode::Lines;             // irrelevant for GL_LINES
	k.doubleSided   = true;
	k.depthWrite    = false;
	k.blendFunction = BlendFunction::Disabled;
	k.polygonOffset = false;
	ApplyDepthPolicy(k);
	return k;
}

void ApplyDepthPolicy(PipelineKey& key) {
	// defaults (main pass)
	key.depthTest  = true;
	key.depthWrite = true;
	key.depthFunc  = DepthFunc::Less;

	// overlays
	if (key.pass == PassKind::Lines || key.pass == PassKind::Wireframe) {
		key.depthTest  = true;
		key.depthWrite = false;
		key.depthFunc  = DepthFunc::Lequal;
	}
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

//static uint64_t MakeSortKey(const DrawItem& it) {
//	//uint64_t k = (uint64_t)(uint32_t)it.pipeline << 32;
//	// sort by pipeline *configuration* before the pipeline handle exists
//	const size_t h = PipelineKeyHash{}(it.key);
//	const uint32_t hk = (uint32_t)(h ^ (h >> 32)); // fold to 32-bit
//	uint64_t k = (uint64_t)hk << 32;
//	k |= (uint64_t)PtrHash16(it.material) << 16;
//	k |= (uint64_t)PtrHash16(it.element);
//	return k;
//}

static inline uint32_t FoldHash32(size_t h) {
	uint32_t x = (uint32_t)h ^ (uint32_t)(h >> 32);
	// cheap mix (avalanche-ish)
	x ^= x >> 16;
	x *= 0x7feb352d;
	x ^= x >> 15;
	x *= 0x846ca68b;
	x ^= x >> 16;
	return x;
}

static uint64_t MakeSortKey(const DrawItem& it) {
	const uint32_t hk = FoldHash32(PipelineKeyHash{}(it.key));
	uint64_t k = (uint64_t)hk << 32;
	k |= (uint64_t)PtrHash16(it.material) << 16;
	k |= (uint64_t)PtrHash16(it.element);
	return k;
}



static int PassOrder(PassKind p) {
	switch (p) {
		case PassKind::Background:		return 0;
		case PassKind::MainOpaque:		return 1;
		case PassKind::MainMask:		return 2;
		case PassKind::MainTransparent:	return 3; // later you’ll change strategy
		case PassKind::Wireframe:		return 4;
		case PassKind::Lines:			return 5;
	}
	return 99;
}

static void SortDrawItems(vector<DrawItem>& items) {
	std::sort(items.begin(), items.end(),
			  [](const DrawItem& a, const DrawItem& b) {
				  int pa = PassOrder(a.pass), pb = PassOrder(b.pass);
				  if (pa != pb) return pa < pb;
				  if (a.sortKey != b.sortKey) return a.sortKey < b.sortKey;
				  return a.sequence < b.sequence;
			  });
}












static void AppendBoxLinesFromCorners(vector<Line>& out,
									  const math::vec3 c[8],
									  const Color& color) {
	// indices: 0..3 top ring, 4..7 bottom ring (match your naming if you like)
	auto add = [&](int a, int b) { out.push_back(Line{c[a], c[b], color}); };

	// top
	add(0,1); add(1,2); add(2,3); add(3,0);
	// bottom
	add(4,5); add(5,6); add(6,7); add(7,4);
	// verticals
	add(0,4); add(1,5); add(2,6); add(3,7);
}

static void AppendAABBLinesWorld(vector<Line>& out,
								 const AABB& aabb,
								 const Color& color) {
	math::vec3 c[8] = {
			{aabb.min.x, aabb.max.y, aabb.min.z}, // 0
			{aabb.min.x, aabb.max.y, aabb.max.z}, // 1
			{aabb.max.x, aabb.max.y, aabb.max.z}, // 2
			{aabb.max.x, aabb.max.y, aabb.min.z}, // 3
			{aabb.min.x, aabb.min.y, aabb.min.z}, // 4
			{aabb.min.x, aabb.min.y, aabb.max.z}, // 5
			{aabb.max.x, aabb.min.y, aabb.max.z}, // 6
			{aabb.max.x, aabb.min.y, aabb.min.z}, }; // 7

	AppendBoxLinesFromCorners(out, c, color);
}

static void AppendOBBLinesFromLocalAABB(vector<Line>& out,
										const AABB& local,
										const math::mat4& model,
										const Color& color) {
	math::vec3 lc[8] = {
			{local.min.x, local.max.y, local.min.z},
			{local.min.x, local.max.y, local.max.z},
			{local.max.x, local.max.y, local.max.z},
			{local.max.x, local.max.y, local.min.z},
			{local.min.x, local.min.y, local.min.z},
			{local.min.x, local.min.y, local.max.z},
			{local.max.x, local.min.y, local.max.z},
			{local.max.x, local.min.y, local.min.z} };

	math::vec3 wc[8];
	for (int i = 0; i < 8; ++i) {
		auto h = model * math::vec4(lc[i], 1.0f);
		wc[i] = math::vec3(h.x, h.y, h.z); // assuming affine; otherwise divide by h.w
	}

	AppendBoxLinesFromCorners(out, wc, color);
}











// turn gathered items into backend-agnostic DrawItems + pass structs (no GL resources)
// "build / buildDrawList / buildCommandList / record"
DrawPacket DrawPacketizer::Packetize(GatherOutput& gatherOutput,
									 uint32_t vertexLayoutKey,
									 const DebugOptions& debugOptions) {

	DrawPacket packet{};

	packet.mainPassItems.reserve(gatherOutput.renderItems.size());
	packet.wireframePassItems.reserve(gatherOutput.renderItems.size());

	packet.backgroundPass.pipeline  = INVALID_PIPELINE_HANDLE;
	packet.backgroundPass.key       = MakeBackgroundPipelineKey();
	packet.backgroundPass.material  = gatherOutput.backgroundMaterial;

	for (const RenderItem& ri : gatherOutput.renderItems) {

		// effective style for this instance (refine later?)
		RenderStyle style = ri.style;

		if (style == RenderStyle::Normal || style == RenderStyle::WireframeOverlay) {

			DrawItem di;
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

			di.pipeline = INVALID_PIPELINE_HANDLE; // resolved later

			di.sortKey = MakeSortKey(di);
			di.sequence = (uint32_t)packet.mainPassItems.size();

			packet.mainPassItems.push_back(std::move(di));
		}

		if (style == RenderStyle::Wireframe || style == RenderStyle::WireframeOverlay) {

			DrawItem di;
			di.elementIndex = ri.elementIndex;
			di.element = ri.element;
			di.material = ri.material; // optional for wire, fine to keep
			di.model = ri.model;
			di.depth = ri.depth;
			di.pass = PassKind::Wireframe;

			di.key = MakeWireframePipelineKey(ri, vertexLayoutKey);
			di.pipeline = INVALID_PIPELINE_HANDLE; // resolved later

			di.sortKey = MakeSortKey(di);
			di.sequence = (uint32_t)packet.wireframePassItems.size();

			packet.wireframePassItems.push_back(std::move(di));
		}
	}

	packet.lightNodes = std::move(gatherOutput.lightNodes);

	{
		auto &lp = packet.linesPass;
		lp.model = math::mat4(1.0f);

		lp.lines = std::move(gatherOutput.physicsDebugLines);

		const bool showBounds = A3D_MASK_CONTAINS(debugOptions, DebugOptions::ShowBoundingBoxes);
		if (showBounds) {
			// reserve: keep what we already have (physics) + our bbox lines
			const size_t perMesh = 24;                 // 12 OBB + 12 AABB
			const size_t sceneAabb = 12;               // scene AABB
			lp.lines.reserve(lp.lines.size() + gatherOutput.meshInstances.size() * perMesh + sceneAabb);

			// order doesn’t really matter with depthTest + no blending, but
			// if you care about "last wins" style later, do physics first then bounds (as here).
			for (const MeshInstance &mi: gatherOutput.meshInstances) {
				AppendOBBLinesFromLocalAABB(lp.lines, mi.mesh->localAABB(), mi.model, *Color::Gray());
				AppendAABBLinesWorld(lp.lines, mi.mesh->worldAABB(mi.model, false), *Color::Red());
			}
			AppendAABBLinesWorld(lp.lines, gatherOutput.scene->aabb(false), *Color::Green());
		}

		lp.key = MakeLinesPipelineKey();
		lp.pipeline = INVALID_PIPELINE_HANDLE; // resolved later if lines exist
	}

	SortDrawItems(packet.mainPassItems);
	SortDrawItems(packet.wireframePassItems);

	return packet;
}
