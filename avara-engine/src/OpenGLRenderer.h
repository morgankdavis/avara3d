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
#import <set>

#include "Renderer.h"
#include "Types.h"


struct FONScontext;


namespace ae {
	
	class OpenGLRenderer : public Renderer {
		
	public:
		
		/**************************************************************************************
		     Types
		 **************************************************************************************/
		
		//typedef std::map<VERTEX_DATA_ID, std::tuple<unsigned, unsigned, unsigned>> VertexIDMapping;

		/***************************************************************************************
		     Lifecycle
		 ***************************************************************************************/
		
		OpenGLRenderer();
		
		OpenGLRenderer(const OpenGLRenderer& other) = delete; // copy constructor
		OpenGLRenderer& operator=(const OpenGLRenderer& other) = delete; // copy assignment
		
		~OpenGLRenderer();

		/***************************************************************************************
		     Renderer
		 ***************************************************************************************/
		
		bool initialize() override;
		
		void beginFrame(const RenderContext& context) override;
		void endFrame(const RenderContext& context) override;
		
		void render(Scene& scene,
					const DEBUG_OPTIONS& debugOptions,
					RenderStats& stats) override;
		void render(Geometry& geometry,
					const glm::mat4& modelMat,
					const glm::mat4& viewMat,
					const glm::mat4& projectionMat,
					const DEBUG_OPTIONS& debugOptions,
					RenderStats& stats) override;
		void render(GeometryElement& element,
					Material& material,
					const glm::mat4& modelMat,
					const glm::mat4& viewMat,
					const glm::mat4& projectionMat,
					const DEBUG_OPTIONS& debugOptions,
					RenderStats& stats) override;
		
		std::shared_ptr<Image> snapshot(const RenderContext& context) const override;
		
		void cleanup() override;
		
	private:
		
		/**************************************************************************************
		     Private
		 **************************************************************************************/

		/* <ae_vertexDataID, <gl_vboHandle, gl_vaoHandle, gl_iboHandle>> */
		//VertexIDMapping								m_vertexDataIDMapping;
		std::map<VERTEX_DATA_ID,
			std::tuple<unsigned, unsigned, unsigned>> 	m_vertexDataIDMapping;
		
		/* <ae_textureID, <gl_textureHandle> */
		std::map<TEXTURE_ID, unsigned>					m_textureIDMapping;
		
		VERTEX_DATA_ID 									m_vertexDataIDCounter;
		TEXTURE_ID 										m_textureIDCounter;

		/* <ae_vertexDataID, <gl_vboHandle, gl_vaoHandle>> */
		std::map<VERTEX_DATA_ID,
			std::pair<unsigned, unsigned>>				m_aabbVertexDataIDMapping;
		
		VERTEX_DATA_ID 									m_aabbVertexDataIDCounter;
		
		unsigned										m_glEnvironmentUBO;
		
		FONScontext* 									m_fonsContext;
		int												m_fonsFont;
		
		
		
		// EXPERIMENTAL
		
		std::set<VERTEX_DATA_ID>						m_frameVertexDataIDs;
	};
}


#endif /* OpenGLRenderer_h */
