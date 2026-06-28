//
//  GatherOutput.h
//  avara3d
//
//  Created by Morgan Davis on 1/3/26.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_RENDER_GATHEROUTPUT_H
#define AVARA3D_RENDER_GATHEROUTPUT_H

#include <memory>
#include <vector>

#include "a3d/Math.h"
#include "a3d/mesh/AABB.h"
#include "a3d/mesh/Line.h"

namespace a3d {

	class Line;
	class Material;
	class Mesh;
	class MeshElement;
	class Node;
	class PhysicalWorld;
	class Scene;

	/// Internal Types ///

	enum class RenderStyle : uint8_t {
		Normal,
		Wireframe,
		WireframeOverlay
	};

	struct RenderItem {
		Mesh*			mesh = 			nullptr; //probably remove after handle conversion
		VertexLayout 	layout = 		VertexLayout::None;
		uint32_t 		elementIndex = 	0;
		MeshElement*	element = 		nullptr; // TODO: remove
		Material*		material = 		nullptr;
		RenderStyle 	style = 		RenderStyle::Normal;
		math::mat4 		model = 		math::mat4(1.0f);
		AABB 			aabb = 			AABB::Zero(); // world space
		float 			depth = 		0.0f; // view-space depth for later
		bool 			transparent = 	false; // for later -- always false in BlendFunction
	};

	struct GatherOutput {
		std::vector<RenderItem> 	renderItems =			{};
		const Scene*				scene =					nullptr; // debug AABB
		std::shared_ptr<Material> 	backgroundMaterial =	nullptr;
		std::vector<Node*> 			lightNodes =			{};
		std::vector<Line> 			debugLines =			{};
	};
}

#endif //AVARA3D_RENDER_GATHEROUTPUT_H
