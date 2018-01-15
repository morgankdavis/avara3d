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

//#include <GL/glew.h>
#include <glm/glm.hpp>

#include "Types.h"


namespace ae {


	class Material;
	class Program;
	

	class GeometryElement {
		
	public:
		
		/***************************************************************************************
		     MARK:   Lifecycle
		 **************************************************************************************/
		
		GeometryElement(std::vector<Vertex>& verticies,
						std::vector<Face>& faces);

		/***************************************************************************************
		     MARK:   Internal
		 **************************************************************************************/
		
		void draw(const glm::mat4& modelMat,
				  const glm::mat4& viewMat,
				  const glm::mat4& projectionMat,
				  Material& material,
				  unsigned glEnvironmentUBO,
				  DebugOption debugOptions,
				  DrawStats& stats);
		
		void hardTransform(const glm::mat4 t, bool norm);
		void generateSmoothNormals();
		void generateFlatNormals();

		void loadVertexData(const Program& program);

		std::vector<Vertex>& vertices();
		std::vector<Face>& faces();
		
	protected:
		
		/***************************************************************************************
		     MARK:   Protected
		 **************************************************************************************/
		
		std::vector<Vertex>						m_vertices;
		std::vector<Face>						m_faces;
		unsigned								m_glVAO;
		unsigned								m_glIBO;

	};
}


#endif /* GeometryElement_h */
