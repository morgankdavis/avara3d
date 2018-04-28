//
//  OpenGLRenderer.h
//	avara-engine
//
//  Created by Morgan Davis on 4/24/18.
//  Copyright © 2018 Morgan K Davis. All rights reserved.
//

#ifndef OpenGLRenderer_h
#define OpenGLRenderer_h


#import <map>
#import <utility>

#include "Renderer.h"
#include "Types.h"


namespace ae {
	
	class OpenGLRenderer : public Renderer {
		
	public:
		
		/***************************************************************************************
		     Lifecycle
		 ***************************************************************************************/
		
		OpenGLRenderer();
		~OpenGLRenderer();

		/***************************************************************************************
		     Renderer
		 ***************************************************************************************/
		
		void render(Scene& scene,
					unsigned framebufferWidth,
					unsigned framebufferHeight,
					const DEBUG_OPTIONS& debugOptions) override;
		void render(Geometry& geometry,
					const glm::mat4& modelMat,
					const glm::mat4& viewMat,
					const glm::mat4& projectionMat,
					const DEBUG_OPTIONS& debugOptions) override;
		void render(GeometryElement& geometryElement,
					Material& material,
					const glm::mat4& modelMat,
					const glm::mat4& viewMat,
					const glm::mat4& projectionMat,
					const DEBUG_OPTIONS& debugOptions) override;
		
		std::shared_ptr<Image> snapshot(unsigned framebufferWidth,
										unsigned framebufferHeight) const override;
		
	protected:
		
		/**************************************************************************************
		     Protected
		 **************************************************************************************/


		
	private:
		
		/**************************************************************************************
		     Private
		 **************************************************************************************/
		
		void bindEnvironment(const Scene& scene, RenderStats& stats) const;
		
		// <ae_vertexDataID, <ogl_vboHandle, ogl_vaoHandle, ogl_iboHandle>>
		std::map<VERTEX_DATA_ID,
			std::tuple<unsigned, unsigned, unsigned>> 		m_vertexDataHandleGLMapping;
		
		// <ae_textureID, <ogl_textureHandle>
		std::map<TEXTURE_ID, unsigned>						m_textureHandleGLMapping;
		
		VERTEX_DATA_ID 										m_vertexDataHandleCounter;
		TEXTURE_ID 											m_textureHandleCounter;
		
		int													m_glEnvironmentUBO;
	};
}


#endif /* OpenGLRenderer_h */
