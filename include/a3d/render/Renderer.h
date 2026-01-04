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

#include "a3d/Types.h"
#include "a3d/render/PipelineKey.h"

namespace a3d {

	struct BackgroundPass;
	struct LinesPass;

	class FrameStatsHistory;
	class Image;
	class Line;
	class Node;
	class Material;
	class Mesh;
	class MeshElement;
	class OGLResourceCache;
	class Point;
	class Profiler;
	class RenderContext;
	class DrawPacket;
	class Scene;

	class Renderer {

	public:

		struct i32Rect {
			int32_t x = 0;
			int32_t y = 0;
			int32_t w = 0;
			int32_t h = 0;
		};

		struct ClearCommand {
			bool 		clearColor = 		true;
			bool 		clearDepth = 		true;
			bool 		clearStencil = 		false;
			math::vec4 	color = 			{0.f, 0.f, 0.f, 1.f};
			float      	depth = 			1.0f;
			int        	stencil = 			0;
			bool 	 	useScissor = 		false;
			i32Rect 	scissorRect =		{};
			// glClear obeys scissor + write masks. if a previous pass did glDepthMask(false)
			// (or colorMask off), clear can do nothing unless overridden
			bool 		forceWriteMasks = 	true;
			// GLuint 	framebuffer = 		0;
			// bool   	bindFramebuffer = 	false;
		};

		struct FrameParams {
			const RenderContext& 	context;
			math::mat4 				view =		math::mat4(1.0f);
			math::mat4 				proj =		math::mat4(1.0f);
			DebugOptions 			debug =		DebugOptions::None;
			FrameStats* 			stats = 	nullptr;
			Profiler* 				profiler = 	nullptr;
		};




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


		virtual void 					clear(const ClearCommand& cmd,
											  const RenderContext& context) = 0;
		virtual void 					renderPacket(DrawPacket& packet,
													 const FrameParams& frame) = 0;

		virtual std::unique_ptr<Image>	snapshot(const RenderContext& context) const = 0;

	protected:

		virtual void 					resolvePacket(DrawPacket& packet,
													  const FrameParams& frame) = 0;
		virtual void 					drawPacket(const DrawPacket& packet,
												   const FrameParams& frame) = 0;

		virtual void 					drawBackground(const BackgroundPass& backgroundPass,
													   const math::mat4& viewMat,
													   const math::mat4& projMat) = 0;


		virtual void 					bindPipeline(PipelineHandle h,
													 const OGLResourceCache& cache) = 0;
		virtual void 					bindMaterial(const Material& material) = 0;
		virtual void 					bindMeshElement(const MeshElement& element) = 0;
		virtual void 					setPerObject(const math::mat4& model,
													 const math::mat4& view,
													 const math::mat4& projection) = 0;
		virtual void 					drawBound() = 0;

		virtual void 					renderLinesPass(const LinesPass& pass,
														const RenderContext& context,
														const math::mat4& viewMat,
														const math::mat4& projectionMat) = 0;
	};
}

#endif /* AVARA3D_RENDERER_H */
