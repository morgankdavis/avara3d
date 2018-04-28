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

		virtual void render(Scene& scene,
							unsigned framebufferWidth,
							unsigned framebufferHeight,
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
		
		virtual std::shared_ptr<Image> snapshot(unsigned framebufferWidth,
												unsigned framebufferHeight) const;
		
		RenderStats& renderStats();

	private:
		
		/**************************************************************************************
		     Private
		 **************************************************************************************/
		
		RenderStats			m_renderStats;
	};
}


#endif /* Renderer_h */
