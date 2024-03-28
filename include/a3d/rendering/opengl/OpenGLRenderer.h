//
//  OpenGLRenderer.h
//	avara3d
//
//  Created by Morgan Davis on 4/24/18.
//  Copyright © 2018 Morgan K Davis. All rights reserved.
//

#ifndef OpenGLRenderer_h
#define OpenGLRenderer_h


#include <map>
#include <unordered_set>
#include <utility>

#include "glm/glm.hpp"

#include "a3d/Types.h"
#include "a3d/rendering/Renderer.h"


namespace a3d {
	
	
	class Color;
	class Font;
	class Mesh;
	class MeshElement;
	class Line;
	class Texture;
	
	
	class OpenGLRenderer : public Renderer {
		
/**************************************************************************************
	Types
 **************************************************************************************/

	public:

		/* <a3d::MeshElement : <gl_vboHandle, gl_vaoHandle, gl_iboHandle>> */
		using MeshElementGLMapping =
				std::map<MeshElement*, std::tuple<unsigned, unsigned, unsigned>>;

		/* <a3d::Texture : <gl_textureHandle> */
		using TextureGLMapping =
				std::map<Texture*, unsigned>;

//		/* <set<a3d::Line> : <gl_vboHandle, gl_vaoHandle>> */
//		using LineSetGLMapping =
//				std::map<LineSet*, std::pair<unsigned, unsigned>>;

		/* <vector<a3d::Line>> : <gl_vboHandle, gl_vaoHandle>> */
		using LinesGLMapping =
				std::map<std::vector<Line>*, std::pair<unsigned, unsigned>>;

		/* <set<a3d::Point> : <gl_vboHandle, gl_vaoHandle>> */
//		using PointSetGLMapping =
//				std::map<PointSet*, std::pair<unsigned, unsigned>>;

		// *** TO BE REMOVED ***
		/* <a3d::Mesh : set<a3d::Line>> */
//		using MeshAABBLineSetMapping =
//				std::map<Mesh*, LineSet*>;

/*********************************************************************************************
	Lifecycle
 *********************************************************************************************/

		OpenGLRenderer();
		OpenGLRenderer(const OpenGLRenderer& other) = delete; // copy constructor
		OpenGLRenderer& operator=(const OpenGLRenderer& other) = delete; // copy assignment
		~OpenGLRenderer() override;

/*********************************************************************************************
	Renderer
 *********************************************************************************************/

		RenderingApi 				renderingApi() const override;

		bool 						initialize(const RenderContext& context) override;

		void 						beginFrame(const Scene& scene,
											   const RenderContext& context,
											   const DebugOptions& debugOptions,
											   Stats& stats) override;
		void 						endFrame(const Scene& scene,
											 const RenderContext& context,
											 const DebugOptions& debugOptions,
											 Stats& stats) override;

		void 						render(const Scene& scene,
										   const DebugOptions& debugOptions,
										   Stats& stats) override;
		void 						render(Mesh& mesh,
										   const glm::mat4& modelMat,
										   const glm::mat4& viewMat,
										   const glm::mat4& projectionMat,
										   const DebugOptions& debugOptions,
										   Stats& stats) override;
		void 						render(MeshElement& element,
										   Material& material,
										   const glm::mat4& modelMat,
										   const glm::mat4& viewMat,
										   const glm::mat4& projectionMat,
										   const DebugOptions& debugOptions,
										   Stats& stats) override;
		void 						render(const std::vector<Line>& lines,
										   const glm::mat4& modelMat,
										   const glm::mat4& viewMat,
										   const glm::mat4& projectionMat) override;
//		void 						render(PointSet& points,
//										   const glm::mat4& modelMat,
//										   const glm::mat4& viewMat,
//										   const glm::mat4& projectionMat) override;

		std::unique_ptr<Image> 		snapshot(const RenderContext& context) const override;

/*********************************************************************************************
	Private
 *********************************************************************************************/

	private:

		MeshElementGLMapping 					_meshElementGLMapping;
		TextureGLMapping						_textureGLMapping;
//		LineSetGLMapping						_lineSetGLMapping;
		LinesGLMapping							_linesGLMapping;
//		PointSetGLMapping						_pointSetGLMapping;
//
//		MeshAABBLineSetMapping					_meshAABBLineSetMapping;

		std::unordered_set<MeshElement*>		_activeMeshElements;
		std::unordered_set<Texture*>			_activeTextures;

		std::unordered_set<Line*>				_activeLines;

//		std::unordered_set<LineSet*>			_activeLineSets;
//		std::unordered_set<PointSet*>			_activePointSets;

		unsigned								_glEnvironmentUBO;
		
		std::shared_ptr<Font>					_overlayFont;
	};
}


#endif /* OpenGLRenderer_h */
