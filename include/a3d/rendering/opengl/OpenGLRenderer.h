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
#include "a3d/rendering/material/Material.h"


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

		/* <A3D_MeshElement : <gl_vboHandle, gl_vaoHandle, gl_iboHandle>> */
		using MeshElementGLMapping =
				std::map<std::shared_ptr<MeshElement>, std::tuple<
						unsigned,
						unsigned,
						unsigned>>;

		/* <A3D_Texture : <gl_textureHandle> */
		using TextureGLMapping =
				std::map<std::shared_ptr<Texture>, unsigned>;

		/* <set<A3D_Line> : <gl_vboHandle, gl_vaoHandle>> */
		using LineSetGLMapping =
				std::map<std::shared_ptr<LineSet>, std::pair<
						unsigned,
						unsigned>>;

		/* <set<A3D_Point> : <gl_vboHandle, gl_vaoHandle>> */
		using PointSetGLMapping =
				std::map<std::shared_ptr<PointSet>, std::pair<
						unsigned,
						unsigned>>;

		/* <A3D_Mesh : set<A3D_Line>> */
		using MeshAABBLineSetMapping =
				std::map<std::shared_ptr<Mesh>,
						std::shared_ptr<LineSet>>;

/*********************************************************************************************
	Lifecycle
 *********************************************************************************************/

		OpenGLRenderer();
		OpenGLRenderer(const OpenGLRenderer& other) = delete; // copy constructor
		OpenGLRenderer& operator=(const OpenGLRenderer& other) = delete; // copy assignment
		~OpenGLRenderer();

/*********************************************************************************************
	Renderer
 *********************************************************************************************/

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
		void 						render(std::shared_ptr<Mesh> mesh,
										   const glm::mat4& modelMat,
										   const glm::mat4& viewMat,
										   const glm::mat4& projectionMat,
										   const DebugOptions& debugOptions,
										   Stats& stats) override;
		void 						render(std::shared_ptr<MeshElement> element,
										   Material& material,
										   const glm::mat4& modelMat,
										   const glm::mat4& viewMat,
										   const glm::mat4& projectionMat,
										   const DebugOptions& debugOptions,
										   Stats& stats) override;
		void 						render(std::shared_ptr<LineSet> lines,
										   const glm::mat4& modelMat,
										   const glm::mat4& viewMat,
										   const glm::mat4& projectionMat) override;
		void 						render(std::shared_ptr<PointSet> points,
										   const glm::mat4& modelMat,
										   const glm::mat4& viewMat,
										   const glm::mat4& projectionMat) override;

		std::shared_ptr<Image> 		snapshot(const RenderContext& context) const override;

/*********************************************************************************************
	Private
 *********************************************************************************************/

	private:

		MeshElementGLMapping 					_meshElementGLMapping;
		TextureGLMapping						_textureGLMapping;
		LineSetGLMapping						_lineSetGLMapping;
		PointSetGLMapping						_pointSetGLMapping;

		MeshAABBLineSetMapping					_meshAABBLineSetMapping;

		std::unordered_set<std::shared_ptr<MeshElement>>	_activeMeshElements;
		std::unordered_set<std::shared_ptr<Texture>>		_activeTextures;
		std::unordered_set<std::shared_ptr<LineSet>>		_activeLineSets;
		std::unordered_set<std::shared_ptr<PointSet>>		_activePointSets;

		unsigned								_glEnvironmentUBO;
		
		std::shared_ptr<Font>					_overlayFont;
	};
}


#endif /* OpenGLRenderer_h */
