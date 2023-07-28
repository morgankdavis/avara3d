//
//  Renderer.h
//	avara-engine
//
//  Created by Morgan Davis on 4/22/18.
//  Copyright © 2018 Morgan K Davis. All rights reserved.
//

#ifndef Renderer_h
#define Renderer_h


#include <memory>
#include <set>

#include <glm/glm.hpp>

#include "Types.h"


namespace ae {
	
	
	class Geometry;
	class GeometryElement;
	class Image;
	class Line;
	class Material;
	class Point;
	class RenderContext;
	class Scene;
	
	
	class Renderer : public std::enable_shared_from_this<Renderer> {

/*********************************************************************************************
	Lifecycle
 *********************************************************************************************/

	public:

		Renderer();
		
		Renderer(const Renderer& other) = delete; // copy constructor
		Renderer& operator=(const Renderer& other) = delete; // copy assignment
		
		virtual ~Renderer();
		
/*********************************************************************************************
	Internal
 *********************************************************************************************/

		virtual bool 		initialize(const RenderContext& context);
		
		virtual void 		beginFrame(const RenderContext& context);
		virtual void 		endFrame(const RenderContext& context);

		virtual void 		render(std::shared_ptr<ae::Scene> scene,
								   const DEBUG_OPTIONS& debugOptions,
								   RenderStats& stats);
		virtual void 		render(std::shared_ptr<Geometry> geometry,
								   const glm::mat4& modelMat,
								   const glm::mat4& viewMat,
								   const glm::mat4& projectionMat,
								   const DEBUG_OPTIONS& debugOptions,
								   RenderStats& stats);
		virtual void 		render(std::shared_ptr<ae::GeometryElement> element,
								   Material& material,
								   const glm::mat4& modelMat,
								   const glm::mat4& viewMat,
								   const glm::mat4& projectionMat,
								   const DEBUG_OPTIONS& debugOptions,
								   RenderStats& stats);
		virtual void 		render(std::shared_ptr<LineSet> lines,
								   const glm::mat4& modelMat,
								   const glm::mat4& viewMat,
								   const glm::mat4& projectionMat);
		virtual void 		render(std::shared_ptr<PointSet> points,
								   const glm::mat4& modelMat,
								   const glm::mat4& viewMat,
								   const glm::mat4& projectionMat);
		
		virtual std::shared_ptr<ae::Image> 	snapshot(const RenderContext& context) const;
		
		RenderStats& 		renderStats();

	private:
		
/**************************************************************************************
	Private
 **************************************************************************************/
		
		RenderStats			_renderStats;
	};
}


#endif /* Renderer_h */
