//
//  OGLRenderer.h
//  avara3d
//
//  Created by Morgan Davis on 4/24/18.
//  Copyright © 2024 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_RENDER_BACKEND_OPENGL_OGLRENDERER_H
#define AVARA3D_RENDER_BACKEND_OPENGL_OGLRENDERER_H

#include <map>
#include <string>
#include <unordered_set>
#include <utility>

#include "a3d/Types.h"
#include "a3d/render/Renderer.h"
#include "a3d/render/backend/opengl/GLTypes.h"
#include "a3d/render/backend/opengl/OGLDebugLines.h"
#include "a3d/render/backend/opengl/OGLDrawTimer.h"
#include "a3d/render/backend/opengl/OGLResourceCache.h"

class ImFont;

namespace a3d {

	struct LinesPass;

	class Color;
	class Font;
	class Mesh;
	class MeshElement;
	class Line;
	class Texture;

	class OGLRenderer : public Renderer {

	public:
		/// Internal Types ///

		struct GLStateCache {
			PipelineHandle 		pipelineHandle = 	INVALID_PIPELINE_HANDLE;
			a3d::gl::enum_t 	program = 			0; // currently bound GL program
			const Material* 	material = 			nullptr; // last bound material
			uint32_t 			indexCount = 		0;
		};

		struct BoundElement {
			a3d::gl::uint_t 	vao = 			0;
			a3d::gl::sizei_t 	indexCount = 	0;
			a3d::gl::enum_t		indexType = 	a3d::gl::value::unsigned_int;
		};

		/// Internal Static Members ///

		using GLGetProcAddress = void* (*)(const char* name);
		static bool InitGL(GLGetProcAddress getProcAddress);

		/// Internal Lifecycle Functions ///

		OGLRenderer();
		OGLRenderer(const OGLRenderer& other) = delete; // copy constructor
		OGLRenderer& operator=(const OGLRenderer& other) = delete; // copy assignment
		OGLRenderer(OGLRenderer&& other) = delete; // move constructor
		OGLRenderer& operator=(OGLRenderer&& other) = delete; // move assignment
		~OGLRenderer() override;

		/// Renderer Internal Member Functions ///

		RenderingApi 			renderingApi() const override;

		bool 					initialize(const RenderContext& context) override;
		bool					isInitialized() const override;

		void 					beginFrame(const Scene& scene,
										   const RenderContext& context,
										   const DebugOptions& debugOptions,
										   FrameStats& stats,
										   Profiler& profiler) override;
		void 					endFrame(const Scene& scene,
										 const RenderContext& context,
										 const DebugOptions& debugOptions,
										 FrameStats& stats,
										 Profiler& profiler,
										 const FrameStatsHistory& statsHistory) override;

		void 					preTraversal(const Scene& scene,
											 const RenderContext& context,
											 const DebugOptions& debugOptions,
											 FrameStats& stats) override;
		void 					postTraversal(const Scene& scene,
											  const RenderContext& context,
											  const std::vector<Node*>& lightNodes,
											  const DebugOptions& debugOptions,
											  FrameStats& stats) override;

		void 					clear(const ClearCommand& cmd,
									  const RenderContext& context) override;

		void 					renderPacket(DrawPacket& packet, const FrameParams& frame) override;

		std::unique_ptr<Image> 	snapshot(const RenderContext& context) const override;

	protected:
		/// Renderer Protected Member Functions ///

		void 					drawBackground(const BackgroundPass& backgroundPass,
											   const math::mat4& viewMat,
											   const math::mat4& projMat) override;

		void 					bindPipeline(PipelineHandle h,
											 const OGLResourceCache& cache) override;
		void 					bindMaterial(const Material& material) override;
		void 					bindMeshElement(const MeshElement& element) override;
		void 					setPerObject(const math::mat4& model,
											 const math::mat4& view,
											 const math::mat4& projection) override;
		void 					drawBound() override;

		void 					renderLinesPass(const LinesPass& pass,
												const RenderContext& context,
												const math::mat4& viewMat,
												const math::mat4& projectionMat) override;

		void 					resolvePacket(DrawPacket& packet, const FrameParams& frame) override;
		void 					drawPacket(const DrawPacket& packet, const FrameParams& frame) override;

	private:
		/// Private Member Variables ///

		bool					_isInitialized;
		unsigned				_glEnvironmentUBO;
		OGLResourceCache 		_resourceCache;
		GLStateCache 			_state;
		BoundElement 			_boundElement;
		std::unique_ptr<Mesh> 	_skyboxMesh; // should be value?
		OGLDebugLines 			_debugLines;
		ImFont*					_overlayTitleImFont;
		ImFont*					_overlayBodyImFont;
		OGLDrawTimer			_drawTimer;
	};
}

#endif // AVARA3D_RENDER_BACKEND_OPENGL_OGLRENDERER_H
