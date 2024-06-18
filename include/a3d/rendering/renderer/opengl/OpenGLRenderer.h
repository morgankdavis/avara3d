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
	
	
	class OpenGLRenderer : public Renderer {

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
	Internal Lifecycle
 *********************************************************************************************/

		OpenGLRenderer();
		OpenGLRenderer(const OpenGLRenderer& other) = delete; // copy constructor
		OpenGLRenderer& operator=(const OpenGLRenderer& other) = delete; // copy assignment
		OpenGLRenderer(OpenGLRenderer&& other) = delete; // move constructor
		OpenGLRenderer& operator=(OpenGLRenderer&& other) = delete; // move assignment
		~OpenGLRenderer() override;

/*********************************************************************************************
	Renderer Internal Members
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
	Private IVars
 *********************************************************************************************/

	private:

		MeshElementGLMapping 							_meshElementGLMapping;
		TextureGLMapping								_textureGLMapping;
		LinesGLMapping									_linesGLMapping;
		std::unordered_set<MeshElement*>				_activeMeshElements;
		std::unordered_set<Texture*>					_activeTextures;
		std::unordered_set<const std::vector<Line>*>	_activeLines;
		unsigned										_glEnvironmentUBO;
		std::unique_ptr<Font>							_overlayFont;
	};
}


#endif /* AVARA3D_OPENGLRENDERER_H */
