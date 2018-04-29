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

#include <glm/glm.hpp>

#include "Types.h"


namespace ae {
	
	
	class Geometry;
	class GeometryElement;
	class Image;
	class Material;
	class RenderContext;
	class Scene;
	
	
	class Renderer : public std::enable_shared_from_this<Renderer> {
		
	public:
		
		/***************************************************************************************
		     Lifecycle
		 ***************************************************************************************/
		
		Renderer();
		virtual ~Renderer();
		
		/***************************************************************************************
		     Internal
		 ***************************************************************************************/

		virtual bool init();
		
		virtual void beginFrame(const RenderContext& context);
		virtual void endFrame(const RenderContext& context);
		
		virtual void render(Scene& scene,
							const DEBUG_OPTIONS& debugOptions);
		virtual void render(Geometry& geometry,
							const glm::mat4& modelMat,
							const glm::mat4& viewMat,
							const glm::mat4& projectionMat,
							const DEBUG_OPTIONS& debugOptions);
		virtual void render(GeometryElement& geometryElement,
							Material& material,
							const glm::mat4& modelMat,
							const glm::mat4& viewMat,
							const glm::mat4& projectionMat,
							const DEBUG_OPTIONS& debugOptions);
		
		virtual std::shared_ptr<Image> snapshot(const RenderContext& context) const;
		
		RenderStats& renderStats();

	private:
		
		/**************************************************************************************
		     Private
		 **************************************************************************************/
		
		RenderStats			m_renderStats;
	};
}


#endif /* Renderer_h */
