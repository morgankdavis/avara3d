//
//  Renderer.h
//  avara3d
//
//  Created by Morgan Davis on 4/22/18.
//  Copyright © 2024 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_RENDERER_H
#define AVARA3D_RENDERER_H

#include <memory>
#include <set>

#include "glm/glm.hpp"

#include "a3d/Types.h"

namespace a3d {

	class FrameStatsHistory;
	class Image;
	class Line;
	class Node;
	class Material;
	class Mesh;
	class MeshElement;
	class Point;
	class Profiler;
	class RenderContext;
	class Scene;

	class Renderer {

	public:
		/// Internal Lifecycle Functions ///

		Renderer();
		Renderer(const Renderer& other) = delete; // copy constructor
		Renderer& operator=(const Renderer& other) = delete; // copy assignment
		virtual ~Renderer() = 0;

		/// Internal Member Functions ///

		virtual RenderingApi 			renderingApi() const = 0;

		virtual bool 					initialize(const RenderContext& context) = 0;
		virtual bool					isInitialized() const = 0;

		virtual void 					beginFrame(const Scene& scene,
												   const RenderContext& context,
												   const DebugOptions& debugOptions,
												   FrameStats& stats,
												   Profiler& profiler) = 0;
		virtual void 					endFrame(const Scene& scene,
												 const RenderContext& context,
												 const DebugOptions& debugOptions,
												 FrameStats& stats,
												 Profiler& profiler,
												 const FrameStatsHistory& statsHistory) = 0;

		virtual void 					preTraversal(const Scene& scene,
												   const RenderContext& context,
												   const DebugOptions& debugOptions,
													 FrameStats& stats) = 0;
		virtual void 					postTraversal(const Scene& scene,
												 const RenderContext& context,
												 const std::vector<Node*>& lightNodes,
												 const DebugOptions& debugOptions,
													  FrameStats& stats) = 0;

		virtual void 					render(const Scene& scene,
											   const RenderContext& context,
											   const DebugOptions& debugOptions,
											   FrameStats& stats) = 0;
		virtual void 					render(Mesh& mesh,
											   const RenderContext& context,
											   const math::mat4& modelMat,
											   const math::mat4& viewMat,
											   const math::mat4& projectionMat,
											   const DebugOptions& debugOptions,
											   FrameStats& stats) = 0;
		virtual void 					render(MeshElement& element,
											   const RenderContext& context,
											   Material& material,
											   const math::mat4& modelMat,
											   const math::mat4& viewMat,
											   const math::mat4& projectionMat,
											   const DebugOptions& debugOptions,
											   FrameStats& stats) = 0;
		virtual void 					render(const std::vector<Line>& lines,
											   const RenderContext& context,
											   const math::mat4& modelMat,
											   const math::mat4& viewMat,
											   const math::mat4& projectionMat) = 0;

		virtual std::unique_ptr<Image>	snapshot(const RenderContext& context) const = 0;

		virtual void					framebufferScaleChanged(const RenderContext& context) = 0;
	};
}

#endif /* AVARA3D_RENDERER_H */
