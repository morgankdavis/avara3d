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

#include <GL/glew.h>
#include <glm/glm.hpp>

#include "Types.h"


namespace ae {


	class Program;


	class GeometryElement {
		
	public:
		
		/***************************************************************************************
		     MARK:   Lifecycle
		 **************************************************************************************/
		
		GeometryElement(std::vector<Vertex> verticies, std::vector<Face> faces);
		
		/***************************************************************************************
		     MARK:   Public
		 **************************************************************************************/

		std::shared_ptr<Program> program() const;
		void program(const std::shared_ptr<Program> program);
		
		/***************************************************************************************
		     MARK:   Internal
		 **************************************************************************************/

		void hardTransform(const glm::mat4 t, bool norm);
		void generateSmoothNormals();
		void generateFlatNormals();

		void loadVertexData();

		GLint glVAO();
		GLint glIBO();
		std::vector<Vertex>& vertices();
		std::vector<Face>& faces();
		
	private:
		
		/***************************************************************************************
		     MARK:   Private
		 **************************************************************************************/
		
		std::shared_ptr<Program>				m_program;
		std::vector<Vertex>						m_vertices;
		std::vector<Face>						m_faces;
		GLint									m_glVAO;
		GLint									m_glIBO;
	};
}


#endif /* GeometryElement_h */
