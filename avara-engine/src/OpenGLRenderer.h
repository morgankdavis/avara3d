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
		
		/* <ae_renderID, <gl_vboHandle, gl_vaoHandle, gl_iboHandle>> */
		typedef std::map<GEOMETRY_ELEMENT_ID,
			std::tuple<unsigned, unsigned, unsigned>> 		GeometryElementIDMapping;
		/* <ae_renderID, <gl_textureHandle> */
		typedef std::map<MATERIAL_PROPERTY_ID, unsigned> 	MaterialPropertyIDMapping;
		/* <ae_renderID, <gl_vboHandle, gl_vaoHandle>> */
		typedef std::map<GEOMETRY_ID,
			std::pair<unsigned, unsigned>>					AABBGeometryIDMapping;

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
		
	private:
		
		/**************************************************************************************
		     Private
		 **************************************************************************************/

		GeometryElementIDMapping 				m_elementIDMapping;
		GEOMETRY_ELEMENT_ID 					m_elementIDCounter;
		
		MaterialPropertyIDMapping				m_materialPropertyIDMapping;
		MATERIAL_PROPERTY_ID 					m_materialPropertyIDCounter;

		AABBGeometryIDMapping					m_geometryAABBIDMapping;
		GEOMETRY_ID 							m_geometryAABBIDCounter;
		
		unsigned								m_glEnvironmentUBO;
		
		std::set<GEOMETRY_ELEMENT_ID>			m_activeElementIDs;
		std::set<MATERIAL_PROPERTY_ID>			m_activeMaterialPropertyIDs;
		std::set<GEOMETRY_ID>					m_activeGeometryAABBIDs;
		
		FONScontext* 							m_fonsContext;
		int										m_fonsFont;
	};
}


#endif /* OpenGLRenderer_h */
