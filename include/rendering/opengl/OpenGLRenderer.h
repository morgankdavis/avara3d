//
//  OpenGLRenderer.h
//	avara-engine
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

#include "rendering/Renderer.h"
#include "Types.h"


namespace ae {
	
	
	class Color;
	class Font;
	class Geometry;
	class GeometryElement;
	class Line;
	class MaterialProperty;
	
	
	class OpenGLRenderer : public Renderer {
		
/**************************************************************************************
	Types
 **************************************************************************************/

	public:

		/* <ae_GeometryElement : <gl_vboHandle, gl_vaoHandle, gl_iboHandle>> */
		using GeometryElementGLMapping =
				std::map<std::shared_ptr<GeometryElement>, std::tuple<
				        unsigned,
						unsigned,
						unsigned>>;

		/* <ae_MaterialProperty : <gl_textureHandle> */
		using MaterialPropertyGLMapping =
				std::map<std::shared_ptr<MaterialProperty>, unsigned>;

		/* <set<ae_Line> : <gl_vboHandle, gl_vaoHandle>> */
		using LineSetGLMapping =
				std::map<std::shared_ptr<LineSet>, std::pair<
						unsigned,
						unsigned>>;

		/* <set<ae_Point> : <gl_vboHandle, gl_vaoHandle>> */
		using PointSetGLMapping =
				std::map<std::shared_ptr<PointSet>, std::pair<
						unsigned,
						unsigned>>;

		/* <ae_Geometry : set<ae_Line>> */
		using GeometryAABBLineSetMapping =
				std::map<std::shared_ptr<Geometry>,
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

		void 						beginFrame(const RenderContext& context) override;
		void 						endFrame(const RenderContext& context) override;

		void 						render(Scene& scene,
										   const DEBUG_OPTIONS& debugOptions,
										   FrameStats& stats) override;
		void 						render(std::shared_ptr<Geometry> geometry,
										   const glm::mat4& modelMat,
										   const glm::mat4& viewMat,
										   const glm::mat4& projectionMat,
										   const DEBUG_OPTIONS& debugOptions,
										   FrameStats& stats) override;
		void 						render(std::shared_ptr<GeometryElement> element,
										   Material& material,
										   const glm::mat4& modelMat,
										   const glm::mat4& viewMat,
										   const glm::mat4& projectionMat,
										   const DEBUG_OPTIONS& debugOptions,
										   FrameStats& stats) override;
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

		GeometryElementGLMapping 				_geometryElementGLMapping;
		MaterialPropertyGLMapping				_materialPropertyGLMapping;
		LineSetGLMapping						_lineSetGLMapping;
		PointSetGLMapping						_pointSetGLMapping;

		GeometryAABBLineSetMapping				_geometryAABBLineSetMapping;

		std::unordered_set<std::shared_ptr<GeometryElement>>	_activeGeometryElements;
		std::unordered_set<std::shared_ptr<MaterialProperty>>	_activeMaterialProperties;
		std::unordered_set<std::shared_ptr<LineSet>>			_activeLineSets;
		std::unordered_set<std::shared_ptr<PointSet>>			_activePointSets;

		unsigned								_glEnvironmentUBO;
		
		std::shared_ptr<Font>					_overlayFont;
	};
}


#endif /* OpenGLRenderer_h */
