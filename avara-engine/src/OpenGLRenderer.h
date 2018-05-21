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
	
	
	class Geometry;
	class GeometryElement;
	class MaterialProperty;
	
	
	class OpenGLRenderer : public Renderer {
		
	public:
		
		/**************************************************************************************
		     Types
		 **************************************************************************************/
		
		/* <ae_obj : <gl_vboHandle, gl_vaoHandle, gl_iboHandle>> */
		using GeometryElementGLMapping =
			std::map<std::shared_ptr<GeometryElement>, std::tuple<unsigned,
																  unsigned,
																  unsigned>>;
		
		/* <ae_obj : <gl_textureHandle> */
		using MaterialPropertyGLMapping =
			std::map<std::shared_ptr<MaterialProperty>, unsigned>;

		/* <ae_obj : <gl_vboHandle, gl_vaoHandle>> */
		using AABBGeometryGLMapping =
			std::map<std::shared_ptr<Geometry>, std::pair<unsigned,
														  unsigned>>;

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
		
		void render(std::shared_ptr<Scene> scene,
					const DEBUG_OPTIONS& debugOptions,
					RenderStats& stats) override;
		void render(std::shared_ptr<Geometry> geometry,
					const glm::mat4& modelMat,
					const glm::mat4& viewMat,
					const glm::mat4& projectionMat,
					const DEBUG_OPTIONS& debugOptions,
					RenderStats& stats) override;
		void render(std::shared_ptr<GeometryElement> element,
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

		GeometryElementGLMapping 					m_geometryElementGLMapping;
		MaterialPropertyGLMapping					m_materialPropertyGLMapping;
		AABBGeometryGLMapping						m_aabbGeometryGLMapping;
		
		unsigned									m_glEnvironmentUBO;
		
		std::set<std::shared_ptr<GeometryElement>>	m_activeGeometryElements;
		std::set<std::shared_ptr<MaterialProperty>>	m_activeMaterialProperties;
		std::set<std::shared_ptr<Geometry>>			m_activeAABBGeometries;
		
		FONScontext* 								m_fonsContext;
		int											m_fonsFont;
	};
}


#endif /* OpenGLRenderer_h */
