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

#ifdef A3D_GL_ES
#include <EGL/egl.h>
#include <GLES3/gl3.h>
#else
#include "glad/glad.h"
#endif

#include "a3d/Types.h"
#include "a3d/profiling/OpenGLDrawTimer.h"
#include "a3d/rendering/renderer/Renderer.h"

class ImFont;

namespace a3d {

	class Color;
	class Font;
	class Mesh;
	class MeshElement;
	class Line;
	class Program; // TEMPROARY
	class Texture;



	struct GLStateCache {
		PipelineHandle 	pipeline = 		INVALID_PIPELINE;
		GLuint 			program = 		0; // currently bound GL program
		const 			Material* 		material = nullptr; // last bound material
		//ShaderKind 		shaderKind = 	ShaderKind::Default;
		uint32_t 		indexCount = 	0;
	};

	struct BoundElement {
		GLuint 		vao = 			0;
		GLsizei 	indexCount = 	0;
		GLenum 		indexType = 	GL_UNSIGNED_INT;
	};



	class OpenGLRenderer : public Renderer {

	public:
		/// Internal Types ///

		struct MeshElementGLRes {
			GLuint vao = 0;
			GLuint vbo = 0;
			GLuint ebo = 0;
			uint32_t indexCount = 0;
		};

		/* <a3d::MeshElement* : <gl_vbo, gl_vao, gl_ebo>> */
		using MeshElementGLMapping =
				std::map<MeshElement*, std::tuple<unsigned, unsigned, unsigned>>; // TODO: REMOVE

		/* <a3d::Texture* : <gl_textureHandle> */
		using TextureGLMapping =
				std::map<Texture*, unsigned>;

		/* <vector<a3d::Line>>* : <gl_vbo, gl_vao>> */
//		using LinesGLMapping =
//				std::map<const std::vector<Line>*, std::pair<unsigned, unsigned>>;

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
									   const math::mat4& viewMat,
									   const math::mat4& projectionMat,
									   const DebugOptions& debugOptions,
									   FrameStats& stats) override;
//		void 					render(Mesh& mesh,
//									   const RenderContext& context,
//									   const math::mat4& modelMat,
//									   const math::mat4& viewMat,
//									   const math::mat4& projectionMat,
//									   const DebugOptions& debugOptions,
//									   FrameStats& stats) override;
//		void 					render(MeshElement& element,
//									   const RenderContext& context,
//									   Material& material,
//									   const math::mat4& modelMat,
//									   const math::mat4& viewMat,
//									   const math::mat4& projectionMat,
//									   const DebugOptions& debugOptions,
//									   FrameStats& stats) override;
		void 					render(const std::vector<Line>& lines,
									   const RenderContext& context,
									   const math::mat4& modelMat,
									   const math::mat4& viewMat,
									   const math::mat4& projectionMat) override;

		void					blank() override;

		std::unique_ptr<Image> 	snapshot(const RenderContext& context) const override;

	private:
		/// Private Member Variables ///

		//RenderContext*									_context;
		bool											_isInitialized;
		MeshElementGLMapping 							_meshElementGLMapping; // TODO: REMOVE
		TextureGLMapping								_textureGLMapping;
//		LinesGLMapping									_linesGLMapping;
//		std::unordered_set<MeshElement*>				_activeMeshElements;
//		std::unordered_set<Texture*>					_activeTextures;
//		std::unordered_set<const std::vector<Line>*>	_activeLines;
		std::unique_ptr<Mesh>							_skyboxMesh;
		std::unique_ptr<Mesh>							_groundPlaneMesh;
		unsigned										_glEnvironmentUBO;
		ImFont*											_overlayTitleImFont;
		ImFont*											_overlayBodyImFont;
//		std::vector<OpenGLDrawItem> 					_drawItems;
		OpenGLDrawTimer									_drawTimer;



		void EnsureDebugLinesBuffers(Program& program);
		unsigned _dbgLinesVBO = 0;
		unsigned _dbgLinesVAO = 0;







		public:
//		RenderResourceCacheOGL& cache() { return _cache; }
		RenderResourceCacheOGL& cache() override;

		void bindPipeline(PipelineHandle h, const RenderResourceCacheOGL& cache) override;
		void bindMaterial(const Material& material) override;
		void bindMeshElement(const MeshElement& element) override;
		void setPerObject(const math::mat4& model,
						  const math::mat4& view,
						  const math::mat4& projection) override;
		void drawBound() override;

		RenderResourceCacheOGL _cache;
		GLStateCache _state;
		BoundElement _boundElement;


		std::unordered_map<MeshElement*, MeshElementGLRes> _meshElementGL;
	};
}

#endif /* AVARA3D_OPENGLRENDERER_H */
