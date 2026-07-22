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
			PipelineId 			pipelineId = 	INVALID_PIPELINE_ID;
			gl::uint_t 			program = 			0; // currently bound GL program
			const Material* 	material = 			nullptr; // last bound material
			uint32_t 			indexCount = 		0;
		};

		struct BoundElement {
			gl::uint_t 		vao = 			0;
			gl::sizei_t 	indexCount = 	0;
			gl::enum_t		indexType = 	gl::value::unsigned_int;
			gl::sizei_t 	vertexCount =	0;
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

		bool 					initialize(const RenderContext& context) override;
		bool					isInitialized() const override;

		void 					beginFrame(const Scene& scene,
										   const RenderContext& context,
										   const Scene::DebugOptions& debugOptions,
										   FrameStats& stats,
										   Profiler& profiler) override;
		void 					endFrame(const Scene& scene,
										 const RenderContext& context,
										 const Scene::DebugOptions& debugOptions,
										 FrameStats& stats,
										 Profiler& profiler,
										 const FrameStatsHistory& statsHistory) override;

		void 					preTraversal(const Scene& scene,
											 const RenderContext& context,
											 const Scene::DebugOptions& debugOptions,
											 FrameStats& stats) override;
		void 					postTraversal(const Scene& scene,
											  const RenderContext& context,
											  const std::vector<Node*>& lightNodes,
											  const Scene::DebugOptions& debugOptions,
											  FrameStats& stats) override;

		void 					clear(const ClearCommand& cmd,
									  const RenderContext& context) override;

		void 					renderPacket(DrawPacket& packet, const FrameParams& frame) override;

		std::unique_ptr<Image> 	snapshot(const RenderContext& context) const override;

	protected:
		/// Renderer Protected Member Functions ///

		void 					drawBackground(const BackgroundPass& backgroundPass,
											   const math::mat4& view,
											   const math::mat4& proj) override;

		void 					bindPipeline(PipelineId pipelineId,
		                                     const OGLResourceCache& cache) override;
		void 					bindMaterial(const Material& material) override;
		void 					bindMeshElement(const MeshElement& element) override;
		void 					applyMVP(const math::mat4& model,
										 const math::mat4& view,
										 const math::mat4& proj) override;
		void 					drawElements() override;
		void					draw(const DrawCommand& cmd);

		void 					renderLinesPass(const LinesPass& pass,
												const RenderContext& context,
												const math::mat4& view,
												const math::mat4& proj) override;

		void 					resolvePacket(DrawPacket& packet,
											   const FrameParams& frame) override;
		void 					drawPacket(const DrawPacket& packet,
										const FrameParams& frame) override;

	private:
		/// Private Member Functions ///

		void 					drawDebugLines(const math::mat4& model,
											  const math::mat4& view,
											  const math::mat4& proj);

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
		ImFont*					_overlayAltImFont;
		OGLDrawTimer			_drawTimer;
	};
}

#endif // AVARA3D_RENDER_BACKEND_OPENGL_OGLRENDERER_H
