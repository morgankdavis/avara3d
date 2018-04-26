//
//  OpenGLRenderer.h
//	avara-engine
//
//  Created by Morgan Davis on 4/24/18.
//  Copyright © 2018 Morgan K Davis. All rights reserved.
//

#ifndef OpenGLRenderer_h
#define OpenGLRenderer_h


#include "Renderer.h"


namespace ae {
	
	class OpenGLRenderer : Renderer {
		
	public:
		
		/***************************************************************************************
		     Lifecycle
		 ***************************************************************************************/
		
		OpenGLRenderer();
		~OpenGLRenderer();
		
		/***************************************************************************************
		     Public
		 ***************************************************************************************/
			

		
		/***************************************************************************************
		     Renderer
		 ***************************************************************************************/

		std::shared_ptr<Image> snapshot(unsigned framebufferWidth,
										unsigned framebufferHeight) const override;
		
		void render(const Scene& scene) override;
		void render(const GeometryElement& geometryElement,
					const Material& material,
					const glm::mat4& modelMat,
					const glm::mat4& viewMat,
					const glm::mat4& projectionMat,
					const DEBUG_OPTIONS& debugOptions) override;
		
	protected:
		
		/**************************************************************************************
		     Protected
		 **************************************************************************************/


		
	private:
		
		/**************************************************************************************
		     Private
		 **************************************************************************************/
		
		void bindEnvironment(const Scene& scene, RenderStats& stats) const;
		
		int			m_glEnvironmentUBO;
	};
}


#endif /* OpenGLRenderer_h */
