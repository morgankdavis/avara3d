//
//  OpenGLRenderer.h
//  avara3d
//
//  Created by Morgan Davis on 4/24/18.
//  Copyright © 2024 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_OPENGLRENDERER_H
#define AVARA3D_OPENGLRENDERER_H

#include <map>
#include <string>
#include <unordered_set>
#include <utility>

#include "a3d/Types.h"
#include "a3d/rendering/renderer/Renderer.h"

class ImFont;

namespace a3d {

	class Color;
	class Font;
	class Mesh;
	class MeshElement;
	class Line;
	class Texture;
	
	class OpenGLRenderer : public Renderer {

	public:
		/// Internal Types ///

		/* <a3d::MeshElement* : <gl_vbo, gl_vao, gl_ebo>> */
		using MeshElementGLMapping =
				std::map<MeshElement*, std::tuple<unsigned, unsigned, unsigned>>;

		/* <a3d::Texture* : <gl_textureHandle> */
		using TextureGLMapping =
				std::map<Texture*, unsigned>;

		/* <vector<a3d::Line>>* : <gl_vbo, gl_vao>> */
		using LinesGLMapping =
				std::map<const std::vector<Line>*, std::pair<unsigned, unsigned>>;

		/// Private Static Members ///

		using GLGetProcAddress = void* (*)(const char* name);
		static bool InitGL(GLGetProcAddress getProcAddress);

		/// Internal Lifecycle Functions ///

		OpenGLRenderer();
		OpenGLRenderer(const OpenGLRenderer& other) = delete; // copy constructor
		OpenGLRenderer& operator=(const OpenGLRenderer& other) = delete; // copy assignment
		OpenGLRenderer(OpenGLRenderer&& other) = delete; // move constructor
		OpenGLRenderer& operator=(OpenGLRenderer&& other) = delete; // move assignment
		~OpenGLRenderer() override;

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

		void 					render(const Scene& scene,
									   const RenderContext& context,
									   const DebugOptions& debugOptions,
									   FrameStats& stats) override;
		void 					render(Mesh& mesh,
									   const RenderContext& context,
									   const math::mat4& modelMat,
									   const math::mat4& viewMat,
									   const math::mat4& projectionMat,
									   const DebugOptions& debugOptions,
									   FrameStats& stats) override;
		void 					render(MeshElement& element,
									   const RenderContext& context,
									   Material& material,
									   const math::mat4& modelMat,
									   const math::mat4& viewMat,
									   const math::mat4& projectionMat,
									   const DebugOptions& debugOptions,
									   FrameStats& stats) override;
		void 					render(const std::vector<Line>& lines,
									   const RenderContext& context,
									   const math::mat4& modelMat,
									   const math::mat4& viewMat,
									   const math::mat4& projectionMat) override;

		std::unique_ptr<Image> 	snapshot(const RenderContext& context) const override;

//		void					viewportScaleChanged(const RenderContext& context) override;

	private:
		/// Private Member Variables ///

		//RenderContext*									_context;
		bool											_isInitialized;
		MeshElementGLMapping 							_meshElementGLMapping;
		TextureGLMapping								_textureGLMapping;
		LinesGLMapping									_linesGLMapping;
		std::unordered_set<MeshElement*>				_activeMeshElements;
		std::unordered_set<Texture*>					_activeTextures;
		std::unordered_set<const std::vector<Line>*>	_activeLines;
		unsigned										_glEnvironmentUBO;
		ImFont*											_overlayTitleImFont;
		ImFont*											_overlayBodyImFont;
	};
}

#endif /* AVARA3D_OPENGLRENDERER_H */
