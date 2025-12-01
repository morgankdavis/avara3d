//
//  OpenGlRenderer.h
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

#include "glm/glm.hpp"

#include "a3d/Types.h"
#include "a3d/rendering/renderer/Renderer.h"


namespace a3d {
	
	
	class Color;
	class Font;
	class Mesh;
	class MeshElement;
	class Line;
	class Texture;
	
	
	class OpenGlRenderer : public Renderer {

/**************************************************************************************
	Internal Types
 **************************************************************************************/

	public:

		/* <a3d::MeshElement* : <gl_vbo, gl_vao, gl_ebo>> */
		using MeshElementGLMapping =
				std::map<MeshElement*, std::tuple<unsigned, unsigned, unsigned>>;

		/* <a3d::Texture* : <gl_textureHandle> */
		using TextureGLMapping =
				std::map<Texture*, unsigned>;

		/* <vector<a3d::Line>>* : <gl_vbo, gl_vao>> */
		using LinesGLMapping =
				std::map<const std::vector<Line>*, std::pair<unsigned, unsigned>>;

/*********************************************************************************************
	Private Static Members
 *********************************************************************************************/

		using GLGetProcAddress = void* (*)(const char* name);
		static bool InitGL(GLGetProcAddress getProcAddress);

/*********************************************************************************************
	Internal Lifecycle Functions
 *********************************************************************************************/

		OpenGlRenderer();
		OpenGlRenderer(const OpenGlRenderer& other) = delete; // copy constructor
		OpenGlRenderer& operator=(const OpenGlRenderer& other) = delete; // copy assignment
		OpenGlRenderer(OpenGlRenderer&& other) = delete; // move constructor
		OpenGlRenderer& operator=(OpenGlRenderer&& other) = delete; // move assignment
		~OpenGlRenderer() override;

/*********************************************************************************************
	Renderer Internal Member Functions
 *********************************************************************************************/

		RenderingApi 			renderingApi() const override;

		bool 					initialize(const RenderContext& context) override;

		void 					beginFrame(const Scene& scene,
										   const RenderContext& context,
										   const DebugOptions& debugOptions,
										   Stats& stats) override;
		void 					endFrame(const Scene& scene,
										 const RenderContext& context,
										 const DebugOptions& debugOptions,
										 Stats& stats) override;

		void 					preTraversal(const Scene& scene,
											 const RenderContext& context,
											 const DebugOptions& debugOptions,
											 Stats& stats) override;
		void 					postTraversal(const Scene& scene,
											  const RenderContext& context,
											  const std::vector<Node*>& lightNodes,
											  const DebugOptions& debugOptions,
											  Stats& stats) override;

		void 					render(const Scene& scene,
									   const DebugOptions& debugOptions,
									   Stats& stats) override;
		void 					render(Mesh& mesh,
									   const glm::mat4& modelMat,
									   const glm::mat4& viewMat,
									   const glm::mat4& projectionMat,
									   const DebugOptions& debugOptions,
									   Stats& stats) override;
		void 					render(MeshElement& element,
									   Material& material,
									   const glm::mat4& modelMat,
									   const glm::mat4& viewMat,
									   const glm::mat4& projectionMat,
									   const DebugOptions& debugOptions,
									   Stats& stats) override;
		void 					render(const std::vector<Line>& lines,
									   const glm::mat4& modelMat,
									   const glm::mat4& viewMat,
									   const glm::mat4& projectionMat) override;

		std::unique_ptr<Image> 	snapshot(const RenderContext& context) const override;

		void					framebufferScaleChanged(const RenderContext& context) override;

/*********************************************************************************************
	 Internal Constant Declarations
 *********************************************************************************************/

		static const std::string 	STATS_TITLE_FONT_NAME;
		static const std::string 	STATS_TITLE_FONT_TYPE;
		static const float 			STATS_TITLE_FONT_SIZE;
		static const std::string 	STATS_BODY_FONT_NAME;
		static const std::string 	STATS_BODY_FONT_TYPE;
		static const float 			STATS_BODY_FONT_SIZE;
		static const float 			STATS_TITLE_TO_BODY_PADDING;

/*********************************************************************************************
	Private Member Variables
 *********************************************************************************************/

	private:

		MeshElementGLMapping 							_meshElementGLMapping;
		TextureGLMapping								_textureGLMapping;
		LinesGLMapping									_linesGLMapping;
		std::unordered_set<MeshElement*>				_activeMeshElements;
		std::unordered_set<Texture*>					_activeTextures;
		std::unordered_set<const std::vector<Line>*>	_activeLines;
		unsigned										_glEnvironmentUBO;
		std::unique_ptr<Font>							_overlayTitleFont;
		std::unique_ptr<Font>							_overlayBodyFont;
	};
}


#endif /* AVARA3D_OPENGLRENDERER_H */
