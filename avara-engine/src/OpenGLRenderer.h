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
		
		bool init() override;
		
		void beginFrame(const RenderContext& context) override;
		void endFrame(const RenderContext& context) override;
		
		void render(Scene& scene,
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
		
		std::shared_ptr<Image> snapshot(const RenderContext& context) const override;
		
	private:
		
		/**************************************************************************************
		     Private
		 **************************************************************************************/

		/* <ae_vertexDataID, <gl_vboHandle, gl_vaoHandle, gl_iboHandle>> */
		std::map<VERTEX_DATA_ID,
			std::tuple<unsigned, unsigned, unsigned>> 		m_vertexDataIDMapping;
		
		/* <ae_textureID, <gl_textureHandle> */
		std::map<TEXTURE_ID, unsigned>						m_textureIDMapping;
		
		VERTEX_DATA_ID 										m_vertexDataIDCounter;
		TEXTURE_ID 											m_textureIDCounter;
		
		unsigned											m_glEnvironmentUBO;
	};
}


#endif /* OpenGLRenderer_h */
