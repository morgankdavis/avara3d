//
//  GeometryElement.h
//	avara-engine
//
//  Created by Morgan Davis on 12/23/16.
//  Copyright © 2016 Morgan K Davis. All rights reserved.
//

#ifndef GeometryElement_h
#define GeometryElement_h


#include <memory>
#include <vector>

#include <glm/glm.hpp>

#include "Types.h"


namespace ae {


	class Material;
	class Program;
	class Renderer;
	

	class GeometryElement {
		
	public:
		
		/***************************************************************************************
		     Lifecycle
		 ***************************************************************************************/
		
		GeometryElement(std::vector<Vertex>& verticies,
						std::vector<Face>& faces);
		~GeometryElement();

		/***************************************************************************************
		     Internal
		 ***************************************************************************************/
		
		void draw(Renderer& renderer,
				  Material& material,
				  const glm::mat4& modelMat,
				  const glm::mat4& viewMat,
				  const glm::mat4& projectionMat,
				  const DEBUG_OPTIONS& debugOptions);
		
//		void draw(const glm::mat4& modelMat,
//				  const glm::mat4& viewMat,
//				  const glm::mat4& projectionMat,
//				  Material& material,
//				  unsigned glEnvironmentUBO,
//				  DEBUG_OPTIONS debugOptions,
//				  RenderStats& stats);
		
		void hardTransform(const glm::mat4 t, bool norm);

		void loadVertexData(const Program& program);

		std::vector<Vertex> vertices() const;
		std::vector<Face> faces() const;
		
		// EXPERIMENTAL
		
		GEOMETRY_ELEMENT_DIRTY_BITS dirtyBits() const;
		void dirtyBits(GEOMETRY_ELEMENT_DIRTY_BITS bits);
		
		VERTEX_DATA_ID vertexDataID() const;
		void vertexDataID(VERTEX_DATA_ID dataID);
		
	protected:
		
		/***************************************************************************************
		     Protected
		 ***************************************************************************************/
		
		std::vector<Vertex>						m_vertices;
		std::vector<Face>						m_faces;
		unsigned								m_glVBO;
		unsigned								m_glVAO;
		unsigned								m_glIBO;

		// EXPERIMENTAL
		
		GEOMETRY_ELEMENT_DIRTY_BITS				m_dirtyBits;
		VERTEX_DATA_ID							m_vertexDataID;
	};
}


#endif /* GeometryElement_h */
